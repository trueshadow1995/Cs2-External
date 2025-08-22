#pragma once
#include <windows.h>

namespace OverlayWindow {
HWND CreateOverlayWindow(HINSTANCE hInstance, int width, int height);
void SetClickThrough(HWND hwnd, bool enable);
}  // namespace OverlayWindow