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
#include <stdio.h>
#include <SDL2/sdl_mixer.h>
#include "config.h"
#include "constants.h"
#include "log.h"

Config g_config = {
  // Options
  FALSE, FALSE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE,
  // Advanced
  3000, 1, 8, 0.5f, 1000,
  // Limits
  9999, 9979, 32767, 200000, 2000000000,
  // Music
  22050, 8192, 64, 0
};

ConfigLink configLinks[] = {
  { "Options", "Log", CVT_BOOL, &g_config.bLog },
  { "Options", "Music", CVT_BOOL, &g_config.bMusic },
  { "Options", "FixCpuUsage", CVT_BOOL, &g_config.bFixCpu },
  { "Options", "Fix64BitCompatibility", CVT_BOOL, &g_config.bFix64BitCompatibility },
  { "Options", "DisableCdCheck", CVT_BOOL, &g_config.bNoCdCheck },
  { "Options", "DisableHostileAi", CVT_BOOL, &g_config.bFixHostileAi },
  { "Options", "SetRetirementYear", CVT_BOOL, &g_config.bSetRetirementYear },
  { "Options", "SetCombatAnimationFrameLength", CVT_BOOL, &g_config.bSetCombatAnimationLength },
  { "Options", "SetPopulationLimit", CVT_BOOL, &g_config.bSetPopulationLimit },
  { "Options", "SetGoldLimit", CVT_BOOL, &g_config.bSetGoldLimit },
  { "Options", "SetMapTilesLimit", CVT_BOOL, &g_config.bSetMapTilesLimit },

  { "Limits", "RetirementYear", CVT_NUMBER, &g_config.dwRetirementYear },
  { "Limits", "RetirementWarningYear", CVT_NUMBER, &g_config.dwRetirementWarningYear },
  { "Limits", "MapTilesLimit", CVT_NUMBER, &g_config.dwMapTilesLimit },
  { "Limits", "PopulationLimit", CVT_NUMBER, &g_config.dwPopulationLimit },
  { "Limits", "GoldLimit", CVT_NUMBER, &g_config.dwGoldLimit },

  { "Advanced", "PurgeMessagesInterval", CVT_NUMBER, &g_config.dwPurgeMessagesInterval },
  { "Advanced", "MessageWaitTimeout", CVT_NUMBER, &g_config.dwMessageWaitTimeout },
  { "Advanced", "CombatAnimationLength", CVT_NUMBER, &g_config.dwCombatAnimationLength },
  { "Advanced", "ProcessTimeToSleepTimeRatio", CVT_FLOAT, &g_config.fSleepRatio },
  { "Advanced", "CpuSamplingInterval", CVT_NUMBER, &g_config.dwCpuSamplingInterval },

  { "Music", "Frequency", CVT_NUMBER, &g_config.dwMusicFreq },
  { "Music", "ChunkSize", CVT_NUMBER, &g_config.dwMusicChunkSize },
  { "Music", "Volume", CVT_NUMBER, &g_config.dwMusicVolume },
  { "Music", "Album", CVT_NUMBER, &g_config.dwMusicAlbum }
};

BOOL HasConfig();
BOOL ReadConfig();
BOOL WriteConfig();

BOOL InitializeConfig()
{
  if (HasConfig()) {
    return ReadConfig();
  }

  return WriteConfig();
}

DWORD GetMinMax(DWORD dwValue, DWORD dwMin, DWORD dwMax)
{
  if (dwValue < dwMin) {
    return dwMin;
  } else if (dwValue > dwMax) {
    return dwMax;
  }

  return dwValue;
}

FLOAT GetMinMaxFloat(FLOAT fValue, FLOAT fMin, FLOAT fMax)
{
  if (fValue < fMin) {
    return fMin;
  } else if (fValue > fMax) {
    return fMax;
  }

  return fValue;
}

