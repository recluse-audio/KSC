/**
 * KSC — Raylib desktop entry point
 * Made by Ryan Devens on 2026-02-25
 */

#include "raylib.h"
#include "RaylibFileParser.h"
#include "RaylibSceneRunner.h"
#include "../SHARED/GAME_RUNNER/GameRunner.h"
#include <filesystem>

static const int SCREEN_W = 320;
static const int SCREEN_H = 240;

int main()
{
    // Walk up from the exe directory until we find a folder containing SD/.
    // This lets the exe run from any working directory.
    {
        std::filesystem::path dir = GetApplicationDirectory();
        while (!std::filesystem::exists(dir / "SD"))
        {
            auto parent = dir.parent_path();
            if (parent == dir) break; // reached filesystem root
            dir = parent;
        }
        ChangeDirectory(dir.string().c_str());
    }

    InitWindow(SCREEN_W, SCREEN_H, "KSC");
    SetTargetFPS(60);

    RaylibFileParser  fileParser;
    RaylibSceneRunner sceneRunner;
    GameRunner        game(fileParser, sceneRunner);

    game.loadScene("/BANNERS/START_SCREEN/Start_Screen.json");

    while (!WindowShouldClose())
    {
        // --- Input ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            game.registerHit((int)mouse.x, (int)mouse.y);
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
