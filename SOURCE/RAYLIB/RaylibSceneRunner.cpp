#include "RaylibSceneRunner.h"
#include <fstream>
#include <string>

RaylibSceneRunner::~RaylibSceneRunner()
{
    if (mCachedTexture.id > 0)
        UnloadTexture(mCachedTexture);
}

std::string RaylibSceneRunner::sdPath(const std::string& path)
{
    if (!path.empty() && path[0] == '/')
        return "SD" + path;
    return path;
}

// -----------------------------------------------------------------
void RaylibSceneRunner::draw(const std::string& path)
{
    if (path.empty()) return;

    std::string full = sdPath(path);

    if (path.size() >= 4 && path.substr(path.size() - 4) == ".png")
        drawPng(full);
    else if (path.size() >= 3 && path.substr(path.size() - 3) == ".md")
        drawMarkdown(full);
}

void RaylibSceneRunner::drawText(const std::string& text, int x, int y)
{
    DrawText(text.c_str(), x, y, 12, GRAY);
}

void RaylibSceneRunner::drawOverlay()
{
    // TODO: draw zone boundaries, navigation hints, HUD elements
}

// -----------------------------------------------------------------
void RaylibSceneRunner::drawPng(const std::string& fullPath)
{
    if (fullPath != mCachedPath)
    {
        if (mCachedTexture.id > 0)
            UnloadTexture(mCachedTexture);
        mCachedTexture = LoadTexture(fullPath.c_str());
        mCachedPath    = fullPath;
    }
    if (mCachedTexture.id > 0)
        DrawTexture(mCachedTexture, 0, 0, WHITE);
}

void RaylibSceneRunner::drawMarkdown(const std::string& fullPath)
{
    std::ifstream file(fullPath);
    if (!file.is_open()) return;

    int y = 10;
    std::string line;
    while (std::getline(file, line))
    {
        // Count leading '#' to determine heading level
        int hashes = 0;
        while (hashes < (int)line.size() && line[hashes] == '#') hashes++;

        // Strip "# " prefix for display
        std::string display = (hashes > 0 && hashes < (int)line.size())
                              ? line.substr(hashes + 1)
                              : line;

        int fontSize  = (hashes == 1) ? 24 : (hashes == 2) ? 18 : 14;
        Color color   = (hashes > 0)  ? WHITE : LIGHTGRAY;

        DrawText(display.c_str(), 20, y, fontSize, color);
        y += fontSize + 8;
    }
}
