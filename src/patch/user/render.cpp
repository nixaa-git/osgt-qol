#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/graphics/background.hpp"
#include "game/struct/graphics/background_blood.hpp"
#include "game/struct/graphics/background_default.hpp"
#include "game/struct/graphics/surface.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

#include "game/struct/component.hpp"
#include "game/struct/components/mapbg.hpp"
#include "game/struct/entity.hpp"
#include "game/struct/renderutils.hpp"
#include "game/struct/rtrect.hpp"
#include "game/struct/variant.hpp"
#include <vcruntime_new_debug.h>

// BackgroundNight::BackgroundNight
REGISTER_GAME_FUNCTION(
    BackgroundNight,
    "48 89 4C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE "
    "FF FF FF 48 89 9C 24 C8 00 00 00",
    __fastcall, Background*, void*, int);

// BackgroundNight::Init
REGISTER_GAME_FUNCTION(
    BackgroundNightInit,
    "40 55 41 56 48 8B EC 48 83 EC 78 48 8B ? ? ? ? ? 48 33 C4 48 89 45 D8 4C 8B F1 84 D2 74 4A",
    __fastcall, void, Background*, bool);

// BackgroundDefault::~Background_Default
REGISTER_GAME_FUNCTION(BackgroundDefaultDtor,
                       "40 53 48 83 EC 20 48 8D 05 B3 C9 30 00 48 8B D9 48 89 01 48 8B 89 10 01",
                       __fastcall, void, Background*);

// MainMenuCreate
REGISTER_GAME_FUNCTION(
    MainMenuCreate,
    "48 8B C4 55 57 41 54 41 56 41 57 48 8D A8 E8 F8 FF FF 48 81 EC F0 07 00 00 48 C7 85 80 01",
    __fastcall, void, Entity*, bool);

// DrawFilledBitmapRect
REGISTER_GAME_FUNCTION(DrawFilledBitmapRect,
                       "48 83 EC 48 66 0F 6E 01 66 0F 6E 49 04 0F B6 44 24 70", __fastcall, void,
                       rtRectf&, uint32_t, uint32_t, void*, bool);

REGISTER_GAME_FUNCTION(
    WorldRendererForceBackground,
    "40 55 56 57 48 8B EC 48 83 EC 40 48 C7 45 E0 FE FF FF FF 48 89 5C 24 70 8B F2 48 8B F9 33",
    __fastcall, void, uint8_t*, int, void*, void*);

struct WorldRenderer
{
    uint8_t pad[200];
    Background* m_pWeather;
    int m_activeWeather;
};
class GoodNightTitleScreen : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Resolve functions
        real::BackgroundNight = game.findMemoryPattern<BackgroundNight_t>(pattern::BackgroundNight);
        real::BackgroundNightInit =
            game.findMemoryPattern<BackgroundNightInit_t>(pattern::BackgroundNightInit);
        real::BackgroundDefaultDtor =
            game.findMemoryPattern<BackgroundDefaultDtor_t>(pattern::BackgroundDefaultDtor);

        // Hook
        game.hookFunctionPatternDirect<MainMenuCreate_t>(pattern::MainMenuCreate, MainMenuCreate,
                                                         &real::MainMenuCreate);

        // Manually invoke weather change
        ChangeMainMenuWeather(real::GetEntityRoot()->GetEntityByName("GUI"));
    }

    static void __fastcall MainMenuCreate(Entity* pEnt, bool unk2)
    {
        // Let the game construct main menu for us.
        real::MainMenuCreate(pEnt, unk2);
        // After which we can change the weather without issue.
        ChangeMainMenuWeather(pEnt);
    }

    static void ChangeMainMenuWeather(Entity* pGUIEnt)
    {
        // Lets retrieve MapBGComponent from GUI -> MainMenu
        MapBGComponent* pMapBGComponent = reinterpret_cast<MapBGComponent*>(
            pGUIEnt->GetEntityByName("MainMenu")->GetComponentByName("MapBGComponent"));

        // Lets create our buffer for BackgroundNight, the struct size is 0x348
        void* buffer = operator new(0x348);
        // Pass it on to constructor, 2 is our "Active Weather ID". Game uses this to determine
        // if to create comet weather or just normal night weather.
        Background* pBackgroundNight = real::BackgroundNight(buffer, 2);
        real::BackgroundNightInit(pBackgroundNight, false);

        // Take a note of current weather.
        Background_Default* pOriginalWeather = (Background_Default*)pMapBGComponent->m_pBackground;
        // Assign our new one in place.
        pMapBGComponent->m_pBackground = pBackgroundNight;
        // Discard the original.
        real::BackgroundDefaultDtor(pOriginalWeather);
    }
};
REGISTER_USER_GAME_PATCH(GoodNightTitleScreen, goodnight_title_screen);

