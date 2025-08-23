#pragma once
#include "../Headers/RenderManager.h"

#include "../Headers/BoneHelper.h"
#include "../Headers/EspHelper.h"
#include "../Headers/FpsCounter.h"
#include "../Headers/Globals.h"
#include "../Headers/HealBarHelper.h"
#include "../Headers/Menu.h"
#include "../Headers/WaterMarkHelper.h"
#include "../ImGui/imgui.h"

namespace RenderManager {

 void RenderFrame() {
  // Get draw lists once per frame
  ImDrawList* backgroundDraw = ImGui::GetBackgroundDrawList();
  ImDrawList* foregroundDraw = ImGui::GetForegroundDrawList();

  // Modules and player
  const auto client = mem.GetModuleAddress("client.dll");
  if (!client) return;

  const uintptr_t localPlayerPawn =
      mem.Read<uintptr_t>(client + offsets::LocalPlayerPawn);
  if (!localPlayerPawn) return;

  const int localTeam = mem.Read<int>(localPlayerPawn + offsets::m_iTeamNum);
  const ViewMatrix_t viewMatrix =
      mem.Read<ViewMatrix_t>(client + offsets::ViewMatrix);

  const uintptr_t entityList =
      mem.Read<uintptr_t>(client + offsets::EntityList);
  if (!entityList) return;

 

  // Render FPS first
  FpsCounter::Render();

  // Loop through all entities
  for (int i = 0; i < 64; ++i) {
    // Read entity pointers
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
    if (!pPawn || pPawn == localPlayerPawn) continue;

    // Batch read essential data
    struct PawnData {
      int health;
      int team;
      uintptr_t gameSceneNode;
    };

    PawnData pdata = mem.Read<PawnData>(
        pPawn + offsets::m_iHealth); // pPawn + ihealth
    pdata.team = mem.Read<int>(pPawn + offsets::m_iTeamNum); // pPawn  + iteam
    pdata.gameSceneNode =
        mem.Read<uintptr_t>(pPawn + offsets::m_pGameSceneNode); 

    if (pdata.health <= 0 || pdata.health > 100 || !pdata.gameSceneNode) 
      continue; 

    // Read bones directly into preallocated buffer
    uintptr_t boneArrayPtr =
        mem.Read<uintptr_t>(pdata.gameSceneNode + offsets::m_modelState + 0x80); 
    if (!boneArrayPtr) continue; 

   

    // Render bones
    BoneHelper::RenderBones(mem, pPawn, localPlayerPawn, pdata.team, localTeam,
                      viewMatrix, backgroundDraw);
    
    // render health bars
    HealthBarHelper::RenderHealth(mem, client, localPlayerPawn, viewMatrix,
                                  backgroundDraw); 
  }
 
  // Render ESP overlay
  ESPHelper::Render(mem, client, localPlayerPawn, viewMatrix, backgroundDraw);

  // Render menu if open
  if (globals::menu_open)
      Menu::Render();

  // Render watermark last
  WaterMark::Render();
}

}  // namespace RenderManager
