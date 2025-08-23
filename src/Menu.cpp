#pragma once
#include "../Headers/Menu.h"

#include <corecrt_math.h>

#include "../Headers/LogoHelper.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

namespace Menu {

void Render() {
  if (!globals::menu_open) return;

  ImGui::SetNextWindowSize({700, 450});

  // Begin ImGui window
  ImGui::Begin("phil9", &globals::menu_open,
               ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoCollapse);

  // Get window info
  ImDrawList* drawList = ImGui::GetWindowDrawList();
  ImVec2 winPos = ImGui::GetWindowPos();
  ImVec2 winSize = ImGui::GetWindowSize();
  float borderThickness = 4.0f;

  // Draw rainbow border
  for (int i = 0; i < (int)borderThickness; ++i) {
    float t = ImGui::GetTime() * 0.5f;  // speed of rainbow
    ImU32 col = ImColor::HSV(fmodf(t + i * 0.05f, 1.0f), 1.0f, 1.0f);
    drawList->AddRect(
        ImVec2(winPos.x - i, winPos.y - i),
        ImVec2(winPos.x + winSize.x + i, winPos.y + winSize.y + i), col, 0.0f,
        0, 1.0f);
  }
  
  // ---- Left Tab Bar ----
  ImGui::BeginChild("TabBar", ImVec2(128, ImGui::GetContentRegionAvail().y),
                    true, ImGuiWindowFlags_NoScrollbar);

  static int current_tab = 0;
  auto tabButton = [&](const char* label, int tab) {
    ImVec4 accentColor = ImVec4(0.08f, 0.53f, 0.79f, 0.50f);
    if (current_tab == tab) {
      ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accentColor);
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, accentColor);
    }
    if (ImGui::Button(label, ImVec2(-1, 48))) current_tab = tab;
    if (current_tab == tab) ImGui::PopStyleColor(3);
  };

  LogoHelper::Render();
  tabButton("Aimbot", 0);
  tabButton("Overlay stuff", 1);
  tabButton("Glow n Chams", 2);
  tabButton("Features", 3);
  tabButton("Skin Changer", 4);

  ImGui::Text("Press DEL to close");
  ImGui::EndChild();

  // ---- Right Tab Content ----
  ImGui::SameLine();
  ImGui::BeginChild("TabContent",
                    ImVec2(ImGui::GetContentRegionAvail().x,
                           ImGui::GetContentRegionAvail().y),
                    true);

  switch (current_tab) {
    case 0:  // Aimbot
      ImGui::Checkbox("Fov circle", &globals::Fov);
      ImGui::SameLine(0, 70);
      ImGui::ColorEdit4("Fov Circle Color", globals::FovColor,
                        ImGuiColorEditFlags_NoInputs);
      ImGui::SliderFloat("AimbotFov", &globals::AimbotFovSize, 0.f, 90.f,
                         "%.0f");
      break;

    case 1:  // ESP
      ImGui::Checkbox("Enemy Player ESP", &globals::EnemyEsp);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Enemy ESP Color", globals::EnemyEspColor,
                        ImGuiColorEditFlags_NoInputs);

      ImGui::Checkbox("Teammate ESP", &globals::TeammateEsp);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Friendly ESP Color", globals::TeammateEspColor,
                        ImGuiColorEditFlags_NoInputs);

      ImGui::Checkbox("Friendly ESP Background",
                      &globals::TeammateEspBackground);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Friendly ESP Background Color",
                        globals::FriendlyEspBackGroundColor,
                        ImGuiColorEditFlags_NoInputs);

      ImGui::Checkbox("Enemy ESP Background", &globals::EnemyEspBackground);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Enemy ESP Background Color",
                        globals::EnemyEspBackGroundColor,
                        ImGuiColorEditFlags_NoInputs);

      ImGui::Checkbox("Friendly Player Health", &globals::TeammateHealth);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Player Health Color", globals::TeammateHealthColor,
                        ImGuiColorEditFlags_NoInputs);

      ImGui::Checkbox("Enemy Player Health", &globals::EnemyHealth);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Player Health Color", globals::EnemyHealthColor,
                        ImGuiColorEditFlags_NoInputs);

      ImGui::Checkbox("Friendly Bone ESP", &globals::FriendlyBones);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Friendly Bone ESP Color", globals::TeammateBoneColor,
                        ImGuiColorEditFlags_NoInputs);
      ImGui::SliderFloat("Friendly Bone ESP Thickness",
                         &globals::BoneEspThickness, 0.f, 90.f, "%.0f");

      ImGui::Checkbox("Enemy Bone ESP", &globals::EnemyBones);
      ImGui::SameLine(220);
      ImGui::ColorEdit4("Enemy Bone ESP Color", globals::EnemyBoneColor,
                        ImGuiColorEditFlags_NoInputs);
      ImGui::SliderFloat("Enemy Bone ESP Thickness", &globals::BoneEspThickness,
                         0.f, 90.f, "%.0f");

      ImGui::Checkbox("Bone Debug", &globals::BoneDebug);
      break;

    case 2:  // Chams/Glow
      break;

    case 3:  // Features
      ImGui::Checkbox("FPS Counter", &globals::FpsCounter);
      ImGui::Checkbox("Water Mark", &globals::WaterMark);
      break;
  }
  
  ImGui::EndChild();
  ImGui::End();  // End main window
}

}  // namespace Menu
