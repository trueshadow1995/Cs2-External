#include <dwmapi.h>
#include <tchar.h>
#include <windows.h>
#include "../Headers/DX11Renderer.h"
#include "../Headers/DX11Renderer.h"
#include "../Headers/Globals.h"
#include "../Headers/ImGuiManager.h"
#include "../Headers/LogoHelper.h"
#include "../Headers/Menu.h"
#include "../Headers/MouseFunc.h"
#include "../Headers/OverlayWindow.h"
#include "../Headers/RenderManager.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

bool globals::menu_open = true;
bool running = true;

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT) {
  // Find target game window
  HWND gameHwnd = FindWindow("SDL_app", nullptr);
  if (!gameHwnd) {
    MessageBox(nullptr, "Counter-Strike 2 window not found!", "Error",
               MB_ICONERROR);
    return -1;
  }
  
  // Create overlay
  HWND overlayHwnd = OverlayWindow::CreateOverlayWindow(hInstance, 1920, 1080);
  if (!DX11Renderer::Init(overlayHwnd)) {
    MessageBox(nullptr, "DX11 Initialization failed!", "Error", MB_ICONERROR);
    return -1;
  }

  ImGuiManager::Init(overlayHwnd);

  MSG msg{};
  bool lastInsert = false;

  while (running) {
    // Handle Windows messages
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT) running = false;
    }

    // Toggle menu with INSERT
    bool insertPressed = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
    if (insertPressed && !lastInsert) {
      globals::menu_open = !globals::menu_open;
      ToggleOverlayInput(overlayHwnd, globals::menu_open);
    }
    lastInsert = insertPressed;

    // Exit on DELETE
    if (GetAsyncKeyState(VK_DELETE) & 0x8000) running = false;

    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    RECT rc{};
    POINT xy{};
    GetClientRect(overlayHwnd, &rc);
    ClientToScreen(overlayHwnd, &xy);
    rc.left = xy.x;
    rc.top = xy.y;

    io.DeltaTime = 1.0f / 60.0f;
    POINT p;
    GetCursorPos(&p);
    io.MousePos.x = p.x - xy.x;
    io.MousePos.y = p.y - xy.y;

    io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
 
    // Render overlay ESP / visuals
    RenderManager::RenderFrame();
  ; 
    // Render menu if toggled
    if (globals::menu_open) {
      Menu::Render();        // Your ImGui menu with tabs/buttons
     
    }

    // End ImGui frame
    ImGui::Render();
    ImGuiManager::EndFrame();
  }

  // Shutdown
  ImGuiManager::Shutdown();
  DX11Renderer::Shutdown();
  DestroyWindow(overlayHwnd);
  return 0;
}
