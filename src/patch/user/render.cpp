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

// Background_Night::Background_Night
REGISTER_GAME_FUNCTION(
    BackgroundNight,
    "48 89 4C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE "
    "FF FF FF 48 89 9C 24 C8 00 00 00",
    __fastcall, Background*, void*, int);

// Background_Undersea::Background_Undersea
REGISTER_GAME_FUNCTION(BackgroundUndersea,
                       "48 89 4C 24 08 55 56 57 48 83 EC 70 48 C7 44 24 20 FE FF FF FF", __fastcall,
                       Background*, void*);

// Background_Monochrome::Background_Monochrome
REGISTER_GAME_FUNCTION(
    BackgroundMonochrome,
    "48 89 4C 24 08 55 56 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 B0 FE FF FF FF 48 89 9C 24 B8 "
    "00 00 00 48 8B F9 E8 ? ? ? ? 90 48 8D ? ? ? ? ? 48 89 07 48 8D 8F 10 01 00 00 E8 ? ? ? ?",
    __fastcall, Background*, void*);

// DrawFilledBitmapRect
REGISTER_GAME_FUNCTION(DrawFilledBitmapRect,
                       "48 83 EC 48 66 0F 6E 01 66 0F 6E 49 04 0F B6 44 24 70", __fastcall, void,
                       rtRectf&, uint32_t, uint32_t, void*, bool);

