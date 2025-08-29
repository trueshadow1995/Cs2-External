#pragma once
#include "../Headers/Globals.h"
#include "../Headers/Math.h"
#include "../ImGui/imgui.h"

// Forward declaration
struct GameData;

namespace HealthBarHelper {
void RenderHealth(const GameData& gameData, ImDrawList* drawList);
}  // namespace HealthBarHelper