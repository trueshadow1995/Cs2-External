#pragma once
#include <Windows.h>   // Must be first
#include <TlHelp32.h>  // After Windows.h
#include <iostream>
#include <string>
#include <optional>


class Memory { // Memory manipulation class
 private: // private members
  DWORD processId = 0; //process ID
  HANDLE processHandle = nullptr; //process handle

  std::optional<DWORD> FindProcessId( // find process ID by name
      const std::string_view processName) const noexcept { // find process ID by name
    PROCESSENTRY32 entry = {}; //process entry struct
    entry.dwSize = sizeof(PROCESSENTRY32); //size of struct

    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //snapshop of all processes

    if (snapshot == INVALID_HANDLE_VALUE) { //invalid snapshot
      return std::nullopt; //return nullopt
    } 

    std::optional<DWORD> result = std::nullopt; //result variable

    if (Process32First(snapshot, &entry)) { //get first process
      do {
        if (processName == entry.szExeFile) { //compare process names
          result = entry.th32ProcessID; //get process ID
          break; //process found
        }
      } while (Process32Next(snapshot, &entry)); //iterate processes
    }

    CloseHandle(snapshot); //close snapshot handle
    return result; //return result
  }

 public: //public members
  Memory(const std::string_view processName) noexcept { //constructor
    processId = *FindProcessId(processName); //find process ID
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId); //open process
  }

  ~Memory() { //destructor
    if (processHandle) { //if process handle is valid
      CloseHandle(processHandle); //close process handle
    }
  }

  const std::uintptr_t GetModuleAddress( //get module base address
      const std::string_view moduleName) const noexcept { //get module base
    MODULEENTRY32 entry = {}; //module entry struct
    entry.dwSize = sizeof(MODULEENTRY32); //size of struct

    const HANDLE snapshot = CreateToolhelp32Snapshot( //snapshot of all modules in process
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId); //snapshot of all modules in process

    if (snapshot == INVALID_HANDLE_VALUE) { //invalid snapshot
      return 0;
    }

    std::uintptr_t result = 0; //result variable

    if (Module32First(snapshot, &entry)) { //get first module
      do {
        if (moduleName == entry.szModule) { //compare module names
          result = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr); //get module base address
          break; 
        }
      } while (Module32Next(snapshot, &entry)); //iterate modules
    }

    CloseHandle(snapshot); //close snapshot handle
    return result;
  }

  template <typename T> //template function to read memory
  const T Read(const std::uintptr_t address) const noexcept {
    T value = {};
    SIZE_T bytesRead = 0;
    ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address),
                      &value, sizeof(T), &bytesRead);
    return value;
  }

  template <typename T> //template function to write memory
  void Write(const std::uintptr_t address, const T& value) const noexcept { //write memory
    SIZE_T bytesWritten = 0; //bytes written variable
    WriteProcessMemory(processHandle, reinterpret_cast<void*>(address), &value, 
                       sizeof(T), &bytesWritten); //write memory
  }

  std::string ReadString(const std::uintptr_t address,
                         const std::size_t size) const { //read string
    std::string value(size, '\0'); //string variable
    SIZE_T bytesRead = 0; //bytes read variable
    ReadProcessMemory(processHandle, reinterpret_cast<const void*>(address),
                      &value[0], size, &bytesRead); //read memory
    value.resize(bytesRead); //resize string to actual size read
    return value; //return string
  }

  template <typename Function>//template function to call virtual function
  Function call_vfunc(PVOID Base, DWORD Index) { //call virtual function
    PDWORD* VTablePointer = (PDWORD*)Base; //get vtable pointer
    PDWORD VTableFunctionBase = *VTablePointer; //get vtable function base
    DWORD dwAddress = VTableFunctionBase[Index]; //get function address
    return (Function)(dwAddress); //return function
  }

  HANDLE GetHandle() const noexcept { return processHandle; } //get process handle

  Memory() {} //default constructor
};

inline Memory mem{"cs2.exe"};