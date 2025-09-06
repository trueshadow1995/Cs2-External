#include <windows.h>

#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>

#include "../Headers/BoneGlobal.h"
#include "../Headers/EntityDataManager.h"
#include "../Headers/Globals.h"
#include "../Headers/Math.h"
#include "../Headers/Offsets.h"
using namespace std::chrono;

// Precompute constants
constexpr std::array<int, 32> ESSENTIAL_BONES = {
    6,  5,  4,  3,  2,  0,  // Head to pelvis
    8,  9,  10,             // Left arm
    13, 14, 15,             // Right arm
    22, 23, 30,             // Left leg
    25, 26, 32,             // Right leg
    11, 12, 16, 17, 38, 39, 40, 66, 7, 18, 19, 37};

constexpr uint64_t BONE_CACHE_DURATION = 14;
constexpr int ENTITY_LIST_INDEX_SHIFT = 9;
constexpr int ENTITY_LIST_INDEX_MASK = 0x1FF;

// Precompute bone address offsets
std::array<size_t, ESSENTIAL_BONES.size()> boneAddressOffsets;


bool IsViewMatrixValid(const ViewMatrix_t& matrix) {
  // Check for NaN, infinity, or obviously invalid values
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (std::isnan(matrix[i][j]) || !std::isfinite(matrix[i][j])) {
        return false;
      }
    }
  }

  // Check for identity matrix (invalid for view matrix)
  if (matrix[0][0] == 1.0f && matrix[1][1] == 1.0f && matrix[2][2] == 1.0f &&
      matrix[3][3] == 1.0f && matrix[0][3] == 0.0f && matrix[1][3] == 0.0f &&
      matrix[2][3] == 0.0f) {
    return false;
  }

  // Check for zero matrix
  if (matrix[0][0] == 0.0f && matrix[1][1] == 0.0f && matrix[2][2] == 0.0f &&
      matrix[3][3] == 0.0f) {
    return false;
  }

  return true;
}

// tries multiple viewmatrixs 
ViewMatrix_t ReadViewMatrixWithRetry(Memory& mem, uintptr_t client) {
  constexpr std::array<uintptr_t, 4> VIEW_MATRIX_OFFSETS = {
      offsets::ViewMatrix, offsets::ViewMatrix + 0x40,
      offsets::ViewMatrix - 0x40, offsets::ViewMatrix + 0x80};

  static int lastGoodOffsetIndex = 0;
  static ViewMatrix_t lastGoodMatrix;

  // First try the last known good offset
  ViewMatrix_t matrix =
      mem.Read<ViewMatrix_t>(client + VIEW_MATRIX_OFFSETS[lastGoodOffsetIndex]);
  if (IsViewMatrixValid(matrix)) {
    lastGoodMatrix = matrix;
    return matrix;
  }

  // If last good failed, try all offsets
  for (int i = 0; i < VIEW_MATRIX_OFFSETS.size(); i++) {
    if (i == lastGoodOffsetIndex) continue;  // Skip the one we already tried

    matrix = mem.Read<ViewMatrix_t>(client + VIEW_MATRIX_OFFSETS[i]);
    if (IsViewMatrixValid(matrix)) {
      lastGoodOffsetIndex = i;
      lastGoodMatrix = matrix;
      return matrix;
    }
  }

  // If all else fails, return last known good matrix
  return lastGoodMatrix;
}

// Initialize bone offsets once
void InitializeBoneOffsets() {
  static bool initialized = false;
  if (!initialized) {
    for (size_t i = 0; i < ESSENTIAL_BONES.size(); ++i) {
      boneAddressOffsets[i] = ESSENTIAL_BONES[i] * sizeof(CBoneData);
    }
    initialized = true;
  }
}

GameData DataManager::GetGameData() {
  std::lock_guard<std::mutex> lock(dataMutex_);
  return gameData_;
}

DataManager::DataManager(Memory& mem, uintptr_t client)
    : mem_(mem), client_(client), running_(false) {
  InitializeBoneOffsets();
  StartUpdateThread();
}

DataManager::~DataManager() { StopUpdateThread(); }

void DataManager::StartUpdateThread() {
  if (!running_) {
    running_ = true;
    updateThread_ = std::thread(&DataManager::UpdateLoop, this);
  }
}

void DataManager::StopUpdateThread() {
  running_ = false;
  if (updateThread_.joinable()) {
    updateThread_.join();
  }
}

bool DataManager::ShouldUpdateBones(uint64_t pawn, uint64_t currentTime) {
  auto it = boneUpdateTimes.find(pawn);
  if (it == boneUpdateTimes.end()) {
    return true;
  }
  return (currentTime - it->second) > BONE_CACHE_DURATION;
}

