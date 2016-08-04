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
#include "net.h"
#include "hook.h"
#include "log.h"
#include "sdllibrary.h"

CHAR g_szNetHostAddress[NET_ADDRESS_CHAR_MAX] = "";

NetConfig g_netConfig = {
  // Callbacks.
  NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  // Configuration.
  4096
};

NetServer g_netServer = { NULL, NULL, 0 };
NetClient g_netClient = { NULL, 0 };
NetConnection g_netConn = { { 0, 0 }, NULL, { 0, 0 }, NULL, NULL, FALSE, 0, 0 };

INT CIV2PATCH_API Net_ActivateServer(void)
{
  LogTrace("Net_ActivateServer");

  if (!g_netConn.bServer) {
    LogError("Not configured to run as a server.");

    return -1;
  }

  // On a server, use the TCP socket to listen for client connections.
  if (_SDLNet_ResolveHost(&g_netConn.directIP, NULL, g_netConn.nDirectPort) == -1) {
    LogError("Failed to resolve host for client listener: %s", _SDLNet_GetError());

    return -1;
  }

  if (!(g_netConn.directSocket = _SDLNet_TCP_Open(&g_netConn.directIP))) {
    LogError("Failed to open client listener socket: %s", _SDLNet_GetError());

    return -1;
  }

  if (_SDLNet_AddSocket(g_netConn.socketSet, (SDLNet_GenericSocket)g_netConn.directSocket) == -1) {
    LogError("Failed to add client listener socket: %s", _SDLNet_GetError());

    return -1;
  }

  g_netServer.unNumClient = 0;
  g_netServer.clientSockets = (TCPsocket *)malloc(g_netConn.unMaxConnection * sizeof(TCPsocket));
  g_netServer.buffers = (NetMessageBuffer **)malloc(g_netConn.unMaxConnection * sizeof(NetMessageBuffer *));

  memset(g_netServer.clientSockets, 0, g_netConn.unMaxConnection * sizeof(TCPsocket));
  memset(g_netServer.buffers, 0, g_netConn.unMaxConnection * sizeof(BYTE *));

  return 0;
}

/**
 * Close connection to the server.
 */
INT CIV2PATCH_API Net_CloseConnection(void)
{
  LogTrace("Net_CloseConnection");
}

/**
 * @param dwTimeout The timeout in milliseconds.
 *
 * @return The number of items left to flush before the timing out.
 */
INT CIV2PATCH_API Net_FlushSendBuffer(DWORD dwTimeout)
{
  LogTrace("Net_FlushSendBuffer(%u)", dwTimeout);

  return 0;
}

LPCSTR CIV2PATCH_API Net_GetCurrentProtoAddr(UINT, INT)
{
  if (strlen(g_szNetHostAddress) > 0) {
    return g_szNetHostAddress;
  }

  IPaddress address;
  LPCSTR lpcsHost = NULL;

  memset(&address, 0, sizeof(IPaddress));

  if (!(lpcsHost = _SDLNet_ResolveIP(&address))) {
    LogError("Failed to resolve host name: %s", _SDLNet_GetError());

    return NULL;
  }

  if (_SDLNet_ResolveHost(&address, lpcsHost, 0) == -1) {
    LogError("Failed to resolve host address: %s", _SDLNet_GetError());

    return NULL;
  }

  BYTE *dotQuad = (BYTE *)&address.host;

  snprintf(g_szNetHostAddress, NET_ADDRESS_CHAR_MAX, "%u.%u.%u.%u", dotQuad[0], dotQuad[1], dotQuad[2], dotQuad[3]);

  return g_szNetHostAddress;
}

