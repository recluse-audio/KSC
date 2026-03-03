#include "SceneView.h"
#include "../GRAPHICS_RENDERER/GraphicsRenderer.h"
#include "../SCENE/Scene.h"
#include "../ZONE/Zone.h"

SceneView::SceneView(GraphicsRenderer& renderer)
: mRenderer(renderer)
{
}

static bool endsWith(const std::string& s, const std::string& suffix)
{
    if (s.size() < suffix.size()) return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static void drawPath(GraphicsRenderer& renderer, const std::string& path, int x, int y)
{
    if (path.empty()) return;

    if (endsWith(path, ".png"))
        renderer.drawImage(path);
    else if (endsWith(path, ".md"))
        renderer.drawText(path, x, y);
    else if (endsWith(path, ".svg"))
        renderer.drawSVG(path, x, y);
}

void SceneView::draw(const Scene& scene, bool overlayVisible)
{
    if (scene.getPrimaryPath().empty())
    {
        for (auto zone : scene.getZones())
        {
            Zone::Bounds b = zone.getBounds();
            mRenderer.drawButton(zone.getLabel(), b.mX, b.mY, b.mW, b.mH);
        }
        return;
    }

    drawPath(mRenderer, scene.getPrimaryPath(), 0, 0);

    if (overlayVisible)
        drawPath(mRenderer, scene.getSecondaryPath(), 0, 20);
}

void SceneView::drawMenu(const Scene& menuScene)
{
    for (auto zone : menuScene.getZones())
    {
        Zone::Bounds b = zone.getBounds();
        mRenderer.drawButton(zone.getLabel(), b.mX, b.mY, b.mW, b.mH);
    }
}
