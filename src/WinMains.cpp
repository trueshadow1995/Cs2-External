
#include <windows.h>
#include <tlhelp32.h>
#include <format>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include "../Headers/EntityDataManager.h"
#include "../Headers/Memory.h"
#include "../Headers/OverlayWindow.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {

  // Wait for game process (cs2.exe)
  DWORD pid = 0;
  bool gameFound = false;
  
  while (!gameFound) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
      PROCESSENTRY32 pe;
      pe.dwSize = sizeof(PROCESSENTRY32);
      if (Process32First(hSnapshot, &pe)) {
        do {
          if (std::string(pe.szExeFile) == "cs2.exe") {
            pid = pe.th32ProcessID;
            gameFound = true;
            break;
          }
        } while (Process32Next(hSnapshot, &pe));
      }
      CloseHandle(hSnapshot);
    }

    if (!gameFound) {
      Sleep(3000);
      std::cout << "Waiting for CS2...\n";
    }
  }

  std::cout << std::format("[DEBUG] CS2 found! PID = {}\n", pid);

  // Create Memory instance with the found PID
  Memory mem("cs2.exe");

  // Initialize Memory base addresses
  const auto client = mem.GetModuleAddress("client.dll");
  const auto engine2 = mem.GetModuleAddress("engine2.dll");

  std::cout << std::format("[DEBUG] client.dll base: 0x{:X}\n", client);
  std::cout << std::format("[DEBUG] engine2.dll base: 0x{:X}\n", engine2);

  // Check if modules were found
  if (client == 0 || engine2 == 0) {
    std::cerr << "[ERROR] Failed to find required game modules!\n";

    // Try again after a short delay - modules might still be loading
    Sleep(2000);
    const auto clientRetry = mem.GetModuleAddress("client.dll");
    const auto engine2Retry = mem.GetModuleAddress("engine2.dll");

    if (clientRetry == 0 || engine2Retry == 0) {
      return -1;
    }
  }

  // Open process handle
  const HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (!handle) {
    return 0;
  }

Overlay overlay(mem); //start overlay

  std::cout << "[DEBUG] Initializing Overlay...\n";
  if (!overlay.Init()) {
    std::cerr << "[ERROR] Overlay initialization failed!\n";
    return -1;
  }


  overlay.Run();

  std::cout << "[DEBUG] Overlay finished. Shutting down...\n";
  overlay.Shutdown();

  std::cout << "[DEBUG] Overlay shutdown complete.\n";
  FreeConsole();
  return 0;
  
 

}

