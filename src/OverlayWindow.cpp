#include "../Headers/OverlayWindow.h"

#include <dwmapi.h>
#include <windows.h>
#include "../Headers/OverlayWindow.h"
bool CounterStrike2 = "cs2.exe";

HWND OverlayWindow::CreateOverlayWindow(HINSTANCE hInstance, int width,
                                        int height) {
  WNDCLASSEX wc{};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = DefWindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "OverlayClass";
  RegisterClassEx(&wc);

  HWND hwnd = CreateWindowEx(
      WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
      wc.lpszClassName, "Overlay", WS_POPUP, 0, 0, width, height, nullptr,
      nullptr, hInstance, nullptr);

  SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

  MARGINS margins{-1};
  DwmExtendFrameIntoClientArea(hwnd, &margins);

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  // Align overlay with game window
  HWND gameHWND = FindWindow("Counter-Strike 2", nullptr);
  if (gameHWND) {
    RECT rc{};
    POINT topLeft{};
    GetClientRect(gameHWND, &rc);
    ClientToScreen(gameHWND, &topLeft);
    SetWindowPos(hwnd, HWND_TOPMOST, topLeft.x, topLeft.y, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_SHOWWINDOW);
  }

  return hwnd;
}


void OverlayWindow::SetClickThrough(HWND hwnd, bool menuOpen) {
  LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

  if (menuOpen) {
    // Menu visible → allow mouse input
    exStyle &= ~WS_EX_TRANSPARENT;
  } else {
    // Menu hidden → pass clicks through
    exStyle |= WS_EX_TRANSPARENT;
  }

  SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
}