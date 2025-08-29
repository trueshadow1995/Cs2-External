#include "../Headers/BoneHelper.h"
#include "../Headers/Globals.h"

namespace BoneEsp {
void RenderBones(const std::array<CBoneData, globals::MAX_BONES>& bones,
                 const ViewMatrix_t& viewMatrix, float screenWidth,
                 float screenHeight, ImDrawList* drawList, bool isTeammate) {
  if (!drawList) return;

  if ((isTeammate && !globals::FriendlyBones) ||
      (!isTeammate && !globals::EnemyBones))
    return;

  const ImColor lineColor =
      isTeammate
          ? ImColor(
                globals::TeammateBoneColor[0], globals::TeammateBoneColor[1],
                globals::TeammateBoneColor[2], globals::TeammateBoneColor[3])
          : ImColor(globals::EnemyBoneColor[0], globals::EnemyBoneColor[1],
                    globals::EnemyBoneColor[2], globals::EnemyBoneColor[3]);

  // Draw bone connections
  for (size_t i = 0; i < BoneConnectionsCount; ++i) {
    const BoneConnection& connection = BoneConnections[i];
    const int b1 = connection.bone1;
    const int b2 = connection.bone2;

    if (b1 < 0 || b1 >= (int)bones.size() || b2 < 0 || b2 >= (int)bones.size())
      continue;

    if (bones[b1].location.IsZero() || bones[b2].location.IsZero()) continue;

    Vector3 screenFrom, screenTo;
    if (bones[b1].location.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                         screenFrom) &&
        bones[b2].location.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                         screenTo) &&
        screenFrom.z > 0 && screenTo.z > 0) {
      drawList->AddLine(ImVec2(screenFrom.x, screenFrom.y),
                        ImVec2(screenTo.x, screenTo.y), lineColor,
                        globals::BoneEspThickness);
    }
  }

  // Debug indices
  if (globals::BoneDebug) {
    for (int i = 0; i < (int)bones.size(); i++) {
      if (!bones[i].location.IsZero()) {
        Vector3 screenPos;
        if (bones[i].location.WorldToScreen(viewMatrix, screenWidth,
                                            screenHeight, screenPos) &&
            screenPos.z > 0) {
          char boneNum[8];
          snprintf(boneNum, sizeof(boneNum), "%d", i);
          drawList->AddText(ImVec2(screenPos.x + 5, screenPos.y - 5),
                            IM_COL32(255, 255, 255, 255), boneNum);
        }
      }
    }
  }

}  
}  // namespace BoneEsp