INT CIV2PATCH_API Net_InFlushSendBuffer(void)
{
  if (g_netConn.socketSet) {
    INT nNumReadySocket = _SDLNet_CheckSockets(g_netConn.socketSet, 0);

    if (nNumReadySocket > 0) {
      // Listen to broadcast messages.
      if (g_netConn.boardcastSocket && SDLNet_SocketReady(g_netConn.boardcastSocket)) {
        UDPpacket *packet = _SDLNet_AllocPacket(g_netConfig.dwMessageMaxSize);

        if (packet) {
          INT nReceiveResult = _SDLNet_UDP_Recv(g_netConn.boardcastSocket, packet);

          if (nReceiveResult == 1) {
            LogTrace("BroadcastReceiveCallback(%d, %d)", packet->len, 0);

            g_netConfig.fpBroadcastReceiveCallback(packet->data, packet->len, 0);
          } else if (nReceiveResult == -1) {
            LogError("Failed to receive broadcast: %s", _SDLNet_GetError());
          }

          _SDLNet_FreePacket(packet);
        } else {
          LogError("Failed to allocate packet: %s", _SDLNet_GetError());
        }
      }

      if (g_netConn.bServer) {
        // Listen to client connections.
        if (SDLNet_SocketReady(g_netConn.directSocket)) {
          if (g_netServer.unNumClient < g_netConn.unMaxConnection) {
            // Find free socket and accept the connection.
            for (UINT i = 0; i < g_netConn.unMaxConnection; i++) {
              if (!g_netServer.clientSockets[i]) {
                TCPsocket socket = _SDLNet_TCP_Accept(g_netConn.directSocket);

                if (socket) {
                  if (_SDLNet_AddSocket(g_netConn.socketSet, (SDLNet_GenericSocket)socket) == -1) {
                    LogError("Failed to add client connection socket: %s", _SDLNet_GetError());
                  } else {
                    UINT unClientId = i + 1;

                    g_netServer.clientSockets[i] = socket;
                    g_netServer.buffers[i] = CreateNetMessageBuffer(g_netConfig.dwMessageMaxSize);
                    g_netServer.unNumClient++;

                    SendNetMessageBuffer((BYTE *)&unClientId, sizeof(unClientId), unClientId, socket);

                    if (g_netConfig.fpNewClientConnectionCallback) {
                      LogDebug("NewClientConnectionCallback %d, %d", g_netServer.unNumClient + 1, unClientId);

                      g_netConfig.fpNewClientConnectionCallback(g_netServer.unNumClient + 1, unClientId);
                    }
                  }
                } else {
                  LogError("Failed to accept client connection: %s", _SDLNet_GetError());
                }

                break;
              }
            }
          }
        }

        // Listen to client messages.
        for (DWORD i = 0; i < g_netConn.unMaxConnection; i++) {
          if (g_netServer.clientSockets[i]) {
            TCPsocket socket = g_netServer.clientSockets[i];
            UINT unClientId = i + 1;

            if (SDLNet_SocketReady(socket)) {
              NetMessageBuffer *buffer = g_netServer.buffers[i];
              BOOL bReceiveResult = LoadNetMessageBuffer(buffer, socket);

              if (bReceiveResult) {
                if (IsNetMessageBufferFull(buffer) && g_netConfig.fpSecureReceiveCallback) {
                  LogTrace("SecureReceiveCallback(%d, %d)", GetNetMessageBufferDestId(buffer), GetNetMessageBufferSize(buffer));

                  g_netConfig.fpSecureReceiveCallback(unClientId, buffer->lpData, GetNetMessageBufferSize(buffer), 0);
                }
              } else {
                // Client disconnected.
                _SDLNet_DelSocket(g_netConn.socketSet, (SDLNet_GenericSocket)socket);
                _SDLNet_TCP_Close(socket);
                FreeNetMessageBuffer(buffer);

                g_netServer.clientSockets[i] = NULL;
                g_netServer.buffers[i] = NULL;
                g_netServer.unNumClient--;

                if (g_netConfig.fpConnectionLostCallback) {
                  LogDebug("ConnectionLostCallback %d", unClientId);

                  g_netConfig.fpConnectionLostCallback(unClientId);
                }
              }
            }
          }
        }
      } else {
        // Client-side listener.
        if (SDLNet_SocketReady(g_netConn.directSocket)) {
          TCPsocket socket = g_netConn.directSocket;
          NetMessageBuffer *buffer = g_netClient.buffer;

          if (buffer) {
            BOOL bReceiveResult = LoadNetMessageBuffer(buffer, socket);

            if (bReceiveResult) {
              if (g_netClient.unClientId) {
                if (IsNetMessageBufferFull(buffer) && g_netConfig.fpSecureReceiveCallback) {
                  LogTrace("SecureReceiveCallback(%d, %d)", GetNetMessageBufferDestId(buffer), GetNetMessageBufferSize(buffer));

                  g_netConfig.fpSecureReceiveCallback(GetNetMessageBufferDestId(buffer), buffer->lpData, GetNetMessageBufferSize(buffer), 0);
                }
              } else if (IsNetMessageBufferFull(buffer)){
                DWORD dwMessageSize = GetNetMessageBufferSize(buffer);

                // Connected to the server.
                if (dwMessageSize == sizeof(g_netClient.unClientId)) {
                  memcpy(&g_netClient.unClientId, buffer->lpData, dwMessageSize);

                  if (g_netConfig.fpClientConnectionToServerCallback) {
                    LogDebug("ClientConnectionToServerCallback %d", g_netClient.unClientId);

                    g_netConfig.fpClientConnectionToServerCallback(g_netClient.unClientId);
                  }
                } else {
                  LogError("Failed to receive connection message.");
                }
              }
            } else {
              // Disconnect
              LogError("Failed to receive message: %s", _SDLNet_GetError());

              if (g_netConfig.fpConnectionLostCallback) {
                LogDebug("ConnectionLostCallback %d", 0);

                g_netConfig.fpConnectionLostCallback(0);
              }

              return -1;
            }
          } else {
            LogError("Failed to receive message due to uninitialized buffer.");
          }
        }
      }
    }
  }

  return 0;
}

