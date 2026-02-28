#include <catch2/catch_test_macros.hpp>
#include "GAME_RUNNER/GameStartManager.h"
#include "GAME_STATE/GameStateComparison.h"
#include "UTIL/TestFileOperator.h"
#include <algorithm>
#include <filesystem>

/**
 * These tests are intended to cover the creation of a new save slot 
 * (aka new game state folder is made in the KSC_DATA)
 * 
 * For tests of switching to the correct start location use test_GameRunner.cpp
 */
namespace fs = std::filesystem;

// Golden Game_State.json — the default file installed with the game.
static const std::string k_GoldenPath   = "/GAME_STATE/Game_State.json";
static const std::string k_CompletePath = "TESTS/GOLDEN/GAME_STATE/Game_State_Complete.json";
static const std::string k_OutputDir    = "TESTS/OUTPUT/GAME_START_MANAGER";

// Helper: set up a fresh TestFileOperator, run save(), return the saved JSON.
static std::string runSaveAndReadSlot(TestFileOperator& fileOp)
{
    fs::remove_all(k_OutputDir);
    fs::create_directories(k_OutputDir);

    fileOp.diskRoot = "KSC_DATA";

    std::string goldenJson = fileOp.load(k_GoldenPath);
    REQUIRE_FALSE(goldenJson.empty());
    fileOp.files[k_GoldenPath] = goldenJson;

    GameStartManager manager(fileOp, k_OutputDir);
    manager.save();

    return fileOp.load(k_OutputDir + "/KSC_SLOT_0/Game_State.json");
}

TEST_CASE("GameStartManager save copies golden Game_State.json into KSC_SLOT_0", "[GameStartManager]")
{
    TestFileOperator fileOp;
    std::string savedJson = runSaveAndReadSlot(fileOp);

    REQUIRE(fs::is_directory(k_OutputDir + "/KSC_SLOT_0"));
    REQUIRE_FALSE(savedJson.empty());

    std::string goldenJson = fileOp.load(k_GoldenPath);
    GameStateComparison cmp(goldenJson, savedJson);
    REQUIRE(cmp.isEqual());
}

TEST_CASE("GameStartManager save slot is not yet the complete game state", "[GameStartManager]")
{
    TestFileOperator fileOp;
    std::string savedJson   = runSaveAndReadSlot(fileOp);
    std::string completeJson = fileOp.load(k_CompletePath);

    REQUIRE_FALSE(savedJson.empty());
    REQUIRE_FALSE(completeJson.empty());

    GameStateComparison cmp(savedJson, completeJson);

    SECTION("save slot differs from complete state")
    {
        REQUIRE_FALSE(cmp.isEqual());
    }

    SECTION("no scalar differences between save slot and complete state")
    {
        auto diff = cmp.getDiff();
        REQUIRE(diff.scalars.empty());
    }

    SECTION("save slot has five undiscovered clues vs complete state")
    {
        auto diff = cmp.getDiff();
        REQUIRE(diff.discoveries.size() == 5);
    }

    SECTION("every undiscovered entry is false in save slot and true in complete state")
    {
        auto diff = cmp.getDiff();
        for (const auto& change : diff.discoveries)
        {
            CHECK(change.mapKey == "avery_locations");
            CHECK(change.before == false);
            CHECK(change.after  == true);
        }
    }
}
