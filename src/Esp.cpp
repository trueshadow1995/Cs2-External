#pragma once
#include "../Headers/ESPHelper.h"
#include "../Headers/EntityDataManager.h"
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"
#include "../ImGui/imgui.h"

namespace ESPHelper {

const ImColor SHADOW_COLOR(0, 0, 0, 120);
const float HEAD_SIZE = 4.0f;
const float BOX_THICKNESS = 1.5f;
const float EXTRA_HEAD_HEIGHT_FACTOR = 0.15f;
const float WIDTH_HEIGHT_RATIO = 2.4f;

void RenderEsp(Memory& mem, uintptr_t client, const EntityInfo& entity,
               uintptr_t localPlayerPawn, int localTeam,
               const ViewMatrix_t& viewMatrix, ImDrawList* drawList) {
  if (!entity.pawn || !drawList || entity.health <= 0) return;

  const bool isTeammate = (entity.team == localTeam);
  if (isTeammate && !globals::TeammateEsp) return;
  if (!isTeammate && !globals::EnemyEsp) return;

  // Cache screen dimensions
  const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
  const float screenWidth = screenSize.x;
  const float screenHeight = screenSize.y;

  // Early exit for off-screen entities
  Vector3 screenFeet{}, screenHead{};
  if (!entity.origin.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                   screenFeet) ||
      !entity.head.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                 screenHead)) {
    return;
  }

  // Check if entity is fully off-screen
  if (screenFeet.x < -50.0f || screenFeet.x > screenWidth + 50.0f ||
      screenFeet.y < -50.0f || screenFeet.y > screenHeight + 50.0f ||
      screenHead.x < -50.0f || screenHead.x > screenWidth + 50.0f ||
      screenHead.y < -50.0f || screenHead.y > screenHeight + 50.0f) {
    return;
  }

  // Precompute box dimensions
  const float height = screenFeet.y - screenHead.y;
  const float width = height / WIDTH_HEIGHT_RATIO;
  const float extraHeadHeight = height * EXTRA_HEAD_HEIGHT_FACTOR;
  const ImVec2 topLeft(screenFeet.x - width * 0.5f,
                       screenHead.y - extraHeadHeight);
  const ImVec2 bottomRight(screenFeet.x + width * 0.5f, screenFeet.y);

  // Cache colors
  const ImColor espColor =
      isTeammate
          ? ImColor(globals::TeammateEspColor[0], globals::TeammateEspColor[1],
                    globals::TeammateEspColor[2], globals::TeammateEspColor[3])
          : ImColor(globals::EnemyEspColor[0], globals::EnemyEspColor[1],
                    globals::EnemyEspColor[2], globals::EnemyEspColor[3]);
  const ImColor bgColor = isTeammate
                              ? ImColor(globals::FriendlyEspBackGroundColor[0],
                                        globals::FriendlyEspBackGroundColor[1],
                                        globals::FriendlyEspBackGroundColor[2],
                                        globals::FriendlyEspBackGroundColor[3])
                              : ImColor(globals::EnemyEspBackGroundColor[0],
                                        globals::EnemyEspBackGroundColor[1],
                                        globals::EnemyEspBackGroundColor[2],
                                        globals::EnemyEspBackGroundColor[3]);

  // Boxes
  if ((isTeammate && globals::TeammateEsp) ||
      (!isTeammate && globals::EnemyEsp)) {
    // Combined shadow and main box (slightly thicker for shadow effect)
    drawList->AddRect(topLeft, bottomRight, SHADOW_COLOR, 0.0f, 0,
                      BOX_THICKNESS + 0.5f);
    drawList->AddRect(topLeft, bottomRight, espColor, 0.0f, 0, BOX_THICKNESS);
  }

  // Background
  if ((isTeammate && globals::TeammateEspBackground) ||
      (!isTeammate && globals::EnemyEspBackground)) {
    drawList->AddRectFilled(topLeft, bottomRight, bgColor);
  }

  // Head Marker
  if (globals::Headmarker && ((isTeammate && globals::TeammateEsp) ||
                              (!isTeammate && globals::EnemyEsp))) {
    drawList->AddCircleFilled(ImVec2(screenHead.x, screenHead.y), HEAD_SIZE,
                              espColor);
  }


  // Name and Distance ESP (combined to reduce draw calls)
  if (((isTeammate && globals::TeammateNameEsp && globals::NameEsp) ||
       (!isTeammate && globals::EnemyNameEsp && globals::NameEsp) ||
       (isTeammate && globals::TeammateDistanceEsp && globals::DistanceEsp) ||
       (!isTeammate && globals::EnemyDistanceEsp && globals::DistanceEsp)) &&
      !entity.name.empty()) {
    // Precompute text
    std::string combinedText;
    if ((isTeammate && globals::TeammateNameEsp && globals::NameEsp) ||
        (!isTeammate && globals::EnemyNameEsp && globals::NameEsp)) {
      combinedText = entity.name;
    }
    if ((isTeammate && globals::TeammateDistanceEsp && globals::DistanceEsp) ||
        (!isTeammate && globals::EnemyDistanceEsp && globals::DistanceEsp)) {
      char distanceText[16];
      snprintf(distanceText, sizeof(distanceText), "%.0fm", entity.distance);
      if (!combinedText.empty()) combinedText += " ";
      combinedText += distanceText;
    }

    if (!combinedText.empty()) {
      // Center text horizontally
      ImVec2 textSize = ImGui::CalcTextSize(combinedText.c_str());
      float textX = screenFeet.x - textSize.x * 0.5f;
      float textY = topLeft.y - 15.0f;

      // Shadow and main text
      drawList->AddText(ImVec2(textX + 1.0f, textY + 1.0f),
                        ImColor(0, 0, 0, 200), combinedText.c_str());
      drawList->AddText(ImVec2(textX, textY), espColor, combinedText.c_str());
    }
  }
}

}  // namespace ESPHelper