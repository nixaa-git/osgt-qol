#include "game/game.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

#include "game/signatures.hpp"
#include <processenv.h>

REGISTER_GAME_FUNCTION(GetSavePath,
                       "48 8B C4 55 48 8D A8 38 FF FF FF 48 81 EC C0 01 00 00 48 C7 44 24 38 FE FF "
                       "FF FF 48 89 58 10 48 89 70 18 48 89 78 20",
                       __fastcall, std::string);

REGISTER_GAME_FUNCTION(AppLoadVarDB,
                       "4C 8B DC 53 48 81 EC B0 00 00 00 49 C7 43 80 FE FF FF FF 48 8B ? ? ? ? ? "
                       "48 33 C4 48 89 84 24 A8 00 00 00 48 8B D9",
                       __fastcall, void, App*);

class SaveAndLogLocationFixer : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();

        // Check if we have environment value asking us to not do this.
        // This would primarily be set by launchers that can do this operation cleaner.
        TCHAR lpBuffer[64];
        DWORD dwRet;

        dwRet = GetEnvironmentVariable("LAUNCHER_CHDIR", lpBuffer, 64);

        if (dwRet != 0 && dwRet < 64)
        {
            if (std::string(lpBuffer) == "1")
                return;
        }

        // We will reload the game not via VariantDB itself, but via App so it gets whatever it
        // needs right.
        real::AppLoadVarDB = game.findMemoryPattern<AppLoadVarDB_t>(pattern::AppLoadVarDB);

        // We will replace normal AppData path with Current Directory.
        game.hookFunctionPatternDirect<GetSavePath_t>(pattern::GetSavePath, GetSavePath,
                                                      &real::GetSavePath);

        // Since we control the save path, we can reload save.dat from our new location.
        real::AppLoadVarDB(real::GetApp());
    }

    static std::string __fastcall GetSavePath()
    {
        TCHAR lpBuffer[MAX_PATH];
        DWORD dwRet;
        dwRet = GetCurrentDirectory(MAX_PATH, lpBuffer);
        if (dwRet == 0)
        {
            return real::GetSavePath();
        }
        return std::string(lpBuffer) + "\\";
    }
};
REGISTER_CORE_GAME_PATCH(SaveAndLogLocationFixer, save_and_log_location_fixer);