/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include <string>
#include <memory>
#include "../SCENE/SceneFactory.h"
#include "../SCENE_VIEW/SceneView.h"
#include "../CONTROLS_VIEW/ControlsView.h"

class FileParser;
class GraphicsRenderer;
class Scene;

/**
 * Owns the active game state and the active Scene. Uses an injected FileParser
 * to load scene JSON from storage and a SceneFactory to build the Scene.
 * Delegates scene rendering to SceneView and controls rendering to ControlsView,
 * both of which use an injected GraphicsRenderer.
 */
class GameRunner
{
public:
    GameRunner(FileParser&       fileParser,
               GraphicsRenderer& renderer,
               std::string       mode       = "locations",
               std::string       locationID = "",
               std::string       noteID     = "");

    /**
     * Render the active scene and controls via their respective views.
     */
    void draw();

    /**
     * Called by platform input callbacks when the user taps/clicks at (x, y).
     * Checks control buttons first, then zone hit-testing on the active scene.
     */
    void registerHit(int x, int y);

    /**
     * Load a scene from the given data-root-relative JSON path, build it via
     * SceneFactory, and take ownership as the new active scene.
     */
    void loadScene(const std::string& path);

    std::string getCurrentMode()       const;
    std::string getCurrentLocationID() const;
    std::string getCurrentNoteID()     const;

private:
    FileParser&            mFileParser;
    GraphicsRenderer&      mRenderer;
    SceneView              mSceneView;
    ControlsView           mControlsView;
    SceneFactory           mSceneFactory;
    std::unique_ptr<Scene> mActiveScene;
    bool                   mOverlayVisible = false;

    std::string mCurrentMode;
    std::string mCurrentLocationID;
    std::string mCurrentNoteID;

    void discoverNote(const std::string& notePath);
    void discoverSceneNote(const std::string& scenePath, const std::string& sceneJson);
    void dispatchCallback(const std::string& callbackId);
    void syncControlsState();
};
