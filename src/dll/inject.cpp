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
    Log("ERROR: Failed to load %s.\n", MODULE_NAME);
  }

  return hModule;
}

BOOL WriteMemory(HANDLE hProcess, LPVOID lpvData, DWORD dwBytes, DWORD dwTargetAddress)
{
  BOOL bSuccess = WriteProcessMemory(hProcess, (LPVOID)dwTargetAddress, lpvData, dwBytes, NULL);

  if (!bSuccess) {
    Log("ERROR: Failed to write to address 0x%x.\n", dwTargetAddress);
  }

  return bSuccess;
}

BOOL HookWindowsAPI(HANDLE hProcess, HMODULE hModule, LPCSTR lpsFunctionName, DWORD dwTargetAddress)
{
  DWORD dwAddress = (DWORD)GetProcAddress(hModule, lpsFunctionName);

  if (!dwAddress) {
    Log("ERROR: Failed to get address of %s.\n", lpsFunctionName);

    return FALSE;
  }

  // Destination call:ds is 6 bytes. However, only 5 is needed for the hook.
  DWORD dwSize = 6;
  BYTE buffer[dwSize] = {0xE8, 0x90, 0x90, 0x90, 0x90, 0x90};
  DWORD dwDistance = ((DWORD)dwAddress - (DWORD)dwTargetAddress - 5);

  memcpy(&buffer[1], &dwDistance, 4);

  return WriteMemory(hProcess, buffer, dwSize, dwTargetAddress);
}

BOOL ConvertValueToByteArray(DWORD dwValue, DWORD dwSize, BYTE *buffer)
{
  memcpy(buffer, &dwValue, dwSize);

  return TRUE;
}
