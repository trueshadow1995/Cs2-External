#include "../Headers/OverlayWindow.h"

#include <d3d11.h>
#include <dwmapi.h>
#include <windows.h>
#include "../Headers/EspHelper.h"
#include <iostream>
#include <thread>
#include "../Headers/LogoHelper.h"
#include "../Headers/RenderManager.h"
#include "../Headers/Styles.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../Headers/Menu.h"
#include "../Headers/Globals.h"
#include "../Headers/EspHelper.h"
#include "../Headers/BoneHelper.h"
#include "../Headers/WaterMarkHelper.h"
#include "../Headers/HealBarHelper.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT CALLBACK Overlay::WindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return 1L;

  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0L;
    case WM_SIZE:
      // Handle window resize if needed
      return 0L;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

Overlay::Overlay(Memory& memory)
    : mem(memory),  // Initialize the Memory reference first
      hInstance(GetModuleHandle(nullptr)),
      hwnd(nullptr),
      running(true),
      swapChain(nullptr),
      device(nullptr),
      deviceContext(nullptr),
      renderTargetView(nullptr),
      bbWidth(1920),
      bbHeight(1080),
      clientBase(0),
      entityManager(nullptr) {
  std::cout << "[DEBUG] Overlay constructor called with Memory reference\n";
}

Overlay::~Overlay() {
  if (entityManager) delete entityManager;
  Shutdown();
}


inline void PrintColored(const std::string& msg, WORD color) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, color);
  std::cout << msg << std::endl;
  SetConsoleTextAttribute(hConsole, 7);  // Reset
}

void Overlay::InitConsole() {
  AllocConsole();
  FILE* file;
  freopen_s(&file, "CONOUT$", "w", stdout);
  freopen_s(&file, "CONOUT$", "w", stderr);

  // Cool debug examples
  PrintColored("[DEBUG] 🟢 Overlay console initialized!",
               FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  PrintColored("[INFO] 🔵 Waiting for game window...",
               FOREGROUND_BLUE | FOREGROUND_INTENSITY);
  PrintColored("[WARN] 🟡 Some warnings here",
               FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
  PrintColored("[ERROR] 🔴 Something went wrong!",
               FOREGROUND_RED | FOREGROUND_INTENSITY);
}
bool Overlay::Init() {
  InitConsole();
  PrintColored("[WARN] Setting up Overlay \n",
               FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

  // Try to find game window
  hwnd = FindWindowA(nullptr, "Counter-Strike 2");
  if (!hwnd) {
    PrintColored("[ERROR] Game Window Not Found!",
                 FOREGROUND_RED | FOREGROUND_INTENSITY);
    return false;
  }
  // Set clientBase from global
  clientBase = globals::clientAddress;

  std::cout << "[DEBUG] Client base from global: 0x" << std::hex << clientBase
            << std::dec << std::endl;

  // If global is still 0, try to get it directly
  if (clientBase == 0) {
    std::cout << "[DEBUG] Global client address is 0, trying direct read...\n";
    clientBase =
        mem.GetModuleAddress("client.dll");  // or mem.GetHandle("client.dll")
    std::cout << "[DEBUG] Direct client base: 0x" << std::hex << clientBase
              << std::dec << std::endl;
  }

  // Check if clientBase is valid
  if (clientBase == 0) {
    std::cout << "[ERROR] Client base is null in Overlay::Init!\n";
    return false;
  }

  // Create entity manager with the client base
  entityManager = new DataManager(mem, clientBase);

  // Register window class
  WNDCLASSEX wc{};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "Phil was also here";
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

  if (!RegisterClassEx(&wc)) return false;

  RECT gameRect{0, 0, 1920, 1080};
  GetWindowRect(hwnd, &gameRect);
  int width = gameRect.right - gameRect.left;
  int height = gameRect.bottom - gameRect.top;

  hwnd = CreateWindowEx(
      WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
      wc.lpszClassName, "Phil was here", WS_POPUP, gameRect.left, gameRect.top, width,
      height, nullptr, nullptr, hInstance, nullptr);
  if (!hwnd) return false;

  SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
  MARGINS margins{-1, -1, -1, -1};
  DwmExtendFrameIntoClientArea(hwnd, &margins);
  ShowWindow(hwnd, SW_SHOW);

  if (!CreateDX11()) return false;

  ImGui::CreateContext();
  Styles::Apply();
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(device, deviceContext);
  LogoHelper::Load(device);
  
  lastFrameTime = std::chrono::high_resolution_clock::now();

  std::cout << "[DEBUG] Overlay initialized successfully!\n";
  return true;
}
// ----------------------
// Overlay render snippet
// ----------------------
void Overlay::RenderGameContent() {
  if (!entityManager) return;

  auto gameData = entityManager->GetGameData();
  if (!gameData.valid) return;

  ImDrawList* drawList = ImGui::GetBackgroundDrawList();
  if (!drawList) return;

  float screenWidth = ImGui::GetIO().DisplaySize.x;
  float screenHeight = ImGui::GetIO().DisplaySize.y;
  HealthBarHelper::RenderHealth(gameData, drawList);
  for (const auto& entity : gameData.entities) {
    // Use the ESP helper
    ESPHelper::RenderEsp(mem, clientBase, entity, gameData.localPlayerPawn,
                         gameData.localTeam, gameData.viewMatrix, drawList);

    // Also render bones if available
    bool hasBones = false;
    for (const auto& bone : entity.bones) {
      if (!bone.location.IsZero()) {
        hasBones = true;
        break;
      }
    }
    
    if (hasBones) {
      bool isTeammate = (entity.team == gameData.localTeam);

      BoneEsp::RenderBones(entity.bones, gameData.viewMatrix, screenWidth,
                           screenHeight, drawList, isTeammate);
    }

  }
}










bool Overlay::CreateDX11() {
  DXGI_SWAP_CHAIN_DESC sd{};
  sd.BufferCount = 2;
  sd.BufferDesc.Width = bbWidth;
  sd.BufferDesc.Height = bbHeight;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 2;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0,
                                       D3D_FEATURE_LEVEL_10_0};
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels,
      ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd, &swapChain, &device,
      nullptr, &deviceContext);

  if (FAILED(hr)) {
    std::cout << "[DEBUG] Failed to create DX11 device\n";
    return false;
  }

  return CreateRTVFromSwapChain();
}

bool Overlay::CreateRTVFromSwapChain() {
  if (!swapChain || !device) return false;

  ID3D11Texture2D* backBuffer = nullptr;
  if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || !backBuffer)
    return false;
  if (FAILED(device->CreateRenderTargetView(backBuffer, nullptr,
                                            &renderTargetView))) {
    backBuffer->Release();
    return false;
  }
  backBuffer->Release();
  return true;
}
void Overlay::RenderFrame() { //render menu here
  if (!deviceContext || !renderTargetView) return;

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
 
  UpdateImGuiInput();

  RenderGameContent();


  if (globals::menu_open) {
    Menu::Render();  // render your ImGui menu
   
  }

  ImGui::Render();
  const float clearColor[4] = {0.f, 0.f, 0.f, 0.f};
  deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
  deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  swapChain->Present(0, 0);
}

