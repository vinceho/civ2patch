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
