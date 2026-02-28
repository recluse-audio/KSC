#include <catch2/catch_test_macros.hpp>
#include "GAME_STATE/GameStateComparison.h"
#include "UTIL/TestFileOperator.h"
#include <algorithm>

// Loads Game_State.json (runtime) vs Game_State_Complete.json (golden).
// The complete state has every clue discovered (true); the runtime state
// still has five clues undiscovered (false).

static const std::vector<std::string> k_ExpectedUndiscovered = {
    "/LOCATIONS/AVERY/CABLE_CABINET/DRAWER/DRAWER_1/POWERFUL_LAZER_CLUE/Powerful_Lazer_Clue.json",
    "/LOCATIONS/AVERY/CABLE_CABINET/DRAWER/DRAWER_2/BLUETOOTH_LAVMIC_CLUE/Bluetooth_LavMic_Clue.json",
    "/LOCATIONS/AVERY/DESK/BOOKS/DSP_CLUE/DSP_Clue.json",
    "/LOCATIONS/AVERY/DESK/BOOKS/INFINITE_JEST/PASSWORD_CLUE/PASSWORD_CLUE.json",
    "/LOCATIONS/AVERY/DESK/BOOKS/SQL_CLUE/SQL_CLUE.json",
};

TEST_CASE("GameStateComparison detects undiscovered clues vs complete golden state",
          "[GameStateComparison]")
{
    TestFileOperator fileOp;
    fileOp.diskRoot = "KSC_DATA";

    std::string currentJson  = fileOp.load("/GAME_STATE/Game_State.json");
    std::string completeJson = fileOp.load("TESTS/GOLDEN/GAME_STATE/Game_State_Complete.json");

    REQUIRE_FALSE(currentJson.empty());
    REQUIRE_FALSE(completeJson.empty());

    GameStateComparison cmp(currentJson, completeJson);

    SECTION("files are not identical")
    {
        REQUIRE_FALSE(cmp.isEqual());
    }

    SECTION("no scalar field differences")
    {
        auto diff = cmp.getDiff();
        REQUIRE(diff.scalars.empty());
    }

    SECTION("exactly five undiscovered clues")
    {
        auto diff = cmp.getDiff();
        REQUIRE(diff.discoveries.size() == k_ExpectedUndiscovered.size());
    }

    SECTION("every discovery change is in avery_locations, false -> true")
    {
        auto diff = cmp.getDiff();
        for (const auto& change : diff.discoveries)
        {
            CHECK(change.mapKey == "avery_locations");
            CHECK(change.before == false);
            CHECK(change.after  == true);
        }
    }

    SECTION("each expected undiscovered path is reported")
    {
        auto diff = cmp.getDiff();
        for (const auto& expectedPath : k_ExpectedUndiscovered)
        {
            auto it = std::find_if(
                diff.discoveries.begin(), diff.discoveries.end(),
                [&](const GameStateComparison::DiscoveryChange& d)
                {
                    return d.path == expectedPath;
                });
            INFO("Missing diff entry for: " << expectedPath);
            CHECK(it != diff.discoveries.end());
        }
    }
}
