#include "RaylibGraphicsRenderer.h"
#include "../../THIRD_PARTY/nanosvg/nanosvg.h"
#include "../../THIRD_PARTY/nanosvg/nanosvgrast.h"
#include <fstream>
#include <string>
#include <vector>

static const int SCALE = 2;

RaylibGraphicsRenderer::~RaylibGraphicsRenderer()
{
    if (mCachedTexture.id > 0)
        UnloadTexture(mCachedTexture);
    for (auto& [path, tex] : mSvgCache)
        if (tex.id > 0)
            UnloadTexture(tex);
}

std::string RaylibGraphicsRenderer::sdPath(const std::string& path)
{
    if (!path.empty() && path[0] == '/')
        return "KSC_DATA" + path;
    return path;
}

// -----------------------------------------------------------------
void RaylibGraphicsRenderer::drawImage(const std::string& path)
{
    drawPng(sdPath(path));
}

void RaylibGraphicsRenderer::drawText(const std::string& path, int /*x*/, int y)
{
    std::string full = sdPath(path);
    if (y > 0)
    {
        DrawRectangle(0, 17 * SCALE, GetScreenWidth(),
                      GetScreenHeight() - 17 * SCALE, {0, 0, 0, 200});
        drawMarkdown(full, y, 0.5f);
    }
    else
    {
        drawMarkdown(full, 10, 1.0f);
    }
}

void RaylibGraphicsRenderer::drawSVG(const std::string& path, int x, int y, int w, int h)
{
    drawSvgAt(sdPath(path), x, y, w, h);
}

// -----------------------------------------------------------------
void RaylibGraphicsRenderer::drawPng(const std::string& fullPath)
{
    if (fullPath != mCachedPath)
    {
        if (mCachedTexture.id > 0)
            UnloadTexture(mCachedTexture);
        mCachedTexture = LoadTexture(fullPath.c_str());
        mCachedPath    = fullPath;
    }
    if (mCachedTexture.id > 0)
        DrawTextureEx(mCachedTexture, {0, 0}, 0.0f, (float)SCALE, WHITE);
}

void RaylibGraphicsRenderer::drawSvgAt(const std::string& fullPath, int x, int y, int targetW, int targetH)
{
    std::string cacheKey = fullPath + "@" + std::to_string(targetW) + "x" + std::to_string(targetH);

    if (mSvgCache.find(cacheKey) == mSvgCache.end())
    {
        std::ifstream file(fullPath);
        if (!file.is_open()) return;
        std::string content((std::istreambuf_iterator<char>(file)), {});

        std::vector<char> buf(content.begin(), content.end());
        buf.push_back('\0');

        NSVGimage* image = nsvgParse(buf.data(), "px", 96.0f);
        if (!image) return;

        float svgW = image->width;
        float svgH = image->height;
        if (svgW <= 0 || svgH <= 0) { nsvgDelete(image); return; }

        int rasterW = (targetW > 0) ? targetW * SCALE : (int)svgW;
        int rasterH = (targetH > 0) ? targetH * SCALE : (int)svgH;
        float scale = (targetW > 0) ? (float)rasterW / svgW : 1.0f;

        NSVGrasterizer* rast = nsvgCreateRasterizer();
        std::vector<uint8_t> pixels(rasterW * rasterH * 4);
        nsvgRasterize(rast, image, 0.0f, 0.0f, scale, pixels.data(), rasterW, rasterH, rasterW * 4);
        nsvgDeleteRasterizer(rast);
        nsvgDelete(image);

        // Force all pixels to white, preserving alpha
        for (size_t i = 0; i < pixels.size(); i += 4)
        {
            pixels[i]     = 255;
            pixels[i + 1] = 255;
            pixels[i + 2] = 255;
        }

        Image img = { (void*)pixels.data(), rasterW, rasterH, 1, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };
        mSvgCache[cacheKey] = LoadTextureFromImage(img);
    }

    const Texture2D& tex = mSvgCache.at(cacheKey);
    if (tex.id > 0)
        DrawTextureEx(tex, {(float)(x * SCALE), (float)(y * SCALE)}, 0.0f, 1.0f, WHITE);
}

void RaylibGraphicsRenderer::drawMarkdown(const std::string& fullPath, int startY, float textScale)
{
    std::ifstream file(fullPath);
    if (!file.is_open()) return;

    int y = startY * SCALE;
    std::string line;
    while (std::getline(file, line))
    {
        int hashes = 0;
        while (hashes < (int)line.size() && line[hashes] == '#') hashes++;

        std::string display = (hashes > 0 && hashes < (int)line.size())
                              ? line.substr(hashes + 1)
                              : line;

        int fontSize = (int)(((hashes == 1) ? 24 : (hashes == 2) ? 18 : 14) * SCALE * textScale);
        Color color  = (hashes > 0) ? WHITE : LIGHTGRAY;

        DrawText(display.c_str(), 20 * SCALE, y, fontSize, color);
        y += fontSize + (int)(8 * SCALE * textScale);
    }
}
