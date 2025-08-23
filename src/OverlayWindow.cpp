#include "../Headers/OverlayWindow.h"

#include <dwmapi.h>
#include <windows.h>
#include "../Headers/OverlayWindow.h"


HWND OverlayWindow::CreateOverlayWindow(HINSTANCE hInstance, int width,
                                        int height) { // create overlay window
  WNDCLASSEX wc{}; // window class struct
  wc.cbSize = sizeof(WNDCLASSEX); // size of struct
  wc.lpfnWndProc = DefWindowProc; // default window procedure
  wc.hInstance = hInstance; // application instance
  wc.lpszClassName = "OverlayClass"; // window class name
  RegisterClassEx(&wc); // register window class

  HWND hwnd = CreateWindowEx( 
      WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
      wc.lpszClassName, "Overlay", WS_POPUP, 0, 0, width, height, nullptr,
      nullptr, hInstance, nullptr); // create window with extended styles

  SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA); // set window to be fully transparent

  MARGINS margins{-1};
  DwmExtendFrameIntoClientArea(hwnd, &margins); // extend frame into client area for blur effect i guess 

  ShowWindow(hwnd, SW_SHOW); // show window
  UpdateWindow(hwnd); // update window

  // Align overlay with game window
  HWND gameHWND = FindWindow("Counter-Strike 2", nullptr); // find game window by its title
  if (gameHWND) {//if game window found
    RECT rc{}; // rect struct
    POINT topLeft{}; //point struct
    GetClientRect(gameHWND, &rc); //get client rect of game window
    ClientToScreen(gameHWND, &topLeft); // client to screen
    SetWindowPos(hwnd, HWND_TOPMOST, topLeft.x, topLeft.y, rc.right - rc.left,
                 rc.bottom - rc.top, SWP_SHOWWINDOW); //set overlay position and size to match game window
  }

  return hwnd; // return overlay window handle
}


void OverlayWindow::SetClickThrough(HWND hwnd, bool menuOpen) { // true = menu open, false = menu closed simple enough 
  LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE); // get current extended style

  if (menuOpen) {  // if menu is open
    // Menu visible → allow mouse input
    exStyle &= ~WS_EX_TRANSPARENT;
  } else {
    // Menu hidden → pass clicks through
    exStyle |= WS_EX_TRANSPARENT;
  }

  SetWindowLong(hwnd, GWL_EXSTYLE, exStyle); //set new extended style
}