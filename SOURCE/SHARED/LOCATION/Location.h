/**
 * Made by Ryan Devens on 2026-02-24
 */

#pragma once
#include <string>
#include <vector>
#include "../ZONE/Zone.h"

/**
 * This class defines the majority of the gameplay.
 */
class Location
{
public:
    Location(std::string locationID, std::string parentID = "Main");

    std::string getLocationID();
    void setLocationID(std::string locationID);
    std::string getParentLocationID();
    std::string getInterceptingZoneID(int x, int y);

private:
    std::string mLocationID = "";
    std::string mParentLocationID = "";
    std::vector<std::string> mChildLocations;
    std::vector<Zone> mZones; // zones of this location that trigger clicks
};
