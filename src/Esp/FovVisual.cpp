#pragma once
#include "../Headers/Globals.h"
#include "../Headers/FovVisuals.h"

namespace FovVisualizer {
void DrawFovCircle(ImDrawList* drawList, float screenWidth,
                   float screenHeight) {
  if (!drawList || !globals::Aimbot || globals::AimbotFovSize <= 0) {
    return;
  }

  ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);

  // Outer outline
  drawList->AddCircle(screenCenter, globals::AimbotFovSize + 1.0f,
                      ImColor(0, 0, 0, 150), 64, 2.0f);

  // Inner circle
  drawList->AddCircle(screenCenter, globals::AimbotFovSize,
                      ImColor(0, 255, 0, 80), 64, 1.5f);

  // Center dot
  drawList->AddCircleFilled(screenCenter, 2.0f, ImColor(255, 0, 0, 255), 12);
}

void DrawFovCircleAdvanced(ImDrawList* drawList, float screenWidth,
                           float screenHeight, bool hasTargetsInFov) {
  if (!drawList || !globals::Aimbot || globals::AimbotFovSize <= 0) {
    return;
  }

  ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);

  // Dynamic color based on targets
  ImU32 circleColor;
  if (hasTargetsInFov) {
    circleColor = ImColor(0, 255, 0, 100);  // Green when targets available
  } else {
    circleColor = ImColor(255, 0, 0, 100);  // Red when no targets
  }

  // Main FOV circle
  drawList->AddCircle(screenCenter, globals::AimbotFovSize, circleColor, 64,
                      1.5f);

  // Crosshair
  float crosshairSize = 6.0f;
  drawList->AddLine(ImVec2(screenCenter.x - crosshairSize, screenCenter.y),
                    ImVec2(screenCenter.x + crosshairSize, screenCenter.y),
                    ImColor(255, 255, 255, 200), 1.2f);
  drawList->AddLine(ImVec2(screenCenter.x, screenCenter.y - crosshairSize),
                    ImVec2(screenCenter.x, screenCenter.y + crosshairSize),
                    ImColor(255, 255, 255, 200), 1.2f);
}
}  // namespace FovVisualizer