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
#ifndef GAME_H
#define GAME_H

#include <windows.h>

BOOL PatchIdleCpu(HANDLE hProcess);
BOOL PatchMapTilesLimit(HANDLE hProcess);
BOOL PatchHostileAi(HANDLE hProcess);
BOOL PatchCdCheck(HANDLE hProcess);
BOOL Patch64BitCompatibility(HANDLE hProcess);
BOOL PatchTimeLimit(HANDLE hProcess);
BOOL PatchPopulationLimit(HANDLE hProcess);
BOOL PatchGoldLimit(HANDLE hProcess);
BOOL PatchMediaPlayback(HANDLE hProcess);
BOOL PatchFastCombat(HANDLE hProcess);

#endif // GAME_H
