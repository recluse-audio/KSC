#include <catch2/catch_test_macros.hpp>
#include "GAME_RUNNER/GameRunner.h"
#include "UTIL/TestFileOperator.h"
#include "UTIL/NullGraphicsRenderer.h"

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
