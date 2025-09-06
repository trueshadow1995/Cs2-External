#pragma once
#include "../Headers/Menu.h"
#define NOMINMAX

// Define missing mouse button constants if not already defined
#ifndef VK_XBUTTON1
#define VK_XBUTTON1 0x05
#define VK_XBUTTON2 0x06
#endif

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

#include "../Headers/Globals.h"
#include "../Headers/LogoHelper.h"
#include "../Headers/Styles.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

// Precompute constants
constexpr int MATRIX_CHAR_COUNT = 100;
constexpr float MATRIX_RESPAWN_Y_OFFSET = -20.0f;
constexpr float MATRIX_ALPHA_MULTIPLIER = 0.5f;
constexpr ImGuiWindowFlags MAIN_WINDOW_FLAGS =
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
    ImGuiWindowFlags_NoScrollWithMouse;

namespace Menu {
bool menuOpen = false;
std::vector<std::string> matrixChars;
std::vector<ImVec2> matrixPositions;
std::vector<float> matrixSpeeds;
std::vector<float> matrixAlphas;
bool enableMatrixEffect = true;

static ImVec2 mainWindowPos;
static bool mainWindowMoved = false;
static bool matrixInitialized = false;

// Precomputed tab data
static constexpr const char* TAB_NAMES[] = {"AIM", "VISUALS", "MISC", "CONFIG"};
static constexpr int TAB_COUNT = IM_ARRAYSIZE(TAB_NAMES);
static constexpr float TAB_WIDTH = 100.0f;

// Precomputed section data
static constexpr const char* SECTION_NAMES[] = {
    "Enemy Settings", "Teammate Settings", "Bones Settings", "Health Settings",
    "Global Settings"};
static constexpr int SECTION_COUNT = IM_ARRAYSIZE(SECTION_NAMES);
static constexpr float SECTION_BUTTON_WIDTH = 120.0f;
static constexpr float SECTION_POPUP_WIDTH = 130.0f;
static constexpr float SECTION_POPUP_HEIGHT = 280.0f;

// HelpMarker function for tooltips
void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

bool SafeColorEdit4(const char* label, float col[4],
                    ImGuiColorEditFlags flags = 0) {
  if (!col) return false;

  float temp[4] = {col[0], col[1], col[2], col[3]};
  bool result =
      ImGui::ColorEdit4(label, temp, flags | ImGuiColorEditFlags_NoInputs);
  if (result) {
    memcpy(col, temp, sizeof(float) * 4);
  }
  return result;
}

void InitMatrixEffect() {
  if (matrixInitialized) return;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> charDis(33, 126);
  std::uniform_real_distribution<float> posDis(0.0f, 1920.0f);
  std::uniform_real_distribution<float> speedDis(80.0f, 250.0f);

  matrixChars.clear();
  matrixPositions.clear();
  matrixSpeeds.clear();
  matrixAlphas.clear();

  matrixChars.reserve(MATRIX_CHAR_COUNT);
  matrixPositions.reserve(MATRIX_CHAR_COUNT);
  matrixSpeeds.reserve(MATRIX_CHAR_COUNT);
  matrixAlphas.reserve(MATRIX_CHAR_COUNT);

  for (int i = 0; i < MATRIX_CHAR_COUNT; i++) {
    matrixChars.emplace_back(1, static_cast<char>(charDis(gen)));
    matrixPositions.emplace_back(posDis(gen), posDis(gen));
    matrixSpeeds.emplace_back(speedDis(gen));
    matrixAlphas.emplace_back(1.0f);
  }

  matrixInitialized = true;
}

void RenderMatrixEffect(ImDrawList* drawList, const ImVec2& windowPos,
                        const ImVec2& winSize) {
  if (!matrixInitialized || !enableMatrixEffect) return;

  const float deltaTime = ImGui::GetIO().DeltaTime;
  const float time = ImGui::GetTime();

  for (size_t i = 0; i < matrixChars.size(); i++) {
    matrixPositions[i].y += matrixSpeeds[i] * deltaTime;

    if (matrixPositions[i].y > winSize.y + 20) {
      matrixPositions[i].y = MATRIX_RESPAWN_Y_OFFSET;
      matrixPositions[i].x =
          static_cast<float>(rand() % static_cast<int>(winSize.x));
      matrixAlphas[i] = 1.0f;
    }

    float alpha = 1.0f - (matrixPositions[i].y / winSize.y);
    alpha = std::max(0.0f, alpha);
    matrixAlphas[i] =
        alpha * (MATRIX_ALPHA_MULTIPLIER +
                 MATRIX_ALPHA_MULTIPLIER * sinf(time * 2.0f + i * 0.1f));

    ImU32 color =
        ImColor(30, 120, 180, static_cast<int>(255 * matrixAlphas[i]));
    drawList->AddText(ImVec2(windowPos.x + matrixPositions[i].x,
                             windowPos.y + matrixPositions[i].y),
                      color, matrixChars[i].c_str());
  }
}

void RenderTabs(int& current_tab, const ImVec2& winSize) {
  const float spacing = ImGui::GetStyle().ItemSpacing.x;
  const float totalTabsWidth =
      TAB_WIDTH * TAB_COUNT + spacing * (TAB_COUNT - 1);
  const float startX = (winSize.x - totalTabsWidth) * 0.5f;

  ImGui::BeginChild("Tabs", ImVec2(winSize.x, 30), false, 0);
  ImGui::SetCursorPosX(startX);

  for (int i = 0; i < TAB_COUNT; i++) {
    if (i > 0) ImGui::SameLine(0, spacing);

    const bool selected = (current_tab == i);
    ImGui::PushStyleColor(ImGuiCol_Button,
                          selected ? ImVec4(0.1f, 0.3f, 0.5f, 0.6f)
                                   : ImVec4(0.06f, 0.06f, 0.10f, 0.4f));
    ImGui::PushStyleColor(ImGuiCol_Text, selected
                                             ? ImVec4(0.2f, 0.6f, 0.8f, 1.0f)
                                             : ImVec4(0.6f, 0.6f, 0.6f, 0.9f));

    if (ImGui::Button(TAB_NAMES[i], ImVec2(TAB_WIDTH, 25))) {
      current_tab = i;
    }

    ImGui::PopStyleColor(2);
  }
  ImGui::EndChild();
}

void RenderAimTab() {
  ImGui::Text("Aimbot Settings");
  ImGui::Checkbox("Enable Aimbot", &globals::Aimbot);
  ImGui::SameLine(0,30);
  HelpMarker("Toggle the aimbot functionality");
  if (ImGui::Checkbox("Show Crosshair", &globals::CrossHair)) {
    // The state is automatically updated
  }
 

  ImGui::Checkbox("Enable Smoothing", &globals::AimbotUseSmoothing);
  ImGui::SameLine(0,15);
  HelpMarker("Makes aimbot movement look more human-like");

  if (globals::AimbotUseSmoothing) {
    ImGui::SliderFloat("Smooth Amount", &globals::AimbotSmoothAmount, 0.01f,
                       1.0f, "%.2f");
    ImGui::Spacing();
    ImGui::Spacing();
    
    ImGui::SameLine();
    HelpMarker("Higher values = smoother but slower aiming");
  }
  ImGui::Checkbox("Fov Circle", &globals::FovCircle);
  ImGui::SliderFloat("FOV Size", &globals::AimbotFovSize, 1.0f, 500.0f, "%.1f");
  ImGui::SameLine();
  HelpMarker("Field of view for target detection");
  // Add FOV style selector
  if (globals::FovCircle) {
    ImGui::Combo("FOV Style", &globals::FovStyle,
                 "Animated\0Simple\0Advanced\0");
  }
  ImGui::SliderFloat("Max Distance", &globals::AimbotMaxDistance, 0.0f, 5000.0f,
                     "%.1f units");

  HelpMarker("Maximum distance to target enemies");

  ImGui::Combo("Aim Bone", &globals::AimbotBone, "Head\0Neck\0Chest\0Body\0");

  HelpMarker("Which body part to aim at");

  // Key binding for aimbot
  ImGui::Text("Aim Key: ");


  static bool bindingKey = false;
  static char keyName[32];

  // Function to convert key code to name
  auto GetKeyName = [](int key) -> const char* {
    switch (key) {
      case VK_LBUTTON:
        return "Left Mouse";
      case VK_RBUTTON:
        return "Right Mouse";
      case VK_MBUTTON:
        return "Middle Mouse";
      case VK_XBUTTON1:
        return "Mouse Side 1";
      case VK_XBUTTON2:
        return "Mouse Side 2";
      case VK_SHIFT:
        return "Shift";
      case VK_CONTROL:
        return "Ctrl";
      case VK_MENU:
        return "Alt";
      case VK_SPACE:
        return "Space";
      case VK_CAPITAL:
        return "Caps Lock";
      case VK_TAB:
        return "Tab";
      case VK_RETURN:
        return "Enter";
      case VK_ESCAPE:
        return "Escape";
      case VK_BACK:
        return "Backspace";
      case VK_DELETE:
        return "Delete";
      default:
        // For letter keys
        if (key >= 'A' && key <= 'Z') {
          snprintf(keyName, sizeof(keyName), "%c", key);
          return keyName;
        }
        // For number keys
        else if (key >= '0' && key <= '9') {
          snprintf(keyName, sizeof(keyName), "%c", key);
          return keyName;
        }
        // For function keys
        else if (key >= VK_F1 && key <= VK_F12) {
          snprintf(keyName, sizeof(keyName), "F%d", key - VK_F1 + 1);
          return keyName;
        }
        return "Unknown";
    }
  };

  // Display current key and BIND button
  ImGui::Text("%s", GetKeyName(globals::AimbotKey));
  ImGui::SameLine();

  if (ImGui::Button("BIND", ImVec2(60, 0))) {
    bindingKey = true;
  }

if (bindingKey) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Press any key...");

    // Check for ALL key presses including mouse buttons
    for (int key = 1; key < 256; key++) {  
        if (GetAsyncKeyState(key) & 0x8000) {
            globals::AimbotKey = key;
            bindingKey = false;
            break;
        }
    }
}

