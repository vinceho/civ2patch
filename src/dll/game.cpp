#include <SDL2/SDL.h>
#include <map>
#include "civ2patch.h"
#include "constants.h"
#include "game.h"
#include "inject.h"
#include "log.h"

#define PEEK_MESSAGE_HOOK "PeekMessageEx"
#define MCI_SEND_CMD_HOOK "mciSendCommandEx"

DWORD g_dwLastMsgStatusTime = 0;

BOOL PatchIdleCpu(HANDLE hProcess)
{
  DWORD dwAddressList[] = {0x5BBB91, 0x5BD2F9, 0x5BD31D};
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

BOOL PatchMapTilesLimit(HANDLE hProcess)
{
  // Change max number of map tiles from 10,000 to 32,767.
  BYTE buffer[] = {0xFF, 0x7F};
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

BOOL PatchTimeLimit(HANDLE hProcess)
{
  {
    // Change retirement warning year from 2000 to 9979.
    BYTE buffer[] = {0xFB, 0x26};

    if(!WriteMemory(hProcess, buffer, 2, 0x48B069)) {
      goto PATCH_TIME_LIMIT_FAILED;
    }
  }

  {
    // Change force retirement year from 2020 to 9999.
    BYTE buffer[] = {0x0F, 0x27};

    if(!WriteMemory(hProcess, buffer, 2, 0x48B2AD) || !WriteMemory(hProcess, buffer, 2, 0x48B0BB)) {
      goto PATCH_TIME_LIMIT_FAILED;
    }
  }

  return TRUE;

PATCH_TIME_LIMIT_FAILED:

  Log("ERROR: Failed to patch time limit.");

  return FALSE;
}

BOOL PatchPopulationLimit(HANDLE hProcess)
{
  // Change max population from 320,000 to 2,000,000,000.
  BYTE buffer[] = {0x40, 0x0D, 0x03, 0x00};
  BOOL bSuccess = (WriteMemory(hProcess, buffer, 4, 0x43CD74) && WriteMemory(hProcess, buffer, 4, 0x43CD81));

  if (!bSuccess) {
    Log("ERROR: Failed to patch population limit.");
  }

  return bSuccess;
}

BOOL PatchGoldLimit(HANDLE hProcess)
{
  // Change max gold from 30,000 to 2,000,000,000.
  BYTE buffer[] = {0x00, 0x94, 0x35, 0x77};
  BOOL bSuccess = (WriteMemory(hProcess, buffer, 4, 0x489608) && WriteMemory(hProcess, buffer, 4, 0x48962A));

  if (!bSuccess) {
    Log("ERROR: Failed to patch gold limit.");
  }

  return bSuccess;
}

BOOL PatchFastCombat(HANDLE hProcess)
{
  // Each combat frame duration reduced from 64ms to 1ms.
  BYTE buffer[] = {0x01};
  BOOL bSuccess = WriteMemory(hProcess, buffer, 1, 0x57F4F6);

  if (!bSuccess) {
    Log("ERROR: Failed to patch fast combat.");
  }

  return bSuccess;
}

BOOL CIV2PATCH_API PeekMessageEx(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
  DWORD dwNow = timeGetTime();

  // Civilization 2 uses filter value 957 as a spinning wait.
  if (wMsgFilterMin == 957) {
    if (g_dwLastMsgStatusTime == 0) {
      g_dwLastMsgStatusTime = dwNow;
    }

    // Purge message queue to fix "Not Responding" problem during long AI turns.
    if ((dwNow - g_dwLastMsgStatusTime) >= MSG_STATUS_INTERVAL) {
      GetQueueStatus(QS_ALLINPUT);
      g_dwLastMsgStatusTime = dwNow;
    } else {
      MsgWaitForMultipleObjectsEx(0, 0, MSG_WAIT_TIMEOUT, QS_ALLINPUT, 0);
    }
  } else {
    MsgWaitForMultipleObjectsEx(0, 0, MSG_WAIT_TIMEOUT, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
    g_dwLastMsgStatusTime = dwNow;
  }

  return PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

// Maps device proxy Id to real Id provided by Windows.
std::map<MCIDEVICEID, MCIDEVICEID> mciDeviceIdMap;

MCIDEVICEID GetNextProxyId()
{
  MCIDEVICEID wNewProxyId = 1;

  for (std::map<MCIDEVICEID, MCIDEVICEID>::iterator it = mciDeviceIdMap.begin(); it != mciDeviceIdMap.end(); ++it) {
    if (it->first > wNewProxyId) {
      wNewProxyId = it->first + 1;
    }
  }

  return wNewProxyId;
}

MCIDEVICEID AddMciDevice(MCIDEVICEID wId)
{
  MCIDEVICEID wNewProxyId = GetNextProxyId();

  mciDeviceIdMap[wNewProxyId] = wId;

  return wNewProxyId;
}

BOOL hasMciDevice(MCIDEVICEID wProxyId)
{
  return (mciDeviceIdMap.find(wProxyId) != mciDeviceIdMap.end());
}

void RemoveMciDevice(MCIDEVICEID wProxyId)
{
  mciDeviceIdMap.erase(wProxyId);
}

#include <SDL2/SDL_mixer.h>

Mix_Music *music = NULL;
HWND hMusicCallback = 0;

void NotifyMusicFinished()
{
  Log("INFO: Music ended.\n");

  if (hMusicCallback) {
    Log("INFO: Calling back.\n");
    PostMessage(hMusicCallback, MM_MCINOTIFY, MCI_NOTIFY_SUCCESSFUL, MAKELONG(1, 0));
  }
}

typedef int (* FP_SDL_INIT)(Uint32);
typedef const char* (* FP_SDL_GET_ERROR)();

MCIERROR CIV2PATCH_API mciSendCommandEx(MCIDEVICEID wProxyId, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam)
{
  Log("INFO: mciSendCommandEx ID: '%d' MSG: '0x%x' FLAGS: '0x%x'.\n", wProxyId, uMsg, fdwCommand);

  if (uMsg == MCI_OPEN && (fdwCommand & MCI_OPEN_TYPE)) {
    MCI_OPEN_PARMS *params = (MCI_OPEN_PARMS *)dwParam;

    if (!strcmp(params->lpstrDeviceType, "cdaudio")) {
      int nMixerFlags = (MIX_INIT_MP3 | MIX_INIT_OGG);

      if (!SDL_Init(SDL_INIT_AUDIO)
          && Mix_Init(nMixerFlags) == nMixerFlags
          && !Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 8194)) {
        Log("INFO: Initialized SDL.\n");

        // Use device Id 0 to denote the SDL audio device.
        if (params) {
          params->wDeviceID = AddMciDevice(0);
        }
      } else {
        Log("ERROR: Failed to initialize SDL '%s'.\n", SDL_GetError());

        return MCIERR_INTERNAL;
      }

      Mix_HookMusicFinished(NotifyMusicFinished);

      return 0;
    }
  } else if (uMsg == MCI_CLOSE) {
    if (hasMciDevice(wProxyId) && mciDeviceIdMap[wProxyId] == 0) {

      if (music) {
        Mix_FreeMusic(music);
        music = NULL;
      }

      Mix_CloseAudio();

      // force a quit
      while(Mix_Init(0)) {
        Mix_Quit();
      }

      SDL_Quit();
      Log("INFO: Uninitialized SDL.\n");

      return 0;
    }
  } else if (hasMciDevice(wProxyId) && mciDeviceIdMap[wProxyId] == 0) {
    if (uMsg == MCI_STATUS) {
       MCI_STATUS_PARMS *params = (MCI_STATUS_PARMS *)dwParam;

       if (params) {
         if (fdwCommand & MCI_STATUS_ITEM) {
           if (params->dwItem & MCI_STATUS_NUMBER_OF_TRACKS) {
             Log("INFO: Setting number of audio tracks.\n");
             // Valid number of tracks: 10, 12, 18, 24
             params->dwReturn = 24;
           } else if (params->dwItem & MCI_STATUS_LENGTH) {
             Log("INFO: Cannot get length of audio tracks.\n");

             return MCIERR_INTERNAL;
           }
         }
       }

       return 0;
    } else if (uMsg == MCI_SEEK) {
      Log("INFO: CD audio seek command is ignored.\n");
    } else if (uMsg == MCI_STOP) {
      if (music) {
        Mix_FreeMusic(music);
        music = NULL;
      }
    } else if (uMsg == MCI_PLAY) {
      MCI_PLAY_PARMS *params = (MCI_PLAY_PARMS *)dwParam;

      if (fdwCommand & MCI_FROM) {
        Log("INFO: Loading music track %d.\n", params->dwFrom);
      }

      if (fdwCommand & MCI_TO) {
        Log("INFO: Play to track %d.\n", params->dwTo);
      }

      if (fdwCommand & MCI_NOTIFY) {
        hMusicCallback = (HWND)LOWORD(params->dwCallback);
      } else {
        Log("INFO: No callback.\n");
        hMusicCallback = 0;
      }

      Mix_VolumeMusic(64);

      if (music) {
        Mix_FreeMusic(music);
        music = NULL;
      }

      music = Mix_LoadMUS("Music\\1.mp3");

      if (!music) {
        Log("ERROR: Failed to load music.\n");
      }

      if(Mix_PlayMusic(music, 1)) {
        Log("ERROR: Failed to play music '%s'.\n", SDL_GetError());
      }
    }

    return 0;
  }

  if (wProxyId == 0 || (hasMciDevice(wProxyId) && mciDeviceIdMap[wProxyId] != 0)) {
    return mciSendCommand(mciDeviceIdMap[wProxyId], uMsg, fdwCommand, dwParam);
  } else {
    return MCIERR_INTERNAL;
  }
}
