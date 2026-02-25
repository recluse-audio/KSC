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