  ImGui::SameLine();
  HelpMarker("Click BIND and press any key to set aimbot activation");




}

void RenderVisualsTab() {
  ImGui::Text("Visual Settings");
  ImGui::Spacing();
  ImGui::Spacing();

  static bool showSection[SECTION_COUNT] = {false};
  const float spacing = ImGui::GetStyle().ItemSpacing.x + 20.0f;
  const float totalButtonsWidth =
      SECTION_COUNT * SECTION_BUTTON_WIDTH + (SECTION_COUNT - 1) * spacing;
  const float startBtnX = (ImGui::GetWindowWidth() - totalButtonsWidth) * 0.5f;
  const float buttonsY = ImGui::GetCursorPosY() + 60.0f;

  ImVec2 buttonPositions[SECTION_COUNT];

  // Render section buttons
  for (int i = 0; i < SECTION_COUNT; i++) {
    ImGui::SetCursorPosX(startBtnX + i * (SECTION_BUTTON_WIDTH + spacing));
    ImGui::SetCursorPosY(buttonsY);
    buttonPositions[i] = ImGui::GetCursorPos();

    if (ImGui::Button(SECTION_NAMES[i], ImVec2(SECTION_BUTTON_WIDTH, 25))) {
      showSection[i] = !showSection[i];
    }
  }

  // Render checkboxes
  const float checkboxOffsetY = -30.0f;
  float espX =
      (buttonPositions[0].x + buttonPositions[1].x + SECTION_BUTTON_WIDTH) *
          0.5f -
      10.0f;
  ImGui::SetCursorPosX(espX);
  ImGui::SetCursorPosY(buttonsY + checkboxOffsetY);
  ImGui::Checkbox("ESP", &globals::Esp);

  for (int i = 2; i < SECTION_COUNT; i++) {
    ImGui::SetCursorPosX(buttonPositions[i].x);
    ImGui::SetCursorPosY(buttonsY + checkboxOffsetY);
    const char* labels[] = {"Bones", "Health Bars", "Head Marker"};
    bool* values[] = {&globals::Bones, &globals::HealthBar,
                      &globals::Headmarker};
    ImGui::Checkbox(labels[i - 2], values[i - 2]);
  }

  // Render section popups
  for (int i = 0; i < SECTION_COUNT; i++) {
    if (!showSection[i]) continue;

    float popupX = buttonPositions[i].x +
                   (SECTION_BUTTON_WIDTH - SECTION_POPUP_WIDTH) * 0.5f;
    float popupY = buttonsY + 40.0f;

    ImGui::SetCursorPosX(popupX);
    ImGui::SetCursorPosY(popupY);

    ImGui::BeginChild(SECTION_NAMES[i],
                      ImVec2(SECTION_POPUP_WIDTH, SECTION_POPUP_HEIGHT), true,
                      ImGuiWindowFlags_NoMove);

    switch (i) {
      case 0:  // Enemy Settings
        ImGui::Checkbox("Enemy EspBox", &globals::EnemyEsp);
        ImGui::Checkbox("Corner Style", &globals::CornerEspBoxStyle);
        ImGui::Checkbox("Enemy Name ESP", &globals::EnemyNameEsp);
        ImGui::Checkbox("Enemy Distance ESP", &globals::EnemyDistanceEsp);
        ImGui::Spacing();
        ImGui::Text("Enemy Colors:");
        SafeColorEdit4("Box Color", globals::EnemyEspColor);
        SafeColorEdit4("Background Color", globals::EnemyEspBackGroundColor);
        break;

      case 1:  // Teammate Settings
        ImGui::Checkbox("Friendly EspBox", &globals::TeammateEsp);
        ImGui::Checkbox("Friendly Name ESP", &globals::TeammateNameEsp);
        ImGui::Checkbox("Friendly Distance ESP", &globals::TeammateDistanceEsp);
        ImGui::Spacing();
        ImGui::Text("Teammate Colors:");
        SafeColorEdit4("Box Color", globals::TeammateEspColor);
        SafeColorEdit4("Background Color", globals::FriendlyEspBackGroundColor);
        break;

      case 2:  // Bones Settings
        ImGui::Checkbox("Enemy Bones", &globals::EnemyBones);
        ImGui::Checkbox("Friendly Bones", &globals::FriendlyBones);
        ImGui::SliderFloat("Bone Thickness", &globals::BoneEspThickness, 0.0f,
                           10.0f, "%.1f");
        ImGui::Checkbox("Bone Debug", &globals::BoneDebug);
        ImGui::Spacing();
        ImGui::Text("Bone Colors:");
        SafeColorEdit4("Enemy Bones", globals::EnemyBoneColor);
        SafeColorEdit4("Friendly Bones", globals::TeammateBoneColor);
        break;

      case 3:  // Health Settings
        ImGui::Checkbox("Enemy Health Bar", &globals::EnemyHealth);
        ImGui::Checkbox("Enemy Health Text", &globals::EnemyHealthText);
        ImGui::Checkbox("Team Health Bar", &globals::TeammateHealth);
        ImGui::Checkbox("Team Health Text", &globals::TeammateHealthTxt);
        ImGui::Spacing();
        ImGui::Text("Health Colors:");
        SafeColorEdit4("Enemy Health", globals::EnemyHealthColor);
        SafeColorEdit4("Team Health", globals::TeammateHealthColor);
        break;

      case 4:  // Global Settings
        ImGui::Checkbox("Name ESP", &globals::NameEsp);
        ImGui::Checkbox("Distance ESP", &globals::DistanceEsp);
        ImGui::Checkbox("ESP Background", &globals::PlayerEspBackGround);
        ImGui::Spacing();
        SafeColorEdit4("Name Color", globals::PlayerNameColor);
        SafeColorEdit4("Distance Color", globals::PlayerDistanceColor);
        SafeColorEdit4("Head Marker Color", globals::headMakerColor);
        ImGui::SliderFloat("Head Marker Size", &globals::Headmarkersize, 1.0f,
                           10.0f, "%.1f");
        break;
    }

    ImGui::EndChild();
  }
}

