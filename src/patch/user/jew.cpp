#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

// shabbat shalom!
// will conflict with light source patch

REGISTER_GAME_FUNCTION(TileIsDarkened, 
    "48 89 5C 24 08 57 48 83 EC 20 48 63 D9",
    __fastcall,
    bool,
    void* _this
);

REGISTER_GAME_FUNCTION(AvatarRenderData_GetClothingItems,
    "84 D2 74 14 48",
    __fastcall,
    __int64,
    void* _this,
    char a2
);

REGISTER_GAME_FUNCTION(NetAvatar_SetCountry,
    "40 55 56 57 41 56 41 57 48 8D 6C 24 D0 "
    "48 81 EC 30 01 00 00 48 C7 44 24 70",
    __fastcall,
    void,
    void* _this,
    std::string country
);

REGISTER_GAME_FUNCTION(GameLogicComponent_GetLocalPlayer,
    "48 8B 81 88 01",
    __fastcall,
    __int64,
    void* _this
);

#define ITEM_ID_TUXEDO 38
#define ITEM_ID_BLACK_PANTS 76
#define ITEM_ID_BRIEFCASE 92
#define ITEM_ID_GAT 1090
#define ITEM_ID_BLACK_BRAIDS 2576
#define ITEM_ID_DISGUISE_GLASSES 2954

/*
plans:

nun rn
*/

class Jew : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        //game.hookFunctionPatternDirect<TileIsDarkened_t>(
        //    pattern::TileIsDarkened, TileIsDarkenedFn, &real::TileIsDarkened);

        game.hookFunctionPatternDirect<AvatarRenderData_GetClothingItems_t>(
            pattern::AvatarRenderData_GetClothingItems, AvatarRenderData_GetClothingItemsFn, &real::AvatarRenderData_GetClothingItems
        );

        game.hookFunctionPatternDirect<NetAvatar_SetCountry_t>(
            pattern::NetAvatar_SetCountry, NetAvatar_SetCountryFn, &real::NetAvatar_SetCountry
        );

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addOptionPage("Jew", "Jewish Patch");
        optionsMgr.addCheckboxOption("Jew", "Preferences", "osgt_qol_jew_israel_flag", 
                                     "Change all flags to Israel (Requires World Rejoin)", 
                                     &OnJewFlagSwitch
        );

        optionsMgr.addCheckboxOption("Jew", "Preferences", "osgt_qol_jew_set_outfits",
                                     "Jewish Outfits",
                                     &OnJewSetOutfit
        );

        optionsMgr.addCheckboxOption("Jew", "Preferences", "osgt_qol_jew_name_prefixes",
                                     "Add name prefixes such as [Goyim] and `3[``Mr Netanyahu`3]``",
                                    &OnJewPrefixSwitch
        );

        Variant* pVarFlag = real::GetApp()->GetVar("osgt_qol_jew_israel_flag");
        if (pVarFlag->GetType() == Variant::TYPE_UNUSED)
            pVarFlag->Set(1U);

        Variant* pVarOutfits = real::GetApp()->GetVar("osgt_qol_jew_set_outfits");
        if (pVarOutfits->GetType() == Variant::TYPE_UNUSED)
            pVarOutfits->Set(1U);
        
        Variant* pVarPrefixes = real::GetApp()->GetVar("osgt_qol_jew_name_prefixes");
        if (pVarPrefixes->GetType() == Variant::TYPE_UNUSED)
            pVarPrefixes->Set(1U);
    }

    static void OnJewFlagSwitch(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_jew_israel_flag")->Set(uint32_t(bChecked));
    }

    static void OnJewSetOutfit(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_jew_set_outfits")->Set(uint32_t(bChecked));
    }

    static void OnJewPrefixSwitch(VariantList* pVariant)
    {
        Entity* pCheckbox = pVariant->Get(1).GetEntity();
        bool bChecked = pCheckbox->GetVar("checked")->GetUINT32() != 0;
        real::GetApp()->GetVar("osgt_qol_jew_name_prefixes")->Set(uint32_t(bChecked));
    }
        
    enum class eBodyParts : unsigned short
    {
        HAT = 0,
        SHIRT = 2,
        PANTS = 4,
        SHOES = 6,
        FACE = 8,
        HAND = 10,
        WINGS = 12,
        HAIR = 14,
    };

    /*
    Clothing item at 0 is 1090
    Clothing item at 2 is 466
    Clothing item at 4 is 344
    Clothing item at 6 is 1178
    Clothing item at 8 is 2954
    Clothing item at 10 is 92
    Clothing item at 12 is 362
    Clothing item at 14 is 2576
    Clothing item at 16 is 0
    Clothing item at 18 is 234
    Clothing item at 20 is 466
    Clothing item at 22 is 344
    Clothing item at 24 is 1178
    */

    static bool __fastcall TileIsDarkenedFn(void* _this)
    {
        return true;
    }


    static __int64 __fastcall AvatarRenderData_GetClothingItemsFn(void* _this, char a2)
    {
        __int64 clothingItems = real::AvatarRenderData_GetClothingItems(_this, a2);

        if (a2 == 1 && real::GetApp()->GetVar("osgt_qol_jew_set_outfits")->GetUINT32() == 1)
        {
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::FACE) = ITEM_ID_DISGUISE_GLASSES;
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::HAT) = ITEM_ID_GAT;
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::HAIR) = ITEM_ID_BLACK_BRAIDS;
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::SHIRT) = ITEM_ID_TUXEDO;
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::PANTS) = ITEM_ID_BLACK_PANTS;
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::HAND) = ITEM_ID_BRIEFCASE;
        }
        return clothingItems;
    }

    static void __fastcall NetAvatar_SetCountryFn(void* _this, std::string country)
    {
        if (real::GetApp()->GetVar("osgt_qol_jew_name_prefixes")->GetUINT32() == 1)
        {
            std::string namecpy = *((std::string*)((uintptr_t)_this + 40));
            bool avatarModState = *((bool*)((uintptr_t)_this + 577));
    
            std::string namePrefix = (namecpy.find("Oslo") == std::string::npos) ? (avatarModState ? "`3[``Mr Netanyahu`3]`` "
                                                    : "`w[Chosen One]`` ") : "`5[Big Yahu]`` ";
    
            *((std::string*)((uintptr_t)_this + 40)) = std::string(namePrefix + namecpy);
        }
        
        if (real::GetApp()->GetVar("osgt_qol_jew_israel_flag")->GetUINT32() == 1)
        {
            real::NetAvatar_SetCountry(_this, "il");
            return;
        }
        
        real::NetAvatar_SetCountry(_this, country);
    }
};
REGISTER_USER_GAME_PATCH(Jew, jew_mod);