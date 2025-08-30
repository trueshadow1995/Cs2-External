#include "../Headers/OverlayWindow.h"

#include <d3d11.h>
#include <dwmapi.h>
#include <windows.h>

#include <iostream>
#include <thread>

#include "../Headers/BoneHelper.h"
#include "../Headers/EspHelper.h"
#include "../Headers/Globals.h"
#include "../Headers/HealBarHelper.h"
#include "../Headers/LogoHelper.h"
#include "../Headers/Menu.h"
#include "../Headers/RenderManager.h"
#include "../Headers/Styles.h"
#include "../Headers/WaterMarkHelper.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

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
      return 0L;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

Overlay::Overlay(Memory& memory)
    : mem(memory),
      hInstance(GetModuleHandle(nullptr)),
      hwnd(nullptr),
      running(true),
      swapChain(nullptr),
      device(nullptr),
      deviceContext(nullptr),
      renderTargetView(nullptr),
      blendState(nullptr),
      bbWidth(1920),
      bbHeight(1080),
      clientBase(0),
      entityManager(nullptr) {
  std::cout << "[DEBUG] Overlay constructor called\n";
}

Overlay::~Overlay() {
  if (entityManager) delete entityManager;
  Shutdown();
}

inline void PrintColored(const std::string& msg, WORD color) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, color);
  std::cout << msg << std::endl;
  SetConsoleTextAttribute(hConsole, 7);
}

void Overlay::InitConsole() {
  AllocConsole();
  FILE* file;
  freopen_s(&file, "CONOUT$", "w", stdout);
  freopen_s(&file, "CONOUT$", "w", stderr);

  PrintColored("[DEBUG] 🟢 Overlay console initialized!",
               FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

bool Overlay::Init() {
  InitConsole();
  PrintColored("[INFO] Setting up Overlay...",
               FOREGROUND_BLUE | FOREGROUND_INTENSITY);

  hwnd = FindWindowA(nullptr, "Counter-Strike 2");
  if (!hwnd) {
    PrintColored("[ERROR] Game Window Not Found!",
                 FOREGROUND_RED | FOREGROUND_INTENSITY);
    return false;
  }

  clientBase = globals::clientAddress;
  if (clientBase == 0) {
    clientBase = mem.GetModuleAddress("client.dll");
  }

  if (clientBase == 0) {
    std::cout << "[ERROR] Client base is null!\n";
    return false;
  }

  entityManager = new DataManager(mem, clientBase);

  WNDCLASSEX wc{};
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = "PhilOverlay";
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

  if (!RegisterClassEx(&wc)) return false;

  RECT gameRect{};
  GetWindowRect(hwnd, &gameRect);
  int width = gameRect.right - gameRect.left;
  int height = gameRect.bottom - gameRect.top;

  hwnd = CreateWindowEx(
      WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
      wc.lpszClassName, "PhilOverlay", WS_POPUP, gameRect.left, gameRect.top,
      width, height, nullptr, nullptr, hInstance, nullptr);
  if (!hwnd) return false;

  SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
  MARGINS margins{-1};
  DwmExtendFrameIntoClientArea(hwnd, &margins);
  ShowWindow(hwnd, SW_SHOW);

  if (!CreateDX11()) return false;

  ImGui::CreateContext();
  Styles::Apply();
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(device, deviceContext);
  LogoHelper::Load(device);

  lastFrameTime = std::chrono::high_resolution_clock::now();
  std::cout << "[DEBUG] Overlay initialized!\n";
  return true;
}

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
    ESPHelper::RenderEsp(mem, clientBase, entity, gameData.localPlayerPawn,
                         gameData.localTeam, gameData.viewMatrix, drawList);

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
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = bbWidth;
  sd.BufferDesc.Height = bbHeight;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels,
      ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd, &swapChain, &device,
      nullptr, &deviceContext);

  if (FAILED(hr)) {
    std::cout << "[ERROR] Failed to create DX11 device\n";
    return false;
  }
  return CreateRTVFromSwapChain();
}

bool Overlay::CreateRTVFromSwapChain() {
  if (!swapChain || !device) return false;
  ID3D11Texture2D* backBuffer = nullptr;
  if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || !backBuffer)
    return false;
  HRESULT hr =
      device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
  backBuffer->Release();
  return SUCCEEDED(hr);
}

void Overlay::SyncOverlayToGameWindow() {
  if (!hwnd) return;
  HWND gameWnd = FindWindowA(nullptr, "Counter-Strike 2");
  if (!gameWnd) return;

  RECT gameRect{};
  if (!GetWindowRect(gameWnd, &gameRect)) return;

  int gameWidth = gameRect.right - gameRect.left;
  int gameHeight = gameRect.bottom - gameRect.top;

  SetWindowPos(hwnd, HWND_TOPMOST, gameRect.left, gameRect.top, gameWidth,
               gameHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);

  if (gameWidth != bbWidth || gameHeight != bbHeight) {
    bbWidth = gameWidth;
    bbHeight = gameHeight;
    if (renderTargetView) {
      renderTargetView->Release();
      renderTargetView = nullptr;
    }
    swapChain->ResizeBuffers(0, bbWidth, bbHeight, DXGI_FORMAT_UNKNOWN, 0);
    CreateRTVFromSwapChain();
    ImGui::GetIO().DisplaySize = ImVec2((float)bbWidth, (float)bbHeight);
  }
}

void Overlay::RenderFrame() {
  if (!deviceContext || !renderTargetView) return;
  SyncOverlayToGameWindow();

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  UpdateImGuiInput();
  RenderGameContent();

  if (globals::menu_open) Menu::Render();

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
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if (msg.message == WM_QUIT) {
        running = false;
        Shutdown();
      }
    }
    HandleInput();
    if (running) RenderFrame();
  }
  Shutdown();  // Ensure cleanup on exit
}

void Overlay::HandleInput() {
  if (GetAsyncKeyState(VK_INSERT) & 1) {
    globals::menu_open = !globals::menu_open;
    UpdateClickThrough();
  }

  // Close program on Delete key with proper shutdown
  if (GetAsyncKeyState(VK_DELETE) & 1) {
    std::cout << "[INFO] Delete key pressed - shutting down...\n";
    running = false;
    PostQuitMessage(0);  // Signal the message loop to exit
  }
}

void Overlay::UpdateClickThrough() {
  if (!hwnd || !IsWindow(hwnd)) return;
  LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
  if (globals::menu_open) {
    exStyle &= ~WS_EX_TRANSPARENT;
  } else {
    exStyle |= WS_EX_TRANSPARENT;
  }
  SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
}

void Overlay::UpdateImGuiInput() {
  ImGuiIO& io = ImGui::GetIO();
  if (!globals::menu_open) {
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    io.MouseDown[0] = io.MouseDown[1] = false;
    return;
  }
  POINT p;
  GetCursorPos(&p);
  ScreenToClient(hwnd, &p);

  io.MousePos = ImVec2((float)p.x, (float)p.y);
  io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
  io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
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