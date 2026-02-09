#pragma once

#include "game/struct/vec.hpp"
#include <stdint.h>
struct GameUpdatePacket
{
    uint8_t m_packetType;
    uint8_t m_npcType;
    uint8_t m_removeQtyOrEntID;
    uint8_t m_addQtyOrAction;
    int m_netID;
    int m_objectID;
    int m_packetFlags;
    float m_floatVal;
    int m_itemObjIDOrDelay;
    CL_Vec2f m_position;
    CL_Vec2f m_velocity;
    float m_angle;
    int m_tileY;
    int m_tileX;
    int m_extendedDataLength;
};
