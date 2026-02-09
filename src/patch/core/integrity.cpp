#include "game/game.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

#include "game/struct/net/gameupdatepacket.hpp"

// DownloadFileComponent::Init
REGISTER_GAME_FUNCTION(DownloadFileComponentInit,
                       "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 28 FE FF FF", __fastcall,
                       void, void*, std::string, void*, void*)

// GameLogicComponent::HandleBattleEvent
REGISTER_GAME_FUNCTION(GameLogicComponentHandleBattleEvent,
                       "40 55 56 57 41 56 41 57 48 8D AC 24 60 FF FF FF 48 81 EC A0 01 00 00 48 C7 "
                       "44 24 48 FE FF FF FF",
                       __fastcall, void, void*, GameUpdatePacket*);

class SecureItemManager : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect<DownloadFileComponentInit_t>(
            pattern::DownloadFileComponentInit, DownloadFileComponentInit,
            &real::DownloadFileComponentInit);
    }

    static void __fastcall DownloadFileComponentInit(void* this_, std::string filePath, void* unk3,
                                                     void* unk4)
    {
        // Prevent path traversal by strictly disallowing usage of ".." in file paths.
        // We do not need to sanitize for / \ : characters as absolute cache path already gets
        // prepended to the file name, so attacking via absolute path isn't going to work.
        // This attack vector is commonly used by malicious servers in order to drop malware to
        // end-user's computer.
        if (filePath.find("..") != std::string::npos)
        {
            std::string msg =
                "The server you were connecting to contained a malicious item definitions file. "
                "Shutting down game to prevent further damages. Offending filename: " +
                filePath;
            // Hide the game window as sometimes the error may get stuck behind it.
            auto& game = game::GameHarness::get();
            game.setWindowVisible(false);
            // Use a regular message box as throwing an exception did not work, most probably UB for
            // the game client.
            MessageBoxA(nullptr, msg.c_str(), "Error", MB_ICONERROR | MB_OK);
            ExitProcess(EXIT_FAILURE);
        }
        real::DownloadFileComponentInit(this_, filePath, unk3, unk4);
    }
};
REGISTER_CORE_GAME_PATCH(SecureItemManager, secure_item_manager);

static const std::string g_battleEventErrorMessage =
    "Server sent a malformed BattleEvent packet, this could be an attempt "
    "at Remote Code Execution. OSGT-QOL has shut down the game to prevent "
    "any further exploitation. If you are a server developer and believe "
    "this is sent in error, please open an issue on GitHub and provide the "
    "raw packet sent to the client. Error code: ";
class SecureBattleEvent : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        game.hookFunctionPatternDirect<GameLogicComponentHandleBattleEvent_t>(
            pattern::GameLogicComponentHandleBattleEvent, GameLogicComponentHandleBattleEvent,
            &real::GameLogicComponentHandleBattleEvent);
    }

    static void __fastcall GameLogicComponentHandleBattleEvent(void* this_,
                                                               GameUpdatePacket* packet)
    {
        // Battle Events are made rather poorly by the game and leaves space for exploitation.
        // For one, event subtype 0 does no proper bounds checking and allows to read past packet
        // bounds. There may not be really that much attack surface here, but it's good to have it
        // patched.
        // For second, more damning, event subtype 3 allows you to point to any arbitrary
        // point in memory with the team ID due to no safety rails in its packet handling code,
        // allowing a threat actor to arbitrarily write memory wherever they please relative to
        // m_battleStatus in GameLogicComponent.
        if (packet->m_tileY == 0)
        {
            if (packet->m_extendedDataLength != 0)
            {
                uint8_t* packetExtraData = ((uint8_t*)(packet)) + 56;
                short nameLen = *(short*)(packetExtraData);
                packetExtraData += nameLen + 6; // Skip name and flags
                uint8_t m_teams = *packetExtraData;
                // The game only has 5 teams it registers, rest could be an attempt at reading past
                // bounds.
                if (m_teams > 5)
                {
                    auto& game = game::GameHarness::get();
                    game.setWindowVisible(false);
                    MessageBoxA(nullptr, (g_battleEventErrorMessage + "0").c_str(), "Error",
                                MB_ICONERROR | MB_OK);
                    ExitProcess(EXIT_FAILURE);
                }
                // Calculate packet length
                int packetLen = nameLen + 6 + 1 + (m_teams * 4);
                if (packetLen != packet->m_extendedDataLength)
                {
                    auto& game = game::GameHarness::get();
                    game.setWindowVisible(false);
                    MessageBoxA(nullptr, (g_battleEventErrorMessage + "1").c_str(), "Error",
                                MB_ICONERROR | MB_OK);
                    ExitProcess(EXIT_FAILURE);
                }
            }
        }
        if (packet->m_tileY == 3)
        {
            // Prevent out-of-bounds writes, the game only has space for 5 teams in BattleStatus
            // struct.
            if (packet->m_itemObjIDOrDelay < 0 || packet->m_itemObjIDOrDelay > 4)
            {
                auto& game = game::GameHarness::get();
                game.setWindowVisible(false);
                MessageBoxA(nullptr, (g_battleEventErrorMessage + "2").c_str(), "Error",
                            MB_ICONERROR | MB_OK);
                ExitProcess(EXIT_FAILURE);
            }
        }
        real::GameLogicComponentHandleBattleEvent(this_, packet);
    }
};
REGISTER_CORE_GAME_PATCH(SecureBattleEvent, secure_battle_event);