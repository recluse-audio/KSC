#include <catch2/catch_test_macros.hpp>
#include "GAME_RUNNER/GameRunner.h"
#include "SCENE/SceneFactory.h"
#include "UTIL/TestFileOperator.h"
#include "UTIL/NullGraphicsRenderer.h"
#include <nlohmann/json.hpp>

// Centroid of a JSON "points" array (hi-res coordinates, pre-scaling).
static std::pair<int,int> jsonPolygonCentroid(const nlohmann::json& points)
{
    int sumX = 0, sumY = 0;
    for (auto& pt : points)
    {
        sumX += pt[0].get<int>();
        sumY += pt[1].get<int>();
    }
    int n = static_cast<int>(points.size());
    return { sumX / n, sumY / n };
}

// Centroid of a Zone::Polygon — coordinates already scaled to 320×240.
static std::pair<int,int> zoneCentroid(const Zone::Polygon& poly)
{
    int sumX = 0, sumY = 0;
    for (auto& [x, y] : poly)
        { sumX += x; sumY += y; }
    int n = static_cast<int>(poly.size());
    return { sumX / n, sumY / n };
}

// Build the scene via SceneFactory and return the centroid of the named zone's
// polygon in 320×240 space. This is the non-circular way to get a hit coordinate
// for hi-res zones: let SceneFactory do the scaling, then read the result.
static std::pair<int,int> scaledHitForZone(TestFileOperator& fileOp,
                                            const std::string& scenePath,
                                            const std::string& zoneId)
{
    SceneFactory factory;
    auto scene = factory.build(fileOp.load(scenePath));
    for (auto& zone : scene->getZones())
    {
        if (zone.getZoneID() != zoneId) continue;
        if (zone.hasPolygon()) return zoneCentroid(zone.getPolygon());
        auto b = zone.getBounds();
        return { b.mX + b.mW / 2, b.mY + b.mH / 2 };
    }
    return { 0, 0 };
}

// Builds a 24-byte buffer whose bytes 16-23 encode (w, h) big-endian — the only
// bytes SceneFactory::parsePngDimensions needs to determine the canvas size.
static std::string fakePngHeader(int w, int h)
{
    std::string hdr(24, '\0');
    hdr[16] = (w >> 24) & 0xFF;  hdr[17] = (w >> 16) & 0xFF;
    hdr[18] = (w >>  8) & 0xFF;  hdr[19] =  w        & 0xFF;
    hdr[20] = (h >> 24) & 0xFF;  hdr[21] = (h >> 16) & 0xFF;
    hdr[22] = (h >>  8) & 0xFF;  hdr[23] =  h        & 0xFF;
    return hdr;
}

// Returns a copy of the JSON document with all zone polygon points scaled to
// the given hi-res canvas dimensions, and hires_image_path updated accordingly.
static nlohmann::json toHiresJson(nlohmann::json doc, int imgW, int imgH,
                                  const std::string& hiresPath)
{
    doc["hires_image_path"] = hiresPath;
    float sx = imgW / 320.0f;
    float sy = imgH / 240.0f;
    for (auto& zone : doc["zones"])
        for (auto& pt : zone["points"])
        {
            pt[0] = (int)(pt[0].get<int>() * sx);
            pt[1] = (int)(pt[1].get<int>() * sy);
        }
    return doc;
}

struct TrackingFileOperator : TestFileOperator
{
    std::string lastLoadPath;
    std::string load(const std::string& path) override
    {
        lastLoadPath = path;
        return TestFileOperator::load(path);
    }
};

static const std::string k_AveryRootPath = "/LOCATIONS/AVERY/ROOT/Avery_Full.json";
static const std::string k_FakeHiresPath = "/LOCATIONS/AVERY/ROOT/Avery_Full_HIRES_TEST.png";

// Actual hi-res PNG dimensions reported by the file header.
static constexpr int k_HiresW = 3936;
static constexpr int k_HiresH = 2648;

// ─── lo-res tests ────────────────────────────────────────────────────────────

