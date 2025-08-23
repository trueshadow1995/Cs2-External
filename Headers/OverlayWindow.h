#pragma once
#include <windows.h>
namespace OverlayWindow { // namespace for overlay window functions
HWND CreateOverlayWindow(HINSTANCE hInstance, int width, int height); // create overlay window



void SetClickThrough(HWND hwnd, bool enable); // true = menu open, false = menu closed Imgui menu click logic


} 