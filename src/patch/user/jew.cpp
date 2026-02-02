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
    void* _this);

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
    }

    enum class eBodyParts : unsigned short
    {
        HAT = 0,
        HAND = 1,
        BACKPACK = 2,
        PANTS = 3,
        CHEST = 4,
        SHOES = 5,
        FACE = 8,
    };

    static bool __fastcall TileIsDarkenedFn(void* _this)
    {
        return true;
    }

    static __int64 __fastcall AvatarRenderData_GetClothingItemsFn(void* _this, char a2)
    {
        __int64 clothingItems = real::AvatarRenderData_GetClothingItems(_this, a2);

        if (a2 == 1)
        {
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::FACE) = 2954;
            *(unsigned short*)((uintptr_t)clothingItems + (unsigned short)eBodyParts::HAT) = 1090;
        }
        return clothingItems;
    }

    static void __fastcall NetAvatar_SetCountryFn(void* _this, std::string country)
    {
        real::NetAvatar_SetCountry(_this, "il");
    }
};
REGISTER_USER_GAME_PATCH(Jew, jew_mod);