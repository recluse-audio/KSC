#include "GameRunner.h"
#include "../FILE_PARSER/FileParser.h"
#include "../SCENE/SceneRunner.h"
#include "../SCENE/Scene.h"

GameRunner::GameRunner(FileParser& fileParser, SceneRunner& sceneRunner,
                       std::string mode, std::string locationID, std::string noteID)
: mFileParser(fileParser)
, mSceneRunner(sceneRunner)
, mCurrentMode(mode)
, mCurrentLocationID(locationID)
, mCurrentNoteID(noteID)
{
}

void GameRunner::draw()
{
    if (mActiveScene)
        mSceneRunner.drawScene(*mActiveScene);
}

void GameRunner::registerHit(int x, int y)
{
    if (!mActiveScene)
        return;
    std::string target = mActiveScene->getInterceptingZoneTarget(x, y);
    if (!target.empty())
        loadScene(target);
}

void GameRunner::loadScene(const std::string& path)
{
    std::string json = mFileParser.load(path);
    mActiveScene = mSceneFactory.build(json);
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
