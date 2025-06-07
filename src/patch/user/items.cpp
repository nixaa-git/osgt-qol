#include "game/game.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

// ItemInfo::Serialize
REGISTER_GAME_FUNCTION(ItemInfoSerialize,
                       "40 53 55 56 57 48 81 EC 88 00 00 00 48 8B ? ? ? ? ? 48 33 C4 48 89 44 24 "
                       "48 4C 89 A4 24 80 00 00 00",
                       __thiscall, void, void* this_, char* pBytes, int* pSeek, bool bLoadFromMem,
                       uint16_t version, bool arg6);

// GetHorseItemTypes
REGISTER_GAME_FUNCTION(
    GetHorseItemTypes,
    "40 55 48 8B EC 48 83 EC 20 65 48 8B 04 25 58 00 00 00 8B ? ? ? ? ? BA 08 00 00 00 48 8B 0C C8 "
    "8B 04 0A 39 ? ? ? ? ? 0F 8F ? ? ? ? 4C 8B ? ? ? ? ? 48 8B ? ? ? ? ? 48 89 5C 24 38 48 89 74 "
    "24 40 48 89 7C 24 48 4C 3B C1 0F 85 ? ? ? ? 48 8D 45 10 C7 45 10 50",
    __fastcall, std::vector<int>*);

// GetCarItemTypes
REGISTER_GAME_FUNCTION(
    GetCarItemTypes,
    "40 55 48 8B EC 48 83 EC 20 65 48 8B 04 25 58 00 00 00 8B ? ? ? ? ? BA 08 00 00 00 48 8B 0C C8 "
    "8B 04 0A 39 ? ? ? ? ? 0F 8F ? ? ? ? 4C 8B ? ? ? ? ? 48 8B ? ? ? ? ? 48 89 5C 24 38 48 89 74 "
    "24 40 48 89 7C 24 48 4C 3B C1 0F 85 ? ? ? ? 48 8D 45 10 C7 45 10 FE",
    __fastcall, std::vector<int>*);

// NetAvatar::Gravity
REGISTER_GAME_FUNCTION(NetAvatarGravity, "40 53 48 83 EC 70 80 B9 A8 01 00 00 00", __fastcall, void,
                       void* this_);

class ItemsDatCrashMeNot : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // This patch extends the game's ability to parse newer items.dat files.
        // Although it does not store the said info for it.
        auto& game = game::GameHarness::get();

        game.hookFunctionPatternDirect<ItemInfoSerialize_t>(
            pattern::ItemInfoSerialize, ItemInfoSerialize, &real::ItemInfoSerialize);
    }

    static void __thiscall ItemInfoSerialize(void* this_, char* pBytes, int* pSeek,
                                             bool bLoadFromMem, uint16_t version, bool arg6)
    {
        real::ItemInfoSerialize(this_, pBytes, pSeek, bLoadFromMem, version, arg6);
        if (version >= 12)
            *pSeek += 13;
        if (version >= 13)
            *pSeek += 4; // Clock divider
        if (version >= 14)
            *pSeek += 4; // Parent ID
        if (version >= 15)
        {
            *pSeek += 25;
            // Alt sit path
            uint16_t strLen = *(uint16_t*)(pBytes + *pSeek);
            *pSeek += strLen + 2;
        }
        if (version >= 16)
        {
            // ItemRender xml
            uint16_t strLen = *(uint16_t*)(pBytes + *pSeek);
            *pSeek += strLen + 2;
        }
        if (version >= 17)
            *pSeek += 4;
        if (version >= 18)
            *pSeek += 4;
        if (version >= 19)
            *pSeek += 9;
        if (version >= 20)
            *pSeek += 1;
        if (version >= 21)
            *pSeek += 1;
    }
};
REGISTER_USER_GAME_PATCH(ItemsDatCrashMeNot, items_dat_crashmenot);

// This can be made into an client<->server api at some point. Kept here in comments, might be
// useful later.
/*
class RidesExtender : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        real::GetHorseItemTypes =
            game.findMemoryPattern<GetHorseItemTypes_t>(pattern::GetHorseItemTypes);
        real::GetCarItemTypes = game.findMemoryPattern<GetCarItemTypes_t>(pattern::GetCarItemTypes);
        std::vector<int>* horse = real::GetHorseItemTypes();
        std::vector<int>* car = real::GetCarItemTypes();

        car->push_back(16076);
    }
};
REGISTER_USER_GAME_PATCH(RidesExtender, extended_rides_demo);

class ParasolPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        game.hookFunctionPatternDirect<NetAvatarGravity_t>(
            pattern::NetAvatarGravity, NetAvatarGravity, &real::NetAvatarGravity);
    }

    static void __fastcall NetAvatarGravity(void* this_)
    {
        int OriginalID = *(int*)((uint8_t*)(this_) + 888);
        if (OriginalID == 98)
        {
            *(int*)((uint8_t*)(this_) + 888) = 1020;
        }
        real::NetAvatarGravity(this_);
        *(int*)((uint8_t*)(this_) + 888) = OriginalID;
    }
};
REGISTER_USER_GAME_PATCH(ParasolPatch, parasol_test);*/
