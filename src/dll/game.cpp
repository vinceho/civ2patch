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
#include "net.h"
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
  DOUBLE dElapsed = dBeginTime - g_dStartTime;

  if (g_dTotalSleepTime < 1.0 || dElapsed < 1.0) {
    MsgWaitForMultipleObjectsEx(0, 0, g_dwMessageWaitTimeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

    DOUBLE dNow = GetTimerCurrentTime();

    // Prime the counters.
    g_dStartTime = dBeginTime;
    g_dTotalSleepTime = (dNow > dBeginTime) ? (dNow - dBeginTime) : 500.0;
  } else if (((dElapsed - g_dTotalSleepTime) / g_dTotalSleepTime) >= g_dSleepRatio) {
    MsgWaitForMultipleObjectsEx(0, 0, g_dwMessageWaitTimeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

    DOUBLE dNow = GetTimerCurrentTime();

    // Overflow check.
    if (dNow >= dBeginTime) {
      if (dNow == dBeginTime) {
        // Low resolution timer. Add 0.5 milliseconds to make up for poor precision.
        g_dTotalSleepTime += 500.0;
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

  // Civilization 2 uses filter value 957 as a spinning wait.
  if (wMsgFilterMin == 957) {
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

/**
 * Network overrides.
 */
INT CIV2PATCH_API XD_ActivateServer(void)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_ACTIVATE_SERVER_HOOK);

  LogDebug("XD_ActivateServer - Starting.");

  if (fpAddress) {
    nResult = ((INT (*)(void))fpAddress)();
  }

  LogDebug("XD_ActivateServer - Return(%d).", nResult);

  return nResult;
}

INT CIV2PATCH_API XD_CloseConnection(void)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_CLOSE_CONN_HOOK);

  LogDebug("XD_CloseConnection - Starting.");

  if (fpAddress) {
    nResult = ((INT (*)(void))fpAddress)();
  }

  LogDebug("XD_CloseConnection - Return(%d).", nResult);

  return nResult;
}

/**
 * @param dwTimeout The timeout in milliseconds.
 *
 * @return The number of items left to flush before the timing out.
 */
INT CIV2PATCH_API XD_FlushSendBuffer(DWORD dwTimeout)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_FLUSH_SEND_BUFFER_HOOK);

  LogDebug("XD_FlushSendBuffer(%d) - Starting.", dwTimeout);

  if (fpAddress) {
    nResult = ((INT (*)(DWORD))fpAddress)(dwTimeout);
  }

  LogDebug("XD_FlushSendBuffer(%d) - Return(%d).", dwTimeout, nResult);

  return nResult;
}

LPSTR CIV2PATCH_API XD_GetCurrentProtoAddr(UINT unArg, INT nArg)
{
  LPSTR lpsResult = NULL;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_GET_CURRENT_PROTO_ADDR_HOOK);

  LogDebug("XD_GetCurrentProtoAddr(%d, %d) - Starting.", unArg, nArg);

  if (fpAddress) {
    lpsResult = ((LPSTR (*)(UINT, INT))fpAddress)(unArg, nArg);
  }

  LogDebug("XD_GetCurrentProtoAddr(%d, %d) - Return(%s).", unArg, nArg, lpsResult);

  return lpsResult;
}

