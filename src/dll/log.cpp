#include <stdio.h>
#include "log.h"

void InitializeLog()
{
  // Purge log file.
  fclose(fopen("civ2patch.log", "w"));
}

void Log(LPCSTR lpcsFormat, ...)
{
  FILE *file = fopen("civ2patch.log", "a");

  va_list args;
  va_start(args, lpcsFormat);
  vfprintf(file, lpcsFormat, args);
  va_end(args);

  fclose(file);
}
