#include "utils/hooking.hpp"
#include "utils/memory.hpp"
#include "utils/pattern.hpp"
#include <winuser.h>

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

    // Do SetFPS stuff.
    using SetFPSLimit_t = void(__fastcall*)(void*, float);
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
        { std::printf("SetFPSLimit hook called with fps: %.2f\n", fps); }, nullptr);
}