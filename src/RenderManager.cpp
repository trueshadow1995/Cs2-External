#include "../Headers/RenderManager.h"
#include "../ImGui/imgui.h"
#include "../Headers/WaterMarkHelper.h"
#include "../Headers/DX11Renderer.h"
#include "../Headers/LogoHelper.h"
#include "../Headers/HealBarHelper.h"






namespace RenderManager {
void RenderFrame() {
    // Example menu
  WaterMark::Render();
   
    ImGui::End();
  
    // ESP drawing would go here (lines, boxes, etc.)
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    draw->AddCircle(ImVec2(960, 540), 50.f, IM_COL32(255, 0, 0, 255), 64, 2.f);
}

}  // namespace RenderManager