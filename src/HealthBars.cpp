#include "../Headers/EntityDataManager.h"
#include "../Headers/Globals.h"
#include "../Headers/HealBarHelper.h"
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"
#include "../ImGui/imgui.h"

namespace HealthBarHelper {

void RenderHealth(const GameData& gameData, ImDrawList* drawList) {
  if (!drawList || !gameData.valid) return;

  int localTeam = gameData.localTeam;
  const ViewMatrix_t& viewMatrix = gameData.viewMatrix;

  for (const auto& entity : gameData.entities) {
    int entityHP = entity.health;
    if (entityHP <= 0 || entityHP > 100) continue;

    int team = entity.team;
    Vector3 origin = entity.origin;
    Vector3 head = entity.head;

    Vector3 screenFeet{}, screenHead{};
    if (!origin.WorldToScreen(viewMatrix, ImGui::GetIO().DisplaySize.x,
                              ImGui::GetIO().DisplaySize.y, screenFeet))
      continue;
    if (!head.WorldToScreen(viewMatrix, ImGui::GetIO().DisplaySize.x,
                            ImGui::GetIO().DisplaySize.y, screenHead))
      continue;
    if (screenFeet.z <= 0.001f || screenHead.z <= 0.001f) continue;

    float height = screenFeet.y - screenHead.y;
    float width = height / 2.4f;
    ImVec2 topLeft(screenFeet.x - width / 2.f, screenHead.y);
    ImVec2 bottomRight(screenFeet.x + width / 2.f, screenFeet.y);

    bool isTeammate = (team == localTeam);

    // Toggles for health bar and text
    bool drawBar = (isTeammate && globals::TeammateHealth) ||
                   (!isTeammate && globals::EnemyHealth);
    bool drawText = (isTeammate && globals::TeammateHealthTxt) ||
                    (!isTeammate && globals::EnemyHealthText);

    if (!drawBar && !drawText) continue;

    float healthPercent = static_cast<float>(entityHP) / 100.0f;

    // Health bar position
    const float healthBarWidth = 3.0f;
    const float healthBarPadding = 2.0f;
    ImVec2 healthBarStart(topLeft.x - healthBarWidth - healthBarPadding,
                          topLeft.y);
    ImVec2 healthBarEnd(healthBarStart.x + healthBarWidth, bottomRight.y);

    if (drawBar) {
      // HEALTH COLOR - ADDED PERCENTAGE-BASED COLORING BACK
      ImColor healthColor;

      if (globals::HealthPercentage) {
        // Percentage-based coloring (green/yellow/red)
        if (healthPercent > 0.7f) {
          healthColor = ImColor(0, 255, 0, 255);  // Green
        } else if (healthPercent > 0.3f) {
          healthColor = ImColor(255, 255, 0, 255);  // Yellow
        } else {
          healthColor = ImColor(255, 0, 0, 255);  // Red
        }
      } else {
        // Team-based coloring (from your globals)
        healthColor = isTeammate ? ImColor(globals::TeammateHealthColor[0],
                                           globals::TeammateHealthColor[1],
                                           globals::TeammateHealthColor[2],
                                           globals::TeammateHealthColor[3])
                                 : ImColor(globals::EnemyHealthColor[0],
                                           globals::EnemyHealthColor[1],
                                           globals::EnemyHealthColor[2],
                                           globals::EnemyHealthColor[3]);
      }

      // Background
      drawList->AddRectFilled(healthBarStart, healthBarEnd,
                              IM_COL32(50, 50, 50, 255));

      // Filled portion (BOTTOM -> TOP)
      float filledHeight = (bottomRight.y - topLeft.y) * healthPercent;
      ImVec2 fillStart(healthBarStart.x, bottomRight.y - filledHeight);
      ImVec2 fillEnd(healthBarEnd.x, bottomRight.y);

      drawList->AddRectFilled(fillStart, fillEnd, healthColor);
      drawList->AddRect(healthBarStart, healthBarEnd, IM_COL32(0, 0, 0, 255));
    }

    // Health text - FIXED POSITION (doesn't move)
    if (drawText) {
      char healthText[16];
      if (globals::HealthPercentage)
        snprintf(healthText, sizeof(healthText), "%d", entityHP);

      // FIXED: Position text at the top of the health bar 
      // health)
      ImVec2 textPos(healthBarStart.x - 15.0f, healthBarStart.y - 2.0f);

      const float fontSize = 12.0f;
      ImFont* smallFont = ImGui::GetIO().Fonts->Fonts[0];

      // Outline
      drawList->AddText(smallFont, fontSize,
                        ImVec2(textPos.x - 1, textPos.y - 1),
                        IM_COL32(0, 0, 0, 255), healthText);
      drawList->AddText(smallFont, fontSize,
                        ImVec2(textPos.x + 1, textPos.y - 1),
                        IM_COL32(0, 0, 0, 255), healthText);
      drawList->AddText(smallFont, fontSize,
                        ImVec2(textPos.x - 1, textPos.y + 1),
                        IM_COL32(0, 0, 0, 255), healthText);
      drawList->AddText(smallFont, fontSize,
                        ImVec2(textPos.x + 1, textPos.y + 1),
                        IM_COL32(0, 0, 0, 255), healthText);

      // Main text
      drawList->AddText(smallFont, fontSize, textPos,
                        IM_COL32(255, 255, 255, 255), healthText);
    }
  }
}

}  // namespace HealthBarHelper