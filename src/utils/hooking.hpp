#pragma once
#include "memory.hpp"
#include "minhook/minhook.h"
#include "pattern.hpp"
#include <windows.h>

namespace hooking
{

inline bool initialized = false;

// Hook a function via address.
template <typename F> bool hookFunction(F target, F detour, F* original);

// Hook a function via IDA pattern (direct).
template <typename F>
bool hookFunctionPatternDirect(const std::string& target, F detour, F* original);

// Hook a function via IDA pattern (resolve function call).
template <typename F>
bool hookFunctionPatternCall(const std::string& target, F detour, F* original, size_t offset = 0);

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

template <typename F>
bool hookFunctionPatternDirect(const std::string& target, F detour, F* original)
{
    // Find pattern address and hook it.
    auto pattern = pattern::parseIDA(target);
    if (!pattern.has_value())
    {
        std::printf("Failed to parse pattern '%s'.\n", target.c_str());
        return false;
    }
    auto addr = memory::find<F>(pattern.value());
    if (addr == nullptr)
    {
        std::printf("Failed to find pattern '%s'.\n", target.c_str());
        return false;
    }
    return hookFunction<F>(addr, detour, original);
}

template <typename F>
bool hookFunctionPatternCall(const std::string& target, F detour, F* original, size_t offset)
{
    // Find pattern address and hook it.
    auto pattern = pattern::parseIDA(target);
    if (!pattern.has_value())
    {
        std::printf("Failed to parse pattern '%s'.\n", target.c_str());
        return false;
    }
    auto addr = memory::find<F>(pattern.value());
    if (addr == nullptr)
    {
        std::printf("Failed to find pattern '%s'.\n", target.c_str());
        return false;
    }
    // Resolve function call.
    auto call = memory::resolveRelCall(addr + offset);
    if (call == nullptr)
    {
        std::printf("Failed to resolve call at 0x%p.\n", addr + offset);
        return false;
    }
    return hookFunction<F>(addr, detour, original);
}

} // namespace hooking