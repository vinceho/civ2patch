#include <stdio.h>
#include <windows.h>
#include "civ2patch.h"
#include "inject.h"
#include "game.h"
#include "log.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
  HANDLE hProcess = NULL;

  switch (dwReason)
  {
    case DLL_PROCESS_ATTACH:
      InitializeLog();

      hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

      if (hProcess) {
        PatchIdleCpu(hProcess);
        PatchMapTilesLimit(hProcess);
        PatchHostileAi(hProcess);
        PatchCdCheck(hProcess);
        Patch64BitCompatibility(hProcess);
        PatchTimeLimit(hProcess);
        PatchPopulationLimit(hProcess);
        PatchGoldLimit(hProcess);
        PatchMediaPlayback(hProcess);
        PatchFastCombat(hProcess);

        CloseHandle(hProcess);
      } else {
        Log("ERROR: Failed to open process.");
      }
      break;
    case DLL_PROCESS_DETACH:
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
  }

  return TRUE;
}
