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
#include "civ2patch.h"
#include "constants.h"

typedef BOOL (WINAPI *__WriteProcessMemory__)(HANDLE,LPVOID,LPCVOID,SIZE_T,SIZE_T*);

const CHAR DLL_NAME[] = "civ2patch.dll";
const CHAR EXE_NAME[] = "civ2.exe";

PROCESS_INFORMATION g_processInfo;
STARTUPINFO g_statusInfo;
PVOID g_memAddress = NULL;
HANDLE g_hDllInjectThread = NULL;

void clean(BOOL bTerminate) {
  if (g_processInfo.hProcess) {
    if (g_hDllInjectThread) {
      CloseHandle(g_hDllInjectThread);
    }

    if (g_memAddress) {
      VirtualFreeEx(g_processInfo.hProcess, g_memAddress, sizeof(DLL_NAME), MEM_RELEASE);
    }

    if (bTerminate) {
      TerminateProcess(g_processInfo.hProcess, 1);
    }
  }
}

void ExitWithError(LPCSTR lpcsMessage)
{
  MessageBox(NULL, lpcsMessage, "Error", MB_OK | MB_ICONERROR);

  clean(TRUE);

  ExitProcess(1);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{


  ZeroMemory(&g_processInfo, sizeof(g_processInfo));
  ZeroMemory(&g_statusInfo, sizeof(g_statusInfo));
  g_statusInfo.cb = sizeof(g_statusInfo);

  if (!CreateProcess(EXE_NAME, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &g_statusInfo, &g_processInfo)) {
    ExitWithError("Failed to run 'civ2.exe'.");
  }

  HMODULE hKernel = GetModuleHandle("Kernel32");

  if (!hKernel) {
    ExitWithError("Failed to get Kernel32 module.");
  }

  FARPROC lpfLoadLibraryAddress = GetProcAddress(hKernel, "LoadLibraryA");
  __WriteProcessMemory__ lpfWriteProcessMemory = (__WriteProcessMemory__)GetProcAddress(hKernel, "WriteProcessMemory");

  if (!lpfLoadLibraryAddress || !lpfWriteProcessMemory) {
    ExitWithError("Failed to get library functions.");
  }

  g_memAddress = VirtualAllocEx(g_processInfo.hProcess, NULL, sizeof(DLL_NAME), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

  if (!g_memAddress) {
    ExitWithError("Failed to allocate memory for DLL injection.");
  }

  if (!lpfWriteProcessMemory(g_processInfo.hProcess, g_memAddress, DLL_NAME, sizeof(DLL_NAME), NULL)) {
    ExitWithError("Failed to write to memory for DLL injection.");
  }

  g_hDllInjectThread = CreateRemoteThread(g_processInfo.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpfLoadLibraryAddress, g_memAddress, 0, NULL);

  if (!g_hDllInjectThread) {
    ExitWithError("Failed to create DLL injection thread.");
  }

  // Wait 30 seconds.
  DWORD dwResult = WaitForSingleObject(g_hDllInjectThread, 30000);

  if (dwResult == WAIT_TIMEOUT || dwResult == WAIT_FAILED) {
    ExitWithError("Failed to execute DLL injection thread.");
  }

  clean(FALSE);
  ResumeThread(g_processInfo.hThread);

  return 0;
}
