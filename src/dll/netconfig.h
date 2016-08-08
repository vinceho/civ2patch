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
#ifndef NET_CONFIG_H
#define NET_CONFIG_H

#include <windows.h>
#include "civ2patch.h"

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

InitializeSocketsCallback GetNetInitializeSocketsCallback();
void SetNetInitializeSocketsCallback(InitializeSocketsCallback callback);
BroadcastReceiveCallback GetNetBroadcastReceiveCallback();
void SetNetBroadcastReceiveCallback(BroadcastReceiveCallback callback);
NewClientConnectionCallback GetNetNewClientConnectionCallback();
void SetNetNewClientConnectionCallback(NewClientConnectionCallback callback);
ClientConnectionToServerCallback GetNetClientConnectionToServerCallback();
void SetNetClientConnectionToServerCallback(ClientConnectionToServerCallback callback);
ConnectionLostCallback GetNetConnectionLostCallback();
void SetNetConnectionLostCallback(ConnectionLostCallback callback);
OversizedMessageCallback GetNetOversizedMessageCallback();
void SetNetOversizedMessageCallback(OversizedMessageCallback callback);
DWORD GetNetMessageMaxSize();
void SetNetMessageMaxSize(DWORD dwMessageMaxSize);
SecureReceiveCallback GetNetSecureReceiveCallback();
void SetNetSecureReceiveCallback(SecureReceiveCallback callback);

#endif // NET_CONFIG_H
