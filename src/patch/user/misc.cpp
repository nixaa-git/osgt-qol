#include "game/game.hpp"
#include "game/signatures.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

static uint8_t* createAccSecurityAddr;
class PauseMenuNoAAP : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Removes the "Secure my Account" button when desired from Pause Menu to restore visual
        // identity of the client.
        // This patch can be "unpatched" and "repatched" on the fly due to how simple the changes
        // done are.
        auto& game = game::GameHarness::get();

        // Default it to enabled.
        Variant* pVariant = real::GetApp()->GetVar("osgt_qol_hide_aap_button");
        if (pVariant->GetType() == Variant::TYPE_UNUSED)
            pVariant->Set(1U);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addCheckboxOption("qol", "UI", "osgt_qol_hide_aap_button",
                                     "Hide \"Secure my Account\" text from Pause Menu",
                                     &OnCheckboxCallback);

        createAccSecurityAddr = game.findMemoryPattern<uint8_t*>(
            "0F 84 ? ? ? ? E8 ? ? ? ? 80 B8 15 02 00 00 00 0F 85 ? ? ? ? E8");

        patchIfNeeded();
    }

    static void OnCheckboxCallback(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_hide_aap_button")->Set(uint32_t(bChecked));
        patchIfNeeded();
    }

    static void patchIfNeeded()
    {
        // Rewrite a JZ to JMP to skip "Secure My Account" block from ever executing.
        if (real::GetApp()->GetVar("osgt_qol_hide_aap_button")->GetUINT32() == 1)
            utils::writeMemoryPattern(createAccSecurityAddr, "90 E9");
        else
            utils::writeMemoryPattern(createAccSecurityAddr, "0F 84");
    }
};
REGISTER_USER_GAME_PATCH(PauseMenuNoAAP, pause_menu_no_aap);
