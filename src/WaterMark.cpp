
#include "../Headers/WaterMarkHelper.h"

#include <cmath>  // for sinf

#include "../Headers/Globals.h"
#include "../ImGui/imgui.h"
#include "../Headers/Memory.h"
 

namespace WaterMark {
void Render() {
  if (!globals::WaterMark) return;  

  const char* watermarkText = "Phil was here | priv secret cheat";
  ImVec2 textSize = ImGui::CalcTextSize(watermarkText);

  ImVec2 screenSize = ImGui::GetIO().DisplaySize;
  const float padding = 8.0f;
  const ImVec2 backgroundPos(screenSize.x - textSize.x - 2.0f * padding - 16.0f,
                             16.0f);

  const ImVec2 backgroundSize(textSize.x + 2.0f * padding,
                              textSize.y + padding + 4.0f);
  const float cornerRadius = 6.0f;

  ImVec4 bgColor(0.1f, 0.1f, 0.1f, 0.85f);

  ImDrawList* drawList = ImGui::GetBackgroundDrawList();
  drawList->AddRectFilled(backgroundPos,
                          ImVec2(backgroundPos.x + backgroundSize.x,
                                 backgroundPos.y + backgroundSize.y),
                          ImColor(bgColor), cornerRadius);

  float time = ImGui::GetTime();
  ImVec4 textColor(0.6f + 0.4f * sinf(time), 0.6f + 0.4f * sinf(time + 2.094f),
                   0.6f + 0.4f * sinf(time + 4.188f), 1.0f);

  ImVec2 shadowPos(backgroundPos.x + padding + 1.0f,
                   backgroundPos.y + padding / 2.0f + 1.0f);
  ImVec2 textPos(backgroundPos.x + padding, backgroundPos.y + padding / 2.0f);

  drawList->AddText(shadowPos, IM_COL32(0, 0, 0, 128), watermarkText);
  drawList->AddText(textPos, ImColor(textColor), watermarkText);
}
}  // namespace WaterMark
