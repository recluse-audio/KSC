#include "Zone.h"
#include "../SCENE/Scene.h"

Zone::Zone(Scene& scene, Bounds bounds, std::string zoneID, std::string target)
: mSceneID(scene.getSceneID())
, mBounds(bounds)
, mZoneID(zoneID)
, mTarget(target)
{
}
