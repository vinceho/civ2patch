#ifndef INJECT_H
#define INJECT_H

#include <windows.h>

BOOL WriteMemory(HANDLE hProcess, LPVOID lpvData, DWORD dwBytes, DWORD dwTargetAddress);
BOOL HookWindowsAPI(HANDLE hProcess, HMODULE hModule, LPCSTR lpcsFunctionName, DWORD dwTargetAddress);
HMODULE GetCurrentModuleHandle();
BOOL ConvertValueToByteArray(DWORD dwValue, DWORD dwSize, BYTE *buffer);

#endif // INJECT_H
