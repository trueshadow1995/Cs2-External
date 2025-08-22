#pragma once
#include "../Headers/Menu.h"
#include "../Headers/LogoHelper.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

namespace Menu {

void Render() {
    if (!globals::menu_open) return; // render only if menu is open
    ImGui::SetNextWindowSize({700, 450});
    ImVec4 borderColor = ImColor::HSV(ImGui::GetTime() / 10, 0.6f, 0.6f).Value;
    ImVec4 borderGradientColorTopLeft = ImVec4(borderColor.x * 0.5f, borderColor.y * 0.5f, borderColor.z * 0.5f, borderColor.w);
    ImVec4 borderGradientColorBottomRight = ImVec4(borderColor.x * 0.7f, borderColor.y * 0.7f, borderColor.z * 0.7f, borderColor.w);
     
    ImGui::Begin("phil9", &globals::menu_open, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
   
    // Left tab bar
    ImGui::BeginChild("TabBar", ImVec2(128, ImGui::GetContentRegionAvail().y), true, ImGuiWindowFlags_NoScrollbar);
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
  
    // Right content
    ImGui::SameLine();
    ImGui::BeginChild("TabContent", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true);

    switch (current_tab) {
        case 0: // Aimbot
            ImGui::Checkbox("Fov circle", &globals::Fov);
            ImGui::SameLine(0, 70);
            ImGui::ColorEdit4("Fov Circle Color", globals::FovColor, ImGuiColorEditFlags_NoInputs);
            ImGui::SliderFloat("AimbotFov", &globals::AimbotFovSize, 0.f, 90.f, "%.0f");
            break;
        case 1: // ESP
            ImGui::Checkbox("Enemy Player ESP", &globals::EnemyEsp);
            ImGui::SameLine(220);
            ImGui::ColorEdit4("Enemy ESP Color", globals::EnemyEspColor, ImGuiColorEditFlags_NoInputs);
            ImGui::Checkbox("Teammate ESP", &globals::TeammateEsp);
            ImGui::SameLine(220);
            ImGui::ColorEdit4("Friendly ESP Color", globals::TeammateEspColor, ImGuiColorEditFlags_NoInputs);
            break;
        case 2: // Chams/Glow
            break;
        case 3: // Features
            ImGui::Checkbox("FPS Counter", &globals::FpsCounter);
            ImGui::Checkbox("Water Mark", &globals::WaterMark);
            break;
    }

    ImGui::EndChild();
    ImGui::End();
}

} // namespace Menu