std::array<CBoneData, globals::MAX_BONES> DataManager::ReadBones(
    uint64_t pawn, const Vector3& entityOrigin) {
  std::array<CBoneData, globals::MAX_BONES> bones{};
  if (!pawn) return bones;

  try {
    {
      std::lock_guard<std::mutex> lock(dataMutex_);
      auto prevIt = boneCache.find(pawn);
      if (prevIt != boneCache.end()) {
        previousBoneCache[pawn] = prevIt->second;
      }
      interpolationStartTimes[pawn] =
          duration_cast<milliseconds>(system_clock::now().time_since_epoch())
              .count();
    }

    uintptr_t gameSceneNode =
        mem_.Read<uintptr_t>(pawn + offsets::m_pGameSceneNode);
    if (!gameSceneNode) return bones;

    const uintptr_t boneArrayPtr =
        mem_.Read<uintptr_t>(gameSceneNode + offsets::m_modelState + 0x80);
    if (!boneArrayPtr) return bones;

    std::vector<uintptr_t> boneAddresses;
    boneAddresses.reserve(ESSENTIAL_BONES.size());

    for (size_t i = 0; i < ESSENTIAL_BONES.size(); i++) {
      boneAddresses.push_back(boneArrayPtr + boneAddressOffsets[i]);
    }

    auto boneData = mem_.ReadBatch<CBoneData>(boneAddresses);

    for (size_t i = 0; i < ESSENTIAL_BONES.size(); i++) {
      if (i < boneData.size() && !boneData[i].location.IsZero() &&
          !std::isnan(boneData[i].location.x) &&
          boneData[i].location.IsValid()) {  // ADDED: Extra validation
        bones[ESSENTIAL_BONES[i]] = boneData[i];
      }
    }

  } catch (...) {
    // error handling
  }

  return bones;
}

std::array<CBoneData, globals::MAX_BONES> DataManager::GetInterpolatedBones(
    uint64_t pawn, uint64_t currentTime) {
  std::array<CBoneData, globals::MAX_BONES> interpolatedBones{};

  std::lock_guard<std::mutex> lock(dataMutex_);

  auto currentIt = boneCache.find(pawn);
  auto previousIt = previousBoneCache.find(pawn);
  auto startTimeIt = interpolationStartTimes.find(pawn);

  if (currentIt == boneCache.end() || previousIt == previousBoneCache.end() ||
      startTimeIt == interpolationStartTimes.end()) {
    return (currentIt != boneCache.end()) ? currentIt->second
                                          : interpolatedBones;
  }

  const auto& currentBones = currentIt->second;
  const auto& previousBones = previousIt->second;
  uint64_t startTime = startTimeIt->second;

  float interpolationTime = static_cast<float>(BONE_CACHE_DURATION);
  float elapsed = static_cast<float>(currentTime - startTime);
  float alpha = elapsed / interpolationTime;

  // Use std::clamp for better performance
  alpha = std::clamp(alpha, 0.0f, 1.0f);

  // Only interpolate essential bones 
  for (int boneId : ESSENTIAL_BONES) {
    if (boneId < globals::MAX_BONES &&
        !currentBones[boneId].location.IsZero() &&
        currentBones[boneId].location.IsValid()) {  //  Validation
      if (!previousBones[boneId].location.IsZero() &&
          previousBones[boneId].location.IsValid()) {  //  Validation
        interpolatedBones[boneId].location =
            Vector3::Lerp(previousBones[boneId].location,
                          currentBones[boneId].location, alpha);
      } else {
        interpolatedBones[boneId].location = currentBones[boneId].location;
      }
    }
  }

  return interpolatedBones;
}

uintptr_t DataManager::GetEntityPawn(int index, uintptr_t entityList) {
  try {
    uintptr_t listEntry = mem_.Read<uintptr_t>(
        entityList + (0x8 * (index >> ENTITY_LIST_INDEX_SHIFT)) + 0x10);
    if (!listEntry) return 0;

    uintptr_t controller = mem_.Read<uintptr_t>(
        listEntry + (0x78 * (index & ENTITY_LIST_INDEX_MASK)));
    if (!controller) return 0;

    uint32_t pawnHandle = mem_.Read<uint32_t>(controller + offsets::m_hPawn);
    if (!pawnHandle) return 0;

    uintptr_t listEntry2 = mem_.Read<uintptr_t>(
        entityList +
        (0x8 * ((pawnHandle & 0x7FFF) >> ENTITY_LIST_INDEX_SHIFT)) + 0x10);
    if (!listEntry2) return 0;

    return mem_.Read<uintptr_t>(listEntry2 +
                                (0x78 * (pawnHandle & ENTITY_LIST_INDEX_MASK)));
  } catch (...) {
    return 0;
  }
}

