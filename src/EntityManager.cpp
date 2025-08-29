#include <algorithm>
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

constexpr std::array<int, 32> ESSENTIAL_BONES = {
    // Core skeleton
    6, 5, 4, 3, 2, 0,  // Head to pelvis
    8, 9, 10,          // Left arm
    13, 14, 15,        // Right arm
    22, 23, 30,    // Left leg
    25, 26, 32,   // Right leg
   
    11, 12, 16, 17, 38, 39, 40, 66, 7, 18, 19, 37}; // dick bone someday ? 

//  cache duration  16ms
constexpr uint64_t BONE_CACHE_DURATION = 14;

GameData DataManager::GetGameData() {
  std::lock_guard<std::mutex> lock(dataMutex_);
  return gameData_;
}

DataManager::DataManager(Memory& mem, uintptr_t client)
    : mem_(mem), client_(client), running_(false) {
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
    // Store previous bones for interpolation
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

    // Get CGameSceneNode
    uintptr_t gameSceneNode =
        mem_.Read<uintptr_t>(pawn + offsets::m_pGameSceneNode);
    if (!gameSceneNode) return bones;

    // Read bone array pointer
    const uintptr_t boneArrayPtr =
        mem_.Read<uintptr_t>(gameSceneNode + offsets::m_modelState + 0x80);
    if (!boneArrayPtr) return bones;

    // Use ReadBatch  all essential bones at once
    std::vector<uintptr_t> boneAddresses;
    boneAddresses.reserve(ESSENTIAL_BONES.size());

    for (int boneId : ESSENTIAL_BONES) {
      boneAddresses.push_back(boneArrayPtr + boneId * sizeof(CBoneData));
    }

    // Batch read all bones
    auto boneData = mem_.ReadBatch<CBoneData>(boneAddresses);

    // Process the batch read results
    for (size_t i = 0; i < ESSENTIAL_BONES.size(); i++) {
      if (i < boneData.size() && !boneData[i].location.IsZero() &&
          !std::isnan(boneData[i].location.x)) {
        bones[ESSENTIAL_BONES[i]] = boneData[i];
      }
    }

  } catch (...) {
    // error handling once again 
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

  // MUCH FASTER interpolation - reduced from 5.0f to 2.0f
  float interpolationTime = 2.0f;
  float elapsed = static_cast<float>(currentTime - startTime);
  float alpha = std::clamp(elapsed / interpolationTime, 0.0f, 1.0f);

  // Linear interpolation for faster response (removed complex easing)
  // alpha = alpha * alpha * alpha * (alpha * (alpha * 6 - 15) + 10);

  // Interpolate with minimal smoothing for faster response
  for (int i = 0; i < globals::MAX_BONES; i++) {
    if (!currentBones[i].location.IsZero()) {
      if (!previousBones[i].location.IsZero()) {
        // Fast linear interpolation
        interpolatedBones[i].location = Vector3::Lerp(
            previousBones[i].location, currentBones[i].location, alpha);
      } else {
        interpolatedBones[i].location = currentBones[i].location;
      }
    }
  }

  return interpolatedBones;
}

uintptr_t DataManager::GetEntityPawn(int index, uintptr_t entityList) {
  try {
    uintptr_t listEntry =
        mem_.Read<uintptr_t>(entityList + (0x8 * (index >> 9)) + 0x10);
    if (!listEntry) return 0;

    uintptr_t controller =
        mem_.Read<uintptr_t>(listEntry + (0x78 * (index & 0x1FF)));
    if (!controller) return 0;

    uint32_t pawnHandle = mem_.Read<uint32_t>(controller + offsets::m_hPawn);
    if (!pawnHandle) return 0;

    uintptr_t listEntry2 = mem_.Read<uintptr_t>(
        entityList + (0x8 * ((pawnHandle & 0x7FFF) >> 9)) + 0x10);
    if (!listEntry2) return 0;

    return mem_.Read<uintptr_t>(listEntry2 + (0x78 * (pawnHandle & 0x1FF)));
  } catch (...) {
    return 0;
  }
}

void DataManager::UpdateLoop() {
  while (running_) {
    GameData newData{};
    uint64_t currentTime =
        duration_cast<milliseconds>(system_clock::now().time_since_epoch()) // when corbi was born
            .count();

    try {
      // Read local player
      uintptr_t localPawn =
          mem_.Read<uintptr_t>(client_ + offsets::LocalPlayerPawn);
      if (!localPawn) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(8));  // Faster sleep
        continue;
      }

      Vector3 localOrigin =
          mem_.Read<Vector3>(localPawn + offsets::m_vOldOrigin);
      newData.viewMatrix =
          mem_.Read<ViewMatrix_t>(client_ + offsets::ViewMatrix);
      newData.localTeam = mem_.Read<int>(localPawn + offsets::m_iTeamNum);
      newData.localPlayerPawn = localPawn;

      // Entity list
      uintptr_t entityList =
          mem_.Read<uintptr_t>(client_ + offsets::EntityList);
      if (!entityList) continue;

      // Process entities 
      for (int i = 0; i < 64; i++) {
        uintptr_t pawn = GetEntityPawn(i, entityList);
        if (!pawn || pawn == localPawn) continue;

        int health = mem_.Read<int>(pawn + offsets::m_iHealth);
        if (health <= 0 || health > 100) continue;

        int team = mem_.Read<int>(pawn + offsets::m_iTeamNum);
        Vector3 entityOrigin = mem_.Read<Vector3>(pawn + offsets::m_vOldOrigin);

        EntityInfo entity{};
        entity.pawn = pawn;
        entity.health = health;
        entity.team = team;
        entity.origin = entityOrigin;
        entity.lastUpdate = currentTime;

        // Always update bones when needed
        bool shouldUpdateBones = ShouldUpdateBones(pawn, currentTime);

        if (shouldUpdateBones) {
          // Read fresh bones using batch reading
          auto bones = ReadBones(pawn, entityOrigin);
          entity.bones = bones;

          // Update cache
          std::lock_guard<std::mutex> lock(dataMutex_);
          boneCache[pawn] = bones;
          boneUpdateTimes[pawn] = currentTime;
          bonePositions[pawn] = entityOrigin;
        } else {
          // Use interpolated bones from cache
          entity.bones = GetInterpolatedBones(pawn, currentTime);
        }

        // Set head position
        if (!entity.bones[6].location.IsZero()) {
          entity.head = entity.bones[6].location;
        } else {
          entity.head = entityOrigin + Vector3{0, 0, 72.0f};
        }

        newData.entities.push_back(entity);
      }

      newData.valid = true;

    } catch (...) {
      //error handling apparently 
    }

    // Update game data
    {
      std::lock_guard<std::mutex> lock(dataMutex_);
      gameData_ = newData;
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(4));  // fuck around with the sleep to make it update quicker 
  }
}