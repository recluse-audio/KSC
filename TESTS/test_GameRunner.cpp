#include <catch2/catch_test_macros.hpp>
#include "GAME_RUNNER/GameRunner.h"
#include "GAME_STATE/GameStateComparison.h"
#include "UTIL/TestFileOperator.h"
#include "UTIL/NullGraphicsRenderer.h"
#include <nlohmann/json.hpp>
#include <filesystem>

namespace fs = std::filesystem;

static const std::string k_GoldenPath      = "/GAME_STATE/Game_State.json";
static const std::string k_AveryNotePath   = "/GAME_STATE/NOTES_STATE/AVERY/Avery_Note.md";
static const std::string k_LibraryNotePath = "/GAME_STATE/NOTES_STATE/LIBRARY/Library_Note.md";
static const std::string k_OutputDir       = "TESTS/OUTPUT/GAME_RUNNER/KSC_DATA";

// Clears the output dir once per test run so slots accumulate cleanly.
static bool s_outputReady = false;
static void prepareOutputDir()
{
    if (!s_outputReady)
    {
        fs::remove_all(k_OutputDir);
        s_outputReady = true;
    }
    fs::create_directories(k_OutputDir);
}

// Pre-loads Game_State + NOTES_STATE files into the in-memory map so
// GameStartManager::save() can list and copy them.
static void loadGameStateFiles(TestFileOperator& fileOp)
{
    for (const std::string& path : { k_GoldenPath, k_AveryNotePath, k_LibraryNotePath })
    {
        std::string content = fileOp.load(path);
        REQUIRE_FALSE(content.empty());
        fileOp.files[path] = content;
    }
}

// Runs a full start-game sequence and returns the slot directory written.
static std::string runStart(TestFileOperator& fileOp)
{
    prepareOutputDir();
    fileOp.diskRoot = "KSC_DATA";
    loadGameStateFiles(fileOp);

    // Pre-compute the slot index that GameStartManager will choose.
    int nextSlot = 0;
    while (fs::exists(k_OutputDir + "/KSC_SLOT_" + std::to_string(nextSlot)))
        nextSlot++;

    NullGraphicsRenderer renderer;
    GameRunner runner(fileOp, renderer, "locations", "", "", k_OutputDir);
    runner.loadScene("/BANNERS/START_SCREEN/Start_Screen.json");

    // Derive hit coordinates from the real start-screen JSON.
    nlohmann::json doc = nlohmann::json::parse(fileOp.load("/BANNERS/START_SCREEN/Start_Screen.json"));
    int hitX = 0, hitY = 0;
    for (auto& zone : doc["zones"])
    {
        if (zone.value("id", "") == "start_button")
        {
            hitX = zone.value("x", 0) + zone.value("width",  0) / 2;
            hitY = zone.value("y", 0) + zone.value("height", 0) / 2;
            break;
        }
    }
    runner.registerHit(hitX, hitY);

    return k_OutputDir + "/KSC_SLOT_" + std::to_string(nextSlot);
}

// ─────────────────────────────────────────────────────────────────────────────

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

TEST_CASE("GameRunner start button navigates to first location", "[GameRunner]")
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

    // Derive hit coordinates from the real start-screen JSON.
    nlohmann::json doc = nlohmann::json::parse(fileOp.load("/BANNERS/START_SCREEN/Start_Screen.json"));
    int hitX = 0, hitY = 0;
    for (auto& zone : doc["zones"])
    {
        if (zone.value("id", "") == "start_button")
        {
            hitX = zone.value("x", 0) + zone.value("width",  0) / 2;
            hitY = zone.value("y", 0) + zone.value("height", 0) / 2;
            break;
        }
    }

    GameRunner runner(fileOp, renderer);
    runner.loadScene("/BANNERS/START_SCREEN/Start_Screen.json");
    runner.registerHit(hitX, hitY);

    // start_button routes through dispatchCallback which hardcodes this path.
    CHECK(fileOp.lastLoadPath == "/LOCATIONS/AVERY/ROOT/Avery_Full.json");
}

TEST_CASE("GameRunner start button creates save slot with default Game_State", "[GameRunner]")
{
    TestFileOperator fileOp;
    std::string slotDir   = runStart(fileOp);
    std::string savedJson = fileOp.load(slotDir + "/Game_State.json");

    REQUIRE(fs::is_directory(slotDir));
    REQUIRE_FALSE(savedJson.empty());

    std::string goldenJson = fileOp.load(k_GoldenPath);
    GameStateComparison cmp(goldenJson, savedJson);
    REQUIRE(cmp.isEqual());
}

TEST_CASE("GameRunner start button creates save slot with default NOTES_STATE", "[GameRunner]")
{
    TestFileOperator fileOp;
    std::string slotDir = runStart(fileOp);

    SECTION("NOTES_STATE/AVERY subdirectory exists")
    {
        REQUIRE(fs::is_directory(slotDir + "/NOTES_STATE/AVERY"));
    }

    SECTION("Avery_Note.md content matches original")
    {
        std::string written  = fileOp.load(slotDir + "/NOTES_STATE/AVERY/Avery_Note.md");
        std::string original = fileOp.load("KSC_DATA/GAME_STATE/NOTES_STATE/AVERY/Avery_Note.md");
        REQUIRE_FALSE(written.empty());
        REQUIRE(written == original);
    }

    SECTION("Library_Note.md content matches original")
    {
        std::string written  = fileOp.load(slotDir + "/NOTES_STATE/LIBRARY/Library_Note.md");
        std::string original = fileOp.load("KSC_DATA/GAME_STATE/NOTES_STATE/LIBRARY/Library_Note.md");
        REQUIRE_FALSE(written.empty());
        REQUIRE(written == original);
    }
}
