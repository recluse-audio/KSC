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

#include "../ESP32FileOperator.h"
#include "../ESP32GraphicsRenderer.h"
#include "../../SHARED/GAME_RUNNER/GameRunner.h"

// --- Pin config ---------------------------------------------------
static const int SD_CS   =  5;
static const int SD_SCK  = 18;
static const int SD_MISO = 19;
static const int SD_MOSI = 23;

SPIClass sdSPI(VSPI);

// --- Globals ------------------------------------------------------
static TFT_eSPI                gTft;
static ESP32FileOperator*      gFileOperator = nullptr;
static ESP32GraphicsRenderer*  gRenderer     = nullptr;
static GameRunner*             gGame         = nullptr;

// -----------------------------------------------------------------
void setup()
{
    Serial.begin(115200);
    delay(2000); // wait for serial monitor to connect

    // Display
    gTft.init();
    gTft.setRotation(1);
    gTft.fillScreen(TFT_BLACK);

    // SD card — explicit VSPI so it doesn't clash with TFT on HSPI
    sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    bool sdOk = false;
    int freqs[] = { 25000000, 10000000, 4000000 };
    for (int f : freqs)
    {
        if (SD.begin(SD_CS, sdSPI, f)) { sdOk = true; break; }
        delay(100);
    }
    if (!sdOk)
    {
        gTft.drawString("SD init failed", 10, 10);
        Serial.println("[KSC] SD init failed — halting.");
        while (true) {}
    }
    Serial.println("[KSC] SD ready.");

    // Build the runner stack and load the opening scene
    gFileOperator = new ESP32FileOperator();
    gRenderer     = new ESP32GraphicsRenderer(gTft);
    gGame         = new GameRunner(*gFileOperator, *gRenderer, "locations", "", "", "/KSC_GAME/SAVED_GAMES");

    gGame->loadScene("/BANNERS/START_SCREEN/Start_Screen.json");
}

static bool gNeedsRedraw = true;

// -----------------------------------------------------------------
void loop()
{
    // --- Input ---
    // Uncomment when touch is wired up. registerHit sets gNeedsRedraw
    // so the scene redraws exactly once per interaction.
    //
    //   uint16_t tx, ty;
    //   if (tft.getTouch(&tx, &ty)) {
    //       while (tft.getTouch(&tx, &ty)) delay(10); // wait for release
    //       gGame->registerHit((int)tx, (int)ty);
    //       gNeedsRedraw = true;
    //   }

    // --- Draw (only when scene has changed) ---
    if (gNeedsRedraw)
    {
        gGame->draw();
        gNeedsRedraw = false;
    }
}
