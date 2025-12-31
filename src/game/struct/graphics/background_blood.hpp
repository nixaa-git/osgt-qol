#pragma once
#include "background.hpp"
#include "background_clouds.hpp"
#include "surface.hpp"

// Clone of Background_Default tinted with red and a moon sprite for Proof-of-Concept on custom
// weathers.
class Background_Blood : public Background
{
  public:
    Background_Blood();
    virtual ~Background_Blood();

    virtual void Render(CL_Vec2f& screenSize, float graphicDetail);
    virtual void Init(bool);
    virtual void Update();

    void DrawHill(int hillLevel, CL_Vec2f bounds);

    Background_Clouds* m_pBGClouds = nullptr;
    SurfaceAnim m_surf_moon;
    Surface* m_surfArray;
};
