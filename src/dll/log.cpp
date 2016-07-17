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
#include "log.h"
#include "constants.h"
#include "config.h"

void InitializeLog()
{
  if (g_config.bLog) {
    // Purge log file.
    fclose(fopen(LOG_FILE, "w"));
  }
}

void Log(LPCSTR lpcsFormat, ...)
{
  if (g_config.bLog) {
    FILE *file = fopen(LOG_FILE, "a");

    va_list args;
    va_start(args, lpcsFormat);
    vfprintf(file, lpcsFormat, args);
    va_end(args);

    fclose(file);
  }
}
