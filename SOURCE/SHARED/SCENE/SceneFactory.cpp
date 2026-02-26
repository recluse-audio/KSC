#include "SceneFactory.h"
#include "../ZONE/Zone.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::unique_ptr<Scene> SceneFactory::build(const std::string& jsonString)
{
    json j = json::parse(jsonString, nullptr, false); // false = no exceptions
    if (j.is_discarded())
        return std::make_unique<Scene>();

    std::string sceneID       = j.value("id",             "");
    std::string parentID      = j.value("parent",         "Main");
    std::string name          = j.value("name",           "");
    std::string primaryPath   = j.value("primary_path",   "");
    std::string secondaryPath = j.value("secondary_path", "");

    auto scene = std::make_unique<Scene>(sceneID, parentID, name, primaryPath, secondaryPath);

    if (j.contains("zones") && j["zones"].is_array())
    {
        for (auto& z : j["zones"])
        {
            Zone::Bounds bounds(
                z.value("x",      0),
                z.value("y",      0),
                z.value("width",  0),
                z.value("height", 0)
            );
            scene->addZone(Zone(*scene, bounds, z.value("id", ""), z.value("target", "")));
        }
    }

    return scene;
}
