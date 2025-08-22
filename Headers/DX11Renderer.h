#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>

namespace DX11Renderer {

// globals declared as extern
extern ID3D11Device* device;
extern ID3D11DeviceContext* device_context;
extern IDXGISwapChain* swap_chain;
extern ID3D11RenderTargetView* render_target_view;
extern D3D_FEATURE_LEVEL level;

bool Init(HWND hwnd);
void BeginScene();
void EndScene();
void Shutdown();

}  // namespace DX11Renderer