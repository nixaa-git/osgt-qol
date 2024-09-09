#include "memory.hpp"

namespace memory
{

bool write(void* address, std::vector<uint8_t> data)
{
    DWORD old = 0;
    if (!VirtualProtect(address, data.size(), PAGE_EXECUTE_READWRITE, &old))
        return false;
    std::memcpy(address, data.data(), data.size());
    return VirtualProtect(address, data.size(), old, &old);
}

bool fill(void* address, size_t size, uint8_t val)
{
    DWORD old = 0;
    if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old))
        return false;
    std::memset(address, val, size);
    return VirtualProtect(address, size, old, &old);
}

bool nop(void* address, size_t size) { return fill(address, size, 0x90); }

} // namespace memory