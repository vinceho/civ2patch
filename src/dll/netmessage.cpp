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
#include "netmessage.h"
#include "log.h"

NetMessage *CreateNetMessage(DWORD dwMaxSize, DWORD dwConnectionTimeout)
{
  NetMessage *message = (NetMessage *)malloc(sizeof(NetMessage));

  memset(message, 0, sizeof(NetMessage));

  message->dwConnectionTimeout = dwConnectionTimeout;
  message->messageBuffer = CreateNetMessageBuffer(dwMaxSize);

  return message;
}

NetMessage *CreateNetMessage(NetMessageBuffer *buffer, DWORD dwConnectionTimeout)
{
  NetMessage *message = (NetMessage *)malloc(sizeof(NetMessage));

  memset(message, 0, sizeof(NetMessage));

  message->dwConnectionTimeout = dwConnectionTimeout;
  message->messageBuffer = buffer;

  return message;
}

NetMessageBuffer *PeekNetMessageMessageBuffer(NetMessage *message)
{
  if(!message) {
    return NULL;
  }

  return message->messageBuffer;
}

BOOL AddNetMessage(NetMessage **messageList, NetMessageBuffer *buffer, DWORD dwConnectionTimeout)
{
  if(!messageList) {
    return FALSE;
  }

  NetMessage *first = *messageList;

  if (!first) {
    first = CreateNetMessage(buffer, dwConnectionTimeout);
    *messageList = first;

    return TRUE;
  }

  for (NetMessage *it = first; it != NULL; it = it->next) {
    if (it->next == NULL) {
      it->next = CreateNetMessage(buffer, dwConnectionTimeout);

      return TRUE;
    }
  }

  return FALSE;
}

/**
 * @return -1 on failure. 0 on success. 1 on waiting for connection.
 */
INT ReceiveNetMessage(NetMessage *message, TCPsocket socket)
{
  if(!message || !socket) {
    return -2;
  }

  if (!message->messageBuffer) {
    return -2;
  }

  NetMessageBuffer *buffer = message->messageBuffer;

  if (!LoadNetMessageBuffer(buffer, socket)) {
    DWORD dwNow = GetTickCount();

    if (message->dwExpirationTime == 0) {
      message->dwExpirationTime = dwNow + message->dwConnectionTimeout;

      // Overflow handling.
      if (message->dwExpirationTime < dwNow) {
        message->dwExpirationTime = 0;
      }
    } else if (dwNow >= message->dwExpirationTime) {
      LogError("Failed to receive message due to connection lost.");
      message->dwExpirationTime = 0;

      return -1;
    }

    return 1;
  } else {
    message->dwExpirationTime = 0;
  }

  return 0;
}

/**
 * @return -1 on failure. 0 on success. 1 on waiting for connection.
 */
INT SendNetMessage(NetMessage *message, TCPsocket socket)
{
  if(!message || !socket) {
    return -2;
  }

  if (!message->messageBuffer) {
    return -2;
  }

  NetMessageBuffer *buffer = message->messageBuffer;

  if (!SendNetMessageBuffer(buffer, socket)) {
    DWORD dwNow = GetTickCount();

    if (message->dwExpirationTime == 0) {
      message->dwExpirationTime = dwNow + message->dwConnectionTimeout;

      // Overflow handling.
      if (message->dwExpirationTime < dwNow) {
        message->dwExpirationTime = 0;
      }
    } else if (dwNow >= message->dwExpirationTime) {
      LogError("Failed to send message due to connection lost.");
      message->dwExpirationTime = 0;

      return -1;
    }

    return 1;
  } else {
    message->dwExpirationTime = 0;
  }

  return 0;
}

NetMessage *PopNetMessage(NetMessage **messageList)
{
  if (!messageList) {
    return NULL;
  }

  NetMessage *message = *messageList;

  if (!message) {
    return NULL;
  }

  *messageList = message->next;
  message->next = NULL; // Unlink from the list.

  return message;
}

void FreeNetMessage(NetMessage *message)
{
  for (NetMessage *it = message; it != NULL;) {
    NetMessage *next = it->next;

    if (it->messageBuffer) {
      FreeNetMessageBuffer(it->messageBuffer);
    }

    free(it);

    it = next;
  }
}
