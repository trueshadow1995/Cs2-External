#pragma once
#include <windows.h>

#include <atomic>
#include <thread>

#include "../Headers/EntityDataManager.h"
#include "../Headers/Globals.h"

class Aimbot {
 private:
  DataManager& entityManager;
  std::atomic<bool> running{false};
  std::thread aimbotThread;

  // Screen dimensions
  float screenWidth;
  float screenHeight;
  Vector3 screenCenter;
  float fovSquared;

  // Helper methods
  Vector3 findClosestTarget(const GameData& gameData);
  void MoveMouseToPosition(const Vector3& position);
  bool IsValidTarget(const EntityInfo& entity, uintptr_t localPawn,
                     int localTeam) const;
  void UpdateScreenDimensions();
  void AimbotThread();

 public:
  Aimbot(DataManager& entityManager);
  ~Aimbot();
  void Start();
  void Stop();
};