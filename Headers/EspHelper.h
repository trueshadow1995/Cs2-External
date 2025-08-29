#pragma once
#include "../Headers/Globals.h"
#include "../Headers/Math.h"
#include "../ImGui/imgui.h"

// Forward declarations
class Memory;
struct EntityInfo;
struct ViewMatrix_t;

namespace ESPHelper {

void RenderEsp(Memory& mem, uintptr_t client, const EntityInfo& entity,
            uintptr_t localPlayerPawn, int localTeam,
            const ViewMatrix_t& viewMatrix, ImDrawList* drawList);

}  // namespace ESPHelper