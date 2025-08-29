#pragma once
#include <array>

#include "../Headers/BoneGlobal.h"
#include "../Headers/BoneMatrix.h"
#include "../Headers/Globals.h"
#include "../Headers/Math.h"
#include "../ImGui/imgui.h"

namespace BoneEsp {
void RenderBones(const std::array<CBoneData, globals::MAX_BONES>& bones,
                 const ViewMatrix_t& viewMatrix, float screenWidth,
                 float screenHeight, ImDrawList* drawList, bool isTeammate);
}  // namespace BoneEsp