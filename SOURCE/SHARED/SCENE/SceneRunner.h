/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include <string>
#include "Scene.h"

/**
 * Abstract base class for platform-specific rendering.
 * Subclass for each platform (ESP32, Raylib desktop, etc.) and implement
 * draw, drawText, and drawOverlay.
 *
 * GameRunner calls drawScene(scene) when the active scene needs to be rendered.
 *
 * Asset conventions:
 *   Locations  — primary: PNG,      secondary: markdown summary
 *   Notes      — primary: markdown, secondary: PNG (optional, may be empty)
 *
 * drawPrimary and drawSecondary are non-virtual. They forward to draw(path),
 * skipping empty paths. The concrete draw implementation inspects the path
 * (e.g. by file extension) to decide how to render it.
 */
class SceneRunner
{
public:
    virtual ~SceneRunner() = default;

    /**
     * Render the given scene. Called by GameRunner.
     */
    void drawScene(const Scene& scene);

protected:
    void drawPrimary(const std::string& path);   // forwards to draw(); no-ops if empty
    void drawSecondary(const std::string& path); // forwards to draw(); no-ops if empty

    /**
     * Render an asset from the given SD-relative path.
     * Implementations determine the render method from the file type
     * (e.g. .png → image, .md → document).
     */
    virtual void draw(const std::string& path) = 0;

    /**
     * Draw a short text label at the given screen coordinates.
     */
    virtual void drawText(const std::string& text, int x, int y) = 0;

    /**
     * Draw any UI overlay on top of the scene (HUD, zone borders, etc.).
     */
    virtual void drawOverlay() = 0;
};
