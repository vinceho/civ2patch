/**
 * This file is part of Civ 2 MGE Patch.
 * Copyright (C) 2016 Vincent Ho
 *
 * Civ 2 MGE Patch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Civ 2 MGE Patch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Civ 2 MGE Patch.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "inject.h"
#include "constants.h"
#include "log.h"

HMODULE GetCurrentModuleHandle()
{
  HMODULE hModule = GetModuleHandle(MODULE_NAME);

  if (!hModule) {
    LogError("Failed to load %s.", MODULE_NAME);
  }

  return hModule;
}

BOOL WriteMemory(HANDLE hProcess, LPVOID lpvData, DWORD dwBytes, DWORD dwTargetAddress)
{
  BOOL bSuccess = WriteProcessMemory(hProcess, (LPVOID)dwTargetAddress, lpvData, dwBytes, NULL);

  if (!bSuccess) {
    LogError("Failed to write to address 0x%x.", dwTargetAddress);
  }

  return bSuccess;
}

BOOL HookWindowsAPI(HANDLE hProcess, HMODULE hModule, LPCSTR lpcsFunctionName, DWORD dwTargetAddress)
{
  DWORD dwAddress = (DWORD)GetProcAddress(hModule, lpcsFunctionName);

  if (!dwAddress) {
    LogError("Failed to get address of %s.", lpcsFunctionName);

    return FALSE;
  }

  DWORD dwSize = 0;
  BYTE readBuffer;

  if (ReadProcessMemory(hProcess, (LPVOID)dwTargetAddress, &readBuffer, 1, NULL)) {
    if (readBuffer == 0xFF) {
      // Opcode call:ds is 6 bytes.
      dwSize = 6;
    } else if (readBuffer == 0xE8) {
      // Opcode call is 5 bytes.
      dwSize = 5;
    } else {
      LogError("Failed to hook '%s' due to unknown opcode at address 0x%x.", lpcsFunctionName, dwTargetAddress);

      return FALSE;
    }
  } else {
    LogError("Failed to hook '%s' due to error reading address 0x%x.", lpcsFunctionName, dwTargetAddress);

    return FALSE;
  }

  BYTE buffer[] = {0xE8, 0x90, 0x90, 0x90, 0x90, 0x90};
  DWORD dwDistance = ((DWORD)dwAddress - (DWORD)dwTargetAddress - 5);

  memcpy(&buffer[1], &dwDistance, 4);

  return WriteMemory(hProcess, buffer, dwSize, dwTargetAddress);
}

BOOL ConvertValueToByteArray(DWORD dwValue, DWORD dwSize, BYTE *buffer)
{
  memcpy(buffer, &dwValue, dwSize);

  return TRUE;
}
