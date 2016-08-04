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
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "audio.h"
#include "constants.h"
#include "log.h"
#include "sdllibrary.h"

// Maps device proxy Id to real Id provided by Windows.
typedef struct _AudioProxyId {
  MCIDEVICEID wProxyId;
  MCIDEVICEID wSystemId;
  struct _AudioProxyId *next;
} AudioProxyId;

AudioProxyId *g_mciDeviceIdList = NULL;
DWORD g_dwCdDeviceProxyId = 0;

DWORD g_dwMusicAlbum = 0;
DWORD g_dwMusicVolume = 0;
Mix_Music *g_music = NULL;
HWND g_hNotifyMusicFinishedCallback = NULL;

void StopMusic();
void NotifyMusicFinishedHandler();
void RemoveAllMciDevice();

BOOL InitializeAudio(DWORD dwFreq, DWORD dwChunkSize, DWORD dwVolume, DWORD dwAlbum)
{
  g_dwMusicVolume = dwVolume;
  g_dwMusicAlbum = dwAlbum;

  if (InitializeSdlLibrary() && InitializeSdlMixerLibrary()) {
    int nMixerFlags = (MIX_INIT_MP3 | MIX_INIT_OGG);

    if (!_SDL_Init(SDL_INIT_AUDIO)
        && _Mix_Init(nMixerFlags) == nMixerFlags
        && !_Mix_OpenAudio(dwFreq, MIX_DEFAULT_FORMAT, 2, dwChunkSize)) {
      _Mix_HookMusicFinished(NotifyMusicFinishedHandler);

      return TRUE;
    } else {
      LogError("Failed to initialize audio: %s", _SDL_GetError());
    }
  } else {
    LogError("Failed to load audio libraries.");
  }

  return FALSE;
}

BOOL ShutdownAudio()
{
  g_dwMusicAlbum = 0;
  g_hNotifyMusicFinishedCallback = NULL;

  RemoveAllMciDevice();

  StopMusic();

  _Mix_CloseAudio();

  ShutdownSdlMixerLibrary();

  return TRUE;
}

DWORD GetNumberMusicTracks()
{
  switch (g_dwMusicAlbum) {
    default:
    case 0: // MGE
      return 12;
    case 1: // Classic
      return 10;
    case 2: // CIC
      return 18;
    case 3: // FW
      return 24;
  }
}

Mix_Music *LoadMusic(DWORD dwTrack) {
  CHAR sTrackName[MAX_PATH];
  Mix_Music *music;

  // Try OGG audio first.
  sprintf(sTrackName, OGG_PATH, dwTrack);
  music = _Mix_LoadMUS(sTrackName);

  if (music) {
    return music;
  }

  sprintf(sTrackName, MP3_PATH, dwTrack);
  return _Mix_LoadMUS(sTrackName);
}

BOOL PlayMusic(DWORD dwTrack, HWND hNotifyMusicFinishedCallback) {
  g_hNotifyMusicFinishedCallback = NULL;

  StopMusic();
  _Mix_VolumeMusic(g_dwMusicVolume);

  g_music = LoadMusic(dwTrack);

  if (!g_music) {
    LogError("Failed to load music track %d: %s", dwTrack, _SDL_GetError());

    return FALSE;
  }

  if(_Mix_PlayMusic(g_music, 1)) {
    LogError("Failed to play music track %d: %s", dwTrack, _SDL_GetError());

    return FALSE;
  }

  g_hNotifyMusicFinishedCallback = hNotifyMusicFinishedCallback;

  return TRUE;
}

void StopMusic() {
  if (g_music) {
    _Mix_FreeMusic(g_music);
    g_music = NULL;
  }
}

void NotifyMusicFinishedHandler()
{
  if (g_hNotifyMusicFinishedCallback) {
    PostMessage(g_hNotifyMusicFinishedCallback, MM_MCINOTIFY, MCI_NOTIFY_SUCCESSFUL, MAKELONG(g_dwCdDeviceProxyId, 0));
  }
}

BOOL IsMusicMciDevice(MCIDEVICEID wProxyId)
{
  return (wProxyId && g_dwCdDeviceProxyId == wProxyId);
}

MCIDEVICEID GetNextMciDeviceProxyId()
{
  MCIDEVICEID wNewProxyId = 1;

  for (AudioProxyId *it = g_mciDeviceIdList; it != NULL; it = it->next) {
    if (it->wProxyId >= wNewProxyId) {
      wNewProxyId = it->wProxyId + 1;
    }
  }

  return wNewProxyId;
}

MCIDEVICEID AddMciDevice(MCIDEVICEID wId)
{
  for (AudioProxyId *it = g_mciDeviceIdList; it != NULL; it = it->next) {
    if (it->wSystemId == wId) {
      return it->wProxyId;
    }
  }

  MCIDEVICEID wNewProxyId = GetNextMciDeviceProxyId();
  AudioProxyId *newAudioProxyId = (AudioProxyId *)malloc(sizeof(AudioProxyId));

  memset(newAudioProxyId, 0, sizeof(AudioProxyId));
  newAudioProxyId->wProxyId = wNewProxyId;
  newAudioProxyId->wSystemId = wId;
  newAudioProxyId->next = g_mciDeviceIdList;

  g_mciDeviceIdList = newAudioProxyId;

  if (wId == MUSIC_DEVICE_ID) {
    g_dwCdDeviceProxyId = wNewProxyId;
  }

  return wNewProxyId;
}

void RemoveMciDevice(MCIDEVICEID wProxyId)
{
  if (wProxyId == g_dwCdDeviceProxyId) {
    g_dwCdDeviceProxyId = 0;
  }

  AudioProxyId *prev = NULL;

  for (AudioProxyId *it = g_mciDeviceIdList; it != NULL; it = it->next) {
    if (it->wProxyId == wProxyId) {
      if (prev) {
        prev->next = it->next;
      }

      if (it == g_mciDeviceIdList) {
        g_mciDeviceIdList = it->next;
      }

      free(it);

      break;
    }

    prev = it;
  }
}

void RemoveAllMciDevice()
{
  for (AudioProxyId *it = g_mciDeviceIdList; it != NULL;) {
    AudioProxyId *next = it->next;

    free(it);
    it = next;
  }

  g_dwCdDeviceProxyId = 0;
  g_mciDeviceIdList = NULL;
}

MCIDEVICEID GetMciDevice(MCIDEVICEID wProxyId)
{
  for (AudioProxyId *it = g_mciDeviceIdList; it != NULL; it = it->next) {
    if (it->wProxyId == wProxyId) {
      return it->wSystemId;
    }
  }

  return 0;
}
