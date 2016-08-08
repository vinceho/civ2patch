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
#ifndef NET_H
#define NET_H

#include <windows.h>
#include "netmessage.h"

#define NET_BROADCAST_IP      "255.255.255.255"
#define NET_BROADCAST_ID      255
#define NET_ADDRESS_CHAR_MAX  16
#define NET_ADDRESS_CHAR_MIN  7

typedef struct {
  BOOL bToServer;
  UINT unId; // Id 0 is reserved for server.
  TCPsocket socket;
  NetMessage *receiveQueue;
  NetMessage *sendQueue;
} NetConnection;

typedef struct {
  CHAR szIp[NET_ADDRESS_CHAR_MAX];
  DWORD dwConnectionPort;
  DWORD dwBroadcastPort;
  DWORD dwConnectionTimeout; // In milliseconds.
  TCPsocket listenSocket;
  IPaddress broadcastIP;
  UDPsocket broadcastSocket;
  SDLNet_SocketSet socketSet;
  BOOL bServer;
  UINT unMaxConnection;
  NetConnection *connections;
} NetInstance;

NetInstance *CreateNetInstance(BOOL bServer, UINT unMaxConnection);
BOOL InitializeNetInstance(NetInstance *instance);
LPCSTR GetNetInstanceIp(NetInstance *instance);
BOOL ActivateNetInstanceServer(NetInstance *instance);
BOOL CloseNetInstanceConnections(NetInstance *instance);
BOOL OpenNetInstanceConnection(NetInstance *instance, LPCSTR lpcsServer);
BOOL BroadcastNetInstanceData(NetInstance *instance, LPVOID lpvMessage, DWORD dwSize);
BOOL QueueNetInstanceData(NetInstance *instance, UINT unDestId, LPBYTE data, DWORD dwSize);
BOOL HasNetInstanceData(NetInstance *instance);
BOOL SendNetInstanceData(NetInstance *instance);
BOOL ProcessNetInstance(NetInstance *instance);
void FreeNetInstance(NetInstance *instance);

#endif // NET_H
