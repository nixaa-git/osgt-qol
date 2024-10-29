#pragma once
#include "game/struct/component.hpp"
#include "game/struct/graphics/background.hpp"

// Expected size: 264 bytes
class MapBGComponent : public EntityComponent
{
  public:
    uint8_t pad[32];
    Background* m_pBackground;
    void* m_pUnk1;
};
static_assert(sizeof(MapBGComponent) == 264, "MapBGComponent class size mismatch.");