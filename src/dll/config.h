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
#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>

enum ConfigValueType {
  CVT_NUMBER,
  CVT_BOOL
};

typedef struct {
  LPCSTR lpcsSection;
  LPCSTR lpcsName;
  ConfigValueType type;
  LPVOID lpvValue;
} ConfigLink;


typedef struct {
  // Options
  BOOL bLog;
  BOOL bMusic;
  BOOL bFixCpu;
  BOOL bFix64BitCompatibility;
  BOOL bNoCdCheck;
  BOOL bFixHostileAi;
  BOOL bSetRetirementYear;
  BOOL bSetCombatAnimationLength;
  BOOL bSetPopulationLimit;
  BOOL bSetGoldLimit;
  BOOL bSetMapTilesLimit;

  // Advanced
  DWORD dwMessageStatusInterval;
  DWORD dwMessageWaitTimeout;
  DWORD dwCombatAnimationLength;

  // Limits
  DWORD dwRetirementYear;
  DWORD dwRetirementWarningYear;
  DWORD dwMapTilesLimit;
  DWORD dwPopulationLimit;
  DWORD dwGoldLimit;

  // Music
  DWORD dwMusicFreq;
  DWORD dwMusicChunkSize;
  DWORD dwMusicVolume;
  DWORD dwMusicAlbum; // MGE, Classic, CIC, FW
} Config;

extern Config g_config;

BOOL InitializeConfig();

#endif // CONFIG_H
