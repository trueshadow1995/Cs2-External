#include "../Headers/ESPHelper.h"
#include "../Headers/EntityDataManager.h"  // For EntityInfo definition
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"

namespace ESPHelper {

void RenderEsp(Memory& mem, uintptr_t client, const EntityInfo& entity,
               uintptr_t localPlayerPawn, int localTeam,
               const ViewMatrix_t& viewMatrix, ImDrawList* drawList) {
  if (!entity.pawn || !drawList) return;

  // Get screen dimensions
  ImVec2 screenSize = ImGui::GetIO().DisplaySize;
  float screenWidth = screenSize.x;
  float screenHeight = screenSize.y;

  int team = entity.team;
  int health = entity.health;
  Vector3 origin = entity.origin;
  Vector3 head = entity.head;

  if (health <= 0) return;

  Vector3 screenFeet{}, screenHead{};
  if (!origin.WorldToScreen(viewMatrix, screenWidth, screenHeight, screenFeet))
    return;
  if (!head.WorldToScreen(viewMatrix, screenWidth, screenHeight, screenHead))
    return;

  float height = screenFeet.y - screenHead.y;
  float width = height / 2.4f;


  const float extraHeadHeight =
      height * 0.15f;  // Add 15% 

  ImVec2 topLeft(
      screenFeet.x - width / 2.f,
      screenHead.y - extraHeadHeight);  
  ImVec2 bottomRight(screenFeet.x + width / 2.f,
                     screenFeet.y);  
  //------------------------------------------------------
  // --- Boxes ---
  if (team == localTeam && globals::TeammateEsp) {
    drawList->AddRect(
        topLeft, bottomRight,
        ImColor(globals::TeammateEspColor[0], globals::TeammateEspColor[1],
                globals::TeammateEspColor[2], globals::TeammateEspColor[3]),
        0.0f, 0, 1.5f);
  } else if (team != localTeam && globals::EnemyEsp) {
    drawList->AddRect(
        topLeft, bottomRight,
        ImColor(globals::EnemyEspColor[0], globals::EnemyEspColor[1],
                globals::EnemyEspColor[2], globals::EnemyEspColor[3]),
        0.0f, 0, 1.5f);
  }
  //------------------------------------------------------------
  // --- Background ---
  if (team == localTeam && globals::TeammateEspBackground) {
    drawList->AddRectFilled(topLeft, bottomRight,
                            ImColor(globals::FriendlyEspBackGroundColor[0],
                                    globals::FriendlyEspBackGroundColor[1],
                                    globals::FriendlyEspBackGroundColor[2],
                                    globals::FriendlyEspBackGroundColor[3]));
  } else if (team != localTeam && globals::EnemyEspBackground) {
    drawList->AddRectFilled(topLeft, bottomRight,
                            ImColor(globals::EnemyEspBackGroundColor[0],
                                    globals::EnemyEspBackGroundColor[1],
                                    globals::EnemyEspBackGroundColor[2],
                                    globals::EnemyEspBackGroundColor[3]));
  }
  //------------------------------------------------------------
  // Draw head
  if (globals::Headmarker) {
    drawList->AddCircle(ImVec2(screenHead.x, screenHead.y), 3.0f,
                        IM_COL32(255, 0, 0, 255));
  }
}

}  // namespace ESPHelper