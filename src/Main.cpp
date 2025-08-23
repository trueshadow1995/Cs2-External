//#pragma once
//#include <d3d11.h>
//#include <dwmapi.h>
//#include <array>
//#include <format>
//#include <iostream>
//#include <string>
//#include "../Headers/C_color.h"
//#include "../Headers/Globals.h"
//#include "../Headers/Math.h"
//#include "../Headers/Memory.h"
//#include "../Headers/Offsets.h"
//#include "../Headers/Styles.h"
//#include "../ImGui/imgui.h"
//#include "../ImGui/imgui_impl_dx11.h"
//#include "../ImGui/imgui_impl_win32.h"
//#include "../ImGui/imgui_internal.h"
//#include <D3DX11.h>
//#include "../Headers/LogoHelper.h"
//#include "../Headers/EspHelper.h"
//#include "../Headers/HealBarHelper.h"
//#include "../Headers/BoneHelper.h"
//#include "../Headers/Menu.h"
//
//
//
//
//bool menu_open;
//
//
//bool isRunning = true;
//inline float fixed_radius = 8.0f;
//
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
//                                                             UINT msg,
//                                                             WPARAM wParam,
//                                                             LPARAM lParam);
//
//LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param,
//                                  LPARAM l_param) {
//  if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
//    return 1L;
//  }
//  if (message == WM_DESTROY) {
//    PostQuitMessage(0);
//    return 0L;
//  }
//  return DefWindowProc(window, message, w_param, l_param);
//}
//
//void create_directx(HWND window) {}
//
//INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {
//  // allocate this program a console FOR DEBUG!!!!
//  if (!AllocConsole()) {
//    return FALSE;
//  };
//
//  // redirect IO
//  FILE* file{nullptr};
//  freopen_s(&file, "CONIN$", "r", stdout);
//  freopen_s(&file, "CONOUT$", "w", stdout);
//  freopen_s(&file, "CONOUT$", "w", stdout);
//
//  // initialize memory class
//  const auto client = mem.GetModuleAddress("client.dll");
//  const auto engine = mem.GetModuleAddress("engine.dll");
//
//  // wait for game
//  DWORD pid = 0;
//  bool gameFound = false;
//
//  while (!gameFound) {
//    // Check if CS:GO process exists
//    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//    if (hSnapshot != INVALID_HANDLE_VALUE) {
//      PROCESSENTRY32 pe;
//      pe.dwSize = sizeof(PROCESSENTRY32);
//      if (Process32First(hSnapshot, &pe)) {
//        do {
//          if (std::string(pe.szExeFile) == "cs2.exe") {
//            pid = pe.th32ProcessID;
//            gameFound = true;
//            break;
//          }
//        } while (Process32Next(hSnapshot, &pe));
//      }
//
//      CloseHandle(hSnapshot);
//    }
//
//    if (!gameFound) {
//      Sleep(3000);
//      std::cout << "Waiting for CS2...\n";
//    }
//  }
//
//  std::cout << "[DEBUG] client.dll base: 0x" << std::hex << client << std::endl;
//  std::cout << std::format("CS2 Has Been Re-Fuckulated\n");
//  std::cout << std::format("Process ID = {}\n", pid);
//  std::cout << std::format("client ID = {}\n", pid);
//
//  const HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, client);
//
//  WNDCLASSEXW wc{};
//  wc.cbSize = sizeof(WNDCLASSEXW);
//  wc.style = CS_HREDRAW | CS_VREDRAW, wc.lpfnWndProc = window_procedure;
//  wc.hInstance = instance;
//  wc.lpszClassName = L"External Overlay Class";
//
//  RegisterClassExW(&wc);
//
//  const HWND window = CreateWindowExW(
//      WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
//      wc.lpszClassName, L"phil.9", WS_POPUP, 0, 0, 1920, 1080, nullptr, nullptr,
//      wc.hInstance, nullptr);
//
//  SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);
//
//  {
//    RECT client_area{};
//    GetClientRect(window, &client_area);
//
//    RECT window_area{};
//    GetWindowRect(window, &window_area);
//
//    POINT diff{};
//    ClientToScreen(window, &diff);
//
//    const MARGINS margins{window_area.left + (diff.x - window_area.left),
//                          window_area.top + (diff.y - window_area.top),
//                          client_area.right, client_area.bottom};
//    DwmExtendFrameIntoClientArea(window, &margins);
//  }
//
//  DXGI_SWAP_CHAIN_DESC sd{};
//  sd.BufferDesc.RefreshRate.Numerator = 60U;
//  sd.BufferDesc.RefreshRate.Denominator = 1U;
//  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//  sd.SampleDesc.Count = 1U;
//  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//  sd.BufferCount = 2U;
//  sd.OutputWindow = window;
//  sd.Windowed = TRUE;
//  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//  sd.BufferDesc.Width = 0U;
//  sd.BufferDesc.Height = 0U;
//  constexpr D3D_FEATURE_LEVEL levels[2]{D3D_FEATURE_LEVEL_11_0,
//                                        D3D_FEATURE_LEVEL_10_0};
//
//
//  ID3D11Device* device{nullptr};
//  ID3D11DeviceContext* device_context{nullptr};
//  IDXGISwapChain* swap_chain{nullptr};
//  ID3D11RenderTargetView* render_target_view{nullptr};
//  D3D_FEATURE_LEVEL level{};
//
//  D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0U,
//                                levels, 2U, D3D11_SDK_VERSION, &sd, &swap_chain,
//                                &device, &level, &device_context);
//
//  ID3D11Texture2D* back_buffer{nullptr};
//  swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));
//
//  if (back_buffer) {
//    device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
//    back_buffer->Release();
//  } else {
//    return 1;
//  }
//
//  HWND menuhandle = FindWindow("Counter-Strike 2", NULL);
//
//  ShowWindow(window, cmd_show);
//  UpdateWindow(window);
//
//  ImGui::CreateContext();
//  Menu::RenderMenu(); 
// 
//
//
//
//  ImGui_ImplWin32_Init(window); //init imgui for win32
//  ImGui_ImplDX11_Init(device, device_context); // init imgui for dx11
//  LogoHelper::Load(device); //rip 12 hours loading a fucking png
//  bool isRunning = true; //main loop bool
//  while (isRunning) { //main loop
//    MSG msg; //windows message struct
//    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) { //message loop
//      TranslateMessage(&msg); //translate message
//      DispatchMessage(&msg); //dispatch message
//
//      if (msg.message == WM_QUIT) { //check for quit message
//        isRunning = false; // set isRunning to false
//      }
//    }
//
//    // Check for Delete key to close the program
//    if (GetAsyncKeyState(VK_DELETE) & 0x8000) { //
//      isRunning = false; //set isRunning to false
//    }
//
//    if (!isRunning) { //if isRunning is false
//      break; //break the loop
//    }
//
//    ImGui_ImplDX11_NewFrame();//start new imgui frame for dx11
//    ImGui_ImplWin32_NewFrame(); //start new imgui frame for win32
//    ImGui::NewFrame();
//
//    auto backgrounddraw = ImGui::GetBackgroundDrawList(); //get backgroun draw list
//    auto foregrounddraw = ImGui::GetForegroundDrawList(); //
//   
//
//
//
//
//    ImGuiIO& io = ImGui::GetIO();
//    RECT rc;
//    POINT xy;
//
//    ZeroMemory(&rc, sizeof(RECT));
//    ZeroMemory(&xy, sizeof(POINT));
//    GetClientRect(menuhandle, &rc);
//    ClientToScreen(menuhandle, &xy);
//    rc.left = xy.x;
//    rc.top = xy.y;
//    io.ImeWindowHandle = menuhandle;
//    io.DeltaTime = 1.0f / 60.0f;
//    POINT p;
//    GetCursorPos(&p);
//    io.MousePos.x = p.x - xy.x;
//    io.MousePos.y = p.y - xy.y;
//
//    if (GetAsyncKeyState(VK_LBUTTON)) {
//      io.MouseDown[0] = true;
//      io.MouseClicked[0] = true;
//      io.MouseClickedPos[0].x = io.MousePos.x;
//      io.MouseClickedPos[0].y = io.MousePos.y;
//    } else {
//      io.MouseDown[0] = false;
//    }
//
//    if (GetAsyncKeyState(VK_INSERT) & 1) menu_open ^= 1;
//
//    if (menu_open) {
//      ImGui::SetNextWindowSize({700, 450});  // actual ImGui window
//
//      ImVec4 borderColor =
//          ImColor::HSV(ImGui::GetTime() / 10, 0.6f, 0.6f).Value;
//      ImVec4 borderGradientColorTopLeft =
//          ImVec4(borderColor.x * 0.5f, borderColor.y * 0.5f,
//                 borderColor.z * 0.5f, borderColor.w);  // glowy color logic
//      ImVec4 borderGradientColorBottomRight =
//          ImVec4(borderColor.x * 0.7f, borderColor.y * 0.7f,
//                 borderColor.z * 0.7f, borderColor.w);
//
//      ImGui::Begin("phil9", &menu_open,
//                   ImGuiWindowFlags_NoSavedSettings |
//                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
//      ImGui::PopStyleColor();
//
//
//      
//
//      ImGui::PushStyleColor(ImGuiCol_Border, borderGradientColorTopLeft);
//
//      ImGui::PushStyleColor(ImGuiCol_BorderShadow,
//                            borderGradientColorBottomRight);
//      ImVec2 windowPos = ImGui::GetWindowPos();
//      static int current_tab = 0;
//      auto button_height = 48;
//      auto button_length = 110;  // button shenanigans
//    
//
//      ImGui::BeginChild("TabBar", ImVec2(128, ImGui::GetContentRegionAvail().y),
//                        true, ImGuiWindowFlags_NoScrollbar);
//      ImGui::PopStyleColor(2);
//      {
//        ImGui::PushStyleColor(ImGuiCol_Border, borderGradientColorTopLeft);
//
//        ImGui::PushStyleColor(ImGuiCol_BorderShadow,
//                              borderGradientColorBottomRight);
// 
//
//        LogoHelper::Render();
//     
//       
//         
//   
//        auto tabButton = [&](const char* label, int tab) {
//          ImVec4 accentColor = ImVec4(0.08f, 0.53f, 0.79f, 0.50f);
//
//          if (current_tab == tab) {
//            ImGui::PushStyleColor(ImGuiCol_Button, accentColor);
//            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accentColor);
//            ImGui::PushStyleColor(ImGuiCol_ButtonActive, accentColor);
//            
//          }
//
//          if (ImGui::Button(label, ImVec2(-1, button_height))) {
//            current_tab = tab;
//          }
//
//          if (current_tab == tab) {
//            ImGui::PopStyleColor(3);
//          }
//        };
//
//        tabButton("Aimbot", 0);
//        tabButton("Overlay stuff", 1);
//        tabButton("Glow n Chams", 2);
//        tabButton("Features", 3);
//        tabButton("Skin Changer", 4);
//       
//        ImGui::Text("Press DEL to close");
//        
//      }
//      ImGui::EndChild();
//
//      ImGui::SameLine();
//      ImGui::PushStyleColor(ImGuiCol_Border, borderGradientColorTopLeft);
//
//      ImGui::PushStyleColor(ImGuiCol_BorderShadow,
//                            borderGradientColorBottomRight);
//      ImGui::BeginChild("TabContent",
//                        ImVec2(ImGui::GetContentRegionAvail().x,
//                               ImGui::GetContentRegionAvail().y),
//                        true);
//
//      
//      ImGui::PopStyleColor(3);
//
//      {
//        switch (current_tab) {
//          case 0:  // aimbot
//            ImGui::Checkbox("Fov circle", &globals::Fov);
//            ImGui::SameLine(0, 70);
//            ImGui::ColorEdit4("Fov Circle Color", globals::FovColor,
//                              ImGuiColorEditFlags_NoInputs);
//
//            ImGui::SliderFloat("AimbotFov", &globals::AimbotFovSize, 0.f, 90.f,
//                               "%.0f");
//
//            break;
//
//          case 1:  // ESP and overlay
//          {
//            // Enemy ESP
//            ImGui::Checkbox("Enemy Player ESP", &globals::EnemyEsp);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Enemy ESP Color", globals::EnemyEspColor,
//                              ImGuiColorEditFlags_NoInputs);
//
//            // Teammate ESP
//            ImGui::Checkbox("Teammate ESP", &globals::TeammateEsp);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Friendly ESP Color", globals::TeammateEspColor,
//                              ImGuiColorEditFlags_NoInputs);
//
//            // ESP background
//            ImGui::Checkbox("Friendly ESP Background",
//                            &globals::TeammateEspBackground);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Friendly ESP Background Color",
//                              globals::FriendlyEspBackGroundColor,
//                              ImGuiColorEditFlags_NoInputs);
//
//            ImGui::Checkbox("Enemy ESP Background",
//                            &globals::EnemyEspBackground);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Enemy ESP Background Color",
//                              globals::EnemyEspBackGroundColor,
//                              ImGuiColorEditFlags_NoInputs);
//
//            // Health ESP
//            ImGui::Checkbox("Friendly Player Health", &globals::TeammateHealth);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Player Health Color",
//                              globals::TeammateHealthColor,
//                              ImGuiColorEditFlags_NoInputs);
//
//            ImGui::Checkbox("Enemy Player Health", &globals::EnemyHealth);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Player Health Color", globals::EnemyHealthColor,
//                              ImGuiColorEditFlags_NoInputs);
//
//            // Bone ESP
//            ImGui::Checkbox("Friendly Bone ESP", &globals::FriendlyBones);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Friendly Bone ESP Color",
//                              globals::TeammateBoneColor,
//                              ImGuiColorEditFlags_NoInputs);
//            ImGui::SliderFloat("Friendly Bone ESP Thickness",
//                               &globals::BoneEspThickness, 0.f, 90.f, "%.0f");
//
//            ImGui::Checkbox("Enemy Bone ESP", &globals::EnemyBones);
//            ImGui::SameLine(220);
//            ImGui::ColorEdit4("Enemy Bone ESP Color", globals::EnemyBoneColor,
//                              ImGuiColorEditFlags_NoInputs);
//            ImGui::SliderFloat("Enemy Bone ESP Thickness",
//                               &globals::BoneEspThickness, 0.f, 90.f, "%.0f");
//
//            ImGui::Checkbox("Bone Debug", &globals::BoneDebug);
//            break;
//          }
//
//          case 2:  // chams and glow
//            break;
//
//          case 3:  // features
//            ImGui::Checkbox("Fps Counter", &globals::FpsCounter);
//            ImGui::Checkbox("Water Mark", &globals::WaterMark);
//            break;
//        }
//      }
//      ImGui::EndChild();
//
//      ImGui::PopStyleColor(4);
//      ImGui::PopStyleVar();
//      ImGui::EndChild();
//    }
//
//    // cheat stuff goes above render
//    auto entityList = mem.Read<uintptr_t>(client + offsets::EntityList);
//
//    const auto localPlayerpawn =
//        mem.Read<std::uintptr_t>(client + offsets::LocalPlayerPawn);
//
//    const auto entlist = mem.Read<std::uintptr_t>(client + offsets::EntityList);
//
//    ViewMatrix_t view_matrix =
//        mem.Read<ViewMatrix_t>(client + offsets::ViewMatrix);
//
//    int localTeam = mem.Read<int>(client + offsets::m_iTeamNum);
//
//    for (int i = 0; i < 64; i++) {
//      uintptr_t listEntry =
//          mem.Read<uintptr_t>(entlist + 0x8 * (i >> 9) + 0x10);
//      if (!listEntry) continue;
//
//      uintptr_t entController =
//          mem.Read<uintptr_t>(listEntry + 0x78 * (i & 0x1FF));
//      if (!entController) continue;
//
//      uint32_t hPawn = mem.Read<uint32_t>(entController + offsets::m_hPawn);
//      if (!hPawn) continue;
//
//      uintptr_t listEntryPawn =
//          mem.Read<uintptr_t>(entlist + 0x8 * ((hPawn & 0x7FFF) >> 9) + 0x10);
//      if (!listEntryPawn) continue;
//
//      uintptr_t pPawn =
//          mem.Read<uintptr_t>(listEntryPawn + 0x78 * (hPawn & 0x1FF));
//      if (!pPawn) continue;
//
//      if (pPawn == localPlayerpawn) continue;
//
//      int health = mem.Read<int>(pPawn + offsets::m_iHealth);
//      int team = mem.Read<int>(pPawn + offsets::m_iTeamNum);
//
//      if (health <= 0 || health > 100) continue;
//
//      int localTeam = mem.Read<int>(localPlayerpawn + offsets::m_iTeamNum);
//
//      uintptr_t list_entry2 =
//          mem.Read<uintptr_t>(entityList + 0x8 * ((pPawn & 0x7FFF) >> 9) + 16);
//
//      uintptr_t pCSPlayerPawnPtr =
//          mem.Read<uintptr_t>(list_entry2 + 120 * (pPawn & 0x1FF));
//
//      Vector3 origin = mem.Read<Vector3>(pPawn + offsets::m_vOldOrigin);
//      Vector3 head = {origin.x, origin.y, origin.z + 75.f};
//
//      Vector3 screenFeetPos = origin.WorldToScreen(view_matrix);
//      Vector3 screenHeadPos = head.WorldToScreen(view_matrix);
//
//      float height = (screenFeetPos.y - screenHeadPos.y);
//      float width = height / 2.4f;
//
//      ImVec2 topLeft(screenHeadPos.x - width / 2.f, screenHeadPos.y);
//      ImVec2 bottomRight(screenHeadPos.x + width / 2.f,
//                         screenHeadPos.y + height);
//
//      
//      auto entity_hp = mem.Read<int>(pPawn + offsets::m_iHealth);
//
//      
//      
//      HealthBarHelper::RenderHealth(mem, client, localPlayerpawn, view_matrix,
//                              backgrounddraw);
//
//
//
//      uintptr_t gameSceneNode =
//          mem.Read<uintptr_t>(pPawn + offsets::m_pGameSceneNode);
//      if (!gameSceneNode) {
//        printf("[-] Could not find GameSceneNode\n");
//        continue;
//      }
//
//      uintptr_t boneArrayPtr =
//          mem.Read<uintptr_t>(gameSceneNode + offsets::m_modelState + 0x80);
//
//      std::array<CBoneData, 64> bones =
//          mem.Read<std::array<CBoneData, 64>>(boneArrayPtr);
//    /*
//      BoneHelper::RenderBones(mem, pPawn, team, localTeam, view_matrix,
//                              backgrounddraw);*/
//    }
//
//
//
//
//    if (globals::FpsCounter) {
//      ImDrawList* fps = ImGui::GetBackgroundDrawList();
//      int Framerate = round(ImGui::GetIO().Framerate);
//      std::string StatStrings = "FPS: " + std::to_string(Framerate);
//      void get_fps();
//
//      fps->AddText(ImGui::GetFont(), 15, ImVec2(50, 50), ImColor(255, 255, 255),
//                   StatStrings.c_str());
//    }
//
//    if (globals::Fov) {
//      ImVec2 center = {960, 540};
//      float radius = globals::AimbotFovSize * 3.141592;
//      int num_segments = 135;
//      float thickness = 2.0f;
//
//      ImColor light_blue(0.3f, 0.8f, 1.0f);
//      ImColor blue(0.0f, 0.0f, 1.0f);
//
//      float time = static_cast<float>(ImGui::GetTime());
//
//      for (int i = 0; i < num_segments; ++i) {
//        float angle_start = (2 * IM_PI * i) / num_segments;
//        float angle_end = (2 * IM_PI * (i + 3)) / num_segments;
//
//        ImVec2 p1 = {center.x + radius * cos(angle_start),
//                     center.y + radius * sin(angle_start)};
//        ImVec2 p2 = {center.x + radius * cos(angle_end),
//                     center.y + radius * sin(angle_end)};
//
//        float spin_factor = sin(time + (i * 0.05f));
//        float t = (spin_factor + 1.0f) * 0.5f;
//
//        float r = light_blue.Value.x * (1.0f - t) + blue.Value.x * t;
//        float g = light_blue.Value.y * (1.0f - t) + blue.Value.y * t;
//        float b = light_blue.Value.z * (1.0f - t) + blue.Value.z * t;
//
//        ImColor current_color(r, g, b);
//
//        backgrounddraw->AddLine(p1, p2, current_color, thickness);
//      }
//    }
//
//    if (globals::WaterMark) {
//      const char* watermarkText = "Phil was here | priv secret cheat";
//      ImVec2 textSize = ImGui::CalcTextSize(watermarkText);
//
//      // Position and padding
//      const ImVec2 backgroundPos(1600.0f, 16.0f);  // Top-right-ish
//      const float padding = 8.0f;
//      const ImVec2 backgroundSize(textSize.x + 2.0f * padding,
//                                  textSize.y + padding + 4.0f);
//      const float cornerRadius = 6.0f;
//
//      ImVec4 bgColor(0.1f, 0.1f, 0.1f,
//                     0.85f);  // Dark gray, semi-transparent, background
//
//      // Draw rounded background
//      backgrounddraw->AddRectFilled(backgroundPos,
//                                    ImVec2(backgroundPos.x + backgroundSize.x,
//                                           backgroundPos.y + backgroundSize.y),
//                                    ImColor(bgColor), cornerRadius);
//
//      // Rainbow text color
//      float time = ImGui::GetTime();
//      ImVec4 textColor(0.6f + 0.4f * sin(time),           // Red
//                       0.6f + 0.4f * sin(time + 2.094f),  // Green (120° shift)
//                       0.6f + 0.4f * sin(time + 4.188f),  // Blue (240° shift)
//                       1.0f                               //  alpha
//      );
//
//      // Text position with shadow
//      ImVec2 shadowPos(backgroundPos.x + padding + 1.0f,
//                       backgroundPos.y + padding / 2.0f + 1.0f);
//      ImVec2 textPos(backgroundPos.x + padding,
//                     backgroundPos.y + padding / 2.0f);
//
//      backgrounddraw->AddText(shadowPos, ImColor(0.0f, 0.0f, 0.0f, 0.5f),
//                              watermarkText);  // Shadow
//      backgrounddraw->AddText(textPos, ImColor(textColor),
//                              watermarkText);  // Main text
//    }
//
//    ImGui::Render();
//    constexpr float clear_color[4] = {0.f, 0.f, 0.f, 0.f};
//    device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
//    device_context->ClearRenderTargetView(render_target_view, clear_color);
//
//    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
//
//    swap_chain->Present(0U, 0U);
//  }
//
//  ImGui_ImplDX11_Shutdown();
//  ImGui_ImplWin32_Shutdown();
//
//  ImGui::DestroyContext();
//
//  if (swap_chain) {
//    swap_chain->Release();
//  }
//
//  if (device_context) {
//    device_context->Release();
//  }
//
//  if (device) {
//    device->Release();
//  }
//
//  if (render_target_view) {
//    render_target_view->Release();
//  }
// 
//  DestroyWindow(window);
//  UnregisterClassW(wc.lpszClassName, wc.hInstance);
//
//  return 0;
//}