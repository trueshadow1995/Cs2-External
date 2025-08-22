#pragma once

#include <d3d11.h>

#include "../Headers/Globals.h"
#include "../Headers/Math.h"
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"
#include "../ImGui/imgui.h"
#include "../Headers/Bones.h"
#include "array"

namespace BoneHelper {
inline void RenderBones(Memory& mem, uintptr_t pPawn, int team, int localTeam,
                        const ViewMatrix_t& view_matrix,
                        ImDrawList* backgrounddraw) {
  if (!pPawn) return;

  uintptr_t gameSceneNode =
      mem.Read<uintptr_t>(pPawn + offsets::m_pGameSceneNode);
  if (!gameSceneNode) {
    printf("[-] Could not find GameSceneNode\n");
  }

  uintptr_t boneArrayPtr =
      mem.Read<uintptr_t>(gameSceneNode + offsets::m_modelState + 0x80);

  std::array<CBoneData, 64> bones =
      mem.Read<std::array<CBoneData, 64>>(boneArrayPtr);

  bool isTeammate = (team == localTeam);

  // Debug bone indices
  if (globals::BoneDebug) {
    for (size_t i = 0; i < bones.size(); ++i) {
      Vector3 world = bones[i].location;
      Vector3 screen = world.WorldToScreen(view_matrix);

      if (screen.z < 0.01f) continue;  // off-screen
      ImGui::GetBackgroundDrawList()->AddText(ImVec2(screen.x, screen.y),
                                              IM_COL32(255, 255, 255, 255),
                                              std::to_string(i).c_str());
    }
  }

  if ((isTeammate && globals::FriendlyBones) ||
      (!isTeammate && globals::EnemyBones)) {
    for (size_t i = 0; i < sizeof(BoneConnections) / sizeof(BoneConnections[0]);
         ++i) {
      int b1 = BoneConnections[i].bone1;
      int b2 = BoneConnections[i].bone2;

      if (b1 < 0 || b1 >= (int)bones.size()) continue;
      if (b2 < 0 || b2 >= (int)bones.size()) continue;

      Vector3 world1 = bones[b1].location;
      Vector3 world2 = bones[b2].location;

      Vector3 screen1 = world1.WorldToScreen(view_matrix);
      Vector3 screen2 = world2.WorldToScreen(view_matrix);

      if (screen1.z < 0.01f || screen2.z < 0.01f) continue;

      ImColor lineColor =
          isTeammate
              ? ImColor(globals::TeammateBoneColor[0],
                        globals::TeammateBoneColor[1],
                        globals::TeammateBoneColor[2],
                        globals::TeammateBoneColor[3])
              : ImColor(globals::EnemyBoneColor[0], globals::EnemyBoneColor[1],
                        globals::EnemyBoneColor[2], globals::EnemyBoneColor[3]);

      if (backgrounddraw)
        backgrounddraw->AddLine(ImVec2(screen1.x, screen1.y),
                                ImVec2(screen2.x, screen2.y), lineColor,
                                globals::BoneEspThickness);
    }
  }
}
}  // namespace BoneHelper