/**
 * Get the version of the multiplayer library.
 *
 * @param lpnMajor The major version number output.
 * @param lpnMinor The minor version number output.
 * @param lpnRevision The revision number output.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API XD_GetXDaemonVersion(LPINT lpnMajor, LPINT lpnMinor, LPINT lpnRevision)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_GET_DAEMON_VERSION_HOOK);

  LogDebug("XD_GetXDaemonVersion(%d, %d, %d) - Starting.", *lpnMajor, *lpnMinor, *lpnRevision);

  if (fpAddress) {
    nResult = ((INT (*)(LPINT, LPINT, LPINT))fpAddress)(lpnMajor, lpnMinor, lpnRevision);
  }

  LogDebug("XD_GetXDaemonVersion(%d, %d, %d) - Return(%d).", *lpnMajor, *lpnMinor, *lpnRevision, nResult);

  return nResult;
}

INT CIV2PATCH_API XD_InFlushSendBuffer(void)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_IN_FLUSH_SEND_BUFFER_HOOK);

  //LogDebug("XD_InFlushSendBuffer - Starting.");

  if (fpAddress) {
    nResult = ((INT (*)(void))fpAddress)();
  }

  //LogDebug("XD_InFlushSendBuffer - Return(%d).", nResult);

  return nResult;
}

INT CIV2PATCH_API XD_InitializeModem(INT nArg)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_INIT_MODEM_HOOK);

  LogDebug("XD_InitializeModem(%d) - Starting.", nArg);

  if (fpAddress) {
    nResult = ((INT (*)(INT))fpAddress)(nArg);
  }

  LogDebug("XD_InitializeModem(%d) - Return(%d).", nArg, nResult);

  return nResult;
}

INT CIV2PATCH_API XD_InitializeSerial(INT nArg)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_INIT_SERIAL_HOOK);

  LogDebug("XD_InitializeSerial(%d) - Starting.", nArg);

  if (fpAddress) {
    nResult = ((INT (*)(INT))fpAddress)(nArg);
  }

  LogDebug("XD_InitializeSerial(%d) - Return(%d).", nArg, nResult);

  return nResult;
}

INT CIV2PATCH_API XD_InitializeSocketsIPXSPX(INT nArg1, INT nArg2, INT nArg3, INT nArg4, UINT unArg5, InitializeSocketsCallback callback)
{
  SetInitializeSocketsCallback(callback);
  callback = &OnInitializeSockets;

  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_INIT_SOCKETS_IPX_SPX_HOOK);

  LogDebug("XD_InitializeSocketsIPXSPX(%d, %d, %d, %d, %d) - Starting.", nArg1, nArg2, nArg3, nArg4, unArg5);

  if (fpAddress) {
    nResult = ((INT (*)(INT, INT, INT, INT, UINT, InitializeSocketsCallback))fpAddress)(nArg1, nArg2, nArg3, nArg4, unArg5, callback);
  }

  LogDebug("XD_InitializeSocketsIPXSPX(%d, %d, %d, %d, %d) - Return(%d).", nArg1, nArg2, nArg3, nArg4, unArg5, nResult);

  return nResult;
}

INT CIV2PATCH_API XD_InitializeSocketsTCP(INT nArg1, INT nArg2, INT nArg3, INT nArg4, UINT unArg5, InitializeSocketsCallback callback)
{
  SetInitializeSocketsCallback(callback);
  callback = &OnInitializeSockets;

  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_INIT_SOCKETS_TCP_HOOK);

  LogDebug("XD_InitializeSocketsTCP(%d, %d, %d, %d, %d) - Starting.", nArg1, nArg2, nArg3, nArg4, unArg5);

  if (fpAddress) {
    nResult = ((INT (*)(INT, INT, INT, INT, UINT, InitializeSocketsCallback))fpAddress)(nArg1, nArg2, nArg3, nArg4, unArg5, callback);
  }

  LogDebug("XD_InitializeSocketsTCP(%d, %d, %d, %d, %d) - Return(%d).", nArg1, nArg2, nArg3, nArg4, unArg5, nResult);

  return nResult;
}

/**
 * Create DirectPlay lobby.
 *
 * @param lpResult The output containing the lobby information.
 * @param pLobbyLaunchInfo The lobby configuration.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API XD_LaunchedByLobby(LPVOID lpvResult, struct LobbyLaunchInfo *pLobbyLaunchInfo)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_LAUNCHED_BY_LOBBY_HOOK);

  LogDebug("XD_LaunchedByLobby - Starting.");

  if (fpAddress) {
    nResult = ((INT (*)(LPVOID, struct LobbyLaunchInfo *))fpAddress)(lpvResult, pLobbyLaunchInfo);
  }

  LogDebug("XD_LaunchedByLobby - Return(%d).", nResult);

  return nResult;
}

/**
 * Send a status message to the DirectPlay lobby.
 *
 * @param dwMessageType The DirectPlay lobby message type.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API XD_LobbySendMessage(DWORD dwMessageType)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_LOBBY_SEND_MESSAGE_HOOK);

  LogDebug("XD_LobbySendMessage(%d) - Starting.", dwMessageType);

  if (fpAddress) {
    nResult = ((INT (*)(DWORD))fpAddress)(dwMessageType);
  }

  LogDebug("XD_LobbySendMessage(%d) - Return(%d).", dwMessageType, nResult);

  return nResult;
}

INT CIV2PATCH_API XD_OpenConnection(LPVOID lpvArg, DWORD dwArg)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_OPEN_CONN_HOOK);

  LogDebug("XD_OpenConnection(%d) - Starting.", dwArg);

  if (fpAddress) {
    nResult = ((INT (*)(LPVOID, DWORD))fpAddress)(lpvArg, dwArg);
  }

  LogDebug("XD_OpenConnection(%d) - Return(%d).", dwArg, nResult);

  return nResult;
}

/**
 * Shutdown all allocated multiplayer resources.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API XD_ResetLibrary(void)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_RESET_LIBRARY_HOOK);

  LogDebug("XD_ResetLibrary - Starting.");

  if (fpAddress) {
    nResult = ((INT (*)(void))fpAddress)();
  }

  LogDebug("XD_ResetLibrary - Return(%d).", nResult);

  return nResult;
}

/**
 * @return The number of bytes sent.
 */
