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
#include <stdio.h>
#include "net.h"
#include "netconfig.h"
#include "config.h"
#include "sdllibrary.h"
#include "log.h"

NetInstance *CreateNetInstance(BOOL bServer, UINT unMaxConnection)
{
  NetInstance *instance = (NetInstance *)malloc(sizeof(NetInstance));

  memset(instance, 0, sizeof(NetInstance));

  instance->dwConnectionPort = GetNetConnectionPort();
  instance->dwBroadcastPort = GetNetBroadcastPort();
  instance->dwConnectionTimeout = GetNetConnectionTimeout();
  instance->bServer = bServer;
  instance->unMaxConnection = unMaxConnection;

  return instance;
}

BOOL InitializeNetInstance(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  INT nMinSocket = instance->bServer ? 2 : 1;
  INT nMaxSocket = instance->unMaxConnection + nMinSocket;

  if (!(instance->socketSet = _SDLNet_AllocSocketSet(nMaxSocket))) {
    LogError("Failed to initialize socket set: %s", _SDLNet_GetError());

    return FALSE;
  }

  // Initialize broadcast socket.
  if (_SDLNet_ResolveHost(&instance->broadcastIP, NET_BROADCAST_IP, instance->dwBroadcastPort) == -1) {
    LogError("Failed to resolve host for broadcast: %s", _SDLNet_GetError());

    return FALSE;
  }

  if (!(instance->broadcastSocket = _SDLNet_UDP_Open(instance->dwBroadcastPort))) {
    LogError("Failed to open broadcast socket: %s", _SDLNet_GetError());

    return FALSE;
  }

  if (_SDLNet_AddSocket(instance->socketSet, (SDLNet_GenericSocket)instance->broadcastSocket) == -1) {
    LogError("Failed to add broadcast socket: %s", _SDLNet_GetError());

    return FALSE;
  }

  // Initialize local IP address.
  IPaddress address;
  LPCSTR lpcsHost = NULL;

  memset(&address, 0, sizeof(IPaddress));

  if (!(lpcsHost = _SDLNet_ResolveIP(&address))) {
    LogError("Failed to resolve host name: %s", _SDLNet_GetError());

    return FALSE;
  }

  if (_SDLNet_ResolveHost(&address, lpcsHost, 0) == -1) {
    LogError("Failed to resolve host address: %s", _SDLNet_GetError());

    return FALSE;
  }

  LPBYTE dotQuad = (LPBYTE)&address.host;

  snprintf(instance->szIp, NET_ADDRESS_CHAR_MAX, "%u.%u.%u.%u", dotQuad[0], dotQuad[1], dotQuad[2], dotQuad[3]);

  // Initialize connection slots.
  instance->connections = (NetConnection *)malloc(sizeof(NetConnection) * instance->unMaxConnection);
  memset(instance->connections, 0, sizeof(NetConnection) * instance->unMaxConnection);

  return TRUE;
}

LPCSTR GetNetInstanceIp(NetInstance *instance)
{
  if (!instance) {
    return NULL;
  }

  return instance->szIp;
}

INT GetNetInstanceFreeConnectionIndex(NetInstance *instance)
{
  if (!instance) {
    return -1;
  }

  if (!instance->connections) {
    return -1;
  }

  for (UINT i = 0; i < instance->unMaxConnection; i++) {
    NetConnection *con = &instance->connections[i];

    if (!con->socket) {
      return i;
    }
  }

  return -1;
}

BOOL ActivateNetInstanceServer(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (!instance->bServer) {
    LogError("Not configured to run as a server.");

    return FALSE;
  }

  IPaddress address;

  if (_SDLNet_ResolveHost(&address, NULL, instance->dwConnectionPort) == -1) {
    LogError("Failed to resolve host for connection listener: %s", _SDLNet_GetError());

    return FALSE;
  }

  if (!(instance->listenSocket = _SDLNet_TCP_Open(&address))) {
    LogError("Failed to open connection listener socket: %s", _SDLNet_GetError());

    return FALSE;
  }

  if (_SDLNet_AddSocket(instance->socketSet, (SDLNet_GenericSocket)instance->listenSocket) == -1) {
    LogError("Failed to add connection listener socket: %s", _SDLNet_GetError());

    return FALSE;
  }

  return TRUE;
}

UINT GetNetInstanceNumConnection(NetInstance *instance)
{
  if (!instance) {
    return 0;
  }

  if (!instance->connections) {
    return 0;
  }

  UINT unNumConnection = 0;

  for (UINT i = 0; i < instance->unMaxConnection; i++) {
    NetConnection *con = &instance->connections[i];

    if (con->socket) {
      unNumConnection++;
    }
  }

  return unNumConnection;
}

