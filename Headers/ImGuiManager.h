#pragma once
#include <windows.h>

namespace ImGuiManager {
void Init(HWND hwnd);
void BeginFrame();
void EndFrame();
void Shutdown();
}  // namespace ImGuiManager