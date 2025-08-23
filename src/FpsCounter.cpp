#pragma once
#include <cmath>  // for round
#include <string>

#include "../Headers/Globals.h"
#include "../ImGui/imgui.h"

namespace FpsCounter {

inline void Render() {
  if (!globals::FpsCounter) return; 

  // Get ImGui's background draw list
  ImDrawList* drawList = ImGui::GetBackgroundDrawList();
  

  // Get the current FPS
  int fps = static_cast<int>(std::round(ImGui::GetIO().Framerate));

  // Build the display string
  std::string fpsText = "FPS: " + std::to_string(fps);

  // Optional: position on screen
  ImVec2 position(50.0f, 50.0f);

  // Draw the FPS text
  drawList->AddText(ImGui::GetFont(), ImGui::GetFont()->FontSize, position,
                    IM_COL32(255, 255, 255, 255), fpsText.c_str());
}

}  // namespace FpsCounter