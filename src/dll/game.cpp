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
#include "civ2patch.h"
#include "constants.h"
#include "game.h"
#include "inject.h"
#include "config.h"
#include "log.h"
#include "audio.h"

#define PEEK_MESSAGE_HOOK "PeekMessageEx"
#define MCI_SEND_CMD_HOOK "mciSendCommandEx"

DWORD g_dwLastMessagePurgeTime = 0;
DWORD g_dwPurgeMessagesInterval = 0;
DWORD g_dwMessageWaitTimeout = 0;
DWORD g_dwStartTime = 0;
DWORD g_dwTotalSleepTime = 0;
FLOAT g_fSleepRatio = 0.0f;

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

BOOL PatchGame(HANDLE hProcess)
{
  BOOL bSuccess = TRUE;

  g_dwPurgeMessagesInterval = g_config.dwPurgeMessagesInterval;
  g_dwMessageWaitTimeout = g_config.dwMessageWaitTimeout;
  g_fSleepRatio = g_config.fSleepRatio;

  if (g_config.bMusic) bSuccess &= PatchMediaPlayback(hProcess);
  if (g_config.bFixCpu) bSuccess &= PatchIdleCpu(hProcess);
  if (g_config.bFix64BitCompatibility) bSuccess &= Patch64BitCompatibility(hProcess);
  if (g_config.bNoCdCheck) bSuccess &= PatchCdCheck(hProcess);
  if (g_config.bFixHostileAi) bSuccess &= PatchHostileAi(hProcess);
  if (g_config.bSetRetirementYear) bSuccess &= PatchTimeLimit(hProcess, g_config.dwRetirementYear, g_config.dwRetirementWarningYear);
  if (g_config.bSetCombatAnimationLength) bSuccess &= PatchFastCombat(hProcess, g_config.dwCombatAnimationLength);
  if (g_config.bSetPopulationLimit) bSuccess &= PatchPopulationLimit(hProcess, g_config.dwPopulationLimit);
  if (g_config.bSetGoldLimit) bSuccess &= PatchGoldLimit(hProcess, g_config.dwGoldLimit);
  if (g_config.bSetMapTilesLimit) bSuccess &= PatchMapTilesLimit(hProcess, g_config.dwMapTilesLimit);

  return bSuccess;
}

BOOL PatchIdleCpu(HANDLE hProcess)
{
  DWORD dwAddressList[] = { 0x5BBA64, 0x5BBB91, 0x5BD2F9, 0x5BD31D};
  DWORD dwSize = sizeof(dwAddressList) / sizeof(DWORD);
  BOOL bSuccess = TRUE;
  HMODULE hModule = GetCurrentModuleHandle();

  if (!hModule) {
    return FALSE;
  }

  for (DWORD i = 0; i < dwSize; i++) {
    bSuccess = HookWindowsAPI(hProcess, hModule, PEEK_MESSAGE_HOOK, dwAddressList[i]);

    if (!bSuccess) {
      Log("ERROR: Failed to patch idle cpu.");
      break;
    }
  }

  return bSuccess;
}

BOOL PatchMediaPlayback(HANDLE hProcess)
{
  DWORD dwAddressList[] = {
    0x5DDA8B, 0x5DDADE, 0x5DDAFE, 0x5DDB58, 0x5DDB7C, 0x5DDB9C, 0x5DDC1F, 0x5DDC43, 0x5DDCD3, 0x5DDD34,
    0x5DDD7B, 0x5DDDC7, 0x5DDDE9, 0x5DDE3F, 0x5DDE79, 0x5DDEBF, 0x5DDF2C, 0x5DDF70, 0x5DDF90, 0x5DDFD0,
    0x5DE03A, 0x5DE06B, 0x5DE09D, 0x5DE0EF, 0x5DE120, 0x5DE133, 0x5DE17A, 0x5DE18D, 0x5DE1C7, 0x5DE206,
    0x5DE23D, 0x5DE27A, 0x5DE2B9, 0x5DE2FD, 0x5DE34A, 0x5DE38F, 0x5DE3CC, 0x5DE458, 0x5DE49D, 0x5DE4DA,
    0x5DE572, 0x5DE5AB, 0x5EC789, 0x5EDE1A, 0x5EDE61, 0x5EDEAF, 0x5EDF11, 0x5EDF85, 0x5EDFA7, 0x5EE039,
    0x5EE06A, 0x5EE0A1, 0x5EE4B8, 0x5EE4ED, 0x5EE518, 0x5EE549, 0x5EE57E, 0x5EE654, 0x5EE679, 0x5EE69E
  };
  DWORD dwSize = sizeof(dwAddressList) / sizeof(DWORD);
  BOOL bSuccess = TRUE;
  HMODULE hModule = GetCurrentModuleHandle();

  if (!hModule) {
    return FALSE;
  }

  for (DWORD i = 0; i < dwSize; i++) {
    bSuccess = HookWindowsAPI(hProcess, hModule, MCI_SEND_CMD_HOOK, dwAddressList[i]);

    if (!bSuccess) {
      Log("ERROR: Failed to patch media playback.");
      break;
    }
  }

  return bSuccess;
}

