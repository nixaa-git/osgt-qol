#pragma once

// Incomplete type
#include <stdint.h>
#include <list>

struct InventoryItem
{
    short itemID;
    uint8_t quantity;
    bool bActive;
};
struct PlayerItems
{
    void* vftable;
    uint8_t pad[0x18];
    std::list<InventoryItem> m_items;
    short m_quickSlots[4];  // 48
    short m_maxSlots;
    uint8_t pad2[30];
};