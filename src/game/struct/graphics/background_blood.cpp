#include "background_blood.hpp"
#include "game/signatures.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "game/struct/graphics/background_clouds.hpp"
#include "game/struct/graphics/surface.hpp"
#include "game/struct/renderutils.hpp"
#include "game/struct/vec.hpp"

#include <string>

Background_Blood::Background_Blood()
{
    // Load surfs and setup them.
    m_surf_moon.SetBlendingMode(Surface::BLENDING_PREMULTIPLIED_ALPHA);
    m_surf_moon.LoadFile("game/moon.rttex");
    m_surf_moon.SetupAnim(1, 1);

    m_surfArray = new Surface[3]();

    m_surfArray[0].SetBlendingMode(Surface::BLENDING_PREMULTIPLIED_ALPHA);
    m_surfArray[1].SetBlendingMode(Surface::BLENDING_PREMULTIPLIED_ALPHA);
    m_surfArray[2].SetBlendingMode(Surface::BLENDING_PREMULTIPLIED_ALPHA);

    m_surfArray[0].LoadFile("game/hills3.rttex");
    m_surfArray[1].LoadFile("game/hills2.rttex");
    m_surfArray[2].LoadFile("game/hills1.rttex");

    m_pBGClouds = new Background_Clouds((Background*)this, "game/cloud.rttex", 0, 0);
}

Background_Blood::~Background_Blood()
{
    delete m_pBGClouds;
    delete[] m_surfArray;
}

void Background_Blood::Render(CL_Vec2f& vScreenPos, float graphicDetail)
{
    // Need to find out what this vec is for. Looks like it's related to some bgfx rotation matrix?
    CL_Vec2f unk4(0.0, 0.0);
    unsigned int skyColour = MAKE_RGBA(90, 10, 10, 255);
    // Get our screen size
    Rectf screenRect;
    real::GetScreenRect(screenRect);
    // Draw the backdrop
    real::DrawFilledRect(screenRect, skyColour, 0.0f, &unk4);
    if (graphicDetail > 0.1)
    {
        // Draw all the "high detail" assets, e.g. sun/hills/clouds.
        CL_Vec2f moonScale = CL_Vec2f((m_screenSize.x / 161.0f) / 6.0f);
        m_surf_moon.BlitScaledAnim(m_screenSize.x * 0.7f, m_screenSize.y * 0.1f, 0, 0, &moonScale,
                                   0);
        // Draw clouds piecemeal e.g. 0-20%, 20-40% to be between hill layers.
        m_pBGClouds->Render(vScreenPos, graphicDetail, 0.0f, 0.2f);
        DrawHill(0, vScreenPos);
        m_pBGClouds->Render(vScreenPos, graphicDetail, 0.2f, 0.4f);
        DrawHill(1, vScreenPos);
        m_pBGClouds->Render(vScreenPos, graphicDetail, 0.4f, 0.6f);
        DrawHill(2, vScreenPos);
        m_pBGClouds->Render(vScreenPos, graphicDetail, 0.6f, 1.0f);
        // Draw the weather change fade effect.
        if (m_fadeProgress > 0.0f)
        {
            real::DrawFilledRect(screenRect, -0x100 - (int)(m_fadeProgress * -255.0), 0.0f, &unk4);
            m_fadeProgress = m_fadeProgress - (real::GetApp()->GetDeltaTick() / 1000.0f);
        }
    }
}

void Background_Blood::Init(bool bInWorld)
{
    if (!bInWorld)
    {
        real::GetScreenRect(m_renderRect);
        m_pBGClouds->InitClouds(20);
    }
    else
    {
        m_renderRect.left = 0;
        m_renderRect.top = 0;
        m_renderRect.bottom = real::GetApp()->GetGameLogic()->GetTileHeight() * 32.0f;
        m_renderRect.right = real::GetApp()->GetGameLogic()->GetTileWidth() * 32.0f;
        m_pBGClouds->InitClouds(50);
    }
}

void Background_Blood::Update()
{
    m_pBGClouds->Update();
    return;
}

void Background_Blood::DrawHill(int hillLevel, CL_Vec2f bounds)
{
    // Scale proportional to the user's screen width
    float hillScale = m_screenSize.x / 512.0f;

    // Use the passed hill level to determine parallax scrolling speed and progress.
    float parallaxStep = (float)(hillLevel + 1) * -0.2f;
    float parallaxScroll = floorf((float)(parallaxStep * bounds.x) * m_scale.x);

    // Determine X location after scrolling
    float x = 0.0f;
    do
    {
        x = parallaxScroll;
        parallaxScroll = x + m_screenSize.x;
    } while (parallaxScroll < 0.0);

    // Fix the hill heights. The game fixes these 2.0f above.
    float hillHeight = m_screenSize.y + 2.0f;
    float y = hillHeight;
    if (hillLevel == 0)
        y = hillHeight - hillScale * 46.0f;
    else if (hillLevel == 1)
        y = hillScale * 60.0f + hillHeight;

    unsigned int RGBA = MAKE_RGBA(255, 40, 40, 255);
    for (; x <= m_screenSize.x; x = x + hillScale * 512.0f)
    {
        CL_Vec2f vScale(hillScale);
        m_surfArray[hillLevel].BlitScaled(x, y, vScale, 4, RGBA, 0.0, NULL, false, false);
    }
}