BOOL PatchMapTilesLimit(HANDLE hProcess, DWORD dwMapTilesLimit)
{
  // Change max number of map tiles from 10,000 to 32,767.
  BYTE buffer[2];
  ConvertValueToByteArray(dwMapTilesLimit, 2, buffer);

  BOOL bSuccess = WriteMemory(hProcess, buffer, 2, 0x41D6FF);

  if (!bSuccess) {
    Log("ERROR: Failed to patch map tiles limit.");
  }

  return bSuccess;
}

BOOL PatchHostileAi(HANDLE hProcess)
{
  BYTE buffer[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
  BOOL bSuccess = WriteMemory(hProcess, buffer, 8, 0x561FC9);

  if(!bSuccess) {
    Log("ERROR: Failed to patch hostile AI.");
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

  Log("ERROR: Failed to patch CD check.");

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
    Log("ERROR: Failed to patch 64bit compatibility.");
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

  Log("ERROR: Failed to patch time limit.");

  return FALSE;
}

BOOL PatchPopulationLimit(HANDLE hProcess, DWORD dwPopulationLimit)
{
  // Change max population from 320,000,000.
  BYTE buffer[4];
  ConvertValueToByteArray(dwPopulationLimit, 4, buffer);

  BOOL bSuccess = (WriteMemory(hProcess, buffer, 4, 0x43CD74) && WriteMemory(hProcess, buffer, 4, 0x43CD81));

  if (!bSuccess) {
    Log("ERROR: Failed to patch population limit.");
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
    Log("ERROR: Failed to patch gold limit.");
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
    Log("ERROR: Failed to patch fast combat.");
  }

  return bSuccess;
}

BOOL CIV2PATCH_API PeekMessageEx(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
  DWORD dwNow = timeGetTime();

  if (!g_dwTotalSleepTime) {
    MsgWaitForMultipleObjectsEx(0, 0, g_dwMessageWaitTimeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

    g_dwStartTime = dwNow;
    g_dwTotalSleepTime += (timeGetTime() - dwNow);
  } else if (((FLOAT)(dwNow - g_dwStartTime - g_dwTotalSleepTime) / (FLOAT)g_dwTotalSleepTime) >= g_fSleepRatio) {
    MsgWaitForMultipleObjectsEx(0, 0, g_dwMessageWaitTimeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
    g_dwTotalSleepTime += (timeGetTime() - dwNow);
  }

  // Civilization 2 uses filter value 957 as a spinning wait.
  if (wMsgFilterMin == 957) {
    if (!g_dwLastMessagePurgeTime) {
      g_dwLastMessagePurgeTime = dwNow;
    }

    // Purge message queue to fix "Not Responding" problem during long AI turns.
    if ((dwNow - g_dwLastMessagePurgeTime) >= g_dwPurgeMessagesInterval) {
      if (LOWORD(GetQueueStatus(QS_ALLINPUT))) {
        MSG msg;

        while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE)) {
          TranslateMessage(&msg);
          DispatchMessageA(&msg);
        }
      }

      g_dwLastMessagePurgeTime = dwNow;
    }
  } else {
    g_dwLastMessagePurgeTime = dwNow;
  }

  return PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

MCIERROR CIV2PATCH_API mciSendCommandEx(MCIDEVICEID wProxyId, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam)
{
  if (uMsg == MCI_OPEN && (fdwCommand & MCI_OPEN_TYPE)) {
    MCI_OPEN_PARMS *params = (MCI_OPEN_PARMS *)dwParam;

    if (params && !strcmp(params->lpstrDeviceType, MUSIC_DEVICE_NAME)) {
      if (!InitializeAudio(g_config.dwMusicFreq, g_config.dwMusicChunkSize, g_config.dwMusicVolume, g_config.dwMusicAlbum)) {
        return MCIERR_INTERNAL;
      }

      params->wDeviceID = AddMciDevice(MUSIC_DEVICE_ID);

      return 0;
    }
  } else if (IsMusicMciDevice(wProxyId)) {
    if (uMsg == MCI_CLOSE) {
      if (!CloseAudio()) {
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
