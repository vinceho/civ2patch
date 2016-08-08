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
  CVT_BOOL,
  CVT_FLOAT
};

typedef struct {
  LPCSTR lpcsSection;
  LPCSTR lpcsName;
  ConfigValueType type;
  LPVOID lpvValue;
} ConfigLink;

typedef struct {
  // Options
  DWORD dwLogLevel;
  BOOL bMusic;
  BOOL bMultiplayer;
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
  DWORD dwPurgeMessagesInterval;
  DWORD dwMessageWaitTimeout;
  DWORD dwCombatAnimationLength;
  FLOAT fSleepRatio;
  DWORD dwCpuSamplingInterval;

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

  // Multiplayer
  DWORD dwNetConnectionPort;
  DWORD dwNetBroadcastPort;
  DWORD dwNetConnectionTimeout;
} Config;

BOOL InitializeConfig();
BOOL IsLogEnabled();
DWORD GetLogLevel();
BOOL IsMusicEnabled();
BOOL IsMultiplayerEnabled();
BOOL IsFixIdleCpuEnabled();
BOOL IsFix64BitEnabled();
BOOL IsNoCdCheckEnabled();
BOOL IsFixHostileAiEnabled();
BOOL IsSetRetirementYearEnabled();
BOOL IsSetCombatAnimationLengthEnabled();
BOOL IsSetPopulationLimitEnabled();
BOOL IsSetGoldLimitEnabled();
BOOL IsSetMapTilesLimitEnabled();
DWORD GetPurgeMessagesInterval();
DWORD GetMessageWaitTimeout();
DWORD GetCombatAnimationLength();
FLOAT GetSleepRatio();
DWORD GetCpuSamplingInterval();
DWORD GetRetirementYear();
DWORD GetRetirementWarningYear();
DWORD GetMapTilesLimit();
DWORD GetPopulationLimit();
DWORD GetGoldLimit();
DWORD GetMusicFrequency();
DWORD GetMusicChunkSize();
DWORD GetMusicVolume();
DWORD GetMusicAlbum();
DWORD GetNetConnectionPort();
DWORD GetNetBroadcastPort();
DWORD GetNetConnectionTimeout();

#endif // CONFIG_H
