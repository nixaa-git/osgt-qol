#include "game/game.hpp"
#include "patch/patch.hpp"

class WeatherRenderFix : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Avoid setting smoothening on for sunset_water.rttex Surface
        // This makes the dark line less noticable on 3.02 clients.
        // TODO: Bisect client verions to see where it wasn't broken
        auto addr =
            game.findMemoryPattern<uint8_t*>("89 B7 BC 01 00 00 33 D2 48 8D 8F 80 01 00 00 E8");
        utils::nopMemory(addr, 20);
    }
};
REGISTER_USER_GAME_PATCH(WeatherRenderFix, weather_render_fix);