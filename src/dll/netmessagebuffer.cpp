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
  buffer->lpData = (BYTE *)malloc(dwMaxSize);

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

INT GetNetMessageBufferDestId(NetMessageBuffer *buffer)
{
  if (buffer && buffer->dwHeaderRead == sizeof(NetMessageBufferHeader)) {
    return buffer->header.unDestId;
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

BOOL IsNetMessageBufferFull(NetMessageBuffer *buffer)
{
  if (!buffer) {
    return FALSE;
  }

  DWORD dwSize = GetNetMessageBufferSize(buffer);

  return ((dwSize > 0) && dwSize == buffer->dwDataRead);
}

BOOL SendNetMessageBuffer(BYTE *buffer, DWORD dwSize, UINT wDestId, TCPsocket socket)
{
  NetMessageBufferHeader header = { dwSize, wDestId };
  DWORD dwHeaderSize = sizeof(NetMessageBufferHeader);
  DWORD dwMessageSize = dwSize + dwHeaderSize;
  BYTE message[dwMessageSize];

  memcpy(message, &header, dwHeaderSize);
  memcpy(&(message[dwHeaderSize]), buffer, dwSize);

  INT nSendResult = _SDLNet_TCP_Send(socket, message, dwMessageSize);

  if (nSendResult != dwMessageSize) {
    LogError("Failed to send message to %u: %s", wDestId, _SDLNet_GetError());

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
    BYTE *headerBuffer = (BYTE *)&buffer->header;
    INT nReadResult = _SDLNet_TCP_Recv(socket, &headerBuffer[buffer->dwHeaderRead], dwRemain);

    if (nReadResult > 0) {
      buffer->dwHeaderRead += nReadResult;

      return TRUE;
    } else if (nReadResult < 0) {
      LogError("Failed to read message header from TCP socket: %s", _SDLNet_GetError());
    }
  }

  if (buffer->dwHeaderRead == dwHeaderSize) {
    DWORD dwSize = GetNetMessageBufferSize(buffer);
    DWORD dwOverflow = 0;

    if (dwSize > 0) {
      if (dwSize > buffer->dwMaxSize) {
        LogError("Expected message size %d is larger than maximum message size %d.", dwSize, buffer->dwMaxSize);

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
      } else if (nReadResult < 0) {
        LogError("Failed to read message data from TCP socket: %s", _SDLNet_GetError());
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
      buffer->lpData = NULL;
    }

    free(buffer);
  }
}
