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
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <windows.h>
#include "netgame.h"
#include "civ2patch.h"
#include "log.h"
#include "net.h"
#include "hook.h"
#include "netconfig.h"
#include "sdllibrary.h"

NetInstance *g_pNetInstance = NULL;
int (*g_fpInFlushSendBuffer)(void) = NULL;

INT CIV2PATCH_API Net_ActivateServer(void)
{
  LogTrace("Net_ActivateServer");

  if (g_pNetInstance && ActivateNetInstanceServer(g_pNetInstance)) {
    return 0;
  }

  return -1;
}

/**
 * Close connection to the server.
 */
INT CIV2PATCH_API Net_CloseConnection(void)
{
  LogTrace("Net_CloseConnection");

  if (g_pNetInstance && CloseNetInstanceConnections(g_pNetInstance)) {
    return 0;
  }

  return -1;
}

/**
 * @param dwTimeout The timeout in milliseconds.
 *
 * @return The number of items left to flush before the timing out.
 */
INT CIV2PATCH_API Net_FlushSendBuffer(DWORD dwTimeout)
{
  LogTrace("Net_FlushSendBuffer(%u)", dwTimeout);

  if (!g_pNetInstance) {
    return 0;
  }

  DWORD dwNow = GetTickCount();
  DWORD dwExpirationTime = 0;

  while (HasNetInstanceData(g_pNetInstance)) {
    if (!dwExpirationTime) {
      dwExpirationTime = dwNow + dwTimeout;

      if (dwExpirationTime < dwNow) {
        dwExpirationTime = 0;
      }
    } else if (dwNow >= dwExpirationTime) {
      return 1;
    }

    SendNetInstanceData(g_pNetInstance);
    dwNow = GetTickCount();
  }

  return 0;
}

LPCSTR CIV2PATCH_API Net_GetCurrentProtoAddr(UINT, INT)
{
  if (g_pNetInstance) {
    return GetNetInstanceIp(g_pNetInstance);
  }

  return NULL;
}

INT CIV2PATCH_API Net_InFlushSendBuffer(void)
{
  if (g_pNetInstance && ProcessNetInstance(g_pNetInstance)) {
    return 0;
  } else {
    if (!g_fpInFlushSendBuffer) {
      g_fpInFlushSendBuffer = (int (*)(void))GetOriginalFunctionAddress(XD_IN_FLUSH_SEND_BUFFER_HOOK);
    }

    return g_fpInFlushSendBuffer();
  }

  return -1;
}

INT CIV2PATCH_API Net_InitializeModem(INT)
{
  LogTrace("Net_InitializeModem");

  return -1;
}

INT CIV2PATCH_API Net_InitializeSerial(INT)
{
  LogTrace("Net_InitializeSerial");

  return -1;
}

INT CIV2PATCH_API Net_InitializeSocketsIPXSPX(INT nModeFlags, INT nDirectPort, INT nBroadcastPort, INT nServer, UINT unMaxConnection, InitializeSocketsCallback callback)
{
  LogTrace("Net_InitializeSocketsIPXSPX");

  return -1;
}

/**
 * Initialize the TCP sockets.
 *
 * @param nModeFlags Whether to enable direct connection or broadcast.
 * @param nDirectPort The port to listen and send data via direct connection.
 * @param nBroadcastPort The port to listen and send data via broadcast.
 * @param nServer If non-zero, then this instance will act as a server.
 * @param unMaxConnection The maximum number of connections including itself.
 * @param callback Not used.
 */
INT CIV2PATCH_API Net_InitializeSocketsTCP(INT nModeFlags, INT nDirectPort, INT nBroadcastPort, INT nServer, UINT unMaxConnection, InitializeSocketsCallback callback)
{
  if (callback) {
    LogTrace("Net_InitializeSocketsTCP callback is not supported.");
  }

  if (g_pNetInstance) {
    FreeNetInstance(g_pNetInstance);
  }

  g_pNetInstance = CreateNetInstance((nServer > 0), unMaxConnection);

  if (g_pNetInstance && InitializeNetInstance(g_pNetInstance)) {
    return 0;
  }

  return -1;
}

INT CIV2PATCH_API Net_LaunchedByLobby(LPVOID, struct LobbyLaunchInfo *)
{
  return -3;
}

INT CIV2PATCH_API Net_LobbySendMessage(DWORD)
{
  LogTrace("Net_LobbySendMessage");

  return -1;
}