TEST_CASE("Avery root desk zone navigates to desk scene", "[AveryRoot]")
{
    TrackingFileOperator fileOp;
    fileOp.diskRoot = "KSC_DATA";
    NullGraphicsRenderer renderer;

    nlohmann::json doc = nlohmann::json::parse(fileOp.load(k_AveryRootPath));
    auto [hitX, hitY] = jsonPolygonCentroid(doc["zones"][0]["points"]);

    GameRunner runner(fileOp, renderer);
    runner.loadScene(k_AveryRootPath);
    runner.registerHit(hitX, hitY);

    CHECK(fileOp.lastLoadPath == "/LOCATIONS/AVERY/DESK/MAIN/Avery_Desk.json");
}

TEST_CASE("Avery root cable_cabinet zone navigates to cable cabinet scene", "[AveryRoot]")
{
    TrackingFileOperator fileOp;
    fileOp.diskRoot = "KSC_DATA";
    NullGraphicsRenderer renderer;

    nlohmann::json doc = nlohmann::json::parse(fileOp.load(k_AveryRootPath));
    auto [hitX, hitY] = jsonPolygonCentroid(doc["zones"][1]["points"]);

    GameRunner runner(fileOp, renderer);
    runner.loadScene(k_AveryRootPath);
    runner.registerHit(hitX, hitY);

    CHECK(fileOp.lastLoadPath == "/LOCATIONS/AVERY/CABLE_CABINET/MAIN/Avery_Cable_Cabinet.json");
}

// ─── hi-res tests ─────────────────────────────────────────────────────────────
// Zone points are defined in 3936×2648 hi-res space. SceneFactory auto-detects
// that coordinates exceed 320×240, reads the PNG header, and scales down.
// Hit coordinates are derived from the SceneFactory-scaled polygon (not from the
// raw JSON), so the test exercises the full scaling pipeline without being circular.

TEST_CASE("Avery root desk zone navigates to desk scene at hi-res", "[AveryRoot]")
{
    TrackingFileOperator fileOp;
    fileOp.diskRoot = "KSC_DATA";
    NullGraphicsRenderer renderer;

    nlohmann::json loRes = nlohmann::json::parse(fileOp.load(k_AveryRootPath));
    fileOp.files[k_AveryRootPath] = toHiresJson(loRes, k_HiresW, k_HiresH, k_FakeHiresPath).dump();
    fileOp.files[k_FakeHiresPath] = fakePngHeader(k_HiresW, k_HiresH);

    // Derive hit from the zone polygon *after* SceneFactory has scaled it to 320×240.
    auto [hitX, hitY] = scaledHitForZone(fileOp, k_AveryRootPath, "desk");

    GameRunner runner(fileOp, renderer);
    runner.loadScene(k_AveryRootPath);
    runner.registerHit(hitX, hitY);

    CHECK(fileOp.lastLoadPath == "/LOCATIONS/AVERY/DESK/MAIN/Avery_Desk.json");
}

TEST_CASE("Avery root cable_cabinet zone navigates to cable cabinet scene at hi-res", "[AveryRoot]")
{
    TrackingFileOperator fileOp;
    fileOp.diskRoot = "KSC_DATA";
    NullGraphicsRenderer renderer;

    nlohmann::json loRes = nlohmann::json::parse(fileOp.load(k_AveryRootPath));
    fileOp.files[k_AveryRootPath] = toHiresJson(loRes, k_HiresW, k_HiresH, k_FakeHiresPath).dump();
    fileOp.files[k_FakeHiresPath] = fakePngHeader(k_HiresW, k_HiresH);

    auto [hitX, hitY] = scaledHitForZone(fileOp, k_AveryRootPath, "cable_cabinet");

    GameRunner runner(fileOp, renderer);
    runner.loadScene(k_AveryRootPath);
    runner.registerHit(hitX, hitY);

    CHECK(fileOp.lastLoadPath == "/LOCATIONS/AVERY/CABLE_CABINET/MAIN/Avery_Cable_Cabinet.json");
}
