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
#ifndef NET_MESSAGE_BUFFER_H
#define NET_MESSAGE_BUFFER_H

#include <windows.h>
#include <SDL2/SDL_net.h>

typedef struct {
  DWORD dwSize;
  UINT unDestId;
} NetMessageBufferHeader;

typedef struct {
  NetMessageBufferHeader header;
  LPBYTE lpData;
  DWORD dwHeaderRead;
  DWORD dwDataRead;
  DWORD dwMaxSize;
} NetMessageBuffer;

NetMessageBuffer *CreateNetMessageBuffer(DWORD dwMaxSize);
INT GetNetMessageBufferDestId(NetMessageBuffer *buffer);
DWORD GetNetMessageBufferSize(NetMessageBuffer *buffer);
BOOL IsNetMessageBufferFull(NetMessageBuffer *buffer);
void ResetNetMessageBuffer(NetMessageBuffer *buffer);
BOOL SendNetMessageBuffer(BYTE *buffer, DWORD dwSize, UINT wDestId, TCPsocket socket);
BOOL LoadNetMessageBuffer(NetMessageBuffer *buffer, TCPsocket socket);
void FreeNetMessageBuffer(NetMessageBuffer *buffer);

#endif // NET_MESSAGE_BUFFER_H
