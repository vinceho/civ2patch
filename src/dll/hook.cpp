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
#include "hook.h"
#include "log.h"

FunctionHook g_functionHooks[] = {
  { // PEEK_MESSAGE_HOOK
    "PeekMessageEx", USER32_MODULE, "PeekMessageA",
    (DWORD[]){ 0x5BBA64, 0x5BBB91, 0x5BD2F9, 0x5BD31D },
    4
  },
  { // MCI_SEND_CMD_HOOK
    "mciSendCommandEx", USER32_MODULE, "mciSendCommandA",
    (DWORD[]){
      0x5DDA8B, 0x5DDADE, 0x5DDAFE, 0x5DDB58, 0x5DDB7C, 0x5DDB9C, 0x5DDC1F, 0x5DDC43, 0x5DDCD3, 0x5DDD34,
      0x5DDD7B, 0x5DDDC7, 0x5DDDE9, 0x5DDE3F, 0x5DDE79, 0x5DDEBF, 0x5DDF2C, 0x5DDF70, 0x5DDF90, 0x5DDFD0,
      0x5DE03A, 0x5DE06B, 0x5DE09D, 0x5DE0EF, 0x5DE120, 0x5DE133, 0x5DE17A, 0x5DE18D, 0x5DE1C7, 0x5DE206,
      0x5DE23D, 0x5DE27A, 0x5DE2B9, 0x5DE2FD, 0x5DE34A, 0x5DE38F, 0x5DE3CC, 0x5DE458, 0x5DE49D, 0x5DE4DA,
      0x5DE572, 0x5DE5AB, 0x5EC789, 0x5EDE1A, 0x5EDE61, 0x5EDEAF, 0x5EDF11, 0x5EDF85, 0x5EDFA7, 0x5EE039,
      0x5EE06A, 0x5EE0A1, 0x5EE4B8, 0x5EE4ED, 0x5EE518, 0x5EE549, 0x5EE57E, 0x5EE654, 0x5EE679, 0x5EE69E
    },
    60
  },
  { // XD_ACTIVATE_SERVER_HOOK
    "Net_ActivateServer", NET_MODULE, "?XD_ActivateServer@@YAHXZ",
    (DWORD[]){ 0x59B191 },
    1
  },
  { // XD_CLOSE_CONN_HOOK
    "Net_CloseConnection", NET_MODULE, "?XD_CloseConnection@@YAHXZ",
    (DWORD[]){ 0x422636, 0x482E56, 0x59B31E },
    3
  },
  { // XD_FLUSH_SEND_BUFFER_HOOK
    "Net_FlushSendBuffer", NET_MODULE, "?XD_FlushSendBuffer@@YAHK@Z",
    (DWORD[]){
      0x41FEBD, 0x41FF8D, 0x41FF9F, 0x422477, 0x422DBA, 0x423024, 0x4230F1, 0x423967, 0x423A08, 0x423A97,
      0x423B88, 0x423C57, 0x423D9A, 0x423FC5, 0x440306, 0x441A5D, 0x47F487, 0x47F4A9, 0x47F4E8, 0x47F527,
      0x47F5A1, 0x480ABA, 0x480B29, 0x480BC2, 0x480C70, 0x480E4B, 0x480EB0, 0x480F0D, 0x480F72, 0x480FE2,
      0x48187F, 0x482712, 0x482DAB, 0x48A54F, 0x48C91E, 0x48C987, 0x48CD04, 0x48CD71, 0x48CE27, 0x48CF50,
      0x4B108D, 0x4B1286, 0x4DECF5, 0x4E2217, 0x4E7439, 0x543E57, 0x55B4E9, 0x55B58C, 0x55B8FF, 0x56005E,
      0x561FFB, 0x562903, 0x56E5EE, 0x56E918, 0x56EBD3, 0x56EC26, 0x57CC3A, 0x57E286, 0x5934AA, 0x593A2D,
      0x594AFD, 0x594F13, 0x594FF1, 0x595047, 0x59517E, 0x59B2F6, 0x5AA42D, 0x5AA49A, 0x5AA550, 0x5AA636,
      0x5AA812, 0x5AA9FD, 0x5AAE08, 0x5ABAF2, 0x5ABF2B, 0x5ABF94, 0x5B3443, 0x5B36C3, 0x5B3847, 0x5B3AC4,
      0x5B3B66, 0x5B4372, 0x5B47DE, 0x5B4878, 0x5B5B8C, 0x5B5D77, 0x5B6021, 0x5B62D9, 0x5B9F69, 0x5B9FA4,
      0x5BA055
    },
    91
  },
  { // XD_GET_CURRENT_PROTO_ADDR_HOOK
    "Net_GetCurrentProtoAddr", NET_MODULE, "?XD_GetCurrentProtoAddr@@YAPADIH@Z",
    (DWORD[]){ 0x444F2C, 0x444F48, 0x4452EC, 0x4453AB },
    4
  },
  { // XD_IN_FLUSH_SEND_BUFFER_HOOK
    "Net_InFlushSendBuffer", NET_MODULE, "?XD_InFlushSendBuffer@@YAHXZ",
    (DWORD[]){
      0x410FA1, 0x411732, 0x412659, 0x47EAD3, 0x47EB07, 0x4E280C, 0x5095A9, 0x509BAE, 0x50A509, 0x50B79B,
      0x50BAEB, 0x50C1DA
    },
    12
  },
  { // XD_INIT_MODEM_HOOK
    "Net_InitializeModem", NET_MODULE, "?XD_InitializeModem@@YAHH@Z",
    (DWORD[]){ 0x59B07F },
    1
  },
  { // XD_INIT_SERIAL_HOOK
    "Net_InitializeSerial", NET_MODULE, "?XD_InitializeSerial@@YAHH@Z",
    (DWORD[]){ 0x59B101 },
    1
  },
  { // XD_INIT_SOCKETS_IPX_SPX_HOOK
    "Net_InitializeSocketsIPXSPX", NET_MODULE, "?XD_InitializeSocketsIPXSPX@@YAHHHHHIP6AXPAXGJ@Z@Z",
    (DWORD[]){ 0x59AF90 },
    1
  },
  { // XD_INIT_SOCKETS_TCP_HOOK
    "Net_InitializeSocketsTCP", NET_MODULE, "?XD_InitializeSocketsTCP@@YAHHHHHIP6AXPAXGJ@Z@Z",
    (DWORD[]){ 0x59AE83 },
    1
  },
  { // XD_LAUNCHED_BY_LOBBY_HOOK
    "Net_LaunchedByLobby", NET_MODULE, "?XD_LaunchedByLobby@@YAHPAXPAULobbyLaunchInfo@@@Z",
    (DWORD[]){ 0x41F990 },
    1
  },
  { // XD_LOBBY_SEND_MESSAGE_HOOK
    "Net_LobbySendMessage", NET_MODULE, "?XD_LobbySendMessage@@YAHK@Z",
    (DWORD[]){ 0x41FAA1, 0x42230E, 0x422343, 0x56E4B6, 0x56E4EB },
    5
  },
  { // XD_OPEN_CONN_HOOK
    "Net_OpenConnection", NET_MODULE, "?XD_OpenConnection@@YAHPAXK@Z",
    (DWORD[]){ 0x422069, 0x422246, 0x422C78, 0x482F39 },
    4
  },
  { // XD_RESET_LIBRARY_HOOK
    "Net_ResetLibrary", NET_MODULE, "?XD_ResetLibrary@@YAHXZ",
    (DWORD[]){ 0x59B4C2, 0x59B4FA },
    2
  },
  { // XD_SEND_BROADCAST_DATA_HOOK
    "Net_SendBroadcastData", NET_MODULE, "?XD_SendBroadcastData@@YAHPAXKJ@Z",
    (DWORD[]){ 0x46B0CB},
    1
  },
  { // XD_SEND_SECURE_DATA_HOOK
    "Net_SendSecureData", NET_MODULE, "?XD_SendSecureData@@YAHGPAXKF@Z",
    (DWORD[]){ 0x46B016 },
    1
  },
  { // XD_SERVER_CLOSE_CONN_HOOK
    "Net_ServerCloseConnection", NET_MODULE, "?XD_ServerCloseConnection@@YAHG@Z",
    (DWORD[]){ 0x56E609 },
    1
  },
  { // XD_SET_BROADCAST_RECEIVE_HOOK
    "Net_SetBroadcastReceive", NET_MODULE, "?XD_SetBroadcastReceive@@YAXP6AXPAXGJ@Z@Z",
    (DWORD[]){ 0x59A90E },
    1
  },
  { // XD_SET_NEW_CLIENT_CONN_HOOK
    "Net_SetNewClientConnection", NET_MODULE, "?XD_SetNewClientConnection@@YAXP6AXGG@Z@Z",
    (DWORD[]){ 0x59A935 },
    1
  },
  { // XD_SET_ON_CLIENT_CONN_SERVER_HOOK
    "Net_SetOnClientConnectionToServer", NET_MODULE, "?XD_SetOnClientConnectionToServer@@YAXP6AXF@Z@Z",
    (DWORD[]){ 0x59A928 },
    1
  },
  { // XD_SET_ON_CONN_LOST_HOOK
    "Net_SetOnConnectionLost", NET_MODULE, "?XD_SetOnConnectionLost@@YAXP6AXG@Z@Z",
    (DWORD[]){ 0x59A942 },
    1
  },
  { // XD_SET_OVERSIZED_MESSAGE_CB_HOOK
    "Net_SetOversizedMessageCB", NET_MODULE, "?XD_SetOversizedMessageCB@@YAHKP6AXK@Z@Z",
    (DWORD[]){ 0x59A954 },
    1
  },
  { // XD_SET_SECURE_RECEIVE_HOOK
    "Net_SetSecureReceive", NET_MODULE, "?XD_SetSecureReceive@@YAXP6AXGPAXKF@Z@Z",
    (DWORD[]){ 0x59A91B },
    1
  },
  { // XD_SHUTDOWN_MODEM_HOOK
    "Net_ShutdownModem", NET_MODULE, "?XD_ShutdownModem@@YAXXZ",
    (DWORD[]){ 0x59B38A },
    1
  },
  { // XD_SHUTDOWN_SOCKETS_HOOK
    "Net_ShutdownSockets", NET_MODULE, "?XD_ShutdownSockets@@YAXXZ",
    (DWORD[]){ 0x59B35C, 0x59B373 },
    2
  },
  { // XD_SHUTDOWN_TEN_HOOK
    "Net_ShutdownTEN", NET_MODULE, "?XD_ShutdownTEN@@YAXXZ",
    (DWORD[]){ 0x59B3A1 },
    1
  },
  { // XD_STOP_CONN_HOOK
    "Net_StopConnections", NET_MODULE, "?XD_StopConnections@@YAHXZ",
    (DWORD[]){ 0x56E6CA, 0x56E983, 0x56E9E2, 0x56EA93 },
    4
  }
};

const FunctionHook *GetFunctionHook(FunctionHookEnum hookId)
{
  if (hookId < LAST_HOOK) {
    return &g_functionHooks[hookId];
  }

  return NULL;
}

FARPROC GetOriginalFunctionAddress(FunctionHookEnum hookId)
{
  if (hookId < LAST_HOOK) {
    const FunctionHook *hook = GetFunctionHook(hookId);

    HMODULE hModule = GetModuleHandle(hook->lpcsModule);
    FARPROC fpAddress = GetProcAddress(hModule, hook->lpcsFunction);

    if (!fpAddress) {
      LogError("Cannot get original function '%s' from '%s'.", hook->lpcsFunction, hook->lpcsModule);
    } else {
      return fpAddress;
    }
  }

  return NULL;
}
