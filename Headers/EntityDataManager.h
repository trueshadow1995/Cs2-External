#pragma once
#include <array>
#include <cstdint>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../Headers/BoneMatrix.h"
#include "../Headers/Globals.h"
#include "../Headers/Math.h"
#include "../Headers/Memory.h"
#include "BoneGlobal.h"

struct EntityInfo {
  uintptr_t pawn = 0;
  int health = 0;
  int team = 0;
  Vector3 origin{};
  Vector3 head{};
  std::array<CBoneData, globals::MAX_BONES> bones{};
  uint64_t lastUpdate = 0;
};

struct GameData {
  bool valid = false;
  uintptr_t localPlayerPawn = 0;
  int localTeam = 0;
  ViewMatrix_t viewMatrix{};
  std::vector<EntityInfo> entities{};
};

class DataManager {
 public:
  DataManager(Memory& mem, uintptr_t client);
  ~DataManager();

  DataManager(const DataManager&) = delete;
  DataManager& operator=(const DataManager&) = delete;

  GameData GetGameData();
  void StartUpdateThread();
  void StopUpdateThread();

 private:
  std::array<CBoneData, globals::MAX_BONES> ReadBones(
      uint64_t pawn, const Vector3& entityOrigin);
  std::array<CBoneData, globals::MAX_BONES> GetInterpolatedBones(
      uint64_t pawn, uint64_t currentTime);
  uintptr_t GetEntityPawn(int index, uintptr_t entityList);
  bool ShouldUpdateBones(uint64_t pawn, uint64_t currentTime);
  void UpdateLoop();

  Memory& mem_;
  uintptr_t client_ = 0;
  std::thread updateThread_;
  bool running_ = false;
  std::mutex dataMutex_;
  GameData gameData_;
  std::unordered_map<uint64_t, std::array<CBoneData, globals::MAX_BONES>>
      smoothedBoneCache;


  // Caching
  std::unordered_map<uint64_t, uint64_t> boneUpdateTimes;
  std::unordered_map<uint64_t, std::array<CBoneData, globals::MAX_BONES>>
      boneCache;
  std::unordered_map<uint64_t, Vector3> bonePositions;

  // Interpolation
  std::unordered_map<uint64_t, std::array<CBoneData, globals::MAX_BONES>>
      previousBoneCache;
  std::unordered_map<uint64_t, uint64_t> interpolationStartTimes;
};