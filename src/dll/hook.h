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
#ifndef HOOK_H
#define HOOK_H

#include <windows.h>

#define WINMM_MODULE  "Winmm.dll"
#define USER32_MODULE "User32.dll"
#define NET_MODULE    "XDaemon.dll"

typedef enum {
  PEEK_MESSAGE_HOOK = 0,
  MCI_SEND_CMD_HOOK,
  XD_ACTIVATE_SERVER_HOOK,
  XD_CLOSE_CONN_HOOK,
  XD_FLUSH_SEND_BUFFER_HOOK,
  XD_GET_CURRENT_PROTO_ADDR_HOOK,
  XD_IN_FLUSH_SEND_BUFFER_HOOK,
  XD_INIT_MODEM_HOOK,
  XD_INIT_SERIAL_HOOK,
  XD_INIT_SOCKETS_IPX_SPX_HOOK,
  XD_INIT_SOCKETS_TCP_HOOK,
  XD_LAUNCHED_BY_LOBBY_HOOK,
  XD_LOBBY_SEND_MESSAGE_HOOK,
  XD_OPEN_CONN_HOOK,
  XD_RESET_LIBRARY_HOOK,
  XD_SEND_BROADCAST_DATA_HOOK,
  XD_SEND_SECURE_DATA_HOOK,
  XD_SERVER_CLOSE_CONN_HOOK,
  XD_SET_BROADCAST_RECEIVE_HOOK,
  XD_SET_NEW_CLIENT_CONN_HOOK,
  XD_SET_ON_CLIENT_CONN_SERVER_HOOK,
  XD_SET_ON_CONN_LOST_HOOK,
  XD_SET_OVERSIZED_MESSAGE_CB_HOOK,
  XD_SET_SECURE_RECEIVE_HOOK,
  XD_SHUTDOWN_MODEM_HOOK,
  XD_SHUTDOWN_SOCKETS_HOOK,
  XD_SHUTDOWN_TEN_HOOK,
  XD_STOP_CONN_HOOK,
  LAST_HOOK
} FunctionHookEnum;

typedef struct {
  LPCSTR lpcsNewFunction;
  LPCSTR lpcsModule;
  LPCSTR lpcsFunction;
  LPDWORD addresses;
  DWORD dwNumAddress;
} FunctionHook;

const FunctionHook *GetFunctionHook(FunctionHookEnum hookId);
FARPROC GetOriginalFunctionAddress(FunctionHookEnum hookId);

#endif // HOOK_H
