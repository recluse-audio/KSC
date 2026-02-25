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

TEST_CASE("Location setLocationID", "[Location]")
{
    Location loc("InitialID");

    SECTION("updates the ID")
    {
        loc.setLocationID("UpdatedID");
        REQUIRE(loc.getLocationID() == "UpdatedID");
    }
}
