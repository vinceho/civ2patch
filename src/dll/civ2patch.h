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

#ifdef __cplusplus
}
#endif

#endif // CIV2PATCH_H
