/**
 * Made by Ryan Devens on 2026-02-24
 */

#pragma once
#include <string>

class Scene; // forward declaration - only a reference is needed
/**
 * Hit area (bounds) within an associated scene
 */
class Zone
{
public:
    // int rectangle class essentially for this zone
    class Bounds
    {
    public:
        int mX = 0;
        int mY = 0;
        int mW = 0;
        int mH = 0;

        Bounds(int x, int y, int w, int h)
        : mX(x)
        , mY(y)
        , mW(w)
        , mH(h)
        {
        }

        Bounds(const Bounds& other)
        : mX(other.mX)
        , mY(other.mY)
        , mW(other.mW)
        , mH(other.mH)
        {
        }
    };

    Zone(Scene& scene, Bounds bounds, std::string zoneID = "Default Zone ID", std::string target = "");

    bool containsPoint(int x, int y)
    {
        bool doesContain = true;

        if(x < mBounds.mX || x > mBounds.mX + mBounds.mW)
            doesContain = false; // too far left or right
        else if(y < mBounds.mY || y > mBounds.mY + mBounds.mH)
            doesContain = false;

        return doesContain;
    }

    const std::string getZoneID()  { return mZoneID; }
    const std::string getSceneID() { return mSceneID; }
    const std::string getTarget()  { return mTarget; }
private:
    std::string mSceneID; // ID of the scene this zone belongs to
    Bounds mBounds;
    std::string mZoneID  = "";
    std::string mTarget  = ""; // SD-relative path of the scene to load on hit
};
