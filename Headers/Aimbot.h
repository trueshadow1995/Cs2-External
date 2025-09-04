#pragma once
#include <windows.h>

#include <vector>

#include "../Headers/EntityDataManager.h"
#include "../Headers/Globals.h"
#include "../Headers/Memory.h"

class Aimbot {
 private:
  Memory& mem;
  uintptr_t client;
  std::vector<Vector3> playerPositions;

  float manual_min(float a, float b);

  float manual_max(float a, float b);

  Vector3 findClosest(const GameData& gameData);
  void MoveMouseToPlayer(Vector3 target);
  bool IsValidTarget(const EntityInfo& entity, const GameData& gameData) const;
  Vector3 GetAimPosition(const EntityInfo& entity) const;
  float CalculateDistance(const Vector3& localPos,
                          const Vector3& enemyPos) const;

 public:
  Aimbot(Memory& memory, uintptr_t clientBase);
  void doAimbot(const GameData& gameData);
};