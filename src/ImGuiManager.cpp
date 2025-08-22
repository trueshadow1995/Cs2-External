#include <windows.h>        // for HWND, POINT, RECT, GetCursorPos, GetClientRect, ClientToScreen, etc.
#include <cstdint>          // optional, for fixed-size types if you use them
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../Headers/DX11Renderer.h"
#include "../Headers/Styles.h"





namespace ImGuiManager {

void Init(HWND hwnd) {
  ImGui::CreateContext();
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(DX11Renderer::device, DX11Renderer::device_context);
  Styles::Apply();
}

void BeginFrame() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
}

void EndFrame() {
  ImGui::Render();
  DX11Renderer::BeginScene();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  DX11Renderer::EndScene();
}

void Shutdown() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

}  // namespace ImGuiManager