INT CIV2PATCH_API XD_SendBroadcastData(LPVOID lpvMessage, DWORD dwSize, LONG lArg)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SEND_BROADCAST_DATA_HOOK);

  LogDebug("XD_SendBroadcastData(%d, %d) - Starting.", dwSize, lArg);

  if (fpAddress) {
    nResult = ((INT (*)(LPVOID, DWORD, LONG))fpAddress)(lpvMessage, dwSize, lArg);
  }

  LogDebug("XD_SendBroadcastData(%d, %d) - Return(%d).", dwSize, lArg, nResult);

  return nResult;
}

INT CIV2PATCH_API XD_SendSecureData(WORD wArg, LPVOID lpvArg, DWORD dwArg, SHORT sArg)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SEND_SECURE_DATA_HOOK);

  LogDebug("XD_SendSecureData(%d, %d, %d) - Starting.", wArg, dwArg, sArg);

  if (fpAddress) {
    nResult = ((INT (*)(WORD, LPVOID, DWORD, SHORT))fpAddress)(wArg, lpvArg, dwArg, sArg);
  }

  LogDebug("XD_SendSecureData(%d, %d, %d) - Return(%d).", wArg, dwArg, sArg, nResult);

  return nResult;
}

INT CIV2PATCH_API XD_ServerCloseConnection(WORD wArg)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SERVER_CLOSE_CONN_HOOK);

  LogDebug("XD_ServerCloseConnection(%d) - Starting.", wArg);

  if (fpAddress) {
    nResult = ((INT (*)(WORD))fpAddress)(wArg);
  }

  LogDebug("XD_ServerCloseConnection(%d) - Return(%d).", wArg, nResult);

  return nResult;
}

