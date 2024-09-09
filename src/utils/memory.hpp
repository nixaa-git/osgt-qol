#pragma once
#include "pattern.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <psapi.h>

namespace memory
{

// Get base address of where the game is loaded in memory.
inline HMODULE getBaseAddress() { return GetModuleHandleA(nullptr); }

// Get handle to the game window.
inline HWND getMainWindow() { return FindWindowA(nullptr, "Growtopia"); }

// Write data from a buffer to a memory address.
bool write(void* address, const std::vector<uint8_t> data);

// Fill a memory region with a specific value.
bool fill(void* address, size_t size, uint8_t val);

// NOP a memory region.
bool nop(void* address, size_t size);

// Find a specific pattern in memory.
template <typename T = void*> T find(const std::string& pattern);

// Get destination of x64 relative call instruction (E8).
template <typename F> inline F resolveRelCall(const void* address);

} // namespace memory

namespace memory // Template function implementations.
{

template <typename T> T find(const BytePattern& pattern)
{
    static_assert(std::is_pointer_v<T>, "T must be a pointer type.");
    // Calculate search begin & end boundaries.
    MODULEINFO mi;
    if (!GetModuleInformation(GetCurrentProcess(), getBaseAddress(), &mi, sizeof(mi)))
        return nullptr;
    uint8_t* begin = reinterpret_cast<uint8_t*>(mi.lpBaseOfDll);
    uint8_t* end = begin + mi.SizeOfImage;
    // Perform search.
    while (begin < end)
    {
        bool match = true;
        for (size_t i = 0; i < pattern.size(); i++)
        {
            if (!pattern[i].has_value())
                continue;
            if (pattern[i].value() != begin[i])
            {
                match = false;
                break;
            }
        }
        if (match)
            return reinterpret_cast<T>(begin);
        begin++;
    }
    return nullptr;
}

template <typename F> inline F resolveRelCall(const void* address)
{
    static_assert(std::is_pointer_v<F> && std::is_function_v<std::remove_pointer_t<F>>,
                  "F must be a function pointer type.");
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(address);
    if (ptr == NULL || *ptr != 0xE8)
        return nullptr;
    int32_t offset;
    std::memcpy(&offset, ptr + 1, sizeof(offset));
    return reinterpret_cast<F>(ptr + 5 + offset);
}

} // namespace memory