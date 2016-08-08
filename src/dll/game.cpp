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
#include <SDL2/SDL.h>
#include <math.h>
#include "civ2patch.h"
#include "constants.h"
#include "game.h"
#include "inject.h"
#include "hook.h"
#include "config.h"
#include "log.h"
#include "audio.h"
#include "timer.h"

DWORD g_dwMessageWaitTimeout = 0;
DOUBLE g_dLastMessagePurgeTime = 0.0;
DOUBLE g_dPurgeMessagesInterval = 0.0;
DOUBLE g_dStartTime = 0.0;
DOUBLE g_dTotalSleepTime = 0.0;
DOUBLE g_dSleepRatio = 0.0;
DOUBLE g_dCpuSamplingInterval = 0.0;

BOOL PatchIdleCpu(HANDLE);
BOOL PatchHostileAi(HANDLE);
BOOL PatchCdCheck(HANDLE);
BOOL Patch64BitCompatibility(HANDLE);
BOOL PatchMapTilesLimit(HANDLE, DWORD);
BOOL PatchTimeLimit(HANDLE, DWORD, DWORD);
BOOL PatchPopulationLimit(HANDLE, DWORD);
BOOL PatchGoldLimit(HANDLE, DWORD);
BOOL PatchMediaPlayback(HANDLE);
BOOL PatchFastCombat(HANDLE, DWORD);
BOOL PatchMultiplayer(HANDLE hProcess);

BOOL PatchGame(HANDLE hProcess)
{
  BOOL bSuccess = TRUE;

  g_dwMessageWaitTimeout = GetMessageWaitTimeout();
  g_dPurgeMessagesInterval = round((DOUBLE)GetPurgeMessagesInterval() * 1000.0);
  g_dCpuSamplingInterval = round((DOUBLE)GetCpuSamplingInterval() * 1000.0);
  g_dSleepRatio = (DOUBLE)GetSleepRatio();

  if (IsMusicEnabled()) bSuccess &= PatchMediaPlayback(hProcess);
  if (IsMultiplayerEnabled()) bSuccess &= PatchMultiplayer(hProcess);
  if (IsFixIdleCpuEnabled()) bSuccess &= PatchIdleCpu(hProcess);
  if (IsFix64BitEnabled()) bSuccess &= Patch64BitCompatibility(hProcess);
  if (IsNoCdCheckEnabled()) bSuccess &= PatchCdCheck(hProcess);
  if (IsFixHostileAiEnabled()) bSuccess &= PatchHostileAi(hProcess);
  if (IsSetRetirementYearEnabled()) bSuccess &= PatchTimeLimit(hProcess, GetRetirementYear(), GetRetirementWarningYear());
  if (IsSetCombatAnimationLengthEnabled()) bSuccess &= PatchFastCombat(hProcess, GetCombatAnimationLength());
  if (IsSetPopulationLimitEnabled()) bSuccess &= PatchPopulationLimit(hProcess, GetPopulationLimit());
  if (IsSetGoldLimitEnabled()) bSuccess &= PatchGoldLimit(hProcess, GetGoldLimit());
  if (IsSetMapTilesLimitEnabled()) bSuccess &= PatchMapTilesLimit(hProcess, GetMapTilesLimit());

  return bSuccess;
}

BOOL PatchIdleCpu(HANDLE hProcess)
{
  const FunctionHook *hook = GetFunctionHook(PEEK_MESSAGE_HOOK);
  LPDWORD lpdwAddressList = hook->addresses;
  DWORD dwSize = hook->dwNumAddress;
  LPCSTR lpcsFunctionName = hook->lpcsNewFunction;
  BOOL bSuccess = TRUE;
  HMODULE hModule = GetCurrentModuleHandle();

  if (!hModule) {
    return FALSE;
  }

  for (DWORD i = 0; i < dwSize; i++) {
    bSuccess = HookWindowsAPI(hProcess, hModule, lpcsFunctionName, lpdwAddressList[i]);

    if (!bSuccess) {
      LogError("Failed to patch idle CPU.");
      break;
    }
  }

  return bSuccess;
}

