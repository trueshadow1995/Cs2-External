#pragma once
#include "../Headers/Globals.h"
#include "../ImGui/imgui.h"

namespace FovVisualizer {
// Crosshair and FOV circle visibility control
extern bool ShowCrosshair;

void ToggleCrosshair();
bool IsCrosshairVisible();

// Crosshair drawing function
void DrawCrosshair(ImDrawList* drawList, float screenWidth, float screenHeight);

// FOV circle drawing functions
void DrawFovCircle(ImDrawList* drawList, float screenWidth, float screenHeight);
void DrawFovCircleAdvanced(ImDrawList* drawList, float screenWidth,
                           float screenHeight, bool hasTargetsInFov);
void DrawAnimatedFovCircle(ImDrawList* drawList, float screenWidth,
                           float screenHeight);

}  // namespace FovVisualizer