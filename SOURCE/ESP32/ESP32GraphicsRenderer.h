/**
 * Made by Ryan Devens on 2026-02-26
 */

#pragma once
#include "../SHARED/GRAPHICS_RENDERER/GraphicsRenderer.h"

/**
 * ESP32 implementation of GraphicsRenderer.
 * Uses TFT_eSPI (or equivalent) to render to the integrated display.
 * All methods are stubs until the ESP32 render pipeline is implemented.
 */
class ESP32GraphicsRenderer : public GraphicsRenderer
{
public:
    void drawImage(const std::string& path) override;
    void drawText(const std::string& path, int x, int y) override;
    void drawSVG(const std::string& path, int x, int y, int w = 0, int h = 0) override;
};
