// Inline PNG bytes
#include <d3d11.h>
#include "../ImGui/imgui.h"
#include <D3DX11tex.h>
#include "../Headers/Logo.h"

namespace LogoHelper {
static ID3D11ShaderResourceView* logoTexture = nullptr;
static ImVec2 g_logoSize(100, 100);

void Load(ID3D11Device* device) {
  if (logoTexture) return;  // already loaded

  ID3D11Resource* texture = nullptr;
  HRESULT hr = D3DX11CreateTextureFromMemory(
      device, Potleaf, sizeof(Potleaf), nullptr, nullptr, &texture, nullptr);

  if (SUCCEEDED(hr) && texture) {
    device->CreateShaderResourceView(texture, nullptr, &logoTexture);
    texture->Release();
  }
}

void Render() {
  if (logoTexture) {
    ImGui::SetCursorPosX((128.0f - g_logoSize.x) / 2.0f);
    ImGui::Image((ImTextureID)logoTexture, g_logoSize);
    ImGui::Spacing();
  } else {
    ImGui::Text("Logo failed to load");
  }
}
}  




