# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

KSC (Kasota Stone Conspiracy) is a narrative mystery game implemented as a lo-fi application targeting an ESP32 with an integrated TFT display. Game logic lives in `SOURCE/SHARED/` as portable C++17 and is tested on desktop via CMake + Catch2.

## Build System

### Common Build Commands

Build scripts are in `SCRIPTS/`.

```bash
# Build and compile tests
python SCRIPTS/build_tests.py

# Run tests after building
cd BUILD
ctest
# or for verbose output:
./Tests
```

### Adding New Files

1. Add files to `SOURCE/SHARED/` or `TESTS/`
2. Update `CMAKE/SOURCES.cmake` or `CMAKE/TESTS.cmake` manually
3. Rebuild

## Project Structure

```
CMAKE/
    SOURCES.cmake       # KSC source file list
    TESTS.cmake         # Test file list
SCRIPTS/
    build_tests.py      # Configure and build Tests target
SOURCE/SHARED/          # Portable game logic (C++17, no platform dependencies)
    SCENE/              # Scene class - navigation and zone lookup
    ZONE/               # Zone class - hit area with bounds
    NOTES/              # Note tracking
    GAME_STATE/         # Global game state
    GAME_RUNNER/        # Abstract GameRunner - platform subclasses implement draw methods
TESTS/
    test_Zone.cpp       # Zone hit detection tests
    test_Scene.cpp      # Scene tests
SD/
    Game_State.json     # Runtime state: currentMode, currentLocation, currentNote
    LOCATIONS/          # Location scene data (JSON + images)
    NOTES/              # Note scene data (JSON + images)
```

## Architecture

### Core Classes

- **Scene** (`SCENE/Scene.h`) - Represents a navigable area (location or note). Holds child Zones and resolves point-in-zone queries via `getInterceptingZoneID(x, y)`. Default-constructible.
- **Zone** (`ZONE/Zone.h` / `Zone.cpp`) - A rectangular hit area within a Scene. Stores `mSceneID` (string, not reference) for full copyability. Constructor takes `Scene&` to extract the ID.
- **Zone::Bounds** - Inner class holding `mX`, `mY`, `mW`, `mH` as integers.
- **GameRunner** (`GAME_RUNNER/GameRunner.h`) - Abstract controller. `draw()` is a non-virtual template method that calls pure-virtual `drawImage`, `drawText`, `drawOverlay` based on `currentMode`. `registerHit(x, y)` is non-virtual; called by platform input callbacks.

### Data Model: Clues

Clues are JSON objects nested inside location JSON files under a `clues` array. They are **not** standalone scene files — they are discovered items within a location.

```json
{
  "id": "UNIQUE_CLUE_ID",
  "name": "Human-readable clue name",
  "description": "What the player observes when they find this.",
  "isDiscovered": false,
  "unlocksNote": "NOTE_ID"
}
```

- `isDiscovered` defaults to `false`; set to `true` when the player finds the clue
- `unlocksNote` is the `id` of the Note in `SD/NOTES/` that this clue reveals

### Zone Target Paths

The `target` field on a zone is an exact path with SD treated as root:

```json
{ "target": "/BANNERS/START_SCREEN/Start_Screen.json" }
{ "target": "/LOCATIONS/DESK/MAIN/Desk_Full.json" }
{ "target": "/NOTES/AVERYS_LOGIN_HISTORY/Averys_Login_History.json" }
```

`GameRunner::registerHit` passes `target` directly to `loadScene` — no mapping or ID resolution occurs.

### Design Philosophy

**Memory model:** on scene change, the incoming scene's JSON is loaded and held in memory. Images and text files are read from SD individually on demand — not pre-loaded or cached. Only one scene JSON resides in memory at a time.

Keep shared source free of platform dependencies so it can be tested on desktop and run on ESP32.

## Testing

Tests use Catch2 v3.1.0 (fetched automatically by CMake via FetchContent).

Test naming convention: `test_<ClassName>.cpp`

Build output goes to `BUILD/` (not tracked in git).