INT CIV2PATCH_API Net_InitializeModem(INT)
{
  LogDebug("Net_SetSecureReceive not implemented.");

  return -1;
}

INT CIV2PATCH_API Net_InitializeSerial(INT)
{
  LogDebug("Net_InitializeSerial not implemented.");

  return -1;
}

INT CIV2PATCH_API Net_InitializeSocketsIPXSPX(INT nModeFlags, INT nDirectPort, INT nBroadcastPort, INT nServer, UINT unMaxConnection, InitializeSocketsCallback callback)
{
  LogDebug("Net_InitializeSocketsIPXSPX not implemented.");

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
    LogDebug("Net_InitializeSocketsTCP callback is not implemented.");
  }

  if (!unMaxConnection) {
    LogError("Failed to initialize TCP sockets: Must allow one or more connections.");

    return -1;
  }

  g_netConn.bServer = (nServer > 0);
  g_netConn.nDirectPort = nDirectPort;
  g_netConn.unMaxConnection = unMaxConnection;

  // Sockets for direct, broadcast, and client connections.
  INT nMaxSocket = g_netConn.bServer ? (2 + unMaxConnection) : 2;

  if (!(g_netConn.socketSet = _SDLNet_AllocSocketSet(nMaxSocket))) {
    LogError("Failed to initialize socket set: %s", _SDLNet_GetError());

    return -1;
  }

  if (_SDLNet_ResolveHost(&g_netConn.broadcastIP, NET_BROADCAST_IP, nBroadcastPort) == -1) {
    LogError("Failed to resolve host for broadcast: %s", _SDLNet_GetError());

    return -1;
  }

  if (!(g_netConn.boardcastSocket = _SDLNet_UDP_Open(nBroadcastPort))) {
    LogError("Failed to open broadcast socket: %s", _SDLNet_GetError());

    return -1;
  }

  if (_SDLNet_AddSocket(g_netConn.socketSet, (SDLNet_GenericSocket)g_netConn.boardcastSocket) == -1) {
    LogError("Failed to add broadcast socket: %s", _SDLNet_GetError());

    return -1;
  }

  return 0;
}

INT CIV2PATCH_API Net_LaunchedByLobby(LPVOID, struct LobbyLaunchInfo *)
{
  return -3;
}

INT CIV2PATCH_API Net_LobbySendMessage(DWORD)
{
  LogDebug("Net_LobbySendMessage not implemented.");

  return -1;
}

INT CIV2PATCH_API Net_OpenConnection(LPCSTR lpcsServer, DWORD dwTimeout)
{
  LogTrace("Net_OpenConnection(%s, %d)", lpcsServer, dwTimeout);

  if (g_netConn.bServer) {
    LogError("Not configured to run as a client.");

    return -1;
  }

  // On a client, use the TCP socket to connect to the server.
  if (_SDLNet_ResolveHost(&g_netConn.directIP, lpcsServer, g_netConn.nDirectPort) == -1) {
    LogError("Failed to resolve host for server connection: %s", _SDLNet_GetError());

    return -1;
  }

  if (!(g_netConn.directSocket = _SDLNet_TCP_Open(&g_netConn.directIP))) {
    LogError("Failed to open server connection socket: %s", _SDLNet_GetError());

    return -1;
  }

  if (_SDLNet_AddSocket(g_netConn.socketSet, (SDLNet_GenericSocket)g_netConn.directSocket) == -1) {
    LogError("Failed to add server connection socket: %s", _SDLNet_GetError());

    return -1;
  }

  g_netClient.buffer = CreateNetMessageBuffer(g_netConfig.dwMessageMaxSize);

  return 0;
}

/**
 * Shutdown and reinitialize all multiplayer resources.
 *
 * @return Non-zero if failure.
 */
INT CIV2PATCH_API Net_ResetLibrary(void)
{
  LogTrace("Net_ResetLibrary");

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

  Net_ShutdownSockets();

  return 0;
}

/**
 * @return The number of bytes sent.
 */
