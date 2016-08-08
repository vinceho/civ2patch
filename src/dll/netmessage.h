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
#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include <windows.h>
#include "netmessagebuffer.h"

typedef struct _NetMessage {
  NetMessageBuffer *messageBuffer;
  DWORD dwExpirationTime; // Current time in milliseconds.
  DWORD dwConnectionTimeout; // In milliseconds.
  struct _NetMessage *next;
} NetMessage;

NetMessage *CreateNetMessage(DWORD dwMaxSize, DWORD dwConnectionTimeout);
NetMessageBuffer *PeekNetMessageMessageBuffer(NetMessage *message);
BOOL AddNetMessage(NetMessage **messageList, NetMessageBuffer *buffer, DWORD dwConnectionTimeout);
INT ReceiveNetMessage(NetMessage *message, TCPsocket socket);
INT SendNetMessage(NetMessage *message, TCPsocket socket);
NetMessage *PopNetMessage(NetMessage **messageList);
void FreeNetMessage(NetMessage *message);

#endif // NET_MESSAGE_H