BOOL ValidateConfig()
{
  // Ensure boolean non-zero value is always TRUE.
  g_config.bLog = (g_config.bLog) ? TRUE : FALSE;
  g_config.bMusic = (g_config.bMusic) ? TRUE : FALSE;;
  g_config.bFixCpu = (g_config.bFixCpu) ? TRUE : FALSE;;
  g_config.bFix64BitCompatibility = (g_config.bFix64BitCompatibility) ? TRUE : FALSE;;
  g_config.bNoCdCheck = (g_config.bNoCdCheck) ? TRUE : FALSE;;
  g_config.bFixHostileAi = (g_config.bFixHostileAi) ? TRUE : FALSE;;
  g_config.bSetRetirementYear = (g_config.bSetRetirementYear) ? TRUE : FALSE;;
  g_config.bSetCombatAnimationLength = (g_config.bSetCombatAnimationLength) ? TRUE : FALSE;;
  g_config.bSetPopulationLimit = (g_config.bSetPopulationLimit) ? TRUE : FALSE;;
  g_config.bSetGoldLimit = (g_config.bSetGoldLimit) ? TRUE : FALSE;;
  g_config.bSetMapTilesLimit = (g_config.bSetMapTilesLimit) ? TRUE : FALSE;;

  g_config.dwPurgeMessagesInterval = GetMinMax(1000, g_config.dwPurgeMessagesInterval, 5000);
  g_config.dwMessageWaitTimeout = GetMinMax(1, g_config.dwMessageWaitTimeout, 100);
  g_config.dwRetirementYear = GetMinMax(1, g_config.dwRetirementYear, 9999);
  g_config.dwRetirementWarningYear = GetMinMax(0, g_config.dwRetirementWarningYear, g_config.dwRetirementYear - 1);
  g_config.dwMapTilesLimit = GetMinMax(10000, g_config.dwMapTilesLimit, 32767);
  g_config.dwPopulationLimit = GetMinMax(32000, g_config.dwPopulationLimit, 2147483647);
  g_config.dwGoldLimit = GetMinMax(30000, g_config.dwGoldLimit, 2147483647);
  g_config.dwCombatAnimationLength = GetMinMax(0, g_config.dwCombatAnimationLength, 64);
  g_config.fSleepRatio = GetMinMaxFloat(0.1f, g_config.fSleepRatio, 10.0f);
  g_config.dwCpuSamplingInterval = GetMinMax(1000, g_config.dwCpuSamplingInterval, 60000);

  // Music
  g_config.dwMusicFreq = GetMinMax(11025, g_config.dwMusicFreq, 44100);
  g_config.dwMusicChunkSize = GetMinMax(4096, g_config.dwMusicChunkSize, 1048576);
  g_config.dwMusicVolume = GetMinMax(0, g_config.dwMusicVolume, MIX_MAX_VOLUME);
  g_config.dwMusicAlbum = GetMinMax(0, g_config.dwMusicAlbum, 3);

  return TRUE;
}

BOOL HasConfig()
{
  FILE *file = file = fopen(CONFIG_FILE, "r");

  if (file) {
    fclose(file);

    return TRUE;
  }

  return FALSE;
}

BOOL ReadConfig()
{
  DWORD dwSize = (sizeof(configLinks) / sizeof(ConfigLink));
  FILE *file = fopen(CONFIG_FILE, "r");
  DWORD dwBufferSize = 64;
  char buffer[dwBufferSize], name[dwBufferSize], value[dwBufferSize];

  // Ignore INI sections when reading for simplicity.
  if (file) {
    while (TRUE) {
      if (fgets(buffer, dwBufferSize, file) == NULL) {
        break;
      }

      if (sscanf(buffer, " %[^= ] = %s ", name, value) == 2) {
        for (DWORD i = 0; i < dwSize; i++) {
          LPCSTR lpcsName = configLinks[i].lpcsName;

          if (!strcmp(lpcsName, name)) {
            switch (configLinks[i].type) {
              case CVT_BOOL:
                *(BOOL *)(configLinks[i].lpvValue) = (BOOL)atoi(value);
                break;
              case CVT_NUMBER:
                *(DWORD *)(configLinks[i].lpvValue) = (DWORD)atoi(value);
                break;
              case CVT_FLOAT:
                *(FLOAT *)(configLinks[i].lpvValue) = (FLOAT)atof(value);
                break;
            }

            break;
          }
        }
      }

      if(feof(file)) {
        break;
      }
    }

    fclose(file);
  }

  return ValidateConfig();
}