BOOL CloseNetInstanceConnection(NetInstance *instance, UINT unConnectionIndex)
{
  if (!instance) {
    return FALSE;
  }

  if (!instance->connections || unConnectionIndex >= instance->unMaxConnection) {
    return FALSE;
  }

  NetConnection *con = &instance->connections[unConnectionIndex];

  if (IsSdlNetLibraryInitialized() && con->socket) {
    if (instance->socketSet) {
      _SDLNet_DelSocket(instance->socketSet, (SDLNet_GenericSocket)con->socket);
    }

    _SDLNet_TCP_Close(con->socket);
  }

  if (con->receiveQueue) {
    FreeNetMessage(con->receiveQueue);
  }

  if (con->sendQueue) {
    FreeNetMessage(con->sendQueue);
  }

  memset(con, 0, sizeof(NetConnection));

  return TRUE;
}

BOOL CloseNetInstanceConnections(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  BOOL bResult = TRUE;

  for (UINT i = 0; i < instance->unMaxConnection; i++) {
    bResult &= CloseNetInstanceConnection(instance, i);
  }

  return bResult;
}

BOOL OpenNetInstanceConnection(NetInstance *instance, LPCSTR lpcsServer)
{
  if (!instance) {
    return FALSE;
  }

  // Minimum length of an IPv4 address is 7 (e.g. 1.1.1.1).
  if (!lpcsServer || strlen(lpcsServer) < NET_ADDRESS_CHAR_MIN) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  INT nConnectionIndex = GetNetInstanceFreeConnectionIndex(instance);

  if (nConnectionIndex < 0) {
    LogError("Failed to open connection due to no free socket.");

    return FALSE;
  }

  IPaddress address;
  NetConnection *con = &instance->connections[nConnectionIndex];

  if (_SDLNet_ResolveHost(&address, lpcsServer, instance->dwConnectionPort) == -1) {
    LogError("Failed to resolve host for connection to the server: %s", _SDLNet_GetError());

    return FALSE;
  }

  if (!(con->socket = _SDLNet_TCP_Open(&address))) {
    LogError("Failed to open connection socket to the server: %s", _SDLNet_GetError());

    return FALSE;
  }

  if (_SDLNet_AddSocket(instance->socketSet, (SDLNet_GenericSocket)con->socket) == -1) {
    LogError("Failed to add server connection socket: %s", _SDLNet_GetError());

    CloseNetInstanceConnection(instance, nConnectionIndex);

    return FALSE;
  }

  con->bToServer = TRUE;
  con->receiveQueue = CreateNetMessage(GetNetMessageMaxSize(), instance->dwConnectionTimeout);

  return TRUE;
}

BOOL ReceiveNetInstanceBroadcast(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  BOOL bResult = FALSE;

  // Listen to broadcast messages.
  if (instance->broadcastSocket && SDLNet_SocketReady(instance->broadcastSocket)) {
    UDPpacket *packet = _SDLNet_AllocPacket(GetNetMessageMaxSize());

    if (packet) {
      INT nReceiveResult = _SDLNet_UDP_Recv(instance->broadcastSocket, packet);
      BroadcastReceiveCallback fpCallback = GetNetBroadcastReceiveCallback();

      if (nReceiveResult == 1) {
        if (fpCallback) {
          LogTrace("BroadcastReceiveCallback(%d, %d)", packet->len, 0);
          fpCallback(packet->data, packet->len, 0);
        }

        bResult = TRUE;
      } else if (nReceiveResult == -1) {
        LogError("Failed to receive broadcast: %s", _SDLNet_GetError());
      }

      _SDLNet_FreePacket(packet);
    } else {
      LogError("Failed to allocate packet: %s", _SDLNet_GetError());
    }
  }

  return bResult;
}

BOOL AcceptNetInstanceConnection(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (instance->listenSocket && SDLNet_SocketReady(instance->listenSocket)) {
    // Listen to client connections.
    INT nConnectionIndex = GetNetInstanceFreeConnectionIndex(instance);

    if (nConnectionIndex >= 0) {
      // Find free socket and accept the connection.
      NetConnection *con = &instance->connections[nConnectionIndex];
      TCPsocket socket = _SDLNet_TCP_Accept(instance->listenSocket);

      if (socket) {
        if (_SDLNet_AddSocket(instance->socketSet, (SDLNet_GenericSocket)socket) == -1) {
          LogError("Failed to add client connection socket: %s", _SDLNet_GetError());
          _SDLNet_TCP_Close(socket);
        } else {
          // Send client the assigned Id.
          UINT unClientId = nConnectionIndex + 1;

          con->socket = socket;
          con->unId = unClientId;
          con->receiveQueue = CreateNetMessage(GetNetMessageMaxSize(), instance->dwConnectionTimeout);

          // Add 1 to include the listener connection.
          UINT unNumConnection = GetNetInstanceNumConnection(instance) + 1;
          NetMessageBuffer *buffer = CreateNetMessageBuffer((LPBYTE)&unClientId, sizeof(unClientId), unClientId, 0);
          NewClientConnectionCallback fpCallback = GetNetNewClientConnectionCallback();

          AddNetMessage(&con->sendQueue, buffer, instance->dwConnectionTimeout);

          if (fpCallback) {
            LogTrace("NewClientConnectionCallback(%d, %d)", unNumConnection, unClientId);
            fpCallback(unNumConnection, unClientId);
          }

          return TRUE;
        }
      } else {
        LogError("Failed to accept client connection: %s", _SDLNet_GetError());
      }
    } else {
      LogError("Failed to accept connection due to no free socket.");
    }

    return FALSE;
  }

  return TRUE;
}

