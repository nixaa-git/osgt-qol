#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/components/gamelogic.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

REGISTER_GAME_FUNCTION(WorldRenderer_DrawTile,
    "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 98",
    __fastcall,
    void*,
    void* _this,
    unsigned short a2,
    int a3,
    CL_Vec2f a4,
    unsigned int a5,
    void* pTile,
    bool a7,
    bool a8
);

class PerformanceTweaks : public patch::BasePatch
{
public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        auto& optionsMgr = game::OptionsManager::get();

        optionsMgr.addOptionPage("PerformanceTweaks", "Performance Tweaks");

        //game.hookFunctionPatternDirect<WorldRenderer_DrawTile_t>(pattern::WorldRenderer_DrawTile, WorldRenderer_DrawTileFn, &real::WorldRenderer_DrawTile);
    }

    static void* __fastcall WorldRenderer_DrawTileFn(void* _this, unsigned short a2, int a3,
                                                     CL_Vec2f a4, unsigned int a5, void* pTile,
                                                     bool a7, bool a8)
    {
        return nullptr;
    }
};
REGISTER_USER_GAME_PATCH(PerformanceTweaks, performance_tweaks);