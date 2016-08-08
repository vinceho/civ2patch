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
#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>

BOOL InitializeAudio(DWORD dwFreq, DWORD dwChunkSize, DWORD dwVolume, DWORD dwAlbum);
BOOL ShutdownAudio();
DWORD GetNumberMusicTracks();
MCIDEVICEID AddMciDevice(MCIDEVICEID wId);
MCIDEVICEID GetMciDevice(MCIDEVICEID wProxyId);
void RemoveMciDevice(MCIDEVICEID wProxyId);
BOOL IsMusicMciDevice(MCIDEVICEID wProxyId);
BOOL PlayMusic(DWORD dwTrack, HWND hNotifyMusicFinishedCallback);
void StopMusic();

#endif // AUDIO_H
