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
