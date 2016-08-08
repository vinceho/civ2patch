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
#ifndef INJECT_H
#define INJECT_H

#include <windows.h>

BOOL WriteMemory(HANDLE hProcess, LPVOID lpvData, DWORD dwBytes, DWORD dwTargetAddress);
BOOL HookWindowsAPI(HANDLE hProcess, HMODULE hModule, LPCSTR lpcsFunctionName, DWORD dwTargetAddress);
HMODULE GetCurrentModuleHandle();
BOOL ConvertValueToByteArray(DWORD dwValue, DWORD dwSize, LPBYTE lpData);

#endif // INJECT_H