void RenderMiscTab() {
  ImGui::Text("Miscellaneous Settings");
  ImGui::Spacing();
  ImGui::Checkbox("FPS Counter", &globals::FpsCounter);
  ImGui::Checkbox("Water Mark", &globals::WaterMark);
  ImGui::Checkbox("Matrix Effect", &enableMatrixEffect);

  ImGui::Spacing();
  ImGui::Spacing();
  ImGui::Text("Performance:");
  ImGui::BulletText("FPS: %.0f", ImGui::GetIO().Framerate);
  ImGui::BulletText("Frame Time: %.2f ms", 1000.0f / ImGui::GetIO().Framerate);
}

void RenderConfigTab() {
  ImGui::Text("Configuration");
  ImGui::Spacing();

  const float buttonWidth = 120.0f;
  if (ImGui::Button("Save Config", ImVec2(buttonWidth, 25))) {
    // Save config implementation
  }
  ImGui::SameLine();
  if (ImGui::Button("Load Config", ImVec2(buttonWidth, 25))) {
    // Load config implementation
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset Config", ImVec2(buttonWidth, 25))) {
    // Reset config implementation
  }

  ImGui::Spacing();
  ImGui::Text("Mouse Sensitivity:");
  ImGui::SliderFloat("##Sensitivity", &globals::MouseSensitivity, 0.1f, 10.0f,
                     "%.1f");
}

