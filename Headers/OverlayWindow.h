#pragma once
#include <d3d11.h>
#include <windows.h>
#include "../Headers/Aimbot.h"
#include <chrono>
#include "../Headers/Aimbot.h"  // ADD THIS LINE
#include "../Headers/EntityDataManager.h"
#include "../Headers/Memory.h"
#include "../Headers/FovVisuals.h"
class Overlay {
 public:
  Overlay(Memory& memory);  
  ~Overlay();

  bool Init();
  void Run();
  void Shutdown();

  void DrawDebugTest();
  void StartAimbot();
  void StopAimbot();
 private:
  HINSTANCE hInstance;
  HWND hwnd;
  bool running;
  Aimbot* aimbot;  


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

  void InitConsole(); //<- DEBUG purposes ;) 
  bool CreateDX11();
  bool CreateRTVFromSwapChain();
  void RenderFrame();
  void RenderGameContent();
  void HandleInput();
  void UpdateClickThrough();
  void UpdateImGuiInput();
  void SyncOverlayToGameWindow(); 

  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam);
};