/**
 * KSC — ESP32 Arduino entry point
 * Made by Ryan Devens on 2026-02-25
 *
 * Board: ESP32 with integrated TFT display
 * Libraries required:
 *   - TFT_eSPI   (display driver)
 *   - SD         (SD card access)
 *   - SPI        (SPI bus)
 */

#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h>

#include "../ESP32FileParser.h"
#include "../ESP32SceneRunner.h"
#include "../../SHARED/GAME_RUNNER/GameRunner.h"

// --- Pin config ---------------------------------------------------
// Adjust SD_CS to match your board's SD chip-select pin.
static const int SD_CS = 5;

// --- Globals ------------------------------------------------------
// Objects are heap-allocated in setup() because GameRunner requires
// references to FileParser and SceneRunner at construction time.
static ESP32FileParser*  gFileParser  = nullptr;
static ESP32SceneRunner* gSceneRunner = nullptr;
static GameRunner*       gGame        = nullptr;

// -----------------------------------------------------------------
void setup()
{
    Serial.begin(115200);

    // SD card
    if (!SD.begin(SD_CS))
    {
        Serial.println("[KSC] SD init failed — halting.");
        while (true) {}
    }
    Serial.println("[KSC] SD ready.");

    // Build the runner stack and load the opening scene
    gFileParser  = new ESP32FileParser();
    gSceneRunner = new ESP32SceneRunner();
    gGame        = new GameRunner(*gFileParser, *gSceneRunner);

    gGame->loadScene("/BANNERS/START_SCREEN/Start_Screen.json");
}

// -----------------------------------------------------------------
void loop()
{
    // --- Input ---
    // Replace with your board's touch or button read.
    // Example for a resistive touch panel via TFT_eSPI:
    //
    //   uint16_t tx, ty;
    //   if (tft.getTouch(&tx, &ty))
    //       gGame->registerHit((int)tx, (int)ty);

    // --- Draw ---
    gGame->draw();
}
