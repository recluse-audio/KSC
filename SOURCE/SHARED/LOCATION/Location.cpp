#include "Location.h"

Location::Location(std::string locationID, std::string parentID)
: mLocationID(locationID)
, mParentLocationID(parentID)
{
}

std::string Location::getLocationID()
{
    return mLocationID;
}

void Location::setLocationID(std::string locationID)
{
    mLocationID = locationID;
}

std::string Location::getParentLocationID()
{
    return mParentLocationID;
}

void Location::addChildLocation(std::string childLocation)
{
    mChildLocations.push_back(childLocation);
}

void Location::addZone(Zone zone)
{
    mZones.push_back(zone);
}

std::vector<std::string> Location::getChildLocations()
{
    return mChildLocations;
}

std::vector<Zone> Location::getZones()
{
    return mZones;
}

std::string Location::getInterceptingZoneID(int x, int y)
{
    std::string interceptingZoneID = "";
    for(auto zone : mZones)
    {
        bool isInsideZone = zone.containsPoint(x, y);
        if(isInsideZone)
        {
            interceptingZoneID = zone.getZoneID();
        }
    }
    return interceptingZoneID;
}