void DataManager::UpdateLoop() {
  auto lastFrameTime = system_clock::now();
  static ViewMatrix_t lastValidViewMatrix;  //  Cache last good matrix

  while (running_) {
    GameData newData{};
    uint64_t currentTime =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count();

    try {
      // Read local player
      uintptr_t localPawn =
          mem_.Read<uintptr_t>(client_ + offsets::LocalPlayerPawn);
      if (!localPawn) {
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        continue;
      }

      // Batch read local player data
      Vector3 localOrigin =
          mem_.Read<Vector3>(localPawn + offsets::m_vOldOrigin);

      // CHANGED: Use the new view matrix reading function
      newData.viewMatrix = ReadViewMatrixWithRetry(mem_, client_);

      newData.localTeam = mem_.Read<int>(localPawn + offsets::m_iTeamNum);
      newData.localPlayerPawn = localPawn;
      newData.localOrigin = localOrigin;

      // ADDED: Validate local origin
      if (localOrigin.IsZero() || !localOrigin.IsValid() ||
          std::abs(localOrigin.x) > 100000.0f ||
          std::abs(localOrigin.y) > 100000.0f) {
        continue;  // Skip frame if origin is invalid
      }

      // Entity list
      uintptr_t entityList =
          mem_.Read<uintptr_t>(client_ + offsets::EntityList);
      if (!entityList) continue;

      // Pre-allocate to avoid reallocations
      newData.entities.reserve(64);

      // Process entities
      for (int i = 0; i < 64; i++) {
        uintptr_t pawn = GetEntityPawn(i, entityList);
        if (!pawn || pawn == localPawn) continue;

        int health = mem_.Read<int>(pawn + offsets::m_iHealth);
        if (health <= 0 || health > 100) continue;

        int team = mem_.Read<int>(pawn + offsets::m_iTeamNum);
        Vector3 entityOrigin = mem_.Read<Vector3>(pawn + offsets::m_vOldOrigin);

        // Skip invalid origins early
        if (entityOrigin.IsZero() || !entityOrigin.IsValid() ||
            std::abs(entityOrigin.x) > 100000.0f ||
            std::abs(entityOrigin.y) > 100000.0f) {
          continue;
        }

        // Read player name from controller only if needed
        std::string playerName = "Player";
        if (globals::NameEsp) {
          uintptr_t listEntry = mem_.Read<uintptr_t>(
              entityList + (0x8 * (i >> ENTITY_LIST_INDEX_SHIFT)) + 0x10);
          if (listEntry) {
            uintptr_t controller = mem_.Read<uintptr_t>(
                listEntry + (0x78 * (i & ENTITY_LIST_INDEX_MASK)));
            if (controller) {
              char nameBuffer[32] = {0};  // Reduced buffer size
              mem_.Read(controller + 0x6E8, nameBuffer, sizeof(nameBuffer));
              if (nameBuffer[0] != '\0') {
                playerName = std::string(nameBuffer);
              }
            }
          }
        }

        // Calculate distance using precomputed method
        float distance = 0.0f;
        if (!localOrigin.IsZero()) {
          distance = localOrigin.Distance(entityOrigin) / 16.0f;
        }

        EntityInfo entity{};
        entity.pawn = pawn;
        entity.health = health;
        entity.team = team;
        entity.origin = entityOrigin;
        entity.lastUpdate = currentTime;
        entity.name = std::move(playerName);  // Use move semantics
        entity.distance = distance;

        // Bone handling
        bool shouldUpdateBones = ShouldUpdateBones(pawn, currentTime);
        if (shouldUpdateBones) {
          auto bones = ReadBones(pawn, entityOrigin);
          entity.bones = bones;

          std::lock_guard<std::mutex> lock(dataMutex_);
          boneCache[pawn] = bones;
          boneUpdateTimes[pawn] = currentTime;
          bonePositions[pawn] = entityOrigin;
        } else {
          entity.bones = GetInterpolatedBones(pawn, currentTime);
        }

        // Set head position
        entity.head = !entity.bones[6].location.IsZero() &&
                              entity.bones[6].location.IsValid()
                          ? entity.bones[6].location
                          : entityOrigin + Vector3{0, 0, 72.0f};

        newData.entities.push_back(std::move(entity));  //  move semantics
      }

      newData.valid = true;
      lastValidViewMatrix =
          newData.viewMatrix;  // UPDATE: Cache the good matrix

    } catch (...) {
      // On error, use last valid view matrix
      newData.viewMatrix = lastValidViewMatrix;
      newData.valid = false;
    }

    // Update game data
    {
      std::lock_guard<std::mutex> lock(dataMutex_);
      gameData_ = std::move(newData);  // move semantics
    }

    // Precise sleep timing
    auto now = system_clock::now();
    auto elapsed = duration_cast<milliseconds>(now - lastFrameTime);
    if (elapsed < std::chrono::milliseconds(4)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(2) - elapsed);
    }
    lastFrameTime = now;
  }
}