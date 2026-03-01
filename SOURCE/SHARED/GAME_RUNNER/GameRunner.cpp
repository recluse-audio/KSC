#include "GameRunner.h"
#include "../FILE_OPERATOR/FileOperator.h"
#include "../GRAPHICS_RENDERER/GraphicsRenderer.h"
#include "../SCENE/Scene.h"
#include <nlohmann/json.hpp>

GameRunner::GameRunner(FileOperator& fileParser, GraphicsRenderer& renderer,
                       std::string mode, std::string locationID, std::string noteID,
                       std::string saveDir)
: mFileOperator(fileParser)
, mRenderer(renderer)
, mSceneView(renderer)
, mControlsView(renderer)
, mGameStartManager(fileParser, std::move(saveDir))
, mCurrentMode(mode)
, mCurrentLocationID(locationID)
, mCurrentNoteID(noteID)
{
    std::string controlsJson = mFileOperator.load("/GUI/Controls_View.json");
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
    {
        loadScene(target);
        return;
    }

    std::string zoneId = mActiveScene->getInterceptingZoneID(x, y);
    if (!zoneId.empty())
        dispatchCallback(zoneId);
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
    else if (callbackId == "start_button")
    {
        mGameStartManager.save();
        loadScene("/LOCATIONS/AVERY/ROOT/Avery_Full.json");
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
    std::string clueText = mFileOperator.load(mActiveScene->getSecondaryPath());
    if (!clueText.empty())
        mFileOperator.appendToFile(mActiveScene->getNoteTarget(), clueText);

    nlohmann::json j = nlohmann::json::parse(sceneJson, nullptr, false);
    if (!j.is_discarded())
    {
        j["isDiscovered"] = true;
        mFileOperator.writeToFile(scenePath, j.dump(2));
    }
    mActiveScene->setIsDiscovered(true);
}

void GameRunner::loadScene(const std::string& path)
{
    mOverlayVisible = false;
    std::string json = mFileOperator.load(path);

#ifdef ARDUINO
    Serial.printf("[GR] loadScene: %s  json=%d bytes\n",
        path.c_str(), (int)json.size());
#endif

    mActiveScene = mSceneFactory.build(json);

    if (!mActiveScene->isDiscovered() && !mActiveScene->getNoteTarget().empty())
        discoverSceneNote(path, json);

    syncControlsState();
}

void GameRunner::discoverNote(const std::string& notePath)
{
    std::string json = mFileOperator.load(notePath);
    nlohmann::json j = nlohmann::json::parse(json, nullptr, false);
    if (j.is_discarded()) return;
    j["isDiscovered"] = true;
    mFileOperator.writeToFile(notePath, j.dump(2));
}

void GameRunner::refreshNote(const std::string& clueArrayKey)
{
    std::string stateJson = mFileOperator.load("/GAME_STATE/Game_State.json");
    nlohmann::json state = nlohmann::json::parse(stateJson, nullptr, false);
    if (state.is_discarded()) return;

    auto& configs = state["note_configs"];
    if (!configs.contains(clueArrayKey)) return;

    std::string notePath = configs[clueArrayKey].value("note_path", "");
    std::string basePath = configs[clueArrayKey].value("base_path", "");
    if (notePath.empty()) return;

    mFileOperator.writeToFile(notePath, mFileOperator.load(basePath));

    auto& clues = state[clueArrayKey];
    for (auto it = clues.begin(); it != clues.end(); ++it)
    {
        if (!it.value().get<bool>()) continue;

        std::string clueJson = mFileOperator.load(it.key());
        nlohmann::json clue = nlohmann::json::parse(clueJson, nullptr, false);
        if (clue.is_discarded()) continue;

        std::string secondaryPath = clue.value("secondary_path", "");
        if (!secondaryPath.empty())
            mFileOperator.appendToFile(notePath, mFileOperator.load(secondaryPath));
    }
}

void GameRunner::setSaveDir(const std::string& dir)
{
    mGameStartManager.setSaveDir(dir);
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
