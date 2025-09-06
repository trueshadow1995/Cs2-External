#include <cmath>

#include "../../ImGui/imgui_internal.h"
#include "../Headers/FovVisuals.h"

namespace FovVisualizer {
// Remove the local ShowCrosshair variable since we're using globals::CrossHair
// bool ShowCrosshair = true;  // Remove this line

void DrawCrosshair(ImDrawList* drawList, float screenWidth,
                   float screenHeight) {
  if (!drawList || !globals::CrossHair)
    return;  // Use globals::CrossHair instead of ShowCrosshair

  ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);

  // Clean, minimal crosshair design
  const float size = 8.0f;
  const float gap = 3.0f;
  const float thickness = 1.5f;
  const ImU32 color = ImColor(255, 255, 255, 220);

  // Draw crosshair lines with gap in the center
  drawList->AddLine(ImVec2(screenCenter.x - size - gap, screenCenter.y),
                    ImVec2(screenCenter.x - gap, screenCenter.y), color,
                    thickness);
  drawList->AddLine(ImVec2(screenCenter.x + gap, screenCenter.y),
                    ImVec2(screenCenter.x + size + gap, screenCenter.y), color,
                    thickness);
  drawList->AddLine(ImVec2(screenCenter.x, screenCenter.y - size - gap),
                    ImVec2(screenCenter.x, screenCenter.y - gap), color,
                    thickness);
  drawList->AddLine(ImVec2(screenCenter.x, screenCenter.y + gap),
                    ImVec2(screenCenter.x, screenCenter.y + size + gap), color,
                    thickness);

  // Small center dot
  drawList->AddCircleFilled(screenCenter, 1.5f, color, 8);
}
void DrawAnimatedFovCircle(ImDrawList* drawList, float screenWidth,
                           float screenHeight) {
  if (!drawList || !globals::FovCircle || globals::AimbotFovSize <= 0) {
    return;
  }

  static float animationTime = 0.0f;
  animationTime += 0.002f;  // Fixed: Much slower animation speed (was 1.01f)

  ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);
  const float radius = globals::AimbotFovSize;

  // Base outline
  drawList->AddCircle(screenCenter, radius, ImColor(0, 0, 0, 150), 64, 2.0f);

  // Draw the animated circle with better performance
  const int segments = 64;
  const float segmentAngle = 2.0f * IM_PI / segments;

  // Create a smooth gradient from dark blue to light blue
  for (int i = 0; i < segments; i++) {
    float angle1 = i * segmentAngle;
    float angle2 = (i + 1) * segmentAngle;

    // Calculate color based on position in circle and animation
    float t = 0.5f + 0.5f * sinf(animationTime + angle1);
    ImU32 color = ImColor(0,                                // R
                          100 + static_cast<int>(155 * t),  // G (100-255)
                          150 + static_cast<int>(105 * t),  // B (150-255)
                          200                               // A
    );

    // Draw segment
    drawList->PathArcTo(screenCenter, radius, angle1, angle2, 8);
    drawList->PathStroke(color, false, 3.0f);
  }

  // Pulsing center dot
  float pulse = 1.5f + sinf(animationTime * 2.0f) * 0.5f;
  drawList->AddCircleFilled(screenCenter, pulse, ImColor(255, 255, 255, 255),
                            12);

}

void DrawFovCircle(ImDrawList* drawList, float screenWidth,
                   float screenHeight) {
  if (!drawList || !globals::FovCircle || globals::AimbotFovSize <= 0) {
    return;
  }

  ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);

  // Simple, clean FOV circle
  drawList->AddCircle(screenCenter, globals::AimbotFovSize + 1.0f,
                      ImColor(0, 0, 0, 150), 64, 2.0f);
  drawList->AddCircle(screenCenter, globals::AimbotFovSize,
                      ImColor(0, 150, 255, 120), 64, 2.0f);
  drawList->AddCircleFilled(screenCenter, 2.0f, ImColor(255, 255, 255, 255),
                            12);
}

void DrawFovCircleAdvanced(ImDrawList* drawList, float screenWidth,
                           float screenHeight, bool hasTargetsInFov) {
  if (!drawList || !globals::FovCircle || globals::AimbotFovSize <= 0) {
    return;
  }

  ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);

  // Dynamic color based on targets - green when enemies detected
  static ImVec4 currentColor = ImVec4(0.0f, 0.4f, 0.8f, 0.8f);  // Blue
  ImVec4 targetColor =
      hasTargetsInFov ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
                      :                      // Green when targets available
          ImVec4(255.0f, 0.0f, 0.0f, 0.8f);  // Blue when no targets

  // Smooth color transition
  currentColor.x += (targetColor.x - currentColor.x) * 0.1f;
  currentColor.y += (targetColor.y - currentColor.y) * 0.1f;
  currentColor.z += (targetColor.z - currentColor.z) * 0.1f;

  // Draw the circle with better visibility
  drawList->AddCircle(screenCenter, globals::AimbotFovSize + 1.0f,
                      ImColor(0, 0, 0, 150), 64, 2.0f);
  drawList->AddCircle(
      screenCenter, globals::AimbotFovSize,
      ImColor(currentColor.x, currentColor.y, currentColor.z, currentColor.w),
      64, 2.0f);
}

}  // namespace FovVisualizer