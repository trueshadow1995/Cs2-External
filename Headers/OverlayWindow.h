#pragma once
#include <d3d11.h>
#include <windows.h>

#include <chrono>

#include "../Headers/EntityDataManager.h"
#include "../Headers/Memory.h"

class Overlay {
 public:
  Overlay(Memory& memory);
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
  Memory& mem;

  void InitConsole();
  bool CreateDX11();
  bool CreateRTVFromSwapChain();
  void RenderFrame();
  void RenderGameContent();
  void HandleInput();
  void UpdateClickThrough();
  void UpdateImGuiInput();
  void SyncOverlayToGameWindow();  // <--- was missing from header

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam);
};