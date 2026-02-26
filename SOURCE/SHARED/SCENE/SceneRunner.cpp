#include "SceneRunner.h"

void SceneRunner::drawScene(const Scene& scene)
{
    drawPrimary(scene.getPrimaryPath());
    drawSecondary(scene.getSecondaryPath());
    drawText(scene.getName(), 0, 0);
    drawOverlay();
}

void SceneRunner::drawPrimary(const std::string& path)
{
    if (!path.empty())
        draw(path);
}

void SceneRunner::drawSecondary(const std::string& path)
{
    if (!path.empty())
        draw(path);
}
