#pragma once

#include <d3d11.h>

#include <array>

#include "../Headers/Bones.h"
#include "../Headers/Globals.h"
#include "../Headers/Math.h"
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"
#include "../ImGui/imgui.h"

namespace BoneHelper {

inline void RenderBones(Memory& mem, uintptr_t pPawn, uintptr_t localPlayerpawn,
                        int team, int localTeam,
                        const ViewMatrix_t& view_matrix,
                        ImDrawList* backgrounddraw) {
  if (!pPawn || !backgrounddraw || pPawn == localPlayerpawn) return;

  const int health = mem.Read<int>(pPawn + offsets::m_iHealth);
  if (health <= 0 || health > 100) return;

  const uintptr_t gameSceneNode =
      mem.Read<uintptr_t>(pPawn + offsets::m_pGameSceneNode);
  if (!gameSceneNode) return;

  const uintptr_t boneArrayPtr =
      mem.Read<uintptr_t>(gameSceneNode + offsets::m_modelState + 0x80);
  if (!boneArrayPtr) return;

  // Read bones individually
  std::array<CBoneData, 64> bones;
  for (size_t i = 0; i < bones.size(); ++i) {
    bones[i] = mem.Read<CBoneData>(boneArrayPtr + i * sizeof(CBoneData));
  }

  const bool isTeammate = (team == localTeam);

  // Precompute screen positions for all bones
  std::array<Vector3, 64> screenPositions;
  for (size_t i = 0; i < bones.size(); ++i) {
    screenPositions[i] = bones[i].location.WorldToScreen(view_matrix);
  }

  // Debug bone indices
  if (globals::BoneDebug) {
    static char buf[4];
    for (size_t i = 0; i < screenPositions.size(); ++i) {
      const Vector3& screen = screenPositions[i];
      if (screen.z < 0.01f) continue;
      snprintf(buf, sizeof(buf), "%zu", i);
      backgrounddraw->AddText(ImVec2(screen.x, screen.y),
                              IM_COL32(255, 255, 255, 255), buf);
    }
  }

  // Skip if drawing not enabled
  if ((isTeammate && !globals::FriendlyBones) ||
      (!isTeammate && !globals::EnemyBones))
    return;

  // Draw bone connections using precomputed screen positions
  for (size_t i = 0; i < sizeof(BoneConnections) / sizeof(BoneConnections[0]);
       ++i) {
    const int b1 = BoneConnections[i].bone1;
    const int b2 = BoneConnections[i].bone2;
    if (b1 < 0 || b1 >= (int)bones.size() || b2 < 0 || b2 >= (int)bones.size())
      continue;

    const Vector3& screen1 = screenPositions[b1];
    const Vector3& screen2 = screenPositions[b2];
    if (screen1.z < 0.01f || screen2.z < 0.01f) continue;

    const ImColor lineColor =
        isTeammate
            ? ImColor(
                  globals::TeammateBoneColor[0], globals::TeammateBoneColor[1],
                  globals::TeammateBoneColor[2], globals::TeammateBoneColor[3])
            : ImColor(globals::EnemyBoneColor[0], globals::EnemyBoneColor[1],
                      globals::EnemyBoneColor[2], globals::EnemyBoneColor[3]);

    backgrounddraw->AddLine(ImVec2(screen1.x, screen1.y),
                            ImVec2(screen2.x, screen2.y), lineColor,
                            globals::BoneEspThickness);
  }
}

}  // namespace BoneHelper