class BubbleOpacityBackport : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Bubble opacity is a vanilla feature, albeit from future version, it should go in save.dat
        Variant* pVariant = real::GetApp()->GetVar("speech_bubble_opacity");
        if (pVariant->GetType() != Variant::TYPE_FLOAT)
            pVariant->Set(1.00f);

        // Add the slider back into options. Normally it's between the music/sfx/gfx sliders, but
        // it'd be too messy to ram it between them. OptionsManager will move it to dedicated
        // "OSGT-QOL Options" area.
        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addSliderOption("speech_bubble_opacity", "Bubble Opacity",
                                   &BubbleOpacitySliderCallback);

        // Hook
        game.hookFunctionPatternDirect<DrawFilledBitmapRect_t>(
            pattern::DrawFilledBitmapRect, DrawFilledBitmapRect, &real::DrawFilledBitmapRect);
    }

    static void BubbleOpacitySliderCallback(Variant* pVariant)
    {
        real::GetApp()->GetVar("speech_bubble_opacity")->Set(pVariant->GetFloat());
    }

    static void __fastcall DrawFilledBitmapRect(rtRectf& r, uint32_t middleColor,
                                                uint32_t borderColor, void* pSurf,
                                                bool bFillMiddleCloserToEdges)
    {
        float opacity = real::GetApp()->GetVar("speech_bubble_opacity")->GetFloat();
        middleColor = ModAlpha(middleColor, (((float)GET_ALPHA(middleColor)) / 255.0f) * opacity);
        borderColor = ModAlpha(borderColor, (((float)GET_ALPHA(borderColor)) / 255.0f) * opacity);
        real::DrawFilledBitmapRect(r, middleColor, borderColor, pSurf, bFillMiddleCloserToEdges);
    }
};
REGISTER_USER_GAME_PATCH(BubbleOpacityBackport, bubble_opacity_backport);

class RemoveCheckboxPadding : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Reverts the checkbox spacings back to how they were since 2013.

        auto& game = game::GameHarness::get();
        auto addr = game.findMemoryPattern<uint8_t*>(
            "E8 ? ? ? ? C7 00 03 00 00 00 F3 44 0F 11 40 10 F3 0F 11");
        // They multiplied some values related to size2d at end of function here in V2.997+. By
        // patching out the 2 MOVSS instructions in CreateCheckbox, we essentially revert that
        // change.
        utils::nopMemory(addr + 11, 11);
    }
};
REGISTER_USER_GAME_PATCH(RemoveCheckboxPadding, old_checkboxes);

class BloodMoonDemoWeather : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // This is a proof-of-concept demo. This should be providerized for other patches to add to
        // this.
        auto& game = game::GameHarness::get();
        // Hook.
        game.hookFunctionPatternDirect<WorldRendererForceBackground_t>(
            pattern::WorldRendererForceBackground, WorldRendererForceBackground,
            &real::WorldRendererForceBackground);
    }
    static void __fastcall WorldRendererForceBackground(uint8_t* this_, int WeatherID, void* unk3,
                                                        void* unk4)
    {
        if (WeatherID == 100)
        {
            // TODO: Providerize this.
            WorldRenderer* pRender = reinterpret_cast<WorldRenderer*>(this_);
            if (pRender->m_pWeather != 0)
                delete pRender->m_pWeather;
            Background_Blood* pNewBG = new Background_Blood();
            // We don't have a good test case right now for this one.
            // Vec2f scale(*(float*)(this_ + 168), *(float*)(this_ + 172));
            // pNewBG->SetScale(scale);
            pNewBG->Init(true);
            pRender->m_activeWeather = WeatherID;
            pRender->m_pWeather = pNewBG;
        }
        else
            real::WorldRendererForceBackground(this_, WeatherID, unk3, unk4);
        return;
    }
};
REGISTER_USER_GAME_PATCH(BloodMoonDemoWeather, blood_moon_demo_weather);
