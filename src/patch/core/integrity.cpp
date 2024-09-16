#include "game/game.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

class IntegrityPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Patch out CRC integrity check.
        auto addr = game.findMemoryPattern<uint8_t*>("00 3B C1 75 ? 85 C9");
        utils::nopMemory(addr + 1, 6);
    }
};
REGISTER_CORE_GAME_PATCH(IntegrityPatch, integrity_bypass);