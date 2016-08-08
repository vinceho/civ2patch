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
#include <math.h>
#include "timer.h"
#include "log.h"

DOUBLE g_dTimerFrequency;
DOUBLE g_dTimerStart;

BOOL g_bTimerHighResolution = FALSE;

BOOL InitializeTimer()
{
  LARGE_INTEGER timerFrequency;

  if (!QueryPerformanceFrequency(&timerFrequency)) {
    LogDebug("High precision timer not supported.");

    g_dTimerStart = round((DOUBLE)timeGetTime() * 1000.0);
  } else {
    LARGE_INTEGER timerCounter;

    g_bTimerHighResolution = TRUE;
    g_dTimerFrequency = (DOUBLE)timerFrequency.QuadPart / 1000000.0;
    QueryPerformanceCounter(&timerCounter);

    g_dTimerStart = (DOUBLE)timerCounter.QuadPart / g_dTimerFrequency;
  }

  return TRUE;
}

/**
 * Get current time.
 *
 * @return Time in microseconds.
 */
DOUBLE GetTimerCurrentTime()
{
  DOUBLE dNow = 0.0;

  if (!g_bTimerHighResolution) {
    timeBeginPeriod(1);
    dNow = round((DOUBLE)timeGetTime() * 1000.0);
    timeEndPeriod(1);
  } else {
    LARGE_INTEGER timerCounter;

    QueryPerformanceCounter(&timerCounter);
    dNow = (DOUBLE)timerCounter.QuadPart / g_dTimerFrequency;
  }

  // Overflow check.
  if (g_dTimerStart > dNow) {
    g_dTimerStart = dNow;
  }

  return round(dNow - g_dTimerStart);
}
