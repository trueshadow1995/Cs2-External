
#include <Windows.h>
#pragma once
#include <TlHelp32.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

class Memory {
 private:
  DWORD processId = 0;
  HANDLE processHandle = nullptr;

  std::optional<DWORD> FindProcessId(
      const std::string_view processName) const noexcept {
    PROCESSENTRY32 entry = {};
    entry.dwSize = sizeof(PROCESSENTRY32);

    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return std::nullopt;

    std::optional<DWORD> result = std::nullopt;

    if (Process32First(snapshot, &entry)) {
      do {
        if (processName == entry.szExeFile) {
          result = entry.th32ProcessID;
          break;
        }
      } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return result;
  }

 public:
  Memory(const std::string_view processName) noexcept {
    auto pid = FindProcessId("cs2.exe");
    if (pid.has_value()) {
      processId = pid.value();
      processHandle = OpenProcess(PROCESS_VM_READ, FALSE, processId);
    }
  }

  ~Memory() {
    if (processHandle) CloseHandle(processHandle);
  }

  // Get module address by name
  uintptr_t GetModuleAddress(const std::string_view moduleName) const noexcept {
    MODULEENTRY32 entry = {};
    entry.dwSize = sizeof(MODULEENTRY32);

    const HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    uintptr_t result = 0;

    if (Module32First(snapshot, &entry)) {
      do {
        if (moduleName == entry.szModule) {
          result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
          break;
        }
      } while (Module32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return result;
  }

  template <typename T>
  T Read(const uintptr_t address) const noexcept {
    T value{};
    if (processHandle && address) {
      SIZE_T bytesRead = 0;
      ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address),
                        &value, sizeof(T), &bytesRead);
    }
    return value;
  }

  // Batch read multiple addresses
  template <typename T>
  std::vector<T> ReadBatch(
      const std::vector<uintptr_t>& addresses) const noexcept {
    std::vector<T> results;
    results.reserve(addresses.size());

    for (const auto& address : addresses) {
      results.push_back(Read<T>(address));
    }
    return results;
  }

  bool IsValidAddress(uintptr_t address) const noexcept {
    if (!processHandle || !address) return false;
    MEMORY_BASIC_INFORMATION mbi;
    return VirtualQueryEx(processHandle, reinterpret_cast<LPCVOID>(address),
                          &mbi, sizeof(mbi)) &&
           (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ |
                           PAGE_EXECUTE_READWRITE)) &&
           !(mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS));
  }

  HANDLE GetHandle() const noexcept { return processHandle; }
  DWORD GetProcessId() const noexcept { return processId; }
  bool IsValid() const noexcept {
    return processHandle != nullptr && processId != 0;
  }
};