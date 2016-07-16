#ifndef LOG_H
#define LOG_H

#include <windows.h>

void InitializeLog();
void Log(LPCSTR lpcsFormat, ...);

#endif // LOG_H
