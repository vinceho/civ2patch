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
#include "net.h"
#include "log.h"

InitializeSocketsCallback g_orgInitializeSocketsCallback = NULL;
BroadcastReceiveCallback g_orgBroadcastReceiveCallback = NULL;
NewClientConnectionCallback g_orgNewClientConnectionCallback = NULL;
ClientConnectionToServerCallback g_orgClientConnectionToServerCallback = NULL;
ConnectionLostCallback g_orgConnectionLostCallback = NULL;
OversizedMessageCallback g_orgOversizedMessageCallback = NULL;
SecureReceiveCallback g_orgSecureReceiveCallback = NULL;

void SetInitializeSocketsCallback(InitializeSocketsCallback callback)
{
  g_orgInitializeSocketsCallback = callback;
}

void SetBroadcastReceiveCallback(BroadcastReceiveCallback callback)
{
  g_orgBroadcastReceiveCallback = callback;
}

void SetNewClientConnectionCallback(NewClientConnectionCallback callback)
{
  g_orgNewClientConnectionCallback = callback;
}

void SetClientConnectionToServerCallback(ClientConnectionToServerCallback callback)
{
  g_orgClientConnectionToServerCallback = callback;
}

void SetConnectionLostCallback(ConnectionLostCallback callback)
{
  g_orgConnectionLostCallback = callback;
}

void SetOversizedMessageCallback(OversizedMessageCallback callback)
{
  g_orgOversizedMessageCallback = callback;
}

void SetSecureReceiveCallback(SecureReceiveCallback callback)
{
  g_orgSecureReceiveCallback = callback;
}

void OnInitializeSockets(LPVOID lpvArg1, WORD wArg2, LONG lArg3)
{
  LogDebug("OnInitializeSockets(%d, %d).", wArg2, lArg3);

  if (g_orgInitializeSocketsCallback) {
    g_orgInitializeSocketsCallback(lpvArg1, wArg2, lArg3);
  }
}

/**
 * @param lpvMessage The received message.
 * @param wSize The size of the message.
 * @param lArg [Unknown]
 */
void OnBroadcastReceive(LPVOID lpvMessage, WORD wSize, LONG lArg)
{
  LogDebug("OnBroadcastReceive(%d, %d).", wSize, lArg);

  if (g_orgBroadcastReceiveCallback) {
    g_orgBroadcastReceiveCallback(lpvMessage, wSize, lArg);
  }
}

/**
 * @param wClientId The client Id.
 * @param wServerId The server Id.
 */
void OnNewClientConnection(WORD wClientId, WORD wServerId)
{
  LogDebug("OnNewClientConnection(%d, %d).", wClientId, wServerId);

  if (g_orgNewClientConnectionCallback) {
    g_orgNewClientConnectionCallback(wClientId, wServerId);
  }
}

/*
 * @param sServerId The server Id.
 */
void OnClientConnectionToServer(SHORT sServerId)
{
  LogDebug("OnClientConnectionToServer(%d).", sServerId);

  if (g_orgClientConnectionToServerCallback) {
    g_orgClientConnectionToServerCallback(sServerId);
  }
}

/**
 * @param wId The server/client Id.
 */
void OnConnectionLost(WORD wId)
{
  LogDebug("OnConnectionLost(%d).", wId);

  if (g_orgConnectionLostCallback) {
    g_orgConnectionLostCallback(wId);
  }
}

/**
 * @param dwSize The size of the message.
 */
void OnOversizedMessage(DWORD dwSize)
{
  LogDebug("OnOversizedMessage(%d).", dwSize);

  if (g_orgOversizedMessageCallback) {
    g_orgOversizedMessageCallback(dwSize);
  }
}

/**
 * @param wDestId The Id of the destination instance.
 * @param lpvMessage The received message.
 * @param dwSize The size of the message.
 * @param sArg [Unknown]
 */
void OnSecureReceive(WORD wDestId, LPVOID lpvMessage, DWORD dwSize, SHORT sArg)
{
  LogDebug("OnSecureReceive(%d).", dwSize);

  if (g_orgSecureReceiveCallback) {
    g_orgSecureReceiveCallback(wDestId, lpvMessage, dwSize, sArg);
  }
}
