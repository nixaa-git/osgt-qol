#include "game.hpp"
#include <format>
#include <string>

#include "game/entity.hpp"

REGISTER_GAME_FUNCTION(AboutMenuAddScrollContent,
                       "48 8B C4 55 41 56 41 57 48 8D A8 F8 FB FF FF 48 81 EC F0 04",
                       __fastcall, void, Entity*);

REGISTER_GAME_FUNCTION(ResizeScrollBounds,
                       "48 8B C4 55 48 8D 68 A1 48 81 EC D0 00 00 00 48 C7 45 A7 FE FF FF FF 48 89 "
                       "58 10 48 89 70 18 48 89 78",
                       __fastcall, void, VariantList*);

REGISTER_GAME_FUNCTION(SetTextEntity,
                       "48 8B C4 55 57 41 56 48 8D 68 A1 48 81 EC B0 00 00 00 48 C7 45 B7 FE FF FF "
                       "FF 48 89 58 18 48 89 70 20 48",
                       __fastcall, void, Entity*, std::string);

class AboutMenuAttribution : public game::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Setup borrowed game functions.s
        real::SetTextEntity = game.findMemoryPattern<SetTextEntity_t>(pattern::SetTextEntity);
        real::ResizeScrollBounds =
            game.findMemoryPattern<ResizeScrollBounds_t>(pattern::ResizeScrollBounds);
        // Hook AboutMenuAddScrollContent.
        game.hookFunctionPatternDirect<AboutMenuAddScrollContent_t>(
            pattern::AboutMenuAddScrollContent, AboutMenuAddScrollContent,
            &real::AboutMenuAddScrollContent);
    }

    static void __fastcall AboutMenuAddScrollContent(Entity* pScrollChild)
    {
        // Let about menu construct itself normally, we can't really do a pre-hook
        // here since the function deletes all children by default.
        real::AboutMenuAddScrollContent(pScrollChild);
        // We take over TextBox2 (the last Entity under children) and insert our own
        // attribution logic to it.
        Entity* pTextBox2 = pScrollChild->GetChildren()->back();
        EntityComponent* pTextComponent = pTextBox2->GetComponentByName("TextBoxRender");
        if (pTextComponent != nullptr)
        {
            Variant* pTextVariant = pTextComponent->GetShared()->GetVarIfExists("text");
            if (pTextVariant != nullptr)
            {
                auto& game = game::GameHarness::get();
                // Create our attribution.
                std::string osgtQolCredits = std::format(
                    "\n`wOSGT-QOL V1.0``\n\n`6Game modification created by `whouz`` and "
                    "`wCernodile``. There are currently `w{}`` patches loaded.``",
                    game.getAppliedPatchCount() - 1);
                real::SetTextEntity(pTextBox2, pTextVariant->GetString() + osgtQolCredits);
                // Signal proton to resize the menu for our text to be visible.
                VariantList vl(pScrollChild->GetParent()->GetParent());
                real::ResizeScrollBounds(&vl);
            }
        }
    }
};
REGISTER_GAME_PATCH(AboutMenuAttribution, about_menu_attrib);