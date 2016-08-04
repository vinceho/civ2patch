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
#ifndef SDL_RUNTIME_H
#define SDL_RUNTIME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include <windows.h>

#define SDL_MODULE_NAME       "SDL2.dll"
#define SDL_MIXER_MODULE_NAME "SDL2_mixer.dll"
#define SDL_NET_MODULE_NAME   "SDL2_net.dll"

// SDL
typedef int (SDLCALL *__SDL_Init__)(Uint32);
typedef void (SDLCALL *__SDL_Quit__)(void);
typedef const char *(SDLCALL *__SDL_GetError__)(void);

// SDL Mixer
typedef int (SDLCALL *__Mix_Init__)(int);
typedef int (SDLCALL *__Mix_OpenAudio__)(int, Uint16, int, int);
typedef Mix_Music *(SDLCALL *__Mix_LoadMUS__)(const char *);
typedef int (SDLCALL *__Mix_PlayMusic__)(Mix_Music *, int);
typedef void (SDLCALL *__Mix_HookMusicFinished__)(void (*)(void));
typedef int (SDLCALL *__Mix_VolumeMusic__)(int);
typedef void (SDLCALL *__Mix_FreeMusic__)(Mix_Music *);
typedef void (SDLCALL *__Mix_CloseAudio__)(void);
typedef void (SDLCALL *__Mix_Quit__)(void);

// SDL Net
typedef int (SDLCALL *__SDLNet_Init__)(void);
typedef int (SDLCALL *__SDLNet_ResolveHost__)(IPaddress *, const char *, Uint16);
typedef const char *(SDLCALL *__SDLNet_ResolveIP__)(const IPaddress *);
typedef SDLNet_SocketSet (SDLCALL *__SDLNet_AllocSocketSet__)(int);
typedef int (SDLCALL *__SDLNet_AddSocket__)(SDLNet_SocketSet, SDLNet_GenericSocket);
typedef int (SDLCALL *__SDLNet_DelSocket__)(SDLNet_SocketSet, SDLNet_GenericSocket);
typedef int (SDLCALL *__SDLNet_CheckSockets__)(SDLNet_SocketSet, Uint32);
typedef void (SDLCALL *__SDLNet_FreeSocketSet__)(SDLNet_SocketSet);
typedef UDPpacket *(SDLCALL *__SDLNet_AllocPacket__)(int);
typedef void (SDLCALL *__SDLNet_FreePacket__)(UDPpacket *);
typedef TCPsocket (SDLCALL *__SDLNet_TCP_Open__)(IPaddress *);
typedef TCPsocket (SDLCALL *__SDLNet_TCP_Accept__)(TCPsocket);
typedef int (SDLCALL *__SDLNet_TCP_Send__)(TCPsocket, const void *, int);
typedef int (SDLCALL *__SDLNet_TCP_Recv__)(TCPsocket, void *, int);
typedef void (SDLCALL *__SDLNet_TCP_Close__)(TCPsocket);
typedef UDPsocket (SDLCALL *__SDLNet_UDP_Open__)(Uint16);
typedef int (SDLCALL *__SDLNet_UDP_Bind__)(UDPsocket, int channel, const IPaddress *);
typedef int (SDLCALL *__SDLNet_UDP_Send__)(UDPsocket, int channel, UDPpacket *);
typedef int (SDLCALL *__SDLNet_UDP_Recv__)(UDPsocket, UDPpacket *);
typedef void (SDLCALL *__SDLNet_UDP_Unbind__)(UDPsocket, int);
typedef void (SDLCALL *__SDLNet_UDP_Close__)(UDPsocket);
typedef void (SDLCALL *__SDLNet_Quit__)(void);
typedef const char *(SDLCALL *__SDLNet_GetError__)(void);

// Functions
extern __SDL_Init__ _SDL_Init;
extern __SDL_Quit__ _SDL_Quit;
extern __SDL_GetError__ _SDL_GetError;
extern __Mix_Init__ _Mix_Init;
extern __Mix_OpenAudio__ _Mix_OpenAudio;
extern __Mix_LoadMUS__ _Mix_LoadMUS;
extern __Mix_PlayMusic__ _Mix_PlayMusic;
extern __Mix_HookMusicFinished__ _Mix_HookMusicFinished;
extern __Mix_VolumeMusic__ _Mix_VolumeMusic;
extern __Mix_FreeMusic__ _Mix_FreeMusic;
extern __Mix_CloseAudio__ _Mix_CloseAudio;
extern __Mix_Quit__ _Mix_Quit;
extern __SDLNet_Init__ _SDLNet_Init;
extern __SDLNet_ResolveHost__ _SDLNet_ResolveHost;
extern __SDLNet_ResolveIP__ _SDLNet_ResolveIP;
extern __SDLNet_AllocSocketSet__ _SDLNet_AllocSocketSet;
extern __SDLNet_AddSocket__ _SDLNet_AddSocket;
extern __SDLNet_DelSocket__ _SDLNet_DelSocket;
extern __SDLNet_CheckSockets__ _SDLNet_CheckSockets;
extern __SDLNet_FreeSocketSet__ _SDLNet_FreeSocketSet;
extern __SDLNet_AllocPacket__ _SDLNet_AllocPacket;
extern __SDLNet_FreePacket__ _SDLNet_FreePacket;
extern __SDLNet_TCP_Open__ _SDLNet_TCP_Open;
extern __SDLNet_TCP_Accept__ _SDLNet_TCP_Accept;
extern __SDLNet_TCP_Send__ _SDLNet_TCP_Send;
extern __SDLNet_TCP_Recv__ _SDLNet_TCP_Recv;
extern __SDLNet_TCP_Close__ _SDLNet_TCP_Close;
extern __SDLNet_UDP_Open__ _SDLNet_UDP_Open;
extern __SDLNet_UDP_Bind__ _SDLNet_UDP_Bind;
extern __SDLNet_UDP_Send__ _SDLNet_UDP_Send;
extern __SDLNet_UDP_Recv__ _SDLNet_UDP_Recv;
extern __SDLNet_UDP_Unbind__ _SDLNet_UDP_Unbind;
extern __SDLNet_UDP_Close__ _SDLNet_UDP_Close;
extern __SDLNet_Quit__ _SDLNet_Quit;
extern __SDLNet_GetError__ _SDLNet_GetError;

BOOL InitializeSdlLibrary();
BOOL InitializeSdlMixerLibrary();
BOOL InitializeSdlNetLibrary();
BOOL ShutdownSdlLibrary();
BOOL ShutdownSdlMixerLibrary();
BOOL ShutdownSdlNetLibrary();
BOOL IsSdlLibraryInitialized();
BOOL IsSdlMixerLibraryInitialized();
BOOL IsSdlNetLibraryInitialized();

#endif // SDL_RUNTIME_H