/**
 * Set the callback for receiving a broadcast message.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API XD_SetBroadcastReceive(BroadcastReceiveCallback callback)
{
  SetBroadcastReceiveCallback(callback);
  callback = &OnBroadcastReceive;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SET_BROADCAST_RECEIVE_HOOK);

  LogDebug("XD_SetBroadcastReceive.");

  if (fpAddress) {
    ((void (*)(BroadcastReceiveCallback))fpAddress)(callback);
  }
}

/**
 * Set the server callback for accepting a client connection.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API XD_SetNewClientConnection(NewClientConnectionCallback callback)
{
  SetNewClientConnectionCallback(callback);
  callback = &OnNewClientConnection;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SET_NEW_CLIENT_CONN_HOOK);

  LogDebug("XD_SetNewClientConnection.");

  if (fpAddress) {
    ((void (*)(NewClientConnectionCallback))fpAddress)(callback);
  }
}

/**
 * Set the client callback for being connected to the server.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API XD_SetOnClientConnectionToServer(ClientConnectionToServerCallback callback)
{
  SetClientConnectionToServerCallback(callback);
  callback = &OnClientConnectionToServer;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SET_ON_CLIENT_CONN_SERVER_HOOK);

  LogDebug("XD_SetOnClientConnectionToServer.");

  if (fpAddress) {
    ((void (*)(ClientConnectionToServerCallback))fpAddress)(callback);
  }
}

/**
 * Set the client callback for losing connection to the server.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API XD_SetOnConnectionLost(ConnectionLostCallback callback)
{
  SetConnectionLostCallback(callback);
  callback = &OnConnectionLost;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SET_ON_CONN_LOST_HOOK);

  LogDebug("XD_SetOnConnectionLost.");

  if (fpAddress) {
    ((void (*)(ConnectionLostCallback))fpAddress)(callback);
  }
}

/**
 * Set the callback for sending a message that exceeds the maximum size.
 * The callback can be triggered during  XD_SendSecureData.
 *
 * @param dwMaxSize The maximum size in bytes. The callback will be called if
 *   the message is equal or larger than this size.
 * @param callback The callback function.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API XD_SetOversizedMessageCB(DWORD dwMaxSize, OversizedMessageCallback callback)
{
  SetOversizedMessageCallback(callback);
  callback = &OnOversizedMessage;

  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SET_OVERSIZED_MESSAGE_CB_HOOK);

  LogDebug("XD_SetOversizedMessageCB(%d) - Starting.", dwMaxSize);

  if (fpAddress) {
    nResult = ((INT (*)(DWORD, OversizedMessageCallback))fpAddress)(dwMaxSize, callback);
  }

  LogDebug("XD_SetOversizedMessageCB(%d) - Return(%d).", dwMaxSize, nResult);

  return nResult;
}

/**
 * Set the callback for receiving a message.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API XD_SetSecureReceive(SecureReceiveCallback callback)
{
  SetSecureReceiveCallback(callback);
  callback = &OnSecureReceive;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SET_SECURE_RECEIVE_HOOK);

  LogDebug("XD_SetSecureReceive.");

  if (fpAddress) {
    ((void (*)(SecureReceiveCallback))fpAddress)(callback);
  }
}

void CIV2PATCH_API XD_ShutdownModem(void)
{
  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SHUTDOWN_MODEM_HOOK);

  LogDebug("XD_ShutdownModem.");

  if (fpAddress) {
    ((void (*)(void))fpAddress)();
  }
}

void CIV2PATCH_API XD_ShutdownSockets(void)
{
  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SHUTDOWN_SOCKETS_HOOK);

  LogDebug("XD_ShutdownSockets.");

  if (fpAddress) {
    ((void (*)(void))fpAddress)();
  }
}

void CIV2PATCH_API XD_ShutdownTEN(void)
{
  FARPROC fpAddress = GetOriginalFunctionAddress(XD_SHUTDOWN_TEN_HOOK);

  LogDebug("XD_ShutdownTEN.");

  if (fpAddress) {
    ((void (*)(void))fpAddress)();
  }
}

INT CIV2PATCH_API XD_StopConnections(void)
{
  INT nResult = -1;

  FARPROC fpAddress = GetOriginalFunctionAddress(XD_STOP_CONN_HOOK);

  LogDebug("XD_StopConnections - Starting.");

  if (fpAddress) {
    nResult = ((INT (*)(void))fpAddress)();
  }

  LogDebug("XD_StopConnections - Return(%d).", nResult);

  return nResult;
}