static std::vector<std::string> displayNames;
class CustomizedTitleScreen : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Resolve functions
        real::BackgroundNight = game.findMemoryPattern<BackgroundNight_t>(pattern::BackgroundNight);
        real::BackgroundUndersea =
            game.findMemoryPattern<BackgroundUndersea_t>(pattern::BackgroundUndersea);
        real::BackgroundMonochrome =
            game.findMemoryPattern<BackgroundMonochrome_t>(pattern::BackgroundMonochrome);

        // Hook
        // pattern::MainMenuCreate collision with drawing.cpp - so we define pattern manually here.
        game.hookFunctionPatternDirect<MainMenuCreate_t>(
            "48 8B C4 55 57 41 54 41 56 41 57 48 8D A8 E8 F8 FF FF 48 81 EC F0 07 00 00 48 C7 85 "
            "80 01",
            MainMenuCreate, &real::MainMenuCreate);
        patched::MainMenuCreate = MainMenuCreate;

        // We will allow the end-user to change their title screen weather preference
        auto& optionsMgr = game::OptionsManager::get();
        // Populate our options
        displayNames.push_back("Sunny");
        displayNames.push_back("Night");
        displayNames.push_back("Undersea");
        displayNames.push_back("Comet (Blue)");
        displayNames.push_back("Spring");
        displayNames.push_back("Monochrome");
        // Register the Multi-Choice option
        optionsMgr.addMultiChoiceOption("osgt_qol_title_bg", "Title Background", displayNames,
                                        &TitleBackgroundOnSelect);
    }

    static void TitleBackgroundOnSelect(VariantList* pVariant)
    {
        // Update the weather index
        Entity* pClickedEnt = pVariant->Get(1).GetEntity();
        Variant* pOptVar = real::GetApp()->GetVar("osgt_qol_title_bg");
        uint32_t idx = pOptVar->GetUINT32();
        if (pClickedEnt->GetName() == "back")
        {
            if (idx == 0)
                idx = (uint32_t)displayNames.size() - 1;
            else
                idx--;
        }
        else if (pClickedEnt->GetName() == "next")
        {
            if (idx >= displayNames.size() - 1)
                idx = 0;
            else
                idx++;
        }
        pOptVar->Set(idx);
        // Update the option label
        Entity* pTextLabel = pClickedEnt->GetParent()->GetEntityByName("txt");
        real::SetTextEntity(pTextLabel, displayNames[idx]);
        Entity* pGUIEnt = real::GetApp()->m_entityRoot->GetEntityByName("GUI");
        if (pGUIEnt->GetEntityByName("MainMenu"))
            ChangeMainMenuWeather(pGUIEnt);
    }

    static void __fastcall MainMenuCreate(Entity* pEnt, bool unk2)
    {
        // Let the game construct main menu for us.
        real::MainMenuCreate(pEnt, unk2);
        // After which we can change the weather without issue.
        ChangeMainMenuWeather(pEnt);

        // Lets also add modloader version label to the menuscreen to the opposite of version
        Entity* pVerLabel = pEnt->GetEntityByName("MainMenu")->GetEntityByName("version");
        CL_Vec2f m_verLabelPos = pVerLabel->GetVar("pos2d")->GetVector2();
        CL_Vec2f m_verLabelSize = pVerLabel->GetVar("size2d")->GetVector2();

        Entity* pTextLabel = real::CreateTextLabelEntity(pEnt->GetEntityByName("MainMenu"), "mltxt",
                                                         0, m_verLabelPos.y - m_verLabelSize.y,
                                                         "`wOSGT-QOL V1.0-ALPHA``");
        // Retrieve fontscale and scale created entity
        uint32_t fontID;
        float fontScale;
        real::GetFontAndScaleToFitThisLinesPerScreenY(fontID, fontScale, 20);
        real::SetupTextEntity(pTextLabel, fontID, fontScale);
        // Also needs fadeinentity
    }

    static void ChangeMainMenuWeather(Entity* pGUIEnt)
    {
        // Lets retrieve MapBGComponent from GUI -> MainMenu
        MapBGComponent* pMapBGComponent = reinterpret_cast<MapBGComponent*>(
            pGUIEnt->GetEntityByName("MainMenu")->GetComponentByName("MapBGComponent"));

        uint32_t weatherIdx = real::GetApp()->GetVar("osgt_qol_title_bg")->GetUINT32();
        Background* pNewBG;
        switch (weatherIdx)
        {
        case 0:
        {
            pNewBG = new Background_Default();
            break;
        }
        case 1:
        {
            // For the weathers we don't have a reversed class for, create a raw buffer.
            // In case of Background_Night, it should be 0x348 in size.
            void* buffer = operator new(0x348);
            // Background_Night takes an additional value in constructor, namely the weather ID
            // which dictates if it should render it as normal night or a comet weather.
            pNewBG = real::BackgroundNight(buffer, 2);
            break;
        }
        case 2:
        {
            // Undersea
            void* buffer = operator new(0x6a0);
            pNewBG = real::BackgroundUndersea(buffer);
            break;
        }
        case 3:
        {
            // Comet (Blue/Weather)
            void* buffer = operator new(0x348);
            pNewBG = real::BackgroundNight(buffer, 17);
            break;
        }
        case 4:
        {
            // Spring
            pNewBG = new Background_Default();
            ((Background_Default*)pNewBG)->m_bIsSpring = true;
            break;
        }
        case 5:
        {
            // Monochrome
            void* buffer = operator new(0x198);
            pNewBG = real::BackgroundMonochrome(buffer);
            break;
        }
        default:
            break;
        }

        if (pNewBG != nullptr)
        {
            // Initialize the weather with bInWorld as false.
            pNewBG->Init(false);
            // Save our old one to delete in a brief second.
            Background* pOriginalWeather = (Background*)pMapBGComponent->m_pBackground;
            // Assign our new one in place.
            pMapBGComponent->m_pBackground = pNewBG;
            // Discard the original.
            delete pOriginalWeather;
        }
    }
};
REGISTER_USER_GAME_PATCH(CustomizedTitleScreen, customized_title_screen);

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
        // This is a demo mod showcasing how to register a custom weather.
        // See src/game/struct/graphics/backgound_blood.cpp for the weather implementation.
        auto& weatherMgr = game::WeatherManager::get();
        weatherMgr.registerWeather("blood_moon", &onWeatherCreate);
    }

    static Background* onWeatherCreate() { return new Background_Blood(); }
};
REGISTER_USER_GAME_PATCH(BloodMoonDemoWeather, blood_moon_demo_weather);
