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
#include <windows.h>
#include "sdllibrary.h"
#include "log.h"

__SDL_Init__ _SDL_Init = NULL;
__SDL_Quit__ _SDL_Quit = NULL;
__SDL_GetError__ _SDL_GetError = NULL;
__Mix_Init__ _Mix_Init = NULL;
__Mix_OpenAudio__ _Mix_OpenAudio = NULL;
__Mix_LoadMUS__ _Mix_LoadMUS = NULL;
__Mix_PlayMusic__ _Mix_PlayMusic = NULL;
__Mix_HookMusicFinished__ _Mix_HookMusicFinished = NULL;
__Mix_VolumeMusic__ _Mix_VolumeMusic = NULL;
__Mix_FreeMusic__ _Mix_FreeMusic = NULL;
__Mix_CloseAudio__ _Mix_CloseAudio = NULL;
__Mix_Quit__ _Mix_Quit = NULL;
__SDLNet_Init__ _SDLNet_Init = NULL;
__SDLNet_ResolveHost__ _SDLNet_ResolveHost = NULL;
__SDLNet_ResolveIP__ _SDLNet_ResolveIP = NULL;
__SDLNet_AllocSocketSet__ _SDLNet_AllocSocketSet = NULL;
__SDLNet_AddSocket__ _SDLNet_AddSocket = NULL;
__SDLNet_DelSocket__ _SDLNet_DelSocket = NULL;
__SDLNet_CheckSockets__ _SDLNet_CheckSockets = NULL;
__SDLNet_FreeSocketSet__ _SDLNet_FreeSocketSet = NULL;
__SDLNet_AllocPacket__ _SDLNet_AllocPacket = NULL;
__SDLNet_FreePacket__ _SDLNet_FreePacket = NULL;
__SDLNet_TCP_Open__ _SDLNet_TCP_Open = NULL;
__SDLNet_TCP_Accept__ _SDLNet_TCP_Accept = NULL;
__SDLNet_TCP_Send__ _SDLNet_TCP_Send = NULL;
__SDLNet_TCP_Recv__ _SDLNet_TCP_Recv = NULL;
__SDLNet_TCP_Close__ _SDLNet_TCP_Close = NULL;
__SDLNet_UDP_Open__ _SDLNet_UDP_Open = NULL;
__SDLNet_UDP_Bind__ _SDLNet_UDP_Bind = NULL;
__SDLNet_UDP_Send__ _SDLNet_UDP_Send = NULL;
__SDLNet_UDP_Recv__ _SDLNet_UDP_Recv = NULL;
__SDLNet_UDP_Unbind__ _SDLNet_UDP_Unbind = NULL;
__SDLNet_UDP_Close__ _SDLNet_UDP_Close = NULL;
__SDLNet_Quit__ _SDLNet_Quit = NULL;
__SDLNet_GetError__ _SDLNet_GetError = NULL;

HINSTANCE g_hSdlLibraryHandle = NULL;
HINSTANCE g_hSdlMixerLibraryHandle = NULL;
HINSTANCE g_hSdlNetLibraryHandle = NULL;

BOOL g_bSdlLibraryInitialized = FALSE;
BOOL g_bSdlMixerLibraryInitialized = FALSE;
BOOL g_bSdlNetLibraryInitialized = FALSE;

BOOL InitializeSdlLibrary()
{
  g_bSdlLibraryInitialized = FALSE;

  if (!g_hSdlLibraryHandle) {
    g_hSdlLibraryHandle = LoadLibrary(SDL_MODULE_NAME);
  }

  _SDL_Init = (__SDL_Init__)GetProcAddress(g_hSdlLibraryHandle, "SDL_Init");
  _SDL_Quit = (__SDL_Quit__)GetProcAddress(g_hSdlLibraryHandle, "SDL_Quit");
  _SDL_GetError = (__SDL_GetError__)GetProcAddress(g_hSdlLibraryHandle, "SDL_GetError");

  if (!_SDL_Init || !_SDL_Quit || !_SDL_GetError) {
    LogError("Failed to load SDL library.");
  } else {
    g_bSdlLibraryInitialized = TRUE;
  }

  return g_bSdlLibraryInitialized;
}

