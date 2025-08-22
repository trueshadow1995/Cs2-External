
#include <stdexcept>
#include <iostream>
#include "../Headers/DX11Renderer.h"
#include "../Headers/LogoHelper.h"


namespace DX11Renderer {
ID3D11Device* device = nullptr;
ID3D11DeviceContext* device_context = nullptr;
IDXGISwapChain* swap_chain = nullptr;
ID3D11RenderTargetView* render_target_view = nullptr;

bool Init(HWND hwnd) {
  DXGI_SWAP_CHAIN_DESC sd{};
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.SampleDesc.Count = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.BufferCount = 2;
  sd.OutputWindow = hwnd;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, levels, 2,
      D3D11_SDK_VERSION, &sd, &swap_chain, &device, nullptr, &device_context);

  if (FAILED(hr)) return false;

  ID3D11Texture2D* backBuffer = nullptr;
  swap_chain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
  device->CreateRenderTargetView(backBuffer, nullptr, &render_target_view);
  backBuffer->Release();
  LogoHelper::Load(device);

  return true;
}

void BeginScene() {
  const float clearColor[4] = {0.f, 0.f, 0.f, 0.f};  // transparent
  device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
  device_context->ClearRenderTargetView(render_target_view, clearColor);
}

void EndScene() {
  if (swap_chain) swap_chain->Present(1, 0);
}

void Shutdown() {
  if (render_target_view) render_target_view->Release();
  if (swap_chain) swap_chain->Release();
  if (device_context) device_context->Release();
  if (device) device->Release();
}
}  // namespace DX11Renderer

