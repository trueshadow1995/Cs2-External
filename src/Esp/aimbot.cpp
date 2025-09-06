#include "../Headers/Aimbot.h"

#include <dwmapi.h>

#include <chrono>
#include <iostream>

// Helper functions
namespace AimbotUtils {
inline float clamp(float value, float minVal, float maxVal) {
  return (value < minVal) ? minVal : (value > maxVal) ? maxVal : value;
}

inline float distanceSquared(float x1, float y1, float x2, float y2) {
  float dx = x2 - x1;
  float dy = y2 - y1;
  return dx * dx + dy * dy;
}
}  // namespace AimbotUtils

Aimbot::Aimbot(DataManager& entityManager) : entityManager(entityManager) {
  UpdateScreenDimensions();
  fovSquared = globals::AimbotFovSize * globals::AimbotFovSize;
}

Aimbot::~Aimbot() { Stop(); }

void Aimbot::UpdateScreenDimensions() {
  // Get the game window dimensions instead of system metrics
  HWND gameWindow = FindWindowA(nullptr, "Counter-Strike 2");
  if (gameWindow) {
    RECT rect;
    if (GetWindowRect(gameWindow, &rect)) {
      screenWidth = static_cast<float>(rect.right - rect.left);
      screenHeight = static_cast<float>(rect.bottom - rect.top);
      screenCenter = Vector3(screenWidth / 2.0f, screenHeight / 2.0f, 0.0f);
      return;
    }
  }

  // Fallback to system metrics if game window not found
  screenWidth = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
  screenHeight = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));
  screenCenter = Vector3(screenWidth / 2.0f, screenHeight / 2.0f, 0.0f);
}

bool Aimbot::IsValidTarget(const EntityInfo& entity, uintptr_t localPawn,
                           int localTeam) const {
  return entity.pawn && entity.pawn != localPawn && entity.health > 0 &&
         entity.team != localTeam;
}

Vector3 Aimbot::findClosestTarget(const GameData& gameData) {
  if (!gameData.valid) return Vector3(0.0f, 0.0f, 0.0f);

  const ViewMatrix_t& viewMatrix = gameData.viewMatrix;
  const int localTeam = gameData.localTeam;
  const uintptr_t localPawn = gameData.localPlayerPawn;

  float closestDistance = FLT_MAX;
  Vector3 closestTarget(0.0f, 0.0f, 0.0f);

  // Update screen dimensions in case they changed
  UpdateScreenDimensions();

  // Find closest target to screen center
  for (const auto& entity : gameData.entities) {
    if (!IsValidTarget(entity, localPawn, localTeam)) continue;

    // Use the pre-calculated head position from entity data
    Vector3 screenHead;
    if (entity.head.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                  screenHead) &&
        screenHead.z > 0) {
      // Check if within FOV using squared distance for performance
      float distSq = AimbotUtils::distanceSquared(
          screenHead.x, screenHead.y, screenCenter.x, screenCenter.y);

      if (distSq <= fovSquared && distSq < closestDistance) {
        closestDistance = distSq;
        closestTarget = screenHead;
      }
    }
  }

  return closestTarget;
}

void Aimbot::MoveMouseToPosition(const Vector3& position) {
  if (position.IsZero()) return;

  // Calculate movement needed
  float dx = position.x - screenCenter.x;
  float dy = position.y - screenCenter.y;

  // Apply smoothing
  if (globals::AimbotUseSmoothing) {
    float smooth = AimbotUtils::clamp(globals::AimbotSmoothAmount, 0.01f, 1.0f);
    dx *= smooth;
    dy *= smooth;
  }

  // Apply mouse sensitivity
  dx *= globals::MouseSensitivity;
  dy *= globals::MouseSensitivity;

  // Prevent micro movements
  if (fabs(dx) < 0.1f && fabs(dy) < 0.1f) return;

  // Use mouse_event for movement
  mouse_event(MOUSEEVENTF_MOVE, static_cast<int>(dx), static_cast<int>(dy), 0,
              0);
}

void Aimbot::AimbotThread() {
  while (running) {
    // Sleep to reduce CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if (!globals::Aimbot || !(GetAsyncKeyState(globals::AimbotKey) & 0x8000)) {
      continue;
    }

    // Update FOV squared in case it changed
    fovSquared = globals::AimbotFovSize * globals::AimbotFovSize;

    // Get game data
    auto gameData = entityManager.GetGameData();
    if (!gameData.valid) continue;

    // Find and aim at closest target
    Vector3 target = findClosestTarget(gameData);
    if (!target.IsZero()) {
      MoveMouseToPosition(target);
    }
  }
}

void Aimbot::Start() {
  if (running) return;

  running = true;
  aimbotThread = std::thread(&Aimbot::AimbotThread, this);

  // Set thread priority to below normal to reduce impact on game performance
  SetThreadPriority(aimbotThread.native_handle(), THREAD_PRIORITY_BELOW_NORMAL);
}

void Aimbot::Stop() {
  running = false;
  if (aimbotThread.joinable()) {
    aimbotThread.join();
  }
}