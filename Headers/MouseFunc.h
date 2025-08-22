#include <WinUser.h>
#include <wtypes.h>





void ToggleOverlayInput(HWND hwnd, bool enable) {
  LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
  if (enable) {
    // Menu open: capture input
    exStyle &= ~WS_EX_TRANSPARENT;  // remove transparent flag
    exStyle |= WS_EX_LAYERED;       // keep layered
  } else {
    // Menu hidden: let input pass through to game
    exStyle |= WS_EX_TRANSPARENT;
    exStyle |= WS_EX_LAYERED;
  }
  SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
  RedrawWindow(hwnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
}
#pragma once
