#pragma once
#include "../Headers/EntityDataManager.h"
#include "../Headers/Globals.h"
#include "../Headers/HealBarHelper.h"
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"
#include "../ImGui/imgui.h"

namespace HealthBarHelper {

void RenderHealth(const GameData& gameData, ImDrawList* drawList) {
  if (!drawList || !gameData.valid) return;

  const int localTeam = gameData.localTeam;
  const ViewMatrix_t& viewMatrix = gameData.viewMatrix;
  const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
  ImFont* smallFont = ImGui::GetIO().Fonts->Fonts.size() > 0
                          ? ImGui::GetIO().Fonts->Fonts[0]
                          : nullptr;
  const float screenWidth = screenSize.x;
  const float screenHeight = screenSize.y;

  // Pre-calculate constants - match ESP helper values
  const float healthBarWidth = 3.0f;
  const float healthBarPadding = 2.0f;
  const float fontSize = 12.0f;
  const float textOffsetX = 15.0f;
  const float EXTRA_HEAD_HEIGHT_FACTOR = 0.15f;  // Match ESP helper
  const float WIDTH_HEIGHT_RATIO = 2.4f;         // Match ESP helper

  for (const auto& entity : gameData.entities) {
    if (entity.health <= 0 || entity.health > 100) continue;

    const bool isTeammate = (entity.team == localTeam);

    // Health bars work independently - no need for main ESP toggle
    const bool drawBar = (isTeammate && globals::TeammateHealth) ||
                         (!isTeammate && globals::EnemyHealth);
    const bool drawText = (isTeammate && globals::TeammateHealthTxt) ||
                          (!isTeammate && globals::EnemyHealthText);

    if (!drawBar && !drawText) continue;

    // Early exit for off-screen entities
    Vector3 screenFeet{}, screenHead{};
    if (!entity.origin.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                     screenFeet) ||
        !entity.head.WorldToScreen(viewMatrix, screenWidth, screenHeight,
                                   screenHead) ||
        screenFeet.z <= 0.001f || screenHead.z <= 0.001f) {
      continue;
    }

    // Check if entity is fully off-screen
    if (screenFeet.x < -50.0f || screenFeet.x > screenWidth + 50.0f ||
        screenFeet.y < -50.0f || screenFeet.y > screenHeight + 50.0f ||
        screenHead.x < -50.0f || screenHead.x > screenWidth + 50.0f ||
        screenHead.y < -50.0f || screenHead.y > screenHeight + 50.0f) {
      continue;
    }

    // Precompute box dimensions - MATCH ESP HELPER EXACTLY
    const float height = screenFeet.y - screenHead.y;
    const float extraHeadHeight = height * EXTRA_HEAD_HEIGHT_FACTOR;
    const float width = height / WIDTH_HEIGHT_RATIO;
    const ImVec2 topLeft(screenFeet.x - width * 0.5f,
                         screenHead.y - extraHeadHeight);
    const ImVec2 bottomRight(screenFeet.x + width * 0.5f, screenFeet.y);

    const float healthPercent = static_cast<float>(entity.health) / 100.0f;

    // Health bar position - now matches ESP box height exactly
    const ImVec2 healthBarStart(topLeft.x - healthBarWidth - healthBarPadding,
                                topLeft.y);
    const ImVec2 healthBarEnd(healthBarStart.x + healthBarWidth, bottomRight.y);

    // Calculate health color for both bar and text
    ImColor healthColor;
    if (globals::HealthBar) {  // Use the HealthBar toggle for gradient
      healthColor = (healthPercent > 0.7f)   ? ImColor(0, 255, 0, 255)
                    : (healthPercent > 0.3f) ? ImColor(255, 255, 0, 255)
                                             : ImColor(255, 0, 0, 255);
    } else {
      healthColor = isTeammate ? ImColor(globals::TeammateHealthColor[0],
                                         globals::TeammateHealthColor[1],
                                         globals::TeammateHealthColor[2],
                                         globals::TeammateHealthColor[3])
                               : ImColor(globals::EnemyHealthColor[0],
                                         globals::EnemyHealthColor[1],
                                         globals::EnemyHealthColor[2],
                                         globals::EnemyHealthColor[3]);
    }

    if (drawBar) {
      // Combined background and border
      drawList->AddRectFilled(healthBarStart, healthBarEnd,
                              IM_COL32(50, 50, 50, 255));
      drawList->AddRect(healthBarStart, healthBarEnd, IM_COL32(0, 0, 0, 255),
                        0.0f, 0, 1.0f);

      // Health fill - full height bar that matches ESP box
      const float filledHeight = (bottomRight.y - topLeft.y) * healthPercent;
      const ImVec2 fillStart(healthBarStart.x, bottomRight.y - filledHeight);
      drawList->AddRectFilled(fillStart, healthBarEnd, healthColor);
    }

    // Health text - uses the same color logic as the health bar
    if (drawText) {
      char healthText[16];
      snprintf(healthText, sizeof(healthText), "%d", entity.health);

      const ImVec2 textPos(healthBarStart.x - textOffsetX, topLeft.y - 2.0f);

      // Single shadow pass with optimized text rendering
      // Text color now matches the health bar color
      if (smallFont) {
        drawList->AddText(smallFont, fontSize,
                          ImVec2(textPos.x + 1.0f, textPos.y + 1.0f),
                          IM_COL32(0, 0, 0, 200), healthText);
        drawList->AddText(smallFont, fontSize, textPos, healthColor,
                          healthText);
      } else {
        drawList->AddText(ImVec2(textPos.x + 1.0f, textPos.y + 1.0f),
                          IM_COL32(0, 0, 0, 200), healthText);
        drawList->AddText(textPos, healthColor, healthText);
      }
    }
  }
}

}  // namespace HealthBarHelper