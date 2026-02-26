/**
 * Made by Ryan Devens on 2026-02-26
 */

#pragma once
#include <string>
#include <vector>

class GraphicsRenderer;

/**
 * Snapshot of game state that ControlsView uses to decide which buttons
 * are visible.
 */
struct ControlsState
{
    bool        isRoot         = false;
    bool        overlayVisible = false;
    bool        hasParent      = false;
    std::string mode           = "locations";
};

/**
 * Renders the persistent navigation controls overlay and resolves hit tests
 * against control buttons. Loaded from a JSON data file via load().
 *
 * draw() iterates visible buttons and calls mRenderer.drawSVG() for each icon.
 * handleHit() returns the callback identifier for the button at (x, y), or ""
 * if no visible button was hit.
 */
class ControlsView
{
public:
    explicit ControlsView(GraphicsRenderer& renderer);

    void        load(const std::string& json);
    void        setState(const ControlsState& state);
    void        draw();
    std::string handleHit(int x, int y) const;

private:
    struct Button
    {
        std::string id;
        int         x           = 0;
        int         y           = 0;
        int         w           = 0;
        int         h           = 0;
        std::string icon;
        std::string callback;
        std::string visibleWhen; // "always", "root", "nonRoot"
    };

    bool isButtonVisible(const Button& btn) const;

    GraphicsRenderer&   mRenderer;
    std::vector<Button> mButtons;
    ControlsState       mState;
};