INT CIV2PATCH_API Net_OpenConnection(LPCSTR lpcsServer, DWORD dwTimeout)
{
  LogTrace("Net_OpenConnection(%s, %u)", lpcsServer, dwTimeout);

  if (g_pNetInstance && OpenNetInstanceConnection(g_pNetInstance, lpcsServer)) {
    return 0;
  }

  return -1;
}

/**
 * Shutdown and reinitialize all multiplayer resources.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API Net_ResetLibrary(void)
{
  LogTrace("Net_ResetLibrary");

  if (g_pNetInstance) {
    FreeNetInstance(g_pNetInstance);
    g_pNetInstance = NULL;
  }

  if (!IsSdlNetLibraryInitialized()) {
    if (InitializeSdlLibrary() && InitializeSdlNetLibrary()) {
      if (_SDL_Init(0) || _SDLNet_Init()) {
        LogError("Failed to initialize SDL Net: %s", _SDL_GetError());

        return -1;
      }
    } else {
      return -1;
    }
  }

  return 0;
}

/**
 * @return The number of bytes sent.
 */
INT CIV2PATCH_API Net_SendBroadcastData(LPVOID lpvMessage, DWORD dwSize, LONG lArg)
{
  LogTrace("Net_SendBroadcastData(%u, %d)", dwSize, lArg);

  if (g_pNetInstance && BroadcastNetInstanceData(g_pNetInstance, lpvMessage, dwSize)) {
    return dwSize;
  }

  return -1;
}

/**
 * @param wDestId The server/client Id to send message to. If set to 255, send
 *   the message to all.
 * @param lpvMessage The message to send.
 * @param dwSize The size of the message.
 * @param sArg [Unknown]
 */
INT CIV2PATCH_API Net_SendSecureData(WORD wDestId, LPVOID lpvMessage, DWORD dwSize, SHORT sArg)
{
  LogTrace("Net_SendSecureData(%u, %u, %d)", wDestId, dwSize, sArg);

  if (g_pNetInstance && QueueNetInstanceData(g_pNetInstance, wDestId, (LPBYTE)lpvMessage, dwSize)) {
    return dwSize;
  }

  return -1;
}

/**
 * Close a client connection.
 */
INT CIV2PATCH_API Net_ServerCloseConnection(WORD wClientId)
{
  LogTrace("Net_ServerCloseConnection(%u)", wClientId);
  return -1;
}

/**
 * Set the callback for receiving a broadcast message.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetBroadcastReceive(BroadcastReceiveCallback callback)
{
  SetNetBroadcastReceiveCallback(callback);
}

/**
 * Set the server callback for accepting a client connection.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetNewClientConnection(NewClientConnectionCallback callback)
{
  SetNetNewClientConnectionCallback(callback);
}

/**
 * Set the client callback for being connected to the server.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetOnClientConnectionToServer(ClientConnectionToServerCallback callback)
{
  SetNetClientConnectionToServerCallback(callback);
}

/**
 * Set the client callback for losing connection to the server.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetOnConnectionLost(ConnectionLostCallback callback)
{
  SetNetConnectionLostCallback(callback);
}

/**
 * Set the callback for sending a message that exceeds the maximum size.
 * The callback can be triggered during  XD_SendSecureData.
 *
 * @param dwMaxSize The maximum size in bytes. The callback will be called if
 *   the message is equal or larger than this size.
 * @param callback The callback function.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API Net_SetOversizedMessageCB(DWORD dwMaxSize, OversizedMessageCallback callback)
{
  if (!dwMaxSize) {
    LogError("Maximum message size cannot be 0.");
    dwMaxSize = 8192;
  }

  SetNetMessageMaxSize(dwMaxSize);
  SetNetOversizedMessageCallback(callback);

  return 0;
}

/**
 * Set the callback for receiving a message.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetSecureReceive(SecureReceiveCallback callback)
{
  SetNetSecureReceiveCallback(callback);
}

void CIV2PATCH_API Net_ShutdownModem(void)
{
  LogTrace("Net_ShutdownModem");
}

void CIV2PATCH_API Net_ShutdownSockets(void)
{
  LogTrace("Net_ShutdownSockets");

  if (g_pNetInstance) {
    FreeNetInstance(g_pNetInstance);
    g_pNetInstance = NULL;
  }
}

void CIV2PATCH_API Net_ShutdownTEN(void)
{
  LogTrace("Net_ShutdownTEN");
}

INT CIV2PATCH_API Net_StopConnections(void)
{
  LogTrace("Net_StopConnections");

  if (g_pNetInstance && CloseNetInstanceConnections(g_pNetInstance)) {
    return 0;
  }

  return -1;
}
