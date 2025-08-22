
#pragma once
#include "../Headers/Globals.h"

#include "../Headers/Memory.h"
#include "../ImGui/imgui.h"






namespace FpsCounter {

inline void Render() {
  if (globals::FpsCounter) {
    ImDrawList* fps = ImGui::GetBackgroundDrawList();
    int Framerate = round(ImGui::GetIO().Framerate);
    std::string StatStrings = "FPS: " + std::to_string(Framerate);
    void get_fps();

    fps->AddText(ImGui::GetFont(), 15, ImVec2(50, 50), ImColor(255, 255, 255),
                 StatStrings.c_str());
  }
}

}  // namespace Watermark