BOOL InitializeSdlMixerLibrary()
{
  g_bSdlMixerLibraryInitialized = FALSE;

  if (!g_hSdlMixerLibraryHandle) {
    g_hSdlMixerLibraryHandle = LoadLibrary(SDL_MIXER_MODULE_NAME);
  }

  _Mix_Init = (__Mix_Init__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_Init");
  _Mix_OpenAudio = (__Mix_OpenAudio__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_OpenAudio");
  _Mix_LoadMUS = (__Mix_LoadMUS__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_LoadMUS");
  _Mix_PlayMusic = (__Mix_PlayMusic__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_PlayMusic");
  _Mix_HookMusicFinished = (__Mix_HookMusicFinished__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_HookMusicFinished");
  _Mix_VolumeMusic = (__Mix_VolumeMusic__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_VolumeMusic");
  _Mix_FreeMusic = (__Mix_FreeMusic__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_FreeMusic");
  _Mix_CloseAudio = (__Mix_CloseAudio__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_CloseAudio");
  _Mix_Quit = (__Mix_Quit__)GetProcAddress(g_hSdlMixerLibraryHandle, "Mix_Quit");

  if (!_Mix_Init || !_Mix_OpenAudio || !_Mix_LoadMUS || !_Mix_PlayMusic || !_Mix_HookMusicFinished
      || !_Mix_VolumeMusic || !_Mix_FreeMusic || !_Mix_CloseAudio || !_Mix_Quit) {
    LogError("Failed to load SDL Audio library.");
  } else {
    g_bSdlMixerLibraryInitialized = TRUE;
  }

  return g_bSdlMixerLibraryInitialized;
}

BOOL InitializeSdlNetLibrary()
{
  g_bSdlNetLibraryInitialized = FALSE;

  if (!g_hSdlNetLibraryHandle) {
    g_hSdlNetLibraryHandle = LoadLibrary(SDL_NET_MODULE_NAME);
  }

  _SDLNet_Init = (__SDLNet_Init__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_Init");
  _SDLNet_ResolveHost = (__SDLNet_ResolveHost__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_ResolveHost");
  _SDLNet_ResolveIP = (__SDLNet_ResolveIP__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_ResolveIP");
  _SDLNet_AllocSocketSet = (__SDLNet_AllocSocketSet__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_AllocSocketSet");
  _SDLNet_AddSocket = (__SDLNet_AddSocket__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_AddSocket");
  _SDLNet_DelSocket = (__SDLNet_DelSocket__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_DelSocket");
  _SDLNet_CheckSockets = (__SDLNet_CheckSockets__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_CheckSockets");
  _SDLNet_FreeSocketSet = (__SDLNet_FreeSocketSet__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_FreeSocketSet");
  _SDLNet_AllocPacket = (__SDLNet_AllocPacket__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_AllocPacket");
  _SDLNet_FreePacket = (__SDLNet_FreePacket__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_FreePacket");
  _SDLNet_TCP_Open = (__SDLNet_TCP_Open__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_TCP_Open");
  _SDLNet_TCP_Accept = (__SDLNet_TCP_Accept__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_TCP_Accept");
  _SDLNet_TCP_Send = (__SDLNet_TCP_Send__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_TCP_Send");
  _SDLNet_TCP_Recv = (__SDLNet_TCP_Recv__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_TCP_Recv");
  _SDLNet_TCP_Close = (__SDLNet_TCP_Close__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_TCP_Close");
  _SDLNet_UDP_Open = (__SDLNet_UDP_Open__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_UDP_Open");
  _SDLNet_UDP_Bind = (__SDLNet_UDP_Bind__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_UDP_Bind");
  _SDLNet_UDP_Send = (__SDLNet_UDP_Send__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_UDP_Send");
  _SDLNet_UDP_Recv = (__SDLNet_UDP_Recv__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_UDP_Recv");
  _SDLNet_UDP_Unbind = (__SDLNet_UDP_Unbind__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_UDP_Unbind");
  _SDLNet_UDP_Close = (__SDLNet_UDP_Close__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_UDP_Close");
  _SDLNet_Quit = (__SDLNet_Quit__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_Quit");
  _SDLNet_GetError = (__SDLNet_GetError__)GetProcAddress(g_hSdlNetLibraryHandle, "SDLNet_GetError");

  if (!_SDLNet_Init || !_SDLNet_ResolveHost || !_SDLNet_ResolveIP || !_SDLNet_AllocSocketSet || !_SDLNet_AddSocket
      || !_SDLNet_DelSocket || !_SDLNet_CheckSockets || !_SDLNet_FreeSocketSet || !_SDLNet_AllocPacket || !_SDLNet_FreePacket
      || !_SDLNet_TCP_Open || !_SDLNet_TCP_Accept || !_SDLNet_TCP_Send || !_SDLNet_TCP_Recv || !_SDLNet_TCP_Close
      || !_SDLNet_UDP_Open || !_SDLNet_UDP_Bind || !_SDLNet_UDP_Send || !_SDLNet_UDP_Recv || !_SDLNet_UDP_Unbind
      || !_SDLNet_UDP_Close || !_SDLNet_Quit || !_SDLNet_GetError) {
    LogError("Failed to load SDL Net library.");
  } else {
    g_bSdlNetLibraryInitialized = TRUE;
  }

  return g_bSdlNetLibraryInitialized;
}

BOOL ShutdownSdlLibrary()
{
  g_bSdlLibraryInitialized = FALSE;

  if (g_hSdlLibraryHandle) {
    if (_SDL_Quit) {
      _SDL_Quit();
    }

    _SDL_Init = NULL;
    _SDL_Quit = NULL;
    _SDL_GetError = NULL;

    FreeLibrary(g_hSdlLibraryHandle);
    g_hSdlLibraryHandle = NULL;
  }

  return TRUE;
}

BOOL ShutdownSdlMixerLibrary()
{
  g_bSdlMixerLibraryInitialized = FALSE;

  if (g_hSdlMixerLibraryHandle) {
    if (_Mix_Init && _Mix_Quit) {
      // Force SDL mixer to quit.
      while(_Mix_Init(0)) {
        _Mix_Quit();
      }
    }

    _Mix_Init = NULL;
    _Mix_OpenAudio = NULL;
    _Mix_LoadMUS = NULL;
    _Mix_PlayMusic = NULL;
    _Mix_HookMusicFinished = NULL;
    _Mix_VolumeMusic = NULL;
    _Mix_FreeMusic = NULL;
    _Mix_CloseAudio = NULL;
    _Mix_Quit = NULL;

    FreeLibrary(g_hSdlMixerLibraryHandle);
    g_hSdlMixerLibraryHandle = NULL;
  }

  return TRUE;
}

BOOL ShutdownSdlNetLibrary()
{
  g_bSdlNetLibraryInitialized = FALSE;

  if (g_hSdlNetLibraryHandle) {
    if (_SDLNet_Quit) {
      _SDLNet_Quit();
    }

    _SDLNet_Init = NULL;
    _SDLNet_ResolveHost = NULL;
    _SDLNet_ResolveIP = NULL;
    _SDLNet_AllocSocketSet = NULL;
    _SDLNet_AddSocket = NULL;
    _SDLNet_DelSocket = NULL;
    _SDLNet_CheckSockets = NULL;
    _SDLNet_FreeSocketSet = NULL;
    _SDLNet_AllocPacket = NULL;
    _SDLNet_FreePacket = NULL;
    _SDLNet_TCP_Open = NULL;
    _SDLNet_TCP_Accept = NULL;
    _SDLNet_TCP_Send = NULL;
    _SDLNet_TCP_Recv = NULL;
    _SDLNet_TCP_Close = NULL;
    _SDLNet_UDP_Open = NULL;
    _SDLNet_UDP_Bind = NULL;
    _SDLNet_UDP_Send = NULL;
    _SDLNet_UDP_Recv = NULL;
    _SDLNet_UDP_Unbind = NULL;
    _SDLNet_UDP_Close = NULL;
    _SDLNet_Quit = NULL;
    _SDLNet_GetError = NULL;

    FreeLibrary(g_hSdlNetLibraryHandle);
    g_hSdlNetLibraryHandle = NULL;
  }

  return TRUE;
}

BOOL IsSdlLibraryInitialized()
{
  return g_bSdlLibraryInitialized;
}

BOOL IsSdlMixerLibraryInitialized()
{
  return g_bSdlMixerLibraryInitialized;
}

BOOL IsSdlNetLibraryInitialized()
{
  return g_bSdlNetLibraryInitialized;
}