void RenderMenu() {
  try {
    if (!globals::menu_open || !ImGui::GetCurrentContext()) return;

    if (!matrixInitialized) InitMatrixEffect();
    Styles::Apply();

    const ImVec2 prevMainWindowPos = mainWindowPos;

    if (ImGui::Begin("Phil.9", &globals::menu_open, MAIN_WINDOW_FLAGS)) {
      mainWindowPos = ImGui::GetWindowPos();
      mainWindowMoved = (prevMainWindowPos.x != mainWindowPos.x ||
                         prevMainWindowPos.y != mainWindowPos.y);

      ImDrawList* drawList = ImGui::GetWindowDrawList();
      const ImVec2 winSize = ImGui::GetWindowSize();

      RenderMatrixEffect(drawList, mainWindowPos, winSize);

      ImGui::SetCursorPos(ImVec2(20, 30));
      LogoHelper::Render();

      static int current_tab = 0;
      RenderTabs(current_tab, winSize);

      ImGui::SetCursorPos(ImVec2(10, 160));
      ImGui::BeginChild("Content", ImVec2(winSize.x - 20, winSize.y - 170),
                        true, 0);

      drawList->AddRectFilled(
          ImGui::GetWindowPos(),
          ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(),
                 ImGui::GetWindowPos().y + ImGui::GetWindowHeight()),
          ImColor(8, 8, 12, 100), 4.0f);

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 8));
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 0.95f));

      switch (current_tab) {
        case 0:
          RenderAimTab();
          break;
        case 1:
          RenderVisualsTab();
          break;
        case 2:
          RenderMiscTab();
          break;
        case 3:
          RenderConfigTab();
          break;
      }

      ImGui::PopStyleColor();
      ImGui::PopStyleVar();
      ImGui::EndChild();

      ImGui::End();
      mainWindowMoved = false;
    } else {
      ImGui::End();
    }

    ImGui::PopStyleVar(8);
    ImGui::PopStyleColor(6);

  } catch (const std::exception& e) {
    OutputDebugStringA(
        ("Menu::RenderMenu exception: " + std::string(e.what()) + "\n")
            .c_str());
  } catch (...) {
    OutputDebugStringA("Menu::RenderMenu unknown exception\n");
  }
}
}  // namespace Menu