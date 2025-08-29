#include "../Headers/Menu.h"

#include <cmath>
#include <stdexcept>

#include "../Headers/Globals.h"
#include "../Headers/LogoHelper.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

namespace Menu {
bool menuOpen = false;  // default closed
bool SafeColorEdit4(const char* label, float col[4],
                    ImGuiColorEditFlags flags = 0) {
  if (!col) return false;

  float temp[4] = {col[0], col[1], col[2], col[3]};
  bool result =
      ImGui::ColorEdit4(label, temp, flags | ImGuiColorEditFlags_NoInputs);
  if (result) {
    col[0] = temp[0];
    col[1] = temp[1];
    col[2] = temp[2];
    col[3] = temp[3];
  }
  return result;
}

void Render() {
  try {
    if (!globals::menu_open) return;

    if (!ImGui::GetCurrentContext()) {
      return;
    }

    ImGui::SetNextWindowSize({700, 450});

    if (!ImGui::Begin("phil9", &globals::menu_open,
                      ImGuiWindowFlags_NoSavedSettings |
                          ImGuiWindowFlags_NoResize |
                          ImGuiWindowFlags_NoCollapse)) {
      ImGui::End();
      return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (!drawList) {
      ImGui::End();
      return;
    }

    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();
    float borderThickness = 4.0f;

    // Draw rainbow border
    for (int i = 0; i < (int)borderThickness; ++i) {
      float t = ImGui::GetTime() * 0.5f;
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
        SafeColorEdit4("Fov Circle Color", globals::FovColor);
        ImGui::SliderFloat("AimbotFov", &globals::AimbotFovSize, 0.f, 90.f,
                           "%.0f");
        break;

      case 1:  // ESP
        ImGui::Checkbox("Enemy Player ESP", &globals::EnemyEsp);
        ImGui::SameLine(220);
        SafeColorEdit4("Enemy ESP Color", globals::EnemyEspColor);

        ImGui::Checkbox("Teammate ESP", &globals::TeammateEsp);
        ImGui::SameLine(220);
        SafeColorEdit4("Friendly ESP Color", globals::TeammateEspColor);

        ImGui::Checkbox("Friendly ESP Background",
                        &globals::TeammateEspBackground);
        ImGui::SameLine(220);
        SafeColorEdit4("Friendly ESP Background Color",
                       globals::FriendlyEspBackGroundColor);

        ImGui::Checkbox("Enemy ESP Background", &globals::EnemyEspBackground);
        ImGui::SameLine(220);
        SafeColorEdit4("Enemy ESP Background Color",
                       globals::EnemyEspBackGroundColor);

        ImGui::Separator();
        ImGui::Text("Health Bars:");

        // Teammate health bar
        ImGui::Checkbox("Teammate Health Bar", &globals::TeammateHealth);
        ImGui::SameLine(220);
        SafeColorEdit4("Teammate Health Color", globals::TeammateHealthColor);

        // Enemy health bar
        ImGui::Checkbox("Enemy Health Bar", &globals::EnemyHealth);
        ImGui::SameLine(220);
        SafeColorEdit4("Enemy Health Color", globals::EnemyHealthColor);


        // Health text
        ImGui::Checkbox("Teammate Health Text", &globals::TeammateHealthTxt);
        ImGui::SameLine(220);
        ImGui::Checkbox("Enemy Health Text", &globals::EnemyHealthText);

        // Show raw HP vs percentage
        ImGui::Checkbox(" Raw Hp gradiant", &globals::HealthPercentage);

        ImGui::Separator();
        ImGui::Text("Bones:");

        ImGui::Checkbox("Friendly Bone ESP", &globals::FriendlyBones);
        ImGui::SameLine(220);
        SafeColorEdit4("Friendly Bone ESP Color", globals::TeammateBoneColor);
        ImGui::SliderFloat("Friendly Bone Thickness",
                           &globals::BoneEspThickness, 0.f, 10.f, "%.1f");

        ImGui::Checkbox("Enemy Bone ESP", &globals::EnemyBones);
        ImGui::SameLine(220);
        SafeColorEdit4("Enemy Bone ESP Color", globals::EnemyBoneColor);
        ImGui::SliderFloat("Enemy Bone Thickness", &globals::BoneEspThickness,
                           0.f, 10.f, "%.1f");

        ImGui::Checkbox("Bone Debug", &globals::BoneDebug);

        ImGui::Separator();
        ImGui::Text("Head Marker");
        ImGui::Checkbox("Enable Head Marker", &globals::Headmarker);
        ImGui::SameLine(220);
        ImGui::ColorEdit3("Head Marker Color", globals::headMakerColor,
                          ImGuiColorEditFlags_NoInputs);
        ImGui::SliderFloat("Head Marker Size", &globals::Headmarkersize, 1.f,
                           15.f, "%.1f");
      
        break;

      case 2:  // Chams/Glow
        break;

      case 3:  // Features
        ImGui::Checkbox("FPS Counter", &globals::FpsCounter);
        ImGui::Checkbox("Water Mark", &globals::WaterMark);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Performance:");
        ImGui::BulletText("FPS: %.0f", ImGui::GetIO().Framerate);
        ImGui::BulletText("Frame Time: %.2f ms",
                          1000.0f / ImGui::GetIO().Framerate);
        break;

      case 4:  // Skin Changer
        break;
    }

    ImGui::EndChild();
    ImGui::End();
  } catch (const std::exception& e) {
    OutputDebugStringA(
        ("Menu::Render exception: " + std::string(e.what()) + "\n").c_str());
  } catch (...) {
    OutputDebugStringA("Menu::Render unknown exception\n");
  }
}

}  // namespace Menu