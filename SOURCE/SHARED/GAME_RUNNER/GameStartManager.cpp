#include "GameStartManager.h"
#include "../FILE_OPERATOR/FileOperator.h"

GameStartManager::GameStartManager(FileOperator& fileOperator, std::string saveDir)
: mFileOperator(fileOperator)
, mSaveDir(std::move(saveDir))
{
}

void GameStartManager::setSaveDir(const std::string& dir)
{
    mSaveDir = dir;
}

std::string GameStartManager::getSaveDir() const
{
    return mSaveDir;
}

int GameStartManager::findNextSlotIndex()
{
    int highest = -1;
    for (const std::string& entry : mFileOperator.listDirectory(mSaveDir))
    {
        if (entry.rfind("KSC_SLOT_", 0) == 0)
        {
            try
            {
                int n = std::stoi(entry.substr(9));
                if (n > highest) highest = n;
            }
            catch (...) {}
        }
    }
    return highest + 1;
}

void GameStartManager::save()
{
    if (mSaveDir.empty()) return;

    int         slot    = findNextSlotIndex();
    std::string slotDir = mSaveDir + "/KSC_SLOT_" + std::to_string(slot);

    for (const std::string& file : mFileOperator.listDirectory("/GAME_STATE"))
    {
        std::string content = mFileOperator.load("/GAME_STATE/" + file);
        mFileOperator.writeToFile(slotDir + "/" + file, content);
    }
}
