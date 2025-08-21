
#pragma once
#include <d3d11.h>
#include "Memory.h"  
#include "Offsets.h"
#include "Math.h"
#include "../ImGui/imgui.h"
#include "Globals.h"
#include "C_color.h"



namespace  HealthBarHelper{
inline void RenderHealth(Memory& mem, uintptr_t client,
                         uintptr_t localPlayerpawn,
                         const ViewMatrix_t& view_matrix,
                         ImDrawList* backgrounddraw) {
  auto entityList = mem.Read<uintptr_t>(client + offsets::EntityList);
  int localTeam = mem.Read<int>(localPlayerpawn + offsets::m_iTeamNum);

  for (int i = 0; i < 64; i++) {
    uintptr_t listEntry =
        mem.Read<uintptr_t>(entityList + 0x8 * (i >> 9) + 0x10);
    if (!listEntry) continue;

    uintptr_t entController =
        mem.Read<uintptr_t>(listEntry + 0x78 * (i & 0x1FF));
    if (!entController) continue;

    uint32_t hPawn = mem.Read<uint32_t>(entController + offsets::m_hPawn);
    if (!hPawn) continue;

    uintptr_t listEntryPawn =
        mem.Read<uintptr_t>(entityList + 0x8 * ((hPawn & 0x7FFF) >> 9) + 0x10);
    if (!listEntryPawn) continue;

    uintptr_t pPawn =
        mem.Read<uintptr_t>(listEntryPawn + 0x78 * (hPawn & 0x1FF));
    if (!pPawn || pPawn == localPlayerpawn) continue;

    int entity_hp = mem.Read<int>(pPawn + offsets::m_iHealth);
    int team = mem.Read<int>(pPawn + offsets::m_iTeamNum);
    if (entity_hp <= 0 || entity_hp > 100) continue;

    Vector3 origin = mem.Read<Vector3>(pPawn + offsets::m_vOldOrigin);
    Vector3 head = {origin.x, origin.y, origin.z + 75.f};

    Vector3 screenFeetPos = origin.WorldToScreen(view_matrix);
    Vector3 screenHeadPos = head.WorldToScreen(view_matrix);

    float height = (screenFeetPos.y - screenHeadPos.y);
    float width = height / 2.4f;

    ImVec2 topLeft(screenHeadPos.x - width / 2.f, screenHeadPos.y);
    ImVec2 bottomRight(screenHeadPos.x + width / 2.f, screenHeadPos.y + height);

    // Health text and bar
    if ((team == localTeam && globals::TeammateHealth) ||
        (team != localTeam && globals::EnemyHealth)) {
      std::string hp_text = std::to_string(entity_hp);
      ImVec2 text_size = ImGui::CalcTextSize(hp_text.c_str());

      ImColor textColor = (team == localTeam)
                              ? ImColor(globals::TeammateHealthColor[0],
                                        globals::TeammateHealthColor[1],
                                        globals::TeammateHealthColor[2],
                                        globals::TeammateHealthColor[3])
                              : ImColor(globals::EnemyHealthColor[0],
                                        globals::EnemyHealthColor[1],
                                        globals::EnemyHealthColor[2],
                                        globals::EnemyHealthColor[3]);

      ImGui::GetBackgroundDrawList()->AddText(
          {topLeft.x - text_size.x - 6, topLeft.y - 3}, textColor,
          hp_text.c_str());

      float health_height = bottomRight.y - topLeft.y;
      float filled_height = health_height * (entity_hp / 100.f);

      // Background bar
      backgrounddraw->AddRectFilled({topLeft.x - 6, topLeft.y},
                                    {topLeft.x - 4, bottomRight.y},
                                    ImColor(30, 30, 30, 255));

      // Foreground gradient
      c_color col_health = c_color::from_hsb((entity_hp / 100.f) * 0.33f, 1, 1);
      backgrounddraw->AddRectFilled(
          {topLeft.x - 6, bottomRight.y - filled_height},
          {topLeft.x - 4, bottomRight.y},
          ImColor(col_health.r, col_health.g, col_health.b, col_health.a));
    }
  }
}
}
 // namespace ESPHelper
#pragma once
