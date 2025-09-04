
#include "../Headers/Aimbot.h"

#include <windows.h>

#include <cfloat>  // For FLT_MAX
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "../Headers/EntityDataManager.h"
#include "../Headers/Globals.h"
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"

float Aimbot::manual_min(float a, float b) { return (a < b) ? a : b; }

float Aimbot::manual_max(float a, float b) { return (a > b) ? a : b; }

Vector3 Aimbot::findClosest(const GameData& gameData) {
  if (playerPositions.empty()) {
    OutputDebugStringA("playerPositions vector was empty.\n");
    return {0.0f, 0.0f, 0.0f};
  }

  Vector3 center_of_screen{(float)GetSystemMetrics(SM_CXSCREEN) / 2,
                           (float)GetSystemMetrics(SM_CYSCREEN) / 2, 0.0f};
  float lowestDistanceSquared = FLT_MAX;
  int index = -1;

  float fovThreshold = globals::AimbotFovSize * globals::AimbotFovSize;
  if (fovThreshold <= 0.0f) fovThreshold = 100.0f;

  for (int i = 0; i < playerPositions.size(); ++i) {
    float dx = playerPositions[i].x - center_of_screen.x;
    float dy = playerPositions[i].y - center_of_screen.y;
    float distanceSquared = dx * dx + dy * dy;

    if (distanceSquared > fovThreshold) continue;

    if (distanceSquared < lowestDistanceSquared) {
      lowestDistanceSquared = distanceSquared;
      index = i;
    }
  }

  if (index != -1) {
    char buf[128];
    snprintf(buf, sizeof(buf),
             "Closest target at (%.1f, %.1f), distance: %.1f\n",
             playerPositions[index].x, playerPositions[index].y,
             std::sqrt(lowestDistanceSquared));
    OutputDebugStringA(buf);
    return playerPositions[index];
  }

  OutputDebugStringA("No valid closest target found within FOV.\n");
  return {0.0f, 0.0f, 0.0f};
}

void Aimbot::MoveMouseToPlayer(Vector3 target) {
  if (target.IsZero()) {
    OutputDebugStringA("Target is zero, skipping movement.\n");
    return;
  }

  Vector3 center_of_screen{(float)GetSystemMetrics(SM_CXSCREEN) / 2,
                           (float)GetSystemMetrics(SM_CYSCREEN) / 2, 0.0f};

  float dx = target.x - center_of_screen.x;
  float dy = target.y - center_of_screen.y;

  // DEBUG
  char debugBuf[128];
  snprintf(debugBuf, sizeof(debugBuf), "Raw delta: (%.1f, %.1f)\n", dx, dy);
  OutputDebugStringA(debugBuf);

  // COMPLETELY DIFFERENT APPROACH: Use angle-based movement
  // Convert pixel delta to angle delta (approximate)
  const float PIXELS_TO_ANGLE = 0.022f;  // Adjust this value

  float angleX = dx * PIXELS_TO_ANGLE;
  float angleY = dy * PIXELS_TO_ANGLE;

  // Convert angle to mouse movement (this is game-specific)
  // Try different inversion combinations

  // Option 1: Direct angle conversion
  int mouseDx = static_cast<int>(angleX * 10.0f);
  int mouseDy = static_cast<int>(angleY * 10.0f);

  // Option 2: Inverted
  // int mouseDx = static_cast<int>(-angleX * 10.0f);
  // int mouseDy = static_cast<int>(-angleY * 10.0f);

  char buf[128];
  snprintf(buf, sizeof(buf), "Angle movement: (%d, %d)\n", mouseDx, mouseDy);
  OutputDebugStringA(buf);

  mouse_event(MOUSEEVENTF_MOVE, mouseDx, mouseDy, 0, 0);
}

Aimbot::Aimbot(Memory& memory, uintptr_t clientBase)
    : mem(memory), client(clientBase) {
  playerPositions.reserve(64);
}

void Aimbot::doAimbot(const GameData& gameData) {
  if (!gameData.valid) {
    OutputDebugStringA("GameData is invalid.\n");
    return;
  }

  if (!globals::Aimbot) {
    return;
  }

  const ViewMatrix_t& viewMatrix = gameData.viewMatrix;
  if (viewMatrix[0][0] == 0.0f && viewMatrix[1][1] == 0.0f) {
    OutputDebugStringA("ViewMatrix is invalid.\n");
    return;
  }

  const int localTeam = gameData.localTeam;
  const uintptr_t localPawn = gameData.localPlayerPawn;
  const float screenWidth = (float)GetSystemMetrics(SM_CXSCREEN);
  const float screenHeight = (float)GetSystemMetrics(SM_CYSCREEN);

  playerPositions.clear();
  int validTargets = 0;

  for (const auto& entity : gameData.entities) {
    if (!entity.pawn || entity.pawn == localPawn || entity.health <= 0 ||
        entity.team == localTeam) {
      continue;
    }

    Vector3 screenHead;
    if (entity.head.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                  screenHead)) {
      if (screenHead.x >= 0 && screenHead.x <= screenWidth &&
          screenHead.y >= 0 && screenHead.y <= screenHeight) {
        playerPositions.push_back(screenHead);
        validTargets++;
      }
    }
  }

  char buf[128];
  snprintf(buf, sizeof(buf), "Found %d valid targets on screen.\n",
           validTargets);
  OutputDebugStringA(buf);

  if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) || globals::AimBotAutofire) {
    Vector3 closest = findClosest(gameData);
    if (!closest.IsZero()) {
      MoveMouseToPlayer(closest);
    }
  }
}