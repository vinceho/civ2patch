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
#include "netmessagebuffer.h"
#include "log.h"
#include "sdllibrary.h"

NetMessageBuffer *CreateNetMessageBuffer(DWORD dwMaxSize)
{
  NetMessageBuffer *buffer = (NetMessageBuffer *)malloc(sizeof(NetMessageBuffer));

  memset(buffer, 0, sizeof(NetMessageBuffer));

  buffer->dwMaxSize = dwMaxSize;
  buffer->lpData = (LPBYTE)malloc(dwMaxSize);

  return buffer;
}

NetMessageBuffer *CreateNetMessageBuffer(LPBYTE data, DWORD dwSize, UINT unDestId, UINT unSrcId)
{
  if (!data) {
    return NULL;
  }

  NetMessageBuffer *buffer = CreateNetMessageBuffer(dwSize);

  buffer->header.dwSize = dwSize;
  buffer->header.unDestId = unDestId;
  buffer->header.unSrcId = unSrcId;
  buffer->dwHeaderRead = sizeof(NetMessageBufferHeader);
  buffer->dwDataRead = dwSize;

  memcpy(buffer->lpData, data, dwSize);

  return buffer;
}

void ResetNetMessageBuffer(NetMessageBuffer *buffer)
{
  if (buffer) {
    memset(&buffer->header, 0, sizeof(NetMessageBufferHeader));
    buffer->dwHeaderRead = 0;
    buffer->dwDataRead = 0;
  }
}

INT GetNetMessageBufferDestinationId(NetMessageBuffer *buffer)
{
  if (buffer && buffer->dwHeaderRead == sizeof(NetMessageBufferHeader)) {
    return buffer->header.unDestId;
  }

  return -1;
}

INT GetNetMessageBufferSourceId(NetMessageBuffer *buffer)
{
  if (buffer && buffer->dwHeaderRead == sizeof(NetMessageBufferHeader)) {
    return buffer->header.unSrcId;
  }

  return -1;
}

DWORD GetNetMessageBufferSize(NetMessageBuffer *buffer)
{
  if (buffer && buffer->dwHeaderRead == sizeof(NetMessageBufferHeader)) {
    return buffer->header.dwSize;
  }

  return 0;
}

LPBYTE GetNetMessageBufferData(NetMessageBuffer *buffer)
{
  if (!buffer) {
    return NULL;
  }

  return buffer->lpData;
}

BOOL IsNetMessageBufferFull(NetMessageBuffer *buffer)
{
  if (!buffer) {
    return FALSE;
  }

  DWORD dwSize = GetNetMessageBufferSize(buffer);

  return ((dwSize > 0) && dwSize == buffer->dwDataRead);
}

BOOL SendNetMessageBuffer(NetMessageBuffer *buffer, TCPsocket socket)
{
  if (!IsNetMessageBufferFull(buffer)) {
    return FALSE;
  }

  DWORD dwHeaderSize = sizeof(NetMessageBufferHeader);
  DWORD dwSize = buffer->header.dwSize;
  DWORD dwMessageSize = dwSize + dwHeaderSize;
  BYTE message[dwMessageSize];

  memcpy(message, &buffer->header, dwHeaderSize);
  memcpy(&(message[dwHeaderSize]), buffer->lpData, dwSize);

  INT nSendResult = _SDLNet_TCP_Send(socket, message, dwMessageSize);

  if (nSendResult != dwMessageSize) {
    return FALSE;
  }

  return TRUE;
}

BOOL LoadNetMessageBuffer(NetMessageBuffer *buffer, TCPsocket socket)
{
  if (!buffer || !socket) {
    return FALSE;
  }

  if (IsNetMessageBufferFull(buffer)) {
    ResetNetMessageBuffer(buffer);
  }

  DWORD dwHeaderSize = sizeof(NetMessageBufferHeader);

  if (buffer->dwHeaderRead < dwHeaderSize) {
    DWORD dwRemain = dwHeaderSize - buffer->dwHeaderRead;
    LPBYTE headerBuffer = (LPBYTE)&buffer->header;
    INT nReadResult = _SDLNet_TCP_Recv(socket, &headerBuffer[buffer->dwHeaderRead], dwRemain);

    if (nReadResult > 0) {
      buffer->dwHeaderRead += nReadResult;

      return TRUE;
    }
  }

  if (buffer->dwHeaderRead == dwHeaderSize) {
    DWORD dwSize = GetNetMessageBufferSize(buffer);
    DWORD dwOverflow = 0;

    if (dwSize > 0) {
      if (dwSize > buffer->dwMaxSize) {
        dwOverflow = dwSize - buffer->dwMaxSize;
      }

      INT nReadResult = 0;

      if (buffer->dwDataRead < buffer->dwMaxSize) {
        DWORD dwRemain = (dwOverflow > 0) ? (buffer->dwMaxSize - buffer->dwDataRead) : (dwSize - buffer->dwDataRead);

        nReadResult = _SDLNet_TCP_Recv(socket, &(buffer->lpData[buffer->dwDataRead]), dwRemain);
      } else if (dwOverflow > 0) {
        // Consume the data stream but ignore the overflow data.
        DWORD dwRemain = dwSize - buffer->dwDataRead;
        BYTE overflowBuffer[dwRemain];

        nReadResult = _SDLNet_TCP_Recv(socket, overflowBuffer, dwRemain);
      }

      if (nReadResult > 0) {
        buffer->dwDataRead += nReadResult;

        return TRUE;
      }
    }
  }

  return FALSE;
}

void FreeNetMessageBuffer(NetMessageBuffer *buffer)
{
  if (buffer) {
    if (buffer->lpData) {
      free(buffer->lpData);
    }

    free(buffer);
  }
}
