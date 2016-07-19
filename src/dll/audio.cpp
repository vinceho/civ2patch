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
#include <map>
#include "audio.h"
#include "constants.h"
#include "log.h"

typedef int (SDLCALL *__SDL_Init__)(Uint32);
typedef void (SDLCALL *__SDL_Quit__)(void);
typedef const char *(SDLCALL *__SDL_GetError__)(void);
typedef int (SDLCALL *__Mix_Init__)(int);
typedef int (SDLCALL *__Mix_OpenAudio__)(int, Uint16, int, int);
typedef Mix_Music *(SDLCALL *__Mix_LoadMUS__)(const char *);
typedef int (SDLCALL *__Mix_PlayMusic__)(Mix_Music *, int);
typedef void (SDLCALL *__Mix_HookMusicFinished__)(void (*)(void));
typedef int (SDLCALL *__Mix_VolumeMusic__)(int);
typedef void (SDLCALL *__Mix_FreeMusic__)(Mix_Music *);
typedef void (SDLCALL *__Mix_CloseAudio__)(void);
typedef void (SDLCALL *__Mix_Quit__)(void);

__SDL_Init__ _SDL_Init;
__SDL_Quit__ _SDL_Quit;
__SDL_GetError__ _SDL_GetError;
__Mix_Init__ _Mix_Init;
__Mix_OpenAudio__ _Mix_OpenAudio;
__Mix_LoadMUS__ _Mix_LoadMUS;
__Mix_PlayMusic__ _Mix_PlayMusic;
__Mix_HookMusicFinished__ _Mix_HookMusicFinished;
__Mix_VolumeMusic__ _Mix_VolumeMusic;
__Mix_FreeMusic__ _Mix_FreeMusic;
__Mix_CloseAudio__ _Mix_CloseAudio;
__Mix_Quit__ _Mix_Quit;

HINSTANCE g_SdlLibraryHandle = NULL;
HINSTANCE g_SdlMixerLibraryHandle = NULL;

// Maps device proxy Id to real Id provided by Windows.
std::map<MCIDEVICEID, MCIDEVICEID> g_mciDeviceIdMap;
DWORD g_dwCdDeviceProxyId = 0;

DWORD g_dwMusicAlbum = 0;
DWORD g_dwMusicVolume = 0;
Mix_Music *g_music = NULL;
HWND g_hNotifyMusicFinishedCallback = NULL;

void StopMusic();
void NotifyMusicFinishedHandler();