BOOL PatchMediaPlayback(HANDLE hProcess)
{
  const FunctionHook *hook = GetFunctionHook(MCI_SEND_CMD_HOOK);
  LPDWORD lpdwAddressList = hook->addresses;
  DWORD dwSize = hook->dwNumAddress;
  LPCSTR lpcsFunctionName = hook->lpcsNewFunction;
  BOOL bSuccess = TRUE;
  HMODULE hModule = GetCurrentModuleHandle();

  if (!hModule) {
    return FALSE;
  }

  for (DWORD i = 0; i < dwSize; i++) {
    bSuccess = HookWindowsAPI(hProcess, hModule, lpcsFunctionName, lpdwAddressList[i]);

    if (!bSuccess) {
      LogError("Failed to patch media playback.");
      break;
    }
  }

  return bSuccess;
}

BOOL PatchMultiplayer(HANDLE hProcess)
{
  HMODULE hModule = GetCurrentModuleHandle();

  if (!hModule) {
    return FALSE;
  }

  for (int i = XD_ACTIVATE_SERVER_HOOK; i <= XD_STOP_CONN_HOOK; i++) {
    const FunctionHook *hook = GetFunctionHook((FunctionHookEnum)i);
    LPDWORD lpdwAddressList = hook->addresses;
    DWORD dwSize = hook->dwNumAddress;
    LPCSTR lpcsFunctionName = hook->lpcsNewFunction;

    for (DWORD j = 0; j < dwSize; j++) {
      if (!HookWindowsAPI(hProcess, hModule, lpcsFunctionName, lpdwAddressList[j])) {
        LogError("Failed to patch multiplayer.");

        return FALSE;
      }
    }
  }

  return TRUE;
}

BOOL PatchMapTilesLimit(HANDLE hProcess, DWORD dwMapTilesLimit)
{
  // Change max number of map tiles from 10,000.
  BYTE buffer[2];
  ConvertValueToByteArray(dwMapTilesLimit, 2, buffer);

  BOOL bSuccess = WriteMemory(hProcess, buffer, 2, 0x41D6FF);

  if (!bSuccess) {
    LogError("Failed to patch map tiles limit.");
  }

  return bSuccess;
}

