#include "../Headers/OverlayWindow.h"

#include <d3d11.h>
#include <dwmapi.h>
#include <windows.h>

#include <iostream>
#include <thread>

#include "../Headers/Aimbot.h"
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
#include "../Headers/FovVisuals.h"

// Precomputed constants
constexpr const char* GAME_WINDOW_NAME = "Counter-Strike 2";
constexpr UINT SYNC_INTERVAL_MS = 100;
constexpr float CLEAR_COLOR[4] = {0.f, 0.f, 0.f, 0.f};

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
      entityManager(nullptr),
      aimbot(nullptr) {
  std::cout << "[DEBUG] Overlay constructor called\n";
}

Overlay::~Overlay() {
  StopAimbot();
  delete entityManager;
  delete aimbot;
  entityManager = nullptr;
  aimbot = nullptr;
  Shutdown();
}

void Overlay::InitConsole() {
  AllocConsole();
  FILE* file;
  freopen_s(&file, "CONOUT$", "w", stdout);
  freopen_s(&file, "CONOUT$", "w", stderr);

  std::cout << "[DEBUG] Overlay console initialized!" << std::endl;
}

bool Overlay::Init() {
  InitConsole();
  std::cout << "[INFO] Setting up Overlay..." << std::endl;

  hwnd = FindWindowA(nullptr, GAME_WINDOW_NAME);
  if (!hwnd) {
    std::cout << "[ERROR] Game Window Not Found!" << std::endl;
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
  aimbot = new Aimbot(*entityManager);

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
  ImGui::GetIO().DisplaySize = ImVec2(1920.0f, 1080.0f);
  Styles::Apply();
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(device, deviceContext);
  LogoHelper::Load(device);
  lastFrameTime = std::chrono::high_resolution_clock::now();

  // Start the aimbot thread
  StartAimbot();

  std::cout << "[DEBUG] Overlay initialized!\n";
  return true;
}

void Overlay::StartAimbot() {
  if (aimbot) {
    aimbot->Start();
  }
}

void Overlay::StopAimbot() {
  if (aimbot) {
    aimbot->Stop();
  }
}


void Overlay::RenderGameContent() {
  if (!entityManager) return;

  WaterMark::Render();

  auto gameData = entityManager->GetGameData();
  if (!gameData.valid) return;

  ImDrawList* drawList = ImGui::GetBackgroundDrawList();
  if (!drawList) return;

  float screenWidth = ImGui::GetIO().DisplaySize.x;
  float screenHeight = ImGui::GetIO().DisplaySize.y;

  // Draw crosshair (completely independent)
  FovVisualizer::DrawCrosshair(drawList, screenWidth, screenHeight);

  // Check if there are targets in FOV for advanced mode
  bool hasTargetsInFov = false;
  if (globals::FovCircle && globals::FovStyle == 2) {
    // Calculate screen center
    ImVec2 screenCenter(screenWidth / 2, screenHeight / 2);
    float fovRadius = globals::AimbotFovSize;

    // Check each entity to see if it's within FOV
    for (const auto& entity : gameData.entities) {
      if (entity.pawn && entity.pawn != gameData.localPlayerPawn &&
          entity.health > 0 && entity.team != gameData.localTeam) {
        // Convert entity head position to screen coordinates
        Vector3 screenHead;
        if (entity.head.WorldToScreen(gameData.viewMatrix, screenWidth,
                                      screenHeight, screenHead) &&
            screenHead.z > 0) {
          // Check if within FOV circle
          float dx = screenHead.x - screenCenter.x;
          float dy = screenHead.y - screenCenter.y;
          float distanceSquared = dx * dx + dy * dy;

          if (distanceSquared <= fovRadius * fovRadius) {
            hasTargetsInFov = true;
            break;
          }
        }
      }
    }
  }

  // Draw FOV circle based on selected style
  if (globals::FovCircle) {
    switch (globals::FovStyle) {
      case 0:  // Animated
        FovVisualizer::DrawAnimatedFovCircle(drawList, screenWidth,
                                             screenHeight);
        break;
      case 1:  // Simple
        FovVisualizer::DrawFovCircle(drawList, screenWidth, screenHeight);
        break;
      case 2:  // Advanced
        FovVisualizer::DrawFovCircleAdvanced(drawList, screenWidth,
                                             screenHeight, hasTargetsInFov);
        break;
      default:
        FovVisualizer::DrawAnimatedFovCircle(drawList, screenWidth,
                                             screenHeight);
        break;
    }
  }

  // Healthbars
  if (globals::HealthBar) {
    HealthBarHelper::RenderHealth(gameData, drawList);
  }

  // ESP boxes
  if (globals::Esp) {
    for (const auto& entity : gameData.entities) {
      ESPHelper::RenderEsp(mem, clientBase, entity, gameData.localPlayerPawn,
                           gameData.localTeam, gameData.viewMatrix, drawList);
    }
  }

  // Bones
  if (globals::Bones) {
    for (const auto& entity : gameData.entities) {
      bool hasBones = false;
      for (int i = 0; i < 10; ++i) {
        if (i < globals::MAX_BONES && !entity.bones[i].location.IsZero()) {
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
}

bool Overlay::CreateDX11() {
  DXGI_SWAP_CHAIN_DESC sd{};
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 1920;
  sd.BufferDesc.Height = 1080;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 0;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sd.Flags = 0;

  D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, featureLevels,
      ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd, &swapChain, &device,
      nullptr, &deviceContext);

  if (FAILED(hr)) {
    std::cout << "[ERROR] Failed to create DX11 device\n";
    return false;
  }

  bbWidth = 1920;
  bbHeight = 1080;

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

  HWND gameWnd = FindWindowA(nullptr, GAME_WINDOW_NAME);
  if (!gameWnd) return;

  RECT gameRect{};
  if (!GetWindowRect(gameWnd, &gameRect)) return;

  SetWindowPos(hwnd, HWND_TOPMOST, gameRect.left, gameRect.top, 1920, 1080,
               SWP_NOACTIVATE | SWP_SHOWWINDOW);

  if (bbWidth != 1920 || bbHeight != 1080) {
    bbWidth = 1920;
    bbHeight = 1080;

    if (renderTargetView) {
      renderTargetView->Release();
      renderTargetView = nullptr;
    }

    HRESULT hr =
        swapChain->ResizeBuffers(0, 1920, 1080, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
      std::cout << "[ERROR] Failed to resize swap chain buffers: " << std::hex
                << hr << std::dec << "\n";
      Shutdown();
      Init();
      return;
    }

    if (!CreateRTVFromSwapChain()) {
      std::cout << "[ERROR] Failed to recreate RTV after resize\n";
      return;
    }

    ImGui::GetIO().DisplaySize = ImVec2(1920.0f, 1080.0f);
  }
}

void Overlay::RenderFrame() {
  static int frameCounter = 0;
  static auto lastSyncTime = std::chrono::high_resolution_clock::now();

  frameCounter++;

  auto currentTime = std::chrono::high_resolution_clock::now();
  if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime -
                                                            lastSyncTime)
              .count() > SYNC_INTERVAL_MS ||
      globals::menu_open) {
    SyncOverlayToGameWindow();
    lastSyncTime = currentTime;
  }

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  UpdateImGuiInput();
  RenderGameContent();

  if (globals::menu_open) Menu::RenderMenu();

  ImGui::Render();
  deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
  deviceContext->ClearRenderTargetView(renderTargetView, CLEAR_COLOR);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  swapChain->Present(0, 0);
}

void Overlay::Run() {
  std::cout << "[DEBUG] Running Overlay...\n";
  MSG msg{};
  using namespace std::chrono;

  const int targetFPS = 500;
  const auto frameDuration = microseconds(105000 / targetFPS);

  while (running) {
    auto frameStart = high_resolution_clock::now();

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

    auto frameEnd = high_resolution_clock::now();
    auto elapsed = duration_cast<microseconds>(frameEnd - frameStart);

    if (elapsed < frameDuration) {
      std::this_thread::sleep_for(frameDuration - elapsed);
    }
  }
  Shutdown();
}

void Overlay::HandleInput() {
  if (GetAsyncKeyState(VK_INSERT) & 1) {
    globals::menu_open = !globals::menu_open;
    UpdateClickThrough();
  }

  if (GetAsyncKeyState(VK_DELETE) & 1) {
    std::cout << "[INFO] Delete key pressed - shutting down...\n";
    running = false;
    PostQuitMessage(0);
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
  StopAimbot();

  if (entityManager) entityManager->StopUpdateThread();

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  if (renderTargetView) {
    renderTargetView->Release();
    renderTargetView = nullptr;
  }
  if (swapChain) {
    swapChain->Release();
    swapChain = nullptr;
  }
  if (deviceContext) {
    deviceContext->Release();
    deviceContext = nullptr;
  }
  if (device) {
    device->Release();
    device = nullptr;
  }
  if (hwnd && IsWindow(hwnd)) {
    DestroyWindow(hwnd);
    hwnd = nullptr;
  }

  FreeConsole();
}