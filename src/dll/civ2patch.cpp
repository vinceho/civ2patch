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
#include <stdio.h>
#include <windows.h>
#include "civ2patch.h"
#include "inject.h"
#include "game.h"
#include "config.h"
#include "timer.h"
#include "log.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
  HANDLE hProcess = NULL;
  BOOL bSuccess = TRUE;

  switch (dwReason)
  {
    case DLL_PROCESS_ATTACH:
      InitializeConfig();
      InitializeLog();
      InitializeTimer();

      hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

      if (hProcess) {
        bSuccess = PatchGame(hProcess);
        CloseHandle(hProcess);
      } else {
        LogError("Failed to open process.");
      }
      break;
    case DLL_PROCESS_DETACH:
      ShutdownLog();
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
  }

  return bSuccess;
}
