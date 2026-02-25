/**
 * Made by Ryan Devens on 2026-02-24
 */

#pragma once
#include "../CLUES/Clues.h"
#include "../LOCATION/Location.h"
/**
 * 
 */
class GameState
{
public:

    const Clues& getClues() { return mClues; }
    const Location& getLocation() { return mCurrentLocation; }
private:
    Clues mClues;
    Location mCurrentLocation;
};