BOOL InitializeAudio(DWORD dwFreq, DWORD dwChunkSize, DWORD dwVolume, DWORD dwAlbum)
{
  g_dwMusicVolume = dwVolume;
  g_dwMusicAlbum = dwAlbum;
  g_SdlLibraryHandle = LoadLibrary("SDL2.dll");
  g_SdlMixerLibraryHandle = LoadLibrary("SDL2_mixer.dll");

  if (g_SdlLibraryHandle && g_SdlMixerLibraryHandle) {
    _SDL_Init = (__SDL_Init__)GetProcAddress(g_SdlLibraryHandle, "SDL_Init");
    _SDL_Quit = (__SDL_Quit__)GetProcAddress(g_SdlLibraryHandle, "SDL_Quit");
    _SDL_GetError = (__SDL_GetError__)GetProcAddress(g_SdlLibraryHandle, "SDL_GetError");
    _Mix_Init = (__Mix_Init__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_Init");
    _Mix_OpenAudio = (__Mix_OpenAudio__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_OpenAudio");
    _Mix_LoadMUS = (__Mix_LoadMUS__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_LoadMUS");
    _Mix_PlayMusic = (__Mix_PlayMusic__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_PlayMusic");
    _Mix_HookMusicFinished = (__Mix_HookMusicFinished__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_HookMusicFinished");
    _Mix_VolumeMusic = (__Mix_VolumeMusic__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_VolumeMusic");
    _Mix_FreeMusic = (__Mix_FreeMusic__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_FreeMusic");
    _Mix_CloseAudio = (__Mix_CloseAudio__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_CloseAudio");
    _Mix_Quit = (__Mix_Quit__)GetProcAddress(g_SdlMixerLibraryHandle, "Mix_Quit");

    if (!_SDL_Init || !_SDL_Quit || !_SDL_GetError || !_Mix_Init
        || !_Mix_OpenAudio || !_Mix_LoadMUS || !_Mix_PlayMusic
        || !_Mix_HookMusicFinished || !_Mix_VolumeMusic || !_Mix_FreeMusic
        || !_Mix_CloseAudio || !_Mix_Quit) {
      Log("ERROR: Failed to load audio libraries.\n");
      return FALSE;
    }

    int nMixerFlags = (MIX_INIT_MP3 | MIX_INIT_OGG);

    if (!_SDL_Init(SDL_INIT_AUDIO)
        && _Mix_Init(nMixerFlags) == nMixerFlags
        && !_Mix_OpenAudio(dwFreq, MIX_DEFAULT_FORMAT, 2, dwChunkSize)) {

      _Mix_HookMusicFinished(NotifyMusicFinishedHandler);
    } else {
      Log("ERROR: Failed to initialize audio: %s.\n", _SDL_GetError());
      return FALSE;
    }

    return TRUE;
  } else {
    Log("ERROR: Failed to load audio libraries.\n");
  }

  return FALSE;
}

BOOL CloseAudio()
{
  g_dwMusicAlbum = 0;
  g_hNotifyMusicFinishedCallback = NULL;

  RemoveMciDevice(g_dwCdDeviceProxyId);

  StopMusic();

  _Mix_CloseAudio();

  // Force SDL2 mixer to quit.
  while(_Mix_Init(0)) {
    _Mix_Quit();
  }

  _SDL_Quit();

  if (g_SdlMixerLibraryHandle) {
    FreeLibrary(g_SdlMixerLibraryHandle);
  }

  if (g_SdlLibraryHandle) {
    FreeLibrary(g_SdlLibraryHandle);
  }

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
    Log("ERROR: Failed to load music track %d: %s.\n", dwTrack, _SDL_GetError());

    return FALSE;
  }

  if(_Mix_PlayMusic(g_music, 1)) {
    Log("ERROR: Failed to play music track %d: %s.\n", dwTrack, _SDL_GetError());

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

  for (std::map<MCIDEVICEID, MCIDEVICEID>::iterator it = g_mciDeviceIdMap.begin(); it != g_mciDeviceIdMap.end(); ++it) {
    if (it->first > wNewProxyId) {
      wNewProxyId = it->first + 1;
    }
  }

  return wNewProxyId;
}

MCIDEVICEID AddMciDevice(MCIDEVICEID wId)
{
  MCIDEVICEID wNewProxyId = GetNextMciDeviceProxyId();

  g_mciDeviceIdMap[wNewProxyId] = wId;

  if (wId == MUSIC_DEVICE_ID) {
    g_dwCdDeviceProxyId = wNewProxyId;
  }

  return wNewProxyId;
}

BOOL HasMciDevice(MCIDEVICEID wProxyId)
{
  return (g_mciDeviceIdMap.find(wProxyId) != g_mciDeviceIdMap.end());
}

void RemoveMciDevice(MCIDEVICEID wProxyId)
{
  if (wProxyId == g_dwCdDeviceProxyId) {
    g_dwCdDeviceProxyId = 0;
  }

  if (HasMciDevice(wProxyId)) {
    g_mciDeviceIdMap.erase(wProxyId);
  }
}

MCIDEVICEID GetMciDevice(MCIDEVICEID wProxyId)
{
  if (HasMciDevice(wProxyId)) {
    return g_mciDeviceIdMap[wProxyId];
  }

  return 0;
}
