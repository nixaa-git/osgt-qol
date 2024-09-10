#include "utils/hooking.hpp"
#include "utils/memory.hpp"
#include "utils/pattern.hpp"
#include <winuser.h>

using SetFPSLimit_t = void(__fastcall*)(void*, float);
using PetRenderDataUpdate_t = void(__fastcall*)(void*, void*, float);

namespace real
{
SetFPSLimit_t SetFPSLimit = nullptr;
PetRenderDataUpdate_t PetRenderData_Update = nullptr;
} // namespace real

namespace hooked
{

void SetFPSLimit(void* this_, float fps)
{
    DEVMODE dm = {0};
    dm.dmSize = sizeof(DEVMODE);
    // Retrieve primary display refresh rate or fall back to vanilla 60 fps.
    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
        real::SetFPSLimit(this_, static_cast<float>(dm.dmDisplayFrequency));
    else
        real::SetFPSLimit(this_, 60.f);
}

void PetRenderDataUpdate(void* this_, void* unk2, float val)
{
    // The game relies on using this for pet movement correction during fps fluctuations. However,
    // this logic only works properly until 60 FPS. So for our high-fps mod, we patch this to force
    // game to always run an update on pets movement, causing them to move smoothly and as expected
    // again. There are some slight drawbacks to this approach, but it's relatively stable and good
    // enough for vast majority of players.
    *(float*)((uint8_t*)(this_) + 92) = 0.016666668f;
    real::PetRenderData_Update(this_, unk2, val);
}

} // namespace hooked

void waitForGameInit()
{
    // Blocks until game processes this message.
    SendMessageA(memory::getMainWindow(), WM_NULL, 0, 0);
}

void createConsole()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

// Entry point.
void setup()
{
    createConsole();
    waitForGameInit();
    std::printf("Game initialized!\n");

    // Patch out the the memory CRC check.
    auto patternCrcCheck = pattern::parseIDA("00 3B C1 75 ? 85 C9");
    if (!patternCrcCheck.has_value())
    {
        std::printf("Failed to parse CRC check pattern!\n");
        return;
    }
    auto addrCrcCheck = memory::find<uint8_t*>(patternCrcCheck.value());
    if (addrCrcCheck == nullptr)
    {
        std::printf("Failed to find CRC check address!\n");
        return;
    }
    memory::nop(((uint8_t*)addrCrcCheck) + 1, 6);
    printf("Patched out CRC check.\n");

    bool ok = false;
    // Hook SetFPSLimit.
    ok = hooking::hookFunctionPatternDirect<SetFPSLimit_t>(
        "4C 8B DC 48 81 EC D8 00 00 00 48 C7 44 24 20 FE FF FF FF "
        "48 8B ? ? ? ? ? 48 33 C4 48 89 84 24 C0 00 00 00 0F 57 C0",
        hooked::SetFPSLimit, &real::SetFPSLimit);
    if (!ok)
    {
        std::printf("Failed to hook SetFPSLimit!\n");
        return;
    }
    hooked::SetFPSLimit(nullptr, 60.0f);
    printf("Hooked SetFPSLimit.\n");

    // Hook PetRenderData::Update.
    ok = hooking::hookFunctionPatternDirect<PetRenderDataUpdate_t>(
        "48 8B C4 48 89 58 10 48 89 68 18 56 57 41 56 48 81 EC A0 00 00 00 0F B7",
        hooked::PetRenderDataUpdate, &real::PetRenderData_Update);
    if (!ok)
    {
        std::printf("Failed to hook PetRenderData::Update!\n");
        return;
    }
    printf("Hooked PetRenderData::Update.\n");
}