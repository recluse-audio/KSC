/**
 * KSC — Raylib desktop entry point
 * Made by Ryan Devens on 2026-02-25
 */

#include "raylib.h"
#include "RaylibFileParser.h"
#include "RaylibGraphicsRenderer.h"
#include "../SHARED/GAME_RUNNER/GameRunner.h"
#include <filesystem>

static const int SCALE    = 2;
static const int SCREEN_W = 320 * SCALE;
static const int SCREEN_H = 240 * SCALE;

int main()
{
    // Walk up from the exe directory until we find a folder containing KSC_DATA/.
    // This lets the exe run from any working directory.
    {
        std::filesystem::path dir = GetApplicationDirectory();
        while (!std::filesystem::exists(dir / "KSC_DATA"))
        {
            auto parent = dir.parent_path();
            if (parent == dir) break; // reached filesystem root
            dir = parent;
        }
        ChangeDirectory(dir.string().c_str());
    }

    InitWindow(SCREEN_W, SCREEN_H, "KSC");
    SetTargetFPS(60);

    RaylibFileParser       fileParser;
    RaylibGraphicsRenderer renderer;
    GameRunner             game(fileParser, renderer);

    game.loadScene("/BANNERS/START_SCREEN/Start_Screen.json");

    while (!WindowShouldClose())
    {
        // --- Input ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            game.registerHit((int)mouse.x / SCALE, (int)mouse.y / SCALE);
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(BLACK);
        game.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
