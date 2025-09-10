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
const float CORNER_SIZE_RATIO =
    0.2f;  // Size of corner segments relative to box size

void RenderEsp(Memory& mem, uintptr_t client, const EntityInfo& entity,
               uintptr_t localPlayerPawn, int localTeam,
               const ViewMatrix_t& viewMatrix, ImDrawList* drawList) {
  if (!entity.pawn || !drawList || entity.health <= 0) return;

  const bool isTeammate = (entity.team == localTeam);

  // Check if ANY ESP feature is enabled for this entity type
  bool anyEspEnabled = false;

  // Check boxes (both normal and corner)
  if ((isTeammate && globals::TeammateEsp) ||
      (!isTeammate && globals::EnemyEsp)) {
    anyEspEnabled = true;
  }

  // Check names
  if (((isTeammate && globals::TeammateNameEsp) ||
       (!isTeammate && globals::EnemyNameEsp)) &&
      globals::NameEsp) {
    anyEspEnabled = true;
  }

  // Check distance
  if (((isTeammate && globals::TeammateDistanceEsp) ||
       (!isTeammate && globals::EnemyDistanceEsp)) &&
      globals::DistanceEsp) {
    anyEspEnabled = true;
  }

  // Check head marker
  if (globals::Headmarker) {
    anyEspEnabled = true;
  }

  // Check corner boxes (they work independently now)
  if (globals::CornerEspBoxStyle) {
    anyEspEnabled = true;
  }

  // If no ESP features are enabled for this entity, return early
  if (!anyEspEnabled) return;

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

  // Calculate center and dimensions for corner boxes
  const float centerX = screenFeet.x;
  const float centerY = (topLeft.y + bottomRight.y) / 2.0f;
  const float boxWidth = width;
  const float boxHeight = bottomRight.y - topLeft.y;

  // Cache colors
  const ImColor espColor =
      isTeammate
          ? ImColor(globals::TeammateEspColor[0], globals::TeammateEspColor[1],
                    globals::TeammateEspColor[2], globals::TeammateEspColor[3])
          : ImColor(globals::EnemyEspColor[0], globals::EnemyEspColor[1],
                    globals::EnemyEspColor[2], globals::EnemyEspColor[3]);

  const ImColor nameColor =
      ImColor(globals::PlayerNameColor[0], globals::PlayerNameColor[1],
              globals::PlayerNameColor[2], globals::PlayerNameColor[3]);
  const ImColor distanceColor =
      ImColor(globals::PlayerDistanceColor[0], globals::PlayerDistanceColor[1],
              globals::PlayerDistanceColor[2], globals::PlayerDistanceColor[3]);

  const ImColor bgColor = isTeammate
                              ? ImColor(globals::FriendlyEspBackGroundColor[0],
                                        globals::FriendlyEspBackGroundColor[1],
                                        globals::FriendlyEspBackGroundColor[2],
                                        globals::FriendlyEspBackGroundColor[3])
                              : ImColor(globals::EnemyEspBackGroundColor[0],
                                        globals::EnemyEspBackGroundColor[1],
                                        globals::EnemyEspBackGroundColor[2],
                                        globals::EnemyEspBackGroundColor[3]);

  // CORNER BOXES - Now work independently
  if (globals::CornerEspBoxStyle) {
    // Calculate corner positions
    const float left = centerX - boxWidth / 2;
    const float right = centerX + boxWidth / 2;
    const float top = centerY - boxHeight / 2;
    const float bottom = centerY + boxHeight / 2;
    const float cornerLength = boxWidth * CORNER_SIZE_RATIO;


    // Top-left corner
    drawList->AddLine(ImVec2(left, top), ImVec2(left + cornerLength, top),
                      espColor);
    drawList->AddLine(ImVec2(left, top), ImVec2(left, top + cornerLength),
                      espColor);

    // Top-right corner
    drawList->AddLine(ImVec2(right, top), ImVec2(right - cornerLength, top),
                      espColor);
    drawList->AddLine(ImVec2(right, top), ImVec2(right, top + cornerLength),
                      espColor);

    // Bottom-left corner
    drawList->AddLine(ImVec2(left, bottom), ImVec2(left + cornerLength, bottom),
                      espColor);
    drawList->AddLine(ImVec2(left, bottom), ImVec2(left, bottom - cornerLength),
                      espColor);

    // Bottom-right corner
    drawList->AddLine(ImVec2(right, bottom),
                      ImVec2(right - cornerLength, bottom),
                      espColor);
    drawList->AddLine(ImVec2(right, bottom),
                      ImVec2(right, bottom - cornerLength),
                      espColor);
  }
  // NORMAL BOXES - Only if corner boxes are NOT enabled
  else if ((isTeammate && globals::TeammateEsp) ||
           (!isTeammate && globals::EnemyEsp)) {
    // Regular box
    drawList->AddRect(topLeft, bottomRight, SHADOW_COLOR, 0.0f, 0,
                      BOX_THICKNESS + 0.5f);
    drawList->AddRect(topLeft, bottomRight, espColor, 0.0f, 0, BOX_THICKNESS);

    // Background (only if normal boxes are enabled)
    if ((isTeammate && globals::TeammateEspBackground) ||
        (!isTeammate && globals::EnemyEspBackground)) {
      drawList->AddRectFilled(topLeft, bottomRight, bgColor);
    }
  }

  // Head Marker
  if (globals::Headmarker) {
    // Use the actual head position from the entity data
    Vector3 headWorldPos = entity.head;

    // Convert to screen coordinates
    Vector3 screenHeadPos;
    if (headWorldPos.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                   screenHeadPos)) {
      // Use fixed screen-space size regardless of distance
      drawList->AddCircleFilled(
          ImVec2(screenHeadPos.x, screenHeadPos.y), globals::Headmarkersize,
          ImColor(globals::headMakerColor[0], globals::headMakerColor[1],
                  globals::headMakerColor[2], globals::headMakerColor[3]));
    }
  }

  // Name ESP (independent of boxes)
  if (((isTeammate && globals::TeammateNameEsp) ||
       (!isTeammate && globals::EnemyNameEsp)) &&
      globals::NameEsp && !entity.name.empty()) {
    // Center text horizontally
    ImVec2 textSize = ImGui::CalcTextSize(entity.name.c_str());
    float textX = screenFeet.x - textSize.x * 0.5f;
    float textY = topLeft.y - 15.0f;

    // Shadow and main text
    drawList->AddText(ImVec2(textX + 1.0f, textY + 1.0f), ImColor(0, 0, 0, 200),
                      entity.name.c_str());
    drawList->AddText(ImVec2(textX, textY), nameColor, entity.name.c_str());
  }

  // Distance ESP (independent of boxes)
  if (((isTeammate && globals::TeammateDistanceEsp) ||
       (!isTeammate && globals::EnemyDistanceEsp)) &&
      globals::DistanceEsp) {
    char distanceText[30];
    snprintf(distanceText, sizeof(distanceText), "%.0fm", entity.distance);

    // Center text horizontally
    ImVec2 textSize = ImGui::CalcTextSize(distanceText);
    float textX = screenFeet.x - textSize.x * 0.5f;

    // Position distance text below name if both are enabled, otherwise above
    // the box
    float textY;
    if (((isTeammate && globals::TeammateNameEsp) ||
         (!isTeammate && globals::EnemyNameEsp)) &&
        globals::NameEsp && !entity.name.empty()) {
      textY = topLeft.y - 27.0f;  // Position above the name
    } else {
      textY = topLeft.y - 15.0f;  // Position above the box
    }

    // Shadow and main text
    drawList->AddText(ImVec2(textX + 1.0f, textY + 1.0f), ImColor(0, 0, 0, 200),
                      distanceText);
    drawList->AddText(ImVec2(textX, textY), distanceColor, distanceText);
  }
}

}  // namespace ESPHelper