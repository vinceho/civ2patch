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

BOOL CIV2PATCH_API WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved);
BOOL CIV2PATCH_API PeekMessageEx(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
MCIERROR CIV2PATCH_API mciSendCommandEx(MCIDEVICEID IDDevice, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam);

int CIV2PATCH_API XD_ActivateServer(void);
int CIV2PATCH_API XD_CloseConnection(void);
int CIV2PATCH_API XD_FlushSendBuffer(unsigned long);
char *CIV2PATCH_API XD_GetCurrentProtoAddr(unsigned int, int);
int CIV2PATCH_API XD_GetXDaemonVersion(int *, int *, int *);
int CIV2PATCH_API XD_InFlushSendBuffer(void);
int CIV2PATCH_API XD_InitializeModem(int);
int CIV2PATCH_API XD_InitializeSerial(int);
int CIV2PATCH_API XD_InitializeSocketsIPXSPX(int, int, int, int, unsigned int, void (*)(void *, unsigned short, long));
int CIV2PATCH_API XD_InitializeSocketsTCP(int, int, int, int, unsigned int, void (*)(void *, unsigned short, long));
int CIV2PATCH_API XD_LaunchedByLobby(void *, struct LobbyLaunchInfo *);
int CIV2PATCH_API XD_LobbySendMessage(unsigned long);
int CIV2PATCH_API XD_OpenConnection(void *, unsigned long);
int CIV2PATCH_API XD_ResetLibrary(void);
int CIV2PATCH_API XD_SendBroadcastData(void *, unsigned long, long);
int CIV2PATCH_API XD_SendSecureData(unsigned short, void *, unsigned long, short);
int CIV2PATCH_API XD_ServerCloseConnection(unsigned short);
void CIV2PATCH_API XD_SetBroadcastReceive(void (*)(void *, unsigned short, long));
void CIV2PATCH_API XD_SetNewClientConnection(void (*)(unsigned short, unsigned short));
void CIV2PATCH_API XD_SetOnClientConnectionToServer(void (*)(short));
void CIV2PATCH_API XD_SetOnConnectionLost(void (*)(unsigned short));
int CIV2PATCH_API XD_SetOversizedMessageCB(unsigned long, void (*)(unsigned long));
void CIV2PATCH_API XD_SetSecureReceive(void (*)(unsigned short, void *, unsigned long, short));
void CIV2PATCH_API XD_ShutdownModem(void);
void CIV2PATCH_API XD_ShutdownSockets(void);
void CIV2PATCH_API XD_ShutdownTEN(void);
int CIV2PATCH_API XD_StopConnections(void);

#ifdef __cplusplus
}
#endif

#endif // CIV2PATCH_H
