#include "game.hpp"

REGISTER_GAME_FUNCTION(
    ItemInfoManagerLoadFromMem,
    "48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 57 41 56 41 57 48 83 EC 40 48 8B 71", __thiscall,
    void, void*, char*, bool);

namespace game
{

ItemAPI& ItemAPI::get()
{
    static ItemAPI instance;
    return instance;
}

void game::ItemAPI::initialize()
{
    auto& game = game::GameHarness::get();

    // Hook.
    game.hookFunctionPatternDirect<ItemInfoManagerLoadFromMem_t>(
        pattern::ItemInfoManagerLoadFromMem, ItemInfoManagerLoadFromMem,
        &real::ItemInfoManagerLoadFromMem);
}

void game::ItemAPI::ItemInfoManagerLoadFromMem(void* this_, char* pBytes, bool arg3)
{
    real::ItemInfoManagerLoadFromMem(this_, pBytes, arg3);

    printf("ItemAPI emitting LoadFromMem finished event.\n");
    auto& itemAPI = game::ItemAPI::get();
    (itemAPI.m_sig_loadFromMem)();
}

}; // namespace game