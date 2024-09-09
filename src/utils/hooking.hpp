#pragma once
#include "minhook/minhook.h"
#include <cstdio>
#include <type_traits>

namespace hooking
{

inline bool initialized = false;

// Hook a function.
template <typename F> bool hookFunction(F target, F detour, F* original);

} // namespace hooking

namespace hooking // Template function implementations.
{

template <typename F> bool hookFunction(F target, F detour, F* original)
{
    static_assert(std::is_pointer_v<F> && std::is_function_v<std::remove_pointer_t<F>>,
                  "F must be a function pointer type.");

    MH_STATUS status;
    if (!initialized)
    {
        if ((status = MH_Initialize()) != MH_OK)
        {
            std::printf("Failed to initialize hooking. (MH_STATUS=%d)\n", status);
            return false;
        }
        initialized = true;
    }
    if (((status = MH_CreateHook(target, detour, reinterpret_cast<LPVOID*>(original))) != MH_OK))
    {
        std::printf("Failed to create hook at 0x%p (MH_STATUS=%d).\n", target, status);
        return false;
    }
    if (((status = MH_EnableHook(target)) != MH_OK))
    {
        std::printf("Failed to enable hook at 0x%p (MH_STATUS=%d).\n", target, status);
        return false;
    }
    return true;
}

} // namespace hooking
