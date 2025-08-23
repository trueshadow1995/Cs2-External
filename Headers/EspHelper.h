

#pragma once
#include <d3d11.h>

#include "../ImGui/imgui.h"
#include "Globals.h"
#include "Math.h"
#include "Memory.h"
#include "Offsets.h"

  const auto client = mem.GetModuleAddress("client.dll");
const auto engine = mem.GetModuleAddress("engine.dll");

auto entityList = mem.Read<uintptr_t>(client + offsets::EntityList);

    const auto localPlayerpawn =
        mem.Read<std::uintptr_t>(client + offsets::LocalPlayerPawn);

    const auto entlist = mem.Read<std::uintptr_t>(client +
    offsets::EntityList);

    ViewMatrix_t view_matrix =
        mem.Read<ViewMatrix_t>(client + offsets::ViewMatrix);

    int localTeam = mem.Read<int>(client + offsets::m_iTeamNum);
namespace ESPHelper {
inline void Render(Memory& mem, uintptr_t client, uintptr_t localPlayerpawn,
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

    int health = mem.Read<int>(pPawn + offsets::m_iHealth);
    int team = mem.Read<int>(pPawn + offsets::m_iTeamNum);
    if (health <= 0 || health > 100) continue;

    Vector3 origin = mem.Read<Vector3>(pPawn + offsets::m_vOldOrigin);
    Vector3 head = {origin.x, origin.y, origin.z + 75.f};

    Vector3 screenFeetPos = origin.WorldToScreen(view_matrix);
    Vector3 screenHeadPos = head.WorldToScreen(view_matrix);

    float height = (screenFeetPos.y - screenHeadPos.y);
    float width = height / 2.4f;

    ImVec2 topLeft(screenHeadPos.x - width / 2.f, screenHeadPos.y);
    ImVec2 bottomRight(screenHeadPos.x + width / 2.f, screenHeadPos.y + height);

    // Box ESP
    if (team == localTeam) {
      if (globals::TeammateEsp) {
        backgrounddraw->AddRect(
            topLeft, bottomRight,
            ImColor(globals::TeammateEspColor[0], globals::TeammateEspColor[1],
                    globals::TeammateEspColor[2],
                    globals::TeammateEspColor[3]));
      }
    } else {
      if (globals::EnemyEsp) {
        backgrounddraw->AddRect(
            topLeft, bottomRight,
            ImColor(globals::EnemyEspColor[0], globals::EnemyEspColor[1],
                    globals::EnemyEspColor[2], globals::EnemyEspColor[3]));
      }
    }

    // Background ESP
    if (team == localTeam && globals::TeammateEspBackground) {
      backgrounddraw->AddRectFilled(
          topLeft, bottomRight,
          ImColor(globals::FriendlyEspBackGroundColor[0],
                  globals::FriendlyEspBackGroundColor[1],
                  globals::FriendlyEspBackGroundColor[2],
                  globals::FriendlyEspBackGroundColor[3]));
    } else if (team != localTeam && globals::EnemyEspBackground) {
      backgrounddraw->AddRectFilled(
          topLeft, bottomRight,
          ImColor(globals::EnemyEspBackGroundColor[0],
                  globals::EnemyEspBackGroundColor[1],
                  globals::EnemyEspBackGroundColor[2],
                  globals::EnemyEspBackGroundColor[3]));
    }
  }
}
}  // namespace ESPHelper