BOOL ReceiveNetInstanceData(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (!instance->connections) {
    return FALSE;
  }

  // Listen to client messages.
  for (UINT i = 0; i < instance->unMaxConnection; i++) {
    NetConnection *con = &instance->connections[i];

    if (con->socket && SDLNet_SocketReady(con->socket)) {
      INT nReceiveResult = ReceiveNetMessage(con->receiveQueue, con->socket);
      NetMessageBuffer *buffer = PeekNetMessageMessageBuffer(con->receiveQueue);

      if (!nReceiveResult) {
        if (IsNetMessageBufferFull(buffer)) {
          SecureReceiveCallback fpCallback = GetNetSecureReceiveCallback();
          INT nDestinationId = GetNetMessageBufferDestinationId(buffer);
          INT nSourceId = GetNetMessageBufferSourceId(buffer);
          DWORD dwMessageSize = GetNetMessageBufferSize(buffer);
          LPBYTE data = GetNetMessageBufferData(buffer);

          if (instance->bServer) {
            // Server needs to forward requests to other clients if its not
            // the final recipient.
            if (nDestinationId > 0) {
              UINT unRealSourceId = (nDestinationId == NET_BROADCAST_ID) ? 0 : nSourceId;

              for (UINT j = 0; j < instance->unMaxConnection; j++) {
                NetConnection *forwardCon = &instance->connections[j];

                if (forwardCon->socket
                    && !forwardCon->bToServer
                    && nSourceId != forwardCon->unId
                    && (nDestinationId == forwardCon->unId || nDestinationId == NET_BROADCAST_ID)) {
                  NetMessageBuffer *newBuffer = CreateNetMessageBuffer(data, dwMessageSize, forwardCon->unId, unRealSourceId);

                  AddNetMessage(&forwardCon->sendQueue, newBuffer, instance->dwConnectionTimeout);
                }
              }
            }

            // Don't process the message on the server if it's not the real
            // recipient.
            if (nDestinationId != NET_BROADCAST_ID && nDestinationId != 0) {
              continue;
            }
          } else {
            if (con->bToServer && !con->unId) {
              // Get client Id from server response if not assigned already.
              ClientConnectionToServerCallback fpCallback = GetNetClientConnectionToServerCallback();

              if (sizeof(con->unId) == dwMessageSize && *(UINT *)data == nDestinationId) {
                con->unId = nDestinationId;

                if (fpCallback) {
                  LogTrace("ClientConnectionToServerCallback(%d)", con->unId);
                  fpCallback(con->unId);
                }
              } else {
                LogError("Failed to connect to server due to incorrect Id assignment response.");
                CloseNetInstanceConnection(instance, i);
              }

              continue;
            }
          }

          if (fpCallback) {
            LogTrace("SecureReceiveCallback(%d, %d)", nSourceId, dwMessageSize);
            fpCallback(nSourceId, data, dwMessageSize, 0);
          }
        }
      } else if (nReceiveResult == -1) {
        if (!con->bToServer || con->unId) {
          LogError("Connection to instance %d lost.", con->unId);

          ConnectionLostCallback fpCallback = GetNetConnectionLostCallback();

          if (fpCallback) {
            LogTrace("ConnectionLostCallback(%d)", con->unId);
            fpCallback(con->unId);
          }
        }

        CloseNetInstanceConnection(instance, i);
      } else if (nReceiveResult == 1) {
        // Timed out.
        ResetNetMessageBuffer(buffer);
      }
    }
  }

  return TRUE;
}

