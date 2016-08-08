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
#ifndef CIV2PATCH_H
#define CIV2PATCH_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_DLL
#define CIV2PATCH_API __declspec(dllexport)
#else
#define CIV2PATCH_API __declspec(dllimport)
#endif

typedef void (*InitializeSocketsCallback)(LPVOID, WORD, LONG);

/**
 * @param lpvMessage The received message.
 * @param wSize The size of the message.
 * @param lArg [Unknown]
 */
typedef void (*BroadcastReceiveCallback)(LPVOID lpvMessage, WORD wSize, LONG lArg);

/**
 * The server callback for accepting a client connection.
 *
 * @param wNumPlayer The total number of players in the game.
 * @param wClientId The client Id connected to the server.
 */
typedef void (*NewClientConnectionCallback)(WORD wNumPlayer, WORD wClientId);

/*
 * The client callback for connecting to the server.
 *
 * @param sClientId The client Id assigned by the server.
 */
typedef void (*ClientConnectionToServerCallback)(SHORT sClientId);

/**
 * @param wId The server/client Id.
 */
typedef void (*ConnectionLostCallback)(WORD);

/**
 * @param dwSize The size of the message.
 */
typedef void (*OversizedMessageCallback)(DWORD dwSize);

/**
 * @param wSenderId The Id of the sender instance. If the original message was
 *   sent using a 255 destination Id, then the sender Id is always the server.
 * @param lpvMessage The received message.
 * @param dwSize The size of the message.
 * @param sArg [Unknown]
 */
typedef void (*SecureReceiveCallback)(WORD wSenderId, LPVOID lpvMessage, DWORD dwSize, SHORT sArg);

BOOL CIV2PATCH_API WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved);
BOOL CIV2PATCH_API PeekMessageEx(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
MCIERROR CIV2PATCH_API mciSendCommandEx(MCIDEVICEID IDDevice, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam);

INT CIV2PATCH_API Net_ActivateServer(void);
INT CIV2PATCH_API Net_CloseConnection(void);
INT CIV2PATCH_API Net_FlushSendBuffer(DWORD);
LPCSTR CIV2PATCH_API Net_GetCurrentProtoAddr(UINT, INT);
INT CIV2PATCH_API Net_InFlushSendBuffer(void);
INT CIV2PATCH_API Net_InitializeModem(INT);
INT CIV2PATCH_API Net_InitializeSerial(INT);
INT CIV2PATCH_API Net_InitializeSocketsIPXSPX(INT, INT, INT, INT, UINT, InitializeSocketsCallback);
INT CIV2PATCH_API Net_InitializeSocketsTCP(INT, INT, INT, INT, UINT, InitializeSocketsCallback);
INT CIV2PATCH_API Net_LaunchedByLobby(LPVOID, struct LobbyLaunchInfo *);
INT CIV2PATCH_API Net_LobbySendMessage(DWORD);
INT CIV2PATCH_API Net_OpenConnection(LPCSTR, DWORD);
INT CIV2PATCH_API Net_ResetLibrary(void);
INT CIV2PATCH_API Net_SendBroadcastData(LPVOID, DWORD, LONG);
INT CIV2PATCH_API Net_SendSecureData(WORD, LPVOID, DWORD, SHORT);
INT CIV2PATCH_API Net_ServerCloseConnection(WORD);
void CIV2PATCH_API Net_SetBroadcastReceive(BroadcastReceiveCallback);
void CIV2PATCH_API Net_SetNewClientConnection(NewClientConnectionCallback);
void CIV2PATCH_API Net_SetOnClientConnectionToServer(ClientConnectionToServerCallback);
void CIV2PATCH_API Net_SetOnConnectionLost(ConnectionLostCallback);
INT CIV2PATCH_API Net_SetOversizedMessageCB(DWORD, OversizedMessageCallback);
void CIV2PATCH_API Net_SetSecureReceive(SecureReceiveCallback);
void CIV2PATCH_API Net_ShutdownModem(void);
void CIV2PATCH_API Net_ShutdownSockets(void);
void CIV2PATCH_API Net_ShutdownTEN(void);
INT CIV2PATCH_API Net_StopConnections(void);

#ifdef __cplusplus
}
#endif

#endif // CIV2PATCH_H
