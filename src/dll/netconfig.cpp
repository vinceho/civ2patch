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
#include "netconfig.h"

// Singleton multiplayer configuration.
NetConfig g_netConfig = {
  // Callbacks.
  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  // Configuration.
  8192
};

InitializeSocketsCallback GetNetInitializeSocketsCallback()
{
  return g_netConfig.fpInitializeSocketsCallback;
}

void SetNetInitializeSocketsCallback(InitializeSocketsCallback callback)
{
  g_netConfig.fpInitializeSocketsCallback = callback;
}

BroadcastReceiveCallback GetNetBroadcastReceiveCallback()
{
  return g_netConfig.fpBroadcastReceiveCallback;
}

void SetNetBroadcastReceiveCallback(BroadcastReceiveCallback callback)
{
  g_netConfig.fpBroadcastReceiveCallback = callback;
}

NewClientConnectionCallback GetNetNewClientConnectionCallback()
{
  return g_netConfig.fpNewClientConnectionCallback;
}

void SetNetNewClientConnectionCallback(NewClientConnectionCallback callback)
{
  g_netConfig.fpNewClientConnectionCallback = callback;
}

ClientConnectionToServerCallback GetNetClientConnectionToServerCallback()
{
  return g_netConfig.fpClientConnectionToServerCallback;
}

void SetNetClientConnectionToServerCallback(ClientConnectionToServerCallback callback)
{
  g_netConfig.fpClientConnectionToServerCallback = callback;
}

ConnectionLostCallback GetNetConnectionLostCallback()
{
  return g_netConfig.fpConnectionLostCallback;
}

void SetNetConnectionLostCallback(ConnectionLostCallback callback)
{
  g_netConfig.fpConnectionLostCallback = callback;
}

OversizedMessageCallback GetNetOversizedMessageCallback()
{
  return g_netConfig.fpOversizedMessageCallback;
}

void SetNetOversizedMessageCallback(OversizedMessageCallback callback)
{
  g_netConfig.fpOversizedMessageCallback = callback;
}

DWORD GetNetMessageMaxSize()
{
  return g_netConfig.dwMessageMaxSize;
}

void SetNetMessageMaxSize(DWORD dwMessageMaxSize)
{
  g_netConfig.dwMessageMaxSize = dwMessageMaxSize;
}

SecureReceiveCallback GetNetSecureReceiveCallback()
{
  return g_netConfig.fpSecureReceiveCallback;
}

void SetNetSecureReceiveCallback(SecureReceiveCallback callback)
{
  g_netConfig.fpSecureReceiveCallback = callback;
}
