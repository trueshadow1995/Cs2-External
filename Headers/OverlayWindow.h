#pragma once
#include <d3d11.h>

#include <chrono>

#include "../Headers/EntityDataManager.h"
#include "../Headers/Memory.h"

class Overlay {
 public:
  Overlay(Memory& memory);  // Only one constructor
  ~Overlay();
  bool Init();
  void Run();
  void Shutdown();

 private:
  HINSTANCE hInstance;
  HWND hwnd;
  bool running;
  IDXGISwapChain* swapChain;
  ID3D11Device* device;
  ID3D11DeviceContext* deviceContext;
  ID3D11RenderTargetView* renderTargetView;
  ID3D11BlendState* blendState;
  int bbWidth, bbHeight;
  std::chrono::high_resolution_clock::time_point lastFrameTime;
  uintptr_t clientBase;
  DataManager* entityManager;
  Memory& mem;  // Change to reference

  void InitConsole();
  bool CreateDX11();
  bool CreateRTVFromSwapChain();
  void RenderFrame();
  void RenderGameContent();
  void HandleInput();
  void UpdateClickThrough();
  void UpdateImGuiInput();
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam);
};