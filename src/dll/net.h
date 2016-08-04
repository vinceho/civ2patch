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
#include "civ2patch.h"
#include "netmessagebuffer.h"

#define NET_SOCKET_MODE_DIRECT    1
#define NET_SOCKET_MODE_BROADCAST 2
#define NET_BROADCAST_IP          "255.255.255.255"
#define NET_ADDRESS_CHAR_MAX      16

typedef struct {
  // Callbacks.
  InitializeSocketsCallback fpInitializeSocketsCallback;
  BroadcastReceiveCallback fpBroadcastReceiveCallback;
  NewClientConnectionCallback fpNewClientConnectionCallback;
  ClientConnectionToServerCallback fpClientConnectionToServerCallback;
  ConnectionLostCallback fpConnectionLostCallback;
  OversizedMessageCallback fpOversizedMessageCallback;
  SecureReceiveCallback fpSecureReceiveCallback;
  // Configuration.
  DWORD dwMessageMaxSize;
} NetConfig;

typedef struct {
  IPaddress directIP;
  TCPsocket directSocket;
  IPaddress broadcastIP;
  UDPsocket boardcastSocket;
  SDLNet_SocketSet socketSet;
  BOOL bServer;
  INT nDirectPort;
  UINT unMaxConnection;
} NetConnection;

typedef struct {
  TCPsocket *clientSockets;
  NetMessageBuffer **buffers;
  UINT unNumClient;
} NetServer;

typedef struct {
  NetMessageBuffer *buffer;
  UINT unClientId;
} NetClient;

#endif // NET_H
