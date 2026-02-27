#include "GameRunner.h"
#include "../FILE_PARSER/FileParser.h"
#include "../GRAPHICS_RENDERER/GraphicsRenderer.h"
#include "../SCENE/Scene.h"
#include <nlohmann/json.hpp>

GameRunner::GameRunner(FileParser& fileParser, GraphicsRenderer& renderer,
                       std::string mode, std::string locationID, std::string noteID)
: mFileParser(fileParser)
, mRenderer(renderer)
, mSceneView(renderer)
, mControlsView(renderer)
, mCurrentMode(mode)
, mCurrentLocationID(locationID)
, mCurrentNoteID(noteID)
{
    std::string controlsJson = mFileParser.load("/GUI/Controls_View.json");
    mControlsView.load(controlsJson);
}

void GameRunner::draw()
{
    if (!mActiveScene) return;
    mSceneView.draw(*mActiveScene, mOverlayVisible);
    mControlsView.draw();
}

void GameRunner::registerHit(int x, int y)
{
    if (!mActiveScene) return;

    std::string cb = mControlsView.handleHit(x, y);
    if (!cb.empty())
    {
        dispatchCallback(cb);
        return;
    }

    std::string noteTarget = mActiveScene->getInterceptingZoneNoteTarget(x, y);
    if (!noteTarget.empty())
    {
        discoverNote(noteTarget);
        return;
    }

    std::string target = mActiveScene->getInterceptingZoneTarget(x, y);
    if (!target.empty())
        loadScene(target);
}

void GameRunner::dispatchCallback(const std::string& callbackId)
{
    if (callbackId == "toggleOverlay")
    {
        mOverlayVisible = !mOverlayVisible;
        syncControlsState();
    }
    else if (callbackId == "navigateUp")
    {
        std::string parent = mActiveScene->getParentPath();
        if (!parent.empty()) loadScene(parent);
    }
    else if (callbackId == "navigatePrev")
    {
        // TODO: navigate to previous root location/note
    }
    else if (callbackId == "navigateNext")
    {
        // TODO: navigate to next root location/note
    }
    else if (callbackId == "switchToLocations")
    {
        mCurrentMode = "locations";
        syncControlsState();
    }
    else if (callbackId == "switchToNotes")
    {
        mCurrentMode = "notes";
        syncControlsState();
    }
}

void GameRunner::syncControlsState()
{
    ControlsState state;
    state.isRoot         = mActiveScene ? mActiveScene->isRoot() : false;
    state.overlayVisible = mOverlayVisible;
    state.hasParent      = mActiveScene && !mActiveScene->getParentPath().empty();
    state.mode           = mCurrentMode;
    mControlsView.setState(state);
}

void GameRunner::discoverSceneNote(const std::string& scenePath, const std::string& sceneJson)
{
    std::string clueText = mFileParser.load(mActiveScene->getSecondaryPath());
    if (!clueText.empty())
        mFileParser.appendToFile(mActiveScene->getNoteTarget(), clueText);

    nlohmann::json j = nlohmann::json::parse(sceneJson, nullptr, false);
    if (!j.is_discarded())
    {
        j["isDiscovered"] = true;
        mFileParser.writeToFile(scenePath, j.dump(2));
    }
    mActiveScene->setIsDiscovered(true);
}

void GameRunner::loadScene(const std::string& path)
{
    mOverlayVisible = false;
    std::string json = mFileParser.load(path);
    mActiveScene = mSceneFactory.build(json);

    if (!mActiveScene->isDiscovered() && !mActiveScene->getNoteTarget().empty())
        discoverSceneNote(path, json);

    syncControlsState();
}

void GameRunner::discoverNote(const std::string& notePath)
{
    std::string json = mFileParser.load(notePath);
    nlohmann::json j = nlohmann::json::parse(json, nullptr, false);
    if (j.is_discarded()) return;
    j["isDiscovered"] = true;
    mFileParser.writeToFile(notePath, j.dump(2));
}

std::string GameRunner::getCurrentMode() const
{
    return mCurrentMode;
}

std::string GameRunner::getCurrentLocationID() const
{
    return mCurrentLocationID;
}

std::string GameRunner::getCurrentNoteID() const
{
    return mCurrentNoteID;
}
