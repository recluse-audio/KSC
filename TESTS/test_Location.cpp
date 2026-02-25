#include <catch2/catch_test_macros.hpp>
#include "LOCATION/Location.h"

TEST_CASE("Location getLocationID", "[Location]")
{
    SECTION("returns the ID passed at construction")
    {
        Location loc("MyLocation");
        REQUIRE(loc.getLocationID() == "MyLocation");
    }
}

TEST_CASE("Location getParentLocationID", "[Location]")
{
    SECTION("returns the parent ID passed at construction")
    {
        Location loc("MyLocation", "ParentLocation");
        REQUIRE(loc.getParentLocationID() == "ParentLocation");
    }

    SECTION("defaults to Main when no parent ID is provided")
    {
        Location loc("MyLocation");
        REQUIRE(loc.getParentLocationID() == "Main");
    }
}

TEST_CASE("Location setLocationID", "[Location]")
{
    Location loc("InitialID");

    SECTION("updates the ID")
    {
        loc.setLocationID("UpdatedID");
        REQUIRE(loc.getLocationID() == "UpdatedID");
    }
}

TEST_CASE("Location addChildLocation / getChildLocations", "[Location]")
{
    Location loc("MyLocation");

    SECTION("starts with no child locations")
    {
        REQUIRE(loc.getChildLocations().empty());
    }

    SECTION("added child location appears in list")
    {
        loc.addChildLocation("ChildA");
        REQUIRE(loc.getChildLocations().size() == 1);
        REQUIRE(loc.getChildLocations()[0] == "ChildA");
    }

    SECTION("multiple children are all returned")
    {
        loc.addChildLocation("ChildA");
        loc.addChildLocation("ChildB");
        REQUIRE(loc.getChildLocations().size() == 2);
        REQUIRE(loc.getChildLocations()[1] == "ChildB");
    }
}

TEST_CASE("Location addZone / getZones", "[Location]")
{
    Location loc("MyLocation");

    SECTION("starts with no zones")
    {
        REQUIRE(loc.getZones().empty());
    }

    SECTION("added zone appears in list")
    {
        Zone::Bounds bounds(0, 0, 100, 100);
        Zone zone(loc, bounds, "ZoneA");
        loc.addZone(zone);
        REQUIRE(loc.getZones().size() == 1);
        REQUIRE(loc.getZones()[0].getZoneID() == "ZoneA");
    }

    SECTION("multiple zones are all returned")
    {
        Zone::Bounds boundsA(0, 0, 50, 50);
        Zone::Bounds boundsB(60, 60, 50, 50);
        loc.addZone(Zone(loc, boundsA, "ZoneA"));
        loc.addZone(Zone(loc, boundsB, "ZoneB"));
        REQUIRE(loc.getZones().size() == 2);
        REQUIRE(loc.getZones()[1].getZoneID() == "ZoneB");
    }
}
