#include <catch2/catch_test_macros.hpp>
#include "GAME_RUNNER/GameRunner.h"
#include "UTIL/TestFileOperator.h"
#include "UTIL/NullGraphicsRenderer.h"
#include <nlohmann/json.hpp>

TEST_CASE("GameRunner initial state reflects constructor arguments", "[GameRunner]")
{
    TestFileOperator     fileOp;
    NullGraphicsRenderer renderer;

    SECTION("default construction")
    {
        GameRunner runner(fileOp, renderer);
        CHECK(runner.getCurrentMode()       == "locations");
        CHECK(runner.getCurrentLocationID() == "");
        CHECK(runner.getCurrentNoteID()     == "");
    }

    SECTION("custom mode and IDs")
    {
        GameRunner runner(fileOp, renderer, "notes", "/LOCATIONS/AVERY/ROOT/Avery_Full.json", "/NOTES/AVERY/Avery_Note.json");
        CHECK(runner.getCurrentMode()       == "notes");
        CHECK(runner.getCurrentLocationID() == "/LOCATIONS/AVERY/ROOT/Avery_Full.json");
        CHECK(runner.getCurrentNoteID()     == "/NOTES/AVERY/Avery_Note.json");
    }
}

TEST_CASE("GameRunner start button loads expected first location.")
{
    struct TrackingFileOperator : TestFileOperator
    {
        std::string lastLoadPath;
        std::string load(const std::string& path) override
        {
            lastLoadPath = path;
            return TestFileOperator::load(path);
        }
    };

    TrackingFileOperator fileOp;
    fileOp.diskRoot = "KSC_DATA";
    NullGraphicsRenderer renderer;

    // Derive expected target and hit coordinates from the real start screen JSON
    nlohmann::json doc = nlohmann::json::parse(fileOp.load("/BANNERS/START_SCREEN/Start_Screen.json"));
    std::string expectedTarget;
    int hitX = 0, hitY = 0;
    for (auto& zone : doc["zones"])
    {
        if (zone.value("id", "") == "start_button")
        {
            expectedTarget = zone.value("target", "");
            hitX = zone.value("x", 0) + zone.value("width",  0) / 2;
            hitY = zone.value("y", 0) + zone.value("height", 0) / 2;
            break;
        }
    }
    REQUIRE(!expectedTarget.empty());

    GameRunner runner(fileOp, renderer);
    runner.loadScene("/BANNERS/START_SCREEN/Start_Screen.json");
    runner.registerHit(hitX, hitY);

    CHECK(fileOp.lastLoadPath == expectedTarget);
}