BOOL WriteConfig()
{
  DWORD dwSize = (sizeof(configLinks) / sizeof(ConfigLink));
  FILE *file = fopen(CONFIG_FILE, "w");
  LPCSTR lpcsLastSection = NULL;

  for (DWORD i = 0; i < dwSize; i++) {
    LPCSTR lpcsCurrentSection = configLinks[i].lpcsSection;

    if (lpcsLastSection == NULL || strcmp(lpcsLastSection, lpcsCurrentSection)) {
      if (lpcsLastSection != NULL) {
        fprintf(file, "\n");
      }

      fprintf(file, "[%s]\n", lpcsCurrentSection);
    }

    lpcsLastSection = lpcsCurrentSection;

    switch (configLinks[i].type) {
      case CVT_BOOL:
      case CVT_NUMBER:
        fprintf(file, "%s=%d\n", configLinks[i].lpcsName, *(DWORD *)(configLinks[i].lpvValue));
        break;
      case CVT_FLOAT:
        fprintf(file, "%s=%f\n", configLinks[i].lpcsName, *(FLOAT *)(configLinks[i].lpvValue));
        break;
    }
  }

  fclose(file);

  return TRUE;
}

/**
 * Configuration getters.
 */
BOOL IsLogEnabled()
{
  return g_config.bLog;
}

BOOL IsMusicEnabled()
{
  return g_config.bMusic;
}

BOOL IsMultiplayerEnabled()
{
  return TRUE;
}

BOOL IsFixIdleCpuEnabled()
{
  return g_config.bFixCpu;
}

BOOL IsFix64BitEnabled()
{
  return g_config.bFix64BitCompatibility;
}

BOOL IsNoCdCheckEnabled()
{
  return g_config.bNoCdCheck;
}

BOOL IsFixHostileAiEnabled()
{
  return g_config.bFixHostileAi;
}

BOOL IsSetRetirementYearEnabled()
{
  return g_config.bSetRetirementYear;
}

BOOL IsSetCombatAnimationLengthEnabled()
{
  return g_config.bSetCombatAnimationLength;
}

BOOL IsSetPopulationLimitEnabled()
{
  return g_config.bSetPopulationLimit;
}

BOOL IsSetGoldLimitEnabled()
{
  return g_config.bSetGoldLimit;
}

BOOL IsSetMapTilesLimitEnabled()
{
  return g_config.bSetMapTilesLimit;
}

DWORD GetPurgeMessagesInterval()
{
  return g_config.dwPurgeMessagesInterval;
}

DWORD GetMessageWaitTimeout()
{
  return g_config.dwMessageWaitTimeout;
}

DWORD GetCombatAnimationLength()
{
  return g_config.dwCombatAnimationLength;
}

FLOAT GetSleepRatio()
{
  return g_config.fSleepRatio;
}

DWORD GetCpuSamplingInterval()
{
  return g_config.dwCpuSamplingInterval;
}

DWORD GetRetirementYear()
{
  return g_config.dwRetirementYear;
}

DWORD GetRetirementWarningYear()
{
  return g_config.dwRetirementWarningYear;
}

DWORD GetMapTilesLimit()
{
  return g_config.dwMapTilesLimit;
}

DWORD GetPopulationLimit()
{
  return g_config.dwPopulationLimit;
}

DWORD GetGoldLimit()
{
  return g_config.dwGoldLimit;
}

DWORD GetMusicFrequency()
{
  return g_config.dwMusicFreq;
}

DWORD GetMusicChunkSize()
{
  return g_config.dwMusicChunkSize;
}

DWORD GetMusicVolume()
{
  return g_config.dwMusicVolume;
}

DWORD GetMusicAlbum()
{
  return g_config.dwMusicAlbum;
}
