/**
 * Made by Ryan Devens on 2026-02-26
 */

#pragma once
#include <string>

/**
 * Abstract base class for platform-specific drawing primitives.
 * Platform implementations handle file I/O and rendering internally.
 * nanosvg rasterization and texture caching are internal platform details.
 */
class GraphicsRenderer
{
public:
    virtual ~GraphicsRenderer() = default;

    /**
     * Load and render an image asset from the given data-root-relative path.
     */
    virtual void drawImage(const std::string& path) = 0;

    /**
     * Load and render a text/markdown asset from the given data-root-relative path
     * at the given screen coordinates.
     */
    virtual void drawText(const std::string& path, int x, int y) = 0;

    /**
     * Load and render an SVG asset from the given path at the given
     * screen coordinates.
     */
    virtual void drawSVG(const std::string& path, int x, int y, int w = 0, int h = 0) = 0;
};
