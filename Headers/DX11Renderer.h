#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>

namespace DX11Renderer { 

// globals declared as extern to be defined in the corresponding .cpp file -phil here :)
extern ID3D11Device* device; //device
extern ID3D11DeviceContext* device_context; //immediate context
extern IDXGISwapChain* swap_chain; // swap chain
extern ID3D11RenderTargetView* render_target_view; //main render target view
extern D3D_FEATURE_LEVEL level;  // feature level

bool Init(HWND hwnd); //intialize dx11
void BeginScene(); //begin scene for rendering
void EndScene(); //end scnene for rendering
void Shutdown(); //shut down dx11

}  // namespace DX11Renderer file end - phil out :D 