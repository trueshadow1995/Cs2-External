#pragma once
#include "../ImGui/imgui.h"

// Wrapper namespaces for easy access to draw lists
namespace DrawLists {
    inline ImDrawList* Background() { return ImGui::GetBackgroundDrawList(); }
    inline ImDrawList* Foreground() { return ImGui::GetForegroundDrawList(); }
    inline ImDrawList* Window() { return ImGui::GetWindowDrawList(); }
}