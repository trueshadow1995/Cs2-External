#pragma once
#include "../Headers/Memory.h"
#include "../ImGui/imgui.h"

// Forward declarations
struct DataManager;
struct GameData;

namespace RenderManager {
  void RenderFrame(Memory& mem, uintptr_t client, DataManager& entityManager,
                 ImDrawList* backgroundDraw);
}
