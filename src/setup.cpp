#include "utils/hooking.hpp"
#include "utils/memory.hpp"
#include "utils/pattern.hpp"
#include <winuser.h>

using SetFPSLimit_t = void(__fastcall*)(void*, float);
using PetRenderDataUpdate_t = void(__fastcall*)(void*, void*, float);
SetFPSLimit_t SetFPSLimit;
PetRenderDataUpdate_t PetRenderData_Update;

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
    std::printf("Waiting for game initialization...\n");
    waitForGameInit();
    std::printf("Game initialized!\n");

    // De-arm the CRC check
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

    // Do SetFPS stuff.
    auto patternSetFPSLimit =
        pattern::parseIDA("4C 8B DC 48 81 EC D8 00 00 00 48 C7 44 24 20 FE FF FF FF 48 8B ? ? ? ? "
                          "? 48 33 C4 48 89 84 24 C0 00 00 00 0F 57 C0");
    if (!patternSetFPSLimit.has_value())
    {
        std::printf("Failed to parse SetFPSLimit pattern!\n");
        return;
    }
    auto addrSetFPSLimit = memory::find<SetFPSLimit_t>(patternSetFPSLimit.value());
    if (addrSetFPSLimit == nullptr)
    {
        std::printf("Failed to find SetFPSLimit address!\n");
        return;
    }
    std::printf("SetFPSLimit address: %p\n", addrSetFPSLimit);
    hooking::hookFunction<SetFPSLimit_t>(
        addrSetFPSLimit, [](void* rcx, float fps)
        {
            DEVMODE dm = {0};
            dm.dmSize = sizeof(DEVMODE);

            // Retrieve primary display refresh rate or fall back to vanilla 60 fps.
            if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
                SetFPSLimit(rcx, (float)dm.dmDisplayFrequency);
            else
                SetFPSLimit(rcx, 60.f);
        },
        &SetFPSLimit);

    // PetRenderData::Update
    auto patternPetRenderDataUpdate = pattern::parseIDA("48 8B C4 48 89 58 10 48 89 68 18 56 57 41 56 48 81 EC A0 00 00 00 0F B7");
    if (!patternPetRenderDataUpdate.has_value())
    {
        std::printf("Failed to parse PetRenderData::Update pattern!\n");
        return;
    }
    auto addrPetRenderDataUpdate =
    memory::find<PetRenderDataUpdate_t>(patternPetRenderDataUpdate.value());
    if (addrPetRenderDataUpdate == nullptr)
    {
        std::printf("Failed to find PetRenderData::Update pattern!\n");
        return;
    }
    std::printf("PetRenderData::Update address: %p\n", addrPetRenderDataUpdate);
    hooking::hookFunction<PetRenderDataUpdate_t>(
        addrPetRenderDataUpdate, [](void* a1, void* a2, float a3)
        {
            /* The game relies on using this for pet movement correction during fps fluctuations.
             * However, this logic only works properly until 60 FPS. So for our high-fps mod, we
             * patch this to force game to always run an update on pets movement, causing them to
             * move smoothly and as expected again.
             * There are some slight drawbacks to this approach, but it's relatively stable and good
             * enough for vast majority of players.
             */
            *(float*)((uint8_t*)(a1) + 92) = 0.016666668f;
            PetRenderData_Update(a1, a2, a3);
        },
        &PetRenderData_Update);
}