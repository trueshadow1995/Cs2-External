#pragma once
#include <windows.h>

namespace ImGuiManager { 
void Init(HWND hwnd); // initialize imgui
void BeginFrame(); // begin imgui frame
void EndFrame(); // end imgui frame
void Shutdown(); // shutdown imgui
}  // namespace ImGuiManager