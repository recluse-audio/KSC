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
    LOCATION/           # Location class - navigation and zone lookup
    ZONE/               # Zone class - hit area with bounds
    CLUES/              # Clue tracking
    GAME_STATE/         # Global game state
TESTS/
    test_Zone.cpp       # Zone hit detection tests
```

## Architecture

### Core Classes

- **Location** (`LOCATION/Location.h`) - Represents a navigable area. Holds child Zones and resolves point-in-zone queries via `getInterceptingZoneID(x, y)`.
- **Zone** (`ZONE/Zone.h`) - A rectangular hit area within a Location. `containsPoint(x, y)` checks whether a screen coordinate falls inside.
- **Zone::Bounds** - Inner class holding `mX`, `mY`, `mW`, `mH` as integers.

### Design Philosophy

File-first: defer loading assets and data from files at runtime rather than compiling them in. Keep shared source free of platform dependencies so it can be tested on desktop and run on ESP32.

## Testing

Tests use Catch2 v3.1.0 (fetched automatically by CMake via FetchContent).

Test naming convention: `test_<ClassName>.cpp`

Build output goes to `BUILD/` (not tracked in git).