BOOL PatchHostileAi(HANDLE hProcess)
{
  BYTE buffer[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
  BOOL bSuccess = WriteMemory(hProcess, buffer, 8, 0x561FC9);

  if(!bSuccess) {
    LogError("Failed to patch hostile AI.");
  }

  return bSuccess;
}

BOOL PatchCdCheck(HANDLE hProcess)
{
  {
    BYTE buffer[] = {0x03};

    if(!WriteMemory(hProcess, buffer, 1, 0x56463C)) {
      goto PATCH_CD_CHECK_FAILED;
    }
  }

  {
    BYTE buffer[] = {
      0xEB, 0x12, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
      0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };

    if (!WriteMemory(hProcess, buffer, 20, 0x56467A)) {
      goto PATCH_CD_CHECK_FAILED;
    }
  }

  {
    BYTE buffer[] = {0x80};

    if (!WriteMemory(hProcess, buffer, 1, 0x5646A7)) {
      goto PATCH_CD_CHECK_FAILED;
    }
  }

  return TRUE;

PATCH_CD_CHECK_FAILED:

  LogError("Failed to patch CD check.");

  return FALSE;
}

BOOL Patch64BitCompatibility(HANDLE hProcess)
{
  BYTE buffer[] = {
    0x50, 0x8B, 0x45, 0x08, 0x50, 0xFF, 0x15, 0x2C, 0x7E, 0x6E,
    0x00, 0x89, 0x45, 0xEC, 0x8B, 0x45, 0xE4, 0x25, 0xFF, 0x00,
    0x00, 0x00, 0x90
  };
  BOOL bSuccess = WriteMemory(hProcess, buffer, 23, 0x5D2A28);

  if (!bSuccess) {
    LogError("Failed to patch 64bit compatibility.");
  }

  return bSuccess;
}

BOOL PatchTimeLimit(HANDLE hProcess, DWORD dwRetirementYear, DWORD dwRetirementWarningYear)
{
  {
    // Change retirement warning year from 2000.
    BYTE buffer[2];
    ConvertValueToByteArray(dwRetirementWarningYear, 2, buffer);

    if(!WriteMemory(hProcess, buffer, 2, 0x48B069)) {
      goto PATCH_TIME_LIMIT_FAILED;
    }
  }

  {
    // Change force retirement year from 2020.
    BYTE buffer[2];
    ConvertValueToByteArray(dwRetirementYear, 2, buffer);

    if(!WriteMemory(hProcess, buffer, 2, 0x48B2AD) || !WriteMemory(hProcess, buffer, 2, 0x48B0BB)) {
      goto PATCH_TIME_LIMIT_FAILED;
    }
  }

  return TRUE;

PATCH_TIME_LIMIT_FAILED:

  LogError("Failed to patch time limit.");

  return FALSE;
}

BOOL PatchPopulationLimit(HANDLE hProcess, DWORD dwPopulationLimit)
{
  // Change max population from 320,000,000.
  BYTE buffer[4];
  ConvertValueToByteArray(dwPopulationLimit, 4, buffer);

  BOOL bSuccess = (WriteMemory(hProcess, buffer, 4, 0x43CD74) && WriteMemory(hProcess, buffer, 4, 0x43CD81));

  if (!bSuccess) {
    LogError("Failed to patch population limit.");
  }

  return bSuccess;
}

BOOL PatchGoldLimit(HANDLE hProcess, DWORD dwGoldLimit)
{
  // Change max gold from 30,000.
  BYTE buffer[4];
  ConvertValueToByteArray(dwGoldLimit, 4, buffer);

  BOOL bSuccess = (WriteMemory(hProcess, buffer, 4, 0x489608) && WriteMemory(hProcess, buffer, 4, 0x48962A));

  if (!bSuccess) {
    LogError("Failed to patch gold limit.");
  }

  return bSuccess;
}

BOOL PatchFastCombat(HANDLE hProcess, DWORD dwCombatAnimationFrameLength)
{
  // Each combat frame duration reduced from 64ms.
  BYTE buffer[1];
  ConvertValueToByteArray(dwCombatAnimationFrameLength, 1, buffer);

  BOOL bSuccess = WriteMemory(hProcess, buffer, 1, 0x57F4F6);

  if (!bSuccess) {
    LogError("Failed to patch fast combat.");
  }

  return bSuccess;
}

/**
 * Messaging overrides.
 */
BOOL CIV2PATCH_API PeekMessageEx(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
  DOUBLE dBeginTime = GetTimerCurrentTime();

  // Civilization 2 uses filter value 957 as a spinning wait.
  if (wMsgFilterMin == 957) {
    DOUBLE dElapsed = dBeginTime - g_dStartTime;

    if (g_dTotalSleepTime < 1.0 || dElapsed < 1.0) {
      MsgWaitForMultipleObjectsEx(0, 0, g_dwMessageWaitTimeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

      DOUBLE dNow = GetTimerCurrentTime();

      // Prime the counters.
      g_dStartTime = dBeginTime;
      g_dTotalSleepTime = (dNow > dBeginTime) ? (dNow - dBeginTime) : 1000.0;
    } else if (((dElapsed - g_dTotalSleepTime) / g_dTotalSleepTime) >= g_dSleepRatio) {
      MsgWaitForMultipleObjectsEx(0, 0, g_dwMessageWaitTimeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

      DOUBLE dNow = GetTimerCurrentTime();

      // Overflow check.
      if (dNow >= dBeginTime) {
        if (dNow == dBeginTime) {
          // Low resolution timer. Add 1 milliseconds to make up for poor precision.
          g_dTotalSleepTime += 1000.0;
        } else {
          g_dTotalSleepTime += (dNow - dBeginTime);
        }
      } else {
        g_dTotalSleepTime = 0.0;
      }

      // Reset
      if (dElapsed >= g_dCpuSamplingInterval) {
        g_dTotalSleepTime = 0.0;
      }
    }

    // Prime last purge time.
    if (g_dLastMessagePurgeTime < 1.0) {
      g_dLastMessagePurgeTime = dBeginTime;
    }

    // Purge message queue to fix "Not Responding" problem during long AI turns.
    if ((dBeginTime - g_dLastMessagePurgeTime) >= g_dPurgeMessagesInterval) {
      if (LOWORD(GetQueueStatus(QS_ALLINPUT))) {
        MSG msg;

        while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
          TranslateMessage(&msg);
          DispatchMessageA(&msg);
        }
      }

      g_dLastMessagePurgeTime = dBeginTime;
    }
  } else {
    g_dLastMessagePurgeTime = dBeginTime;
  }

  return PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

/**
 * Media overrides.
 */
MCIERROR CIV2PATCH_API mciSendCommandEx(MCIDEVICEID wProxyId, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam)
{
  if (uMsg == MCI_OPEN && (fdwCommand & MCI_OPEN_TYPE)) {
    MCI_OPEN_PARMS *params = (MCI_OPEN_PARMS *)dwParam;

    if (params && !strcmp(params->lpstrDeviceType, MUSIC_DEVICE_NAME)) {
      if (!InitializeAudio(GetMusicFrequency(), GetMusicChunkSize(), GetMusicVolume(), GetMusicAlbum())) {
        return MCIERR_INTERNAL;
      }

      params->wDeviceID = AddMciDevice(MUSIC_DEVICE_ID);

      return 0;
    }
  } else if (IsMusicMciDevice(wProxyId)) {
    if (uMsg == MCI_CLOSE) {
      if (!ShutdownAudio()) {
        return MCIERR_INTERNAL;
      }
    } else if (uMsg == MCI_STATUS) {
      MCI_STATUS_PARMS *params = (MCI_STATUS_PARMS *)dwParam;

      if (params) {
        if (fdwCommand & MCI_STATUS_ITEM) {
          if (params->dwItem & MCI_STATUS_NUMBER_OF_TRACKS) {
            params->dwReturn = GetNumberMusicTracks();
          }
        }
      }

      return 0;
    } else if (uMsg == MCI_STOP) {
      StopMusic();
    } else if (uMsg == MCI_PLAY) {
      MCI_PLAY_PARMS *params = (MCI_PLAY_PARMS *)dwParam;

      if (params) {
        DWORD dwTrackFrom = (fdwCommand & MCI_FROM) ? params->dwFrom : 0;
        HWND hCallback = (fdwCommand & MCI_NOTIFY) ? (HWND)LOWORD(params->dwCallback) : 0;

        // Music track number starts at 2.
        if (PlayMusic(dwTrackFrom - 1, hCallback)) {
          return 0;
        }
      }

      return MCIERR_INTERNAL;
    }

    return 0;
  }

  // Forward non CD audio MCI calls.
  if (!IsMusicMciDevice(wProxyId)) {
    MCIERROR mciError = mciSendCommand(GetMciDevice(wProxyId), uMsg, fdwCommand, dwParam);

    // Manage device Id so it does not conflict with the fake CD audio device Id.
    if (uMsg == MCI_OPEN) {
      MCI_OPEN_PARMS *params = (MCI_OPEN_PARMS *)dwParam;

      if (params) {
        params->wDeviceID = AddMciDevice(params->wDeviceID);
      }
    } else if (uMsg == MCI_CLOSE) {
      RemoveMciDevice(wProxyId);
    }

    return mciError;
  } else {
    return MCIERR_INTERNAL;
  }
}
