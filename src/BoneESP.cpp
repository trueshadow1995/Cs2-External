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

  // Precompute thickness to avoid repeated global access
  const float thickness = globals::BoneEspThickness;

  // Draw bone connections
  for (size_t i = 0; i < BoneConnectionsCount; ++i) {
    const BoneConnection& connection = BoneConnections[i];
    const int b1 = connection.bone1;
    const int b2 = connection.bone2;

    if (b1 < 0 || b1 >= globals::MAX_BONES || b2 < 0 ||
        b2 >= globals::MAX_BONES)
      continue;

    // Use references to avoid repeated array lookups
    const CBoneData& bone1 = bones[b1];
    const CBoneData& bone2 = bones[b2];

    if (bone1.location.IsZero() || bone2.location.IsZero()) continue;

    Vector3 screenFrom, screenTo;
    if (bone1.location.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                     screenFrom) &&
        bone2.location.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                     screenTo) &&
        screenFrom.z > 0 && screenTo.z > 0) {
      drawList->AddLine(ImVec2(screenFrom.x, screenFrom.y),
                        ImVec2(screenTo.x, screenTo.y), lineColor, thickness);
    }
  }

  // Debug indices
  if (globals::BoneDebug) {
    char boneNum[8];  // Move outside loop to avoid repeated allocation

    for (int i = 0; i < globals::MAX_BONES; i++) {
      const CBoneData& bone = bones[i];  // Use reference

      if (!bone.location.IsZero()) {
        Vector3 screenPos;
        if (bone.location.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                        screenPos) &&
            screenPos.z > 0) {
          snprintf(boneNum, sizeof(boneNum), "%d", i);
          drawList->AddText(ImVec2(screenPos.x + 5, screenPos.y - 5),
                            IM_COL32(255, 255, 255, 255), boneNum);
        }
      }
    }
  }
}
}  // namespace BoneEsp