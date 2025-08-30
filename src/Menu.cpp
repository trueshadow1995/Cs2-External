#include "../Headers/Menu.h"

#include <cmath>
#include <random>
#include <stdexcept>

#include "../Headers/Globals.h"
#include "../Headers/LogoHelper.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

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

namespace Menu {
bool menuOpen = false;
std::vector<std::string> matrixChars;
std::vector<ImVec2> matrixPositions;
std::vector<float> matrixSpeeds;
std::vector<float> matrixAlphas;

void InitMatrixEffect() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> charDis(33, 126);
  std::uniform_real_distribution<> posDis(0, 1920);
  std::uniform_real_distribution<> speedDis(80.0f, 250.0f);

  matrixChars.clear();
  matrixPositions.clear();
  matrixSpeeds.clear();
  matrixAlphas.clear();

  for (int i = 0; i < 100; i++) {
    matrixChars.push_back(std::string(1, static_cast<char>(charDis(gen))));
    matrixPositions.push_back(ImVec2(posDis(gen), posDis(gen)));
    matrixSpeeds.push_back(speedDis(gen));
    matrixAlphas.push_back(1.0f);
  }
}

void Render() {
  try {
    if (!globals::menu_open) return;
    if (!ImGui::GetCurrentContext()) return;

    if (matrixChars.empty()) {
      InitMatrixEffect();
    }

    // Clean styling - remove borders
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,
                        0.0f);  // No window border
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(6, 30));  // Top padding for header space
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 3));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 3.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize,
                        0.0f);  // No child borders

    // Dark background with a little bit of transparence mauaha - no borders
    // either
    ImGui::PushStyleColor(ImGuiCol_WindowBg,
                          ImVec4(0.03f, 0.03f, 0.06f, 0.95f));
    ImGui::PushStyleColor(
        ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));  // Transparent border
    ImGui::PushStyleColor(ImGuiCol_Text,
                          ImVec4(0.9f, 0.9f, 0.9f, 0.95f));  // dimmed txt
    ImGui::PushStyleColor(
        ImGuiCol_ChildBg,
        ImVec4(0.07f, 0.07f, 0.10f, 0.5f));  // dark child backgground
    ImGui::PushStyleColor(
        ImGuiCol_TitleBg,
        ImVec4(0.05f, 0.05f, 0.08f, 0.95f));  // dark tittle background
    ImGui::PushStyleColor(
        ImGuiCol_TitleBgActive,
        ImVec4(0.07f, 0.07f, 0.12f, 0.95f));  // dark active tittle background

    // Fixed window size
    ImGui::SetNextWindowSize({850, 650},
                             ImGuiCond_Always);  // Increased size for boxes
    ImGui::SetNextWindowBgAlpha(0.95f);

    // Disable window resizing and add NoResize flag
    if (!ImGui::Begin(
            "Phil.9", &globals::menu_open,
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse)) {
      ImGui::End();
      ImGui::PopStyleVar(8);
      ImGui::PopStyleColor(6);
      return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();
    float time = ImGui::GetTime();

    // Matrix rain background
    for (size_t i = 0; i < matrixChars.size(); i++) {
      matrixPositions[i].y += matrixSpeeds[i] * ImGui::GetIO().DeltaTime;
      if (matrixPositions[i].y > winSize.y + 20) {
        matrixPositions[i].y = -20;
        matrixPositions[i].x =
            static_cast<float>(rand() % static_cast<int>(winSize.x));
        matrixAlphas[i] = 1.0f;
      }

      float alpha = (1.0f - (matrixPositions[i].y / winSize.y)) < 0.0f
                        ? 0.0f
                        : (1.0f - (matrixPositions[i].y / winSize.y));
      matrixAlphas[i] = alpha * (0.5f + 0.5f * sinf(time * 2.0f + i * 0.1f));
      ImU32 color =
          ImColor(30, 120, 180, static_cast<int>(255 * matrixAlphas[i]));

      drawList->AddText(ImVec2(winPos.x + matrixPositions[i].x,
                               winPos.y + matrixPositions[i].y),
                        color, matrixChars[i].c_str());
    }

    // Logo in top-left
    ImGui::SetCursorPos(ImVec2(20, 30));
    LogoHelper::Render();

    // Tabs row
    ImGui::SetCursorPos(
        ImVec2(5, 100));  // Spacing from top for logo and better look
    static int current_tab = 0;
    const char* tab_names[] = {"AIM", "VIS", "ESP", "GLOW", "MISC", "CFG"};

    // Create a centered tab bar with proper padding
    float tab_bar_width = winSize.x;  // full width of the window
    float total_tabs_width =
        (70 * IM_ARRAYSIZE(tab_names)) +
        (ImGui::GetStyle().ItemSpacing.x *
         (IM_ARRAYSIZE(tab_names) - 1));  // 70 is button width
    float tab_bar_padding =
        (tab_bar_width - total_tabs_width) / 2;  // centering tabs

    ImGui::BeginChild("Tabs", ImVec2(winSize.x, 30), false, 0);  // no border
    {
      // Set cursor position with proper padding
      ImGui::SetCursorPosX(tab_bar_padding);

      for (int i = 0; i < IM_ARRAYSIZE(tab_names); i++) {  // tab buttons
        if (i > 0) ImGui::SameLine();

        bool is_selected = (current_tab == i);  // highlight current tab
        if (is_selected) {
          ImGui::PushStyleColor(ImGuiCol_Button,
                                ImVec4(0.1f, 0.3f, 0.5f, 0.6f));
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.6f, 0.8f, 1.0f));
        } else {
          ImGui::PushStyleColor(ImGuiCol_Button,
                                ImVec4(0.06f, 0.06f, 0.10f, 0.4f));
          ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 0.9f));
        }

        if (ImGui::Button(tab_names[i], ImVec2(70, 25))) {
          current_tab = i;
        }
        ImGui::PopStyleColor(2);
      }
    }
    ImGui::EndChild();

    // Main content area - starts below tabs with proper padding
    ImGui::SetCursorPos(ImVec2(10, 140));  // Adjusted for better spacing
    ImGui::BeginChild("Content", ImVec2(winSize.x - 20, winSize.y - 150), true,
                      0);
    {
      // dark content background for better text visibility
      drawList->AddRectFilled(
          ImGui::GetWindowPos(),
          ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(),
                 ImGui::GetWindowPos().y + ImGui::GetWindowHeight()),
          ImColor(8, 8, 12, 100), 4.0f);

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                          ImVec2(15, 8));  // Increased padding
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 0.95f));

      // Tab content with box organization
      switch (current_tab) {
        case 0:  // Aimbot
          ImGui::Text("Aimbot Settings");
         
          ImGui::Checkbox("Fov circle", &globals::Fov);
          ImGui::SameLine();
          SafeColorEdit4("Fov Circle Color", globals::FovColor);
          ImGui::SliderFloat("AimbotFov", &globals::AimbotFovSize, 0.f, 90.f,
                             "%.0f");
          break;

        case 1:  // Visuals
          ImGui::Text("Visual Settings");
         

          // First row - Player ESP boxes
          ImGui::BeginChild("PlayerESPRow", ImVec2(0, 100), false, 0);
          {
            ImGui::Columns(2, "PlayerESPColumns", false);

            // Enemy ESP Box
            ImGui::BeginChild("EnemyBox", ImVec2(0, 90), true, 0);
            {
              ImGui::Text("Enemy ESP");
              ImGui::Checkbox("Enable", &globals::EnemyEsp);
              ImGui::Checkbox("Background", &globals::EnemyEspBackground);
              SafeColorEdit4("Color", globals::EnemyEspColor);
              SafeColorEdit4("BG Color", globals::EnemyEspBackGroundColor);
            }
            ImGui::EndChild();

            ImGui::NextColumn();

            // Teammate ESP Box
            ImGui::BeginChild("TeammateBox", ImVec2(0, 90), true, 0);
            {
              ImGui::Text("Teammate ESP");
              ImGui::Checkbox("Enable", &globals::TeammateEsp);
              ImGui::Checkbox("Background", &globals::TeammateEspBackground);
              SafeColorEdit4("Color", globals::TeammateEspColor);
              SafeColorEdit4("BG Color", globals::FriendlyEspBackGroundColor);
            }
            ImGui::EndChild();

            ImGui::Columns(1);
          }
          ImGui::EndChild();

          // Second row - Health boxes
          ImGui::BeginChild("HealthRow", ImVec2(0, 100), false, 0);
          {
            ImGui::Columns(2, "HealthColumns", false);

            // Enemy Health Box
            ImGui::BeginChild("EnemyHealthBox", ImVec2(0, 90), true, 0);
            {
              ImGui::Text("Enemy Health");
              ImGui::Checkbox("Health Bar", &globals::EnemyHealth);
              ImGui::Checkbox("Health Text", &globals::EnemyHealthText);
              SafeColorEdit4("Color", globals::EnemyHealthColor);
            }
            ImGui::EndChild();

            ImGui::NextColumn();

            // Teammate Health Box
            ImGui::BeginChild("TeammateHealthBox", ImVec2(0, 90), true, 0);
            {
              ImGui::Text("Teammate Health");
              ImGui::Checkbox("Health Bar", &globals::TeammateHealth);
              ImGui::Checkbox("Health Text", &globals::TeammateHealthTxt);
              SafeColorEdit4("Color", globals::TeammateHealthColor);
            }
            ImGui::EndChild();

            ImGui::Columns(1);
          }
          ImGui::EndChild();

          // Third row - Bones boxes
          ImGui::BeginChild("BonesRow", ImVec2(0, 120), false, 0);
          {
            ImGui::Columns(2, "BonesColumns", false);

            // Enemy Bones Box
            ImGui::BeginChild("EnemyBonesBox", ImVec2(0, 110), true, 0);
            {
              ImGui::Text("Enemy Bones");
              ImGui::Checkbox("Enable", &globals::EnemyBones);
              ImGui::SliderFloat("Thickness", &globals::BoneEspThickness, 0.f,
                                 10.f, "%.1f");
              SafeColorEdit4("Color", globals::EnemyBoneColor);
            }
            ImGui::EndChild();

            ImGui::NextColumn();

            // Teammate Bones Box
            ImGui::BeginChild("TeammateBonesBox", ImVec2(0, 110), true, 0);
            {
              ImGui::Text("Teammate Bones");
              ImGui::Checkbox("Enable", &globals::FriendlyBones);
              ImGui::SliderFloat("Thickness", &globals::BoneEspThickness, 0.f,
                                 10.f, "%.1f");
              SafeColorEdit4("Color", globals::TeammateBoneColor);
            }
            ImGui::EndChild();

            ImGui::Columns(1);
          }
          ImGui::EndChild();

          // Fourth row - Misc Visuals
          ImGui::BeginChild("MiscVisualsRow", ImVec2(0, 100), false, 0);
          {
            ImGui::Columns(2, "MiscVisualsColumns", false);

            // Head Marker Box
            ImGui::BeginChild("HeadMarkerBox", ImVec2(0, 90), true, 0);
            {
              ImGui::Text("Head Marker");
              ImGui::Checkbox("Enable", &globals::Headmarker);
              ImGui::SliderFloat("Size", &globals::Headmarkersize, 1.f, 15.f,
                                 "%.1f");
              ImGui::ColorEdit3("Color", globals::headMakerColor,
                                ImGuiColorEditFlags_NoInputs);
            }
            ImGui::EndChild();

            ImGui::NextColumn();

            // Health Settings Box
            ImGui::BeginChild("HealthSettingsBox", ImVec2(0, 90), true, 0);
            {
              ImGui::Text("Health Settings");
              ImGui::Checkbox("Raw HP Gradient", &globals::HealthPercentage);
              ImGui::Checkbox("Bone Debug", &globals::BoneDebug);
            }
            ImGui::EndChild();

            ImGui::Columns(1);
          }
          ImGui::EndChild();
          break;

        case 2:  // ESP
          ImGui::Text("ESP Settings");
          ImGui::Separator();
          // Additional ESP features can go here
          break;

        case 3:  // GLOW
          ImGui::Text("Glow Settings");
          ImGui::Separator();
          // Glow settings can go here
          break;

        case 4:  // MISC
          ImGui::Text("Misc Settings");
          ImGui::Separator();
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

        case 5:  // CFG
          ImGui::Text("Config Settings");
          ImGui::Separator();
          if (ImGui::Button("Save Config", ImVec2(120, 25))) {
          }
          ImGui::SameLine();
          if (ImGui::Button("Load Config", ImVec2(120, 25))) {
          }
          ImGui::SameLine();
          if (ImGui::Button("Reset Config", ImVec2(120, 25))) {
          }
          break;
      }

      ImGui::PopStyleColor();
      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleVar(8);
    ImGui::PopStyleColor(6);

  } catch (const std::exception& e) {
    OutputDebugStringA(
        ("Menu::Render exception: " + std::string(e.what()) + "\n").c_str());
  } catch (...) {
    OutputDebugStringA("Menu::Render unknown exception\n");
  }
}

}  // namespace menu