void Overlay::Run() {
  std::cout << "[DEBUG] Running Overlay...\n";
  MSG msg{};
  while (running) {
    // Windows message loop
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT) running = false;
    }

    HandleInput();  // check for INSERT toggle
    RenderFrame();  // draw game + menu

    
  }
}

void Overlay::HandleInput() {
  // Toggle menu visibility with INSERT
  if (GetAsyncKeyState(VK_INSERT) & 1) {
    globals::menu_open = !globals::menu_open;
    UpdateClickThrough();
  }
}

void Overlay::UpdateClickThrough() {
  if (!hwnd || !IsWindow(hwnd)) return;

  LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
  if (globals::menu_open) {
    // Menu open → remove WS_EX_TRANSPARENT to receive input
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
  } else {
    // Menu closed → make overlay click-through
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
  }
}
void Overlay::UpdateImGuiInput() {
  ImGuiIO& io = ImGui::GetIO();

  if (!globals::menu_open) {
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);  // ignore mouse when menu closed
    io.MouseDown[0] = false;
    io.MouseDown[1] = false;
    return;
  }

  // Mouse position in overlay client space
  POINT p;
  GetCursorPos(&p);
  ScreenToClient(hwnd, &p);
  io.MousePos = ImVec2((float)p.x, (float)p.y);

  // Mouse buttons
  io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
  io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

  // Optional keyboard updates:
  io.KeyCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
  io.KeyShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
  io.KeyAlt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
}

void Overlay::Shutdown() {
  if (entityManager) entityManager->StopUpdateThread();

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  if (renderTargetView) renderTargetView->Release();
  if (swapChain) swapChain->Release();
  if (deviceContext) deviceContext->Release();
  if (device) device->Release();
  if (hwnd && IsWindow(hwnd)) DestroyWindow(hwnd);

  FreeConsole();
}
