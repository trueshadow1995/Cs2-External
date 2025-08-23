#include <windows.h>        
#include <cstdint>         
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../Headers/DX11Renderer.h"
#include "../Headers/Styles.h"




namespace ImGuiManager { 

void Init(HWND hwnd) { //initialize imgui
  ImGui::CreateContext(); //create imgui context

 
  Styles::Apply();   //apply custom style stolen from spyder
                    

  ImGui_ImplWin32_Init(hwnd); //initialize win32 impl
  ImGui_ImplDX11_Init(DX11Renderer::device, DX11Renderer::device_context); //initialize dx11 impl
}

void BeginFrame() { //begin imgui frame
  ImGui_ImplDX11_NewFrame(); //new frame for dx11
  ImGui_ImplWin32_NewFrame(); //new frame for win32
  ImGui::NewFrame(); //new imgui frame
}

void EndFrame() { //end imgui frame
  ImGui::Render(); //render imgui
  DX11Renderer::BeginScene(); //begin dx11 scene
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); //render imgui draw data
  DX11Renderer::EndScene(); //end dx11 scene
}

void Shutdown() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

}  // namespace ImGuiManager