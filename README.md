This repository is no longer in development. Please use [Civilization II User Interface Additions](https://github.com/FoxAhead/Civ2-UI-Additions) by @FoxAhead instead.

# Civilization 2 Multiplayer Gold Edition Patch

## Synopsis

The project modifies Civilization 2 Multiplyer Gold Edition to include additional features and fixes.

## Requirement

Only the unmodified Civilization 2 Multiplyer Gold Edition v1.3 US version is supported.

## Installation

Extract all of the files into the folder where Civilization 2 Multiplyer Gold Edition is installed. Launch `civ2patch.exe`.

## Configuring

When running for the very first time, a configuration file `civ2patch.ini` will be created.

### Options

All of the fields in the `Options` section are feature toggles. To enable a feature, set the value to `1`. To disable a feature, set the value to `0`.

* `Log` When enabled, error messages will be written to the `civ2patch.log` file.
* `Music` When enabled, the application will look in the `Music` folder and play MP3 or OGG tracks. The name of the files must be in the format of `Track##.mp3` or `Track##.ogg`, where `##` is the 2 digits track number.
* `Multiplayer` When enabled, SDL will be used for networking instead of Winsock.
* `FixCpuUsage` When enabled, reduce CPU usage when the application is idle.
* `Fix64BitCompatibility` When enabled, patch the application to run on 64-bit Windows.
* `DisableCdCheck` When enabled, allow the application to run without the game CD.
* `DisableHostileAi` When enabled, AI will not be unreasonably hostile to the player.
* `SetRetirementYear` When enabled, allow the retirement year to be modified.
* `SetCombatAnimationFrameLength` When enabled, allow the combat animation speed to be modified.
* `SetPopulationLimit` When enabled, allow the population limit to be modified.
* `SetGoldLimit` When enabled, allow the gold limit to be modified.
* `SetMapTilesLimit` When enabled, allow the number of map tiles limit to be modified.

### Limits

When the associated feature has been enabled, these fields allow the user to set the behaviour of the features.

* `RetirementYear` The year at which the game will force the user to retire. (Min: `1`, Max: `9999`)
* `RetirementWarningYear` The year at which the game will warn the user of inevitable retirement. (Min: `0`, Max: `RetirementYear - 1`)
* `MapTilesLimit` The maximum number of map tiles when generating a new map. (Min: `10000`, Max: `32767`)
* `PopulationLimit` The maximum population in 10000. (Min: `32000`, Max: `2147483647`)
* `GoldLimit` The maximum amount of gold. (Min: `30000`, Max: `2147483647`)

### Advanced

These fields should not be changed. Refer to the source code to see what they do.

* `PurgeMessagesInterval` The time interval in milliseconds at which the Windows message queue is purged.
* `MessageWaitTimeout` The time to wait for user inputs when the application idles.
* `CombatAnimationLength` Reduce this number to speed up combat animation.
* `ProcessTimeToSleepTimeRatio` The ratio between process time to sleep time. The lower the number, the less CPU cycles are used. However, low value may cause the game to run slowly.
* `CpuSamplingInterval` The time interval in milliseconds to recalculate the process time to sleep time ratio.

### Music

These fields configures the music player when the feature is enabled.

* `Frequency` The audio sampling frequency.
* `ChunkSize` The audio buffer size in bytes.
* `Volume` The music volume.
* `Album` The soundtrack to use. This determines the number of tracks the game will recognize. (`0` for MGE, `1` for Classic, `2` for Conflicts in Civilization, `3` for Fantastic Worlds)

### Multiplayer

These fields configures network connections when the feature is enabled.

* `ConnectionPort` The TCP port for connecting to the server/client. (Min: `1024`, Max: `65535`)
* `BroadcastPort` The UDP port for broadcasting messages over LAN. (Min: `1024`, Max: `65535`)
* `ConnectionTimeout` The time in milliseconds to wait for connection before disconnecting. (Min: `1000`, Max: `60000`)

## Building

This project uses the following tools and libraries:

* MinGW (http://www.mingw.org)
* SDL 2.0 (https://www.libsdl.org)
* SDL_mixer 2.0 (https://www.libsdl.org/projects/SDL_mixer/)

Once all the software has been installed, run the Makefile to build the project.

`mingw32-make`
