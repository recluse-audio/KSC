/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include "../SHARED/SCENE/SceneRunner.h"

/**
 * ESP32 implementation of SceneRunner.
 * Uses TFT_eSPI (or equivalent) to render scenes to the integrated display.
 * draw() inspects the file extension to decide whether to render an image or document.
 */
class ESP32SceneRunner : public SceneRunner
{
protected:
    void draw(const std::string& path) override;
    void drawText(const std::string& text, int x, int y) override;
    void drawOverlay() override;
};
