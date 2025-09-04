#pragma once
#include "../ImGui/imgui.h"

namespace FovVisualizer {
void DrawFovCircle(ImDrawList* drawList, float screenWidth, float screenHeight);
void DrawFovCircleAdvanced(ImDrawList* drawList, float screenWidth,
                           float screenHeight, bool hasTargetsInFov = false);
}  // namespace FovVisualizer