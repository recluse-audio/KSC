/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include "../SHARED/SCENE/SceneRunner.h"
#include "raylib.h"
#include <string>

/**
 * Raylib (desktop) implementation of SceneRunner.
 * draw() inspects the file extension:
 *   .png  — loaded as a Raylib texture and drawn at the origin
 *   .md   — read line by line and rendered as plain text
 *
 * The most recently loaded PNG is cached to avoid reloading every frame.
 */
class RaylibSceneRunner : public SceneRunner
{
public:
    ~RaylibSceneRunner();

protected:
    void draw(const std::string& path) override;
    void drawText(const std::string& text, int x, int y) override;
    void drawOverlay() override;

private:
    std::string mCachedPath;
    Texture2D   mCachedTexture = {};

    static std::string sdPath(const std::string& path);
    void drawPng(const std::string& fullPath);
    void drawMarkdown(const std::string& fullPath);
};