INT CIV2PATCH_API Net_SendBroadcastData(LPVOID lpvMessage, DWORD dwSize, LONG lArg)
{
  LogTrace("Net_SendBroadcastData(%d, %d)", dwSize, lArg);

  if (dwSize >= g_netConfig.dwMessageMaxSize) {
    if (g_netConfig.fpOversizedMessageCallback) {
      g_netConfig.fpOversizedMessageCallback(dwSize);
    }

    return -1;
  }

  INT nResult = -1;

  if (g_netConn.boardcastSocket) {
    UDPpacket* packet = _SDLNet_AllocPacket(dwSize);

    if (!packet) {
      LogError("Failed to allocate broadcast packet: %s", _SDLNet_GetError());

      return -1;
    }

    packet->address = g_netConn.broadcastIP;
    packet->len = dwSize;
    memcpy(packet->data, lpvMessage, dwSize);

    if (!_SDLNet_UDP_Send(g_netConn.boardcastSocket, -1, packet)) {
      LogError("Failed to send broadcast: %s", _SDLNet_GetError());
    } else {
      nResult = dwSize;
    }

    _SDLNet_FreePacket(packet);
  }

  return nResult;
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

  if (dwSize >= g_netConfig.dwMessageMaxSize) {
    if (g_netConfig.fpOversizedMessageCallback) {
      g_netConfig.fpOversizedMessageCallback(dwSize);
    }

    return -1;
  }

  if (!g_netConn.bServer && wDestId == 255) {
    wDestId = 0;
  }

  if (wDestId == 0) {
    if (!g_netConn.bServer) {
      TCPsocket socket = g_netConn.directSocket;

      if (g_netClient.unClientId && socket) {
        BOOL bSendResult = SendNetMessageBuffer((BYTE *)lpvMessage, dwSize, wDestId, socket);

        if (!bSendResult) {
          LogError("Failed to send message from client: %s", _SDLNet_GetError());
        }

        return dwSize;
      } else {
        LogError("Client cannot send message due to no connection to the server.");
      }
    } else {
      LogError("Server cannot send message to itself.");
    }
  } else {
    if (g_netConn.bServer) {
      for (UINT i = 0; i < g_netConn.unMaxConnection; i++) {
        UINT unClientId = i + 1;

        if (unClientId == wDestId || wDestId == 255) {
          TCPsocket socket = g_netServer.clientSockets[i];

          if (socket) {
            BOOL bSendResult = SendNetMessageBuffer((BYTE *)lpvMessage, dwSize, unClientId, socket);

            if (!bSendResult) {
              LogError("Failed to send message from server to client %d: %s", unClientId, _SDLNet_GetError());
            }

            if (wDestId != 255) {
              return dwSize;
            }
          }
        }
      }

      return dwSize;
    } else {
      LogError("Client can only send message to the server.");
    }
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
  g_netConfig.fpBroadcastReceiveCallback = callback;
}

/**
 * Set the server callback for accepting a client connection.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetNewClientConnection(NewClientConnectionCallback callback)
{
  g_netConfig.fpNewClientConnectionCallback = callback;
}

/**
 * Set the client callback for being connected to the server.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetOnClientConnectionToServer(ClientConnectionToServerCallback callback)
{
  g_netConfig.fpClientConnectionToServerCallback = callback;
}

/**
 * Set the client callback for losing connection to the server.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetOnConnectionLost(ConnectionLostCallback callback)
{
  g_netConfig.fpConnectionLostCallback = callback;
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
    dwMaxSize = 4096;
  }

  g_netConfig.fpOversizedMessageCallback = callback;
  g_netConfig.dwMessageMaxSize = dwMaxSize;

  return 0;
}

/**
 * Set the callback for receiving a message.
 *
 * @param callback The callback function.
 */
void CIV2PATCH_API Net_SetSecureReceive(SecureReceiveCallback callback)
{
  g_netConfig.fpSecureReceiveCallback = callback;
}

void CIV2PATCH_API Net_ShutdownModem(void)
{
  LogTrace("Net_ShutdownModem");
}

void CIV2PATCH_API Net_ShutdownSockets(void)
{
  LogTrace("Net_ShutdownSockets");

  // Clean up resources.
  if (g_netConn.boardcastSocket) {
    _SDLNet_UDP_Close(g_netConn.boardcastSocket);
    g_netConn.boardcastSocket = NULL;
  }
}

void CIV2PATCH_API Net_ShutdownTEN(void)
{
  LogTrace("Net_ShutdownTEN");
}

INT CIV2PATCH_API Net_StopConnections(void)
{
  LogDebug("Net_StopConnections");
  return -1;
}
