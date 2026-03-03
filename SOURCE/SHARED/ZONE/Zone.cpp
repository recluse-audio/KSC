#include "Zone.h"
#include "../SCENE/Scene.h"

Zone::Zone(Scene& scene, Bounds bounds, std::string zoneID, std::string target, std::string noteTarget, std::string label)
: mSceneID(scene.getSceneID())
, mBounds(bounds)
, mZoneID(zoneID)
, mTarget(target)
, mNoteTarget(noteTarget)
, mLabel(label)
{
}
