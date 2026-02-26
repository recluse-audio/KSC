/**
 * Made by Ryan Devens on 2026-02-25
 */

#pragma once
#include <string>
#include <memory>
#include "../SCENE/SceneFactory.h"

class FileParser;
class SceneRunner;
class Scene;

/**
 * Owns the active game state (mode, current location/note IDs) and the
 * active Scene. Uses an injected FileParser to load scene JSON from storage
 * and a SceneFactory to build the Scene from that string. Delegates all
 * rendering to an injected SceneRunner.
 */
class GameRunner
{
public:
    GameRunner(FileParser& fileParser,
               SceneRunner& sceneRunner,
               std::string mode       = "locations",
               std::string locationID = "",
               std::string noteID     = "");

    /**
     * Render the active scene via the SceneRunner.
     */
    void draw();

    /**
     * Called by platform input callbacks when the user taps/clicks at (x, y).
     * Uses the active scene's zones to resolve the hit.
     */
    void registerHit(int x, int y);

    /**
     * Load a scene from the given SD-relative JSON path, build it via
     * SceneFactory, and take ownership as the new active scene.
     */
    void loadScene(const std::string& path);

    std::string getCurrentMode()       const;
    std::string getCurrentLocationID() const;
    std::string getCurrentNoteID()     const;

private:
    FileParser&  mFileParser;
    SceneRunner& mSceneRunner;
    SceneFactory mSceneFactory;
    std::unique_ptr<Scene> mActiveScene;

    std::string mCurrentMode;
    std::string mCurrentLocationID;
    std::string mCurrentNoteID;
};
