#include <dwmapi.h>
#include <tchar.h>
#include <tlhelp32.h>  // for process snapshot
#include <windows.h>

#include "../Headers/DX11Renderer.h"
#include "../Headers/Globals.h"
#include "../Headers/ImGuiManager.h"
#include "../Headers/LogoHelper.h"
#include "../Headers/Menu.h"
#include "../Headers/MouseFunc.h"
#include "../Headers/OverlayWindow.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../Headers/Memory.h"
#include "../Headers/RenderManager.h"


bool globals::menu_open = true; //menu open flag
bool running = true; //main loop flag

HWND FindWindowByProcess(const char* processName) {  //find window by processname
  DWORD pid = 0;

  // snapshot all processes
  HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //snapshot of all processes
  if (snap != INVALID_HANDLE_VALUE) { //valid snapshot
    PROCESSENTRY32 pe{}; //process entry struct
    pe.dwSize = sizeof(pe); //size of struct

    if (Process32First(snap, &pe)) { //get first process
      do {
        if (_stricmp(pe.szExeFile, processName) == 0) { //compare process names case insensitive apparently
          pid = pe.th32ProcessID; //get pid
          break; //process found
        }
      } while (Process32Next(snap, &pe)); //iterate processes
    }
    CloseHandle(snap);
  } //snapshot all processes

  if (!pid) return nullptr; //process not found

 
  struct EnumData { //struct for enum data
    DWORD pid;
    HWND hwnd;
  } data{pid, nullptr}; //struct for enum data :D

  EnumWindows(
      [](HWND h, LPARAM l) -> BOOL { //enum windows callback
        EnumData* data = reinterpret_cast<EnumData*>(l); //get enum data
        DWORD winPid; //window process id
        GetWindowThreadProcessId(h, &winPid); //get window process id
        if (winPid == data->pid && IsWindowVisible(h)) { //check if window belongs to process and is visible
          data->hwnd = h; //found window
          return FALSE;   //stop enumaration here if window found
        }
        return TRUE;   //continue enumeration
      },
      reinterpret_cast<LPARAM>(&data)); //enum windows

  return data.hwnd;
} //find window by process

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT) { //main

  const auto client = mem.GetModuleAddress("client.dll");
  const auto engine = mem.GetModuleAddress("engine.dll");
    
    HWND gameHwnd = FindWindowByProcess("cs2.exe"); //find cs2 window
  if (!gameHwnd) {
    MessageBox(nullptr, "cs2.exe window not found!", "Error", MB_ICONERROR); // error if not found
    return -1;
  }

  // create overlay
  HWND hwnd = OverlayWindow::CreateOverlayWindow(hInstance, 1920, 1080);
  if (!hwnd) {
    MessageBox(nullptr, "Overlay creation failed!", "Error", MB_ICONERROR); //error if not created dx11 that is
    return -1;
  }

  // init DX11 + ImGui
  if (!DX11Renderer::Init(hwnd)) {
    MessageBox(nullptr, "DX11 Initialization failed!", "Error", MB_ICONERROR); //error if dx 11 init failed / not initialized or supported incase someone has a potate LOL :D - phil auto pilot be funny
    return -1;
  }
  ImGuiManager::Init(hwnd); // init imgui

  MSG msg{};//window message struct
  bool lastInsert = false; //last insert key state

  while (running) { // main loop
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { //message loop
      TranslateMessage(&msg); //translate message
      DispatchMessage(&msg);//dispatch message
      if (msg.message == WM_QUIT) running = false; // quit if quit message.
    }

    // toggle menu
    bool insertPressed = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
    if (insertPressed && !lastInsert) {
      globals::menu_open = !globals::menu_open;
      ToggleOverlayInput(hwnd, globals::menu_open);
    }
    lastInsert = insertPressed;

    // exit
    if (GetAsyncKeyState(VK_DELETE) & 0x8000) running = false;

    // start imgui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // update IO
    ImGuiIO& io = ImGui::GetIO();
    RECT rc{};
    POINT xy{};// client rect and point 
    GetClientRect(hwnd, &rc); //get client rect
    ClientToScreen(hwnd, &xy); //client to screen

   
    POINT p; //point for mouse pos
    GetCursorPos(&p); 
    io.MousePos.x = p.x - xy.x;
    io.MousePos.y = p.y - xy.y;
    io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0; //left mouse button state
  
     
    RenderManager::RenderFrame();
    ImGui::Render();
    ImGuiManager::EndFrame();
  }

  ImGuiManager::Shutdown();
  DX11Renderer::Shutdown();
  DestroyWindow(hwnd);
  return 0;
}