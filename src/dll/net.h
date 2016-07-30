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
#ifndef NET_H
#define NET_H

#include <windows.h>

typedef void (*InitializeSocketsCallback)(LPVOID, WORD, LONG);
typedef void (*BroadcastReceiveCallback)(LPVOID, WORD, LONG);
typedef void (*NewClientConnectionCallback)(WORD, WORD);
typedef void (*ClientConnectionToServerCallback)(SHORT);
typedef void (*ConnectionLostCallback)(WORD);
typedef void (*OversizedMessageCallback)(DWORD);
typedef void (*SecureReceiveCallback)(WORD, LPVOID, DWORD, SHORT);

void SetInitializeSocketsCallback(InitializeSocketsCallback callback);
void SetBroadcastReceiveCallback(BroadcastReceiveCallback callback);
void SetNewClientConnectionCallback(NewClientConnectionCallback callback);
void SetClientConnectionToServerCallback(ClientConnectionToServerCallback callback);
void SetConnectionLostCallback(ConnectionLostCallback callback);
void SetOversizedMessageCallback(OversizedMessageCallback callback);
void SetSecureReceiveCallback(SecureReceiveCallback callback);

void OnInitializeSockets(LPVOID, WORD, LONG);
void OnBroadcastReceive(LPVOID, WORD, LONG);
void OnNewClientConnection(WORD, WORD);
void OnClientConnectionToServer(SHORT);
void OnConnectionLost(WORD);
void OnOversizedMessage(DWORD);
void OnSecureReceive(WORD, LPVOID, DWORD, SHORT);

#endif // NET_H
