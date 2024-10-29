#pragma once
#include "background.hpp"
#include "background_clouds.hpp"
#include "surface.hpp"

class Background_Default : public Background
{
  public:
    Background_Default();
    virtual ~Background_Default();

    virtual void Render(Vec2f& screenSize, float graphicDetail);
    virtual void Init(bool);
    virtual void Update();

    void DrawHill(int hillLevel, Vec2f bounds);

    bool m_bIsSpring;
    Background_Clouds* m_pBGClouds;
    SurfaceAnim m_surf_sun;
    Surface* m_surfArray;
};
static_assert(sizeof(Background_Default) == 416, "BG_default mismatch");