BOOL BroadcastNetInstanceData(NetInstance *instance, LPVOID lpvMessage, DWORD dwSize)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (dwSize >= GetNetMessageMaxSize()) {
    OversizedMessageCallback fpCallback = GetNetOversizedMessageCallback();

    if (fpCallback) {
      fpCallback(dwSize);
    }

    return FALSE;
  }

  if (!instance->broadcastSocket) {
    return FALSE;
  }

  BOOL bResult = TRUE;
  UDPpacket *packet = _SDLNet_AllocPacket(dwSize);

  if (!packet) {
    LogError("Failed to allocate broadcast packet: %s", _SDLNet_GetError());

    return FALSE;
  }

  packet->address = instance->broadcastIP;
  packet->len = dwSize;
  memcpy(packet->data, lpvMessage, dwSize);

  if (!_SDLNet_UDP_Send(instance->broadcastSocket, -1, packet)) {
    LogError("Failed to send broadcast: %s", _SDLNet_GetError());
    bResult = FALSE;
  }

  _SDLNet_FreePacket(packet);

  return bResult;
}

BOOL QueueNetInstanceData(NetInstance *instance, UINT unDestId, LPBYTE data, DWORD dwSize)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (dwSize >= GetNetMessageMaxSize()) {
    OversizedMessageCallback fpCallback = GetNetOversizedMessageCallback();

    if (fpCallback) {
      fpCallback(dwSize);
    }

    return FALSE;
  }

  if (!instance->connections) {
    return FALSE;
  }

  for (UINT i = 0; i < instance->unMaxConnection; i++) {
    NetConnection *con = &instance->connections[i];

    if (con->socket) {
      UINT unSrcId = con->bToServer ? con->unId : 0;
      UINT unTargetId = con->bToServer ? 0 : con->unId;

      if (!instance->bServer && con->bToServer) {
        // If instance is not the server, sent the message to the server to be
        // forwarded to be final destination.
        unTargetId = unDestId;
      } else if (unDestId != unTargetId && unDestId != NET_BROADCAST_ID) {
        continue;
      }

      NetMessageBuffer *buffer = CreateNetMessageBuffer(data, dwSize, unTargetId, unSrcId);

      AddNetMessage(&con->sendQueue, buffer, instance->dwConnectionTimeout);
    }
  }

  return TRUE;
}

BOOL HasNetInstanceData(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (!instance->connections) {
    return FALSE;
  }

  for (UINT i = 0; i < instance->unMaxConnection; i++) {
    NetConnection *con = &instance->connections[i];

    if (con->socket && con->sendQueue) {
      return TRUE;
    }
  }

  return FALSE;
}

BOOL SendNetInstanceData(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (!instance->connections) {
    return FALSE;
  }

  for (UINT i = 0; i < instance->unMaxConnection; i++) {
    NetConnection *con = &instance->connections[i];

    if (con->socket) {
      NetMessage *message = con->sendQueue;

      if (message) {
        if (!IsNetMessageBufferFull(PeekNetMessageMessageBuffer(message))) {
          LogError("Failed to send incomplete message.");
          FreeNetMessage(PopNetMessage(&con->sendQueue));

          continue;
        }

        INT nSendResult = SendNetMessage(message, con->socket);

        if (!nSendResult) {
          FreeNetMessage(PopNetMessage(&con->sendQueue));
        } else if (nSendResult == -1) {
          if (!con->bToServer || con->unId) {
            LogError("Connection to instance %d lost.", con->unId);

            ConnectionLostCallback fpCallback = GetNetConnectionLostCallback();

            if (fpCallback) {
              LogTrace("ConnectionLostCallback(%d)", con->unId);
              fpCallback(con->unId);
            }
          }

          CloseNetInstanceConnection(instance, i);
        }
      }
    }
  }

  return TRUE;
}

BOOL ProcessNetInstance(NetInstance *instance)
{
  if (!instance) {
    return FALSE;
  }

  if (!IsSdlNetLibraryInitialized()) {
    return FALSE;
  }

  if (instance->socketSet) {
    INT nNumReadySocket = _SDLNet_CheckSockets(instance->socketSet, 0);

    if (nNumReadySocket > 0) {
      ReceiveNetInstanceBroadcast(instance);
      AcceptNetInstanceConnection(instance);
      ReceiveNetInstanceData(instance);
    }
  }

  SendNetInstanceData(instance);

  return TRUE;
}

void FreeNetInstance(NetInstance *instance)
{
  if (instance) {
    CloseNetInstanceConnections(instance);

    if (instance->connections) {
      free(instance->connections);
    }

    if (IsSdlNetLibraryInitialized()) {
      if (instance->broadcastSocket) {
        if (instance->socketSet) {
          _SDLNet_DelSocket(instance->socketSet, (SDLNet_GenericSocket)instance->broadcastSocket);
        }

        _SDLNet_UDP_Close(instance->broadcastSocket);
      }

      if (instance->listenSocket) {
        if (instance->socketSet) {
          _SDLNet_DelSocket(instance->socketSet, (SDLNet_GenericSocket)instance->listenSocket);
        }

        _SDLNet_TCP_Close(instance->listenSocket);
      }

      if (instance->socketSet) {
        _SDLNet_FreeSocketSet(instance->socketSet);
      }
    }

    free(instance);
  }
}
