#include "game/game.hpp"
#include "game/signatures.hpp"
#include "game/struct/variant.hpp"
#include "patch/patch.hpp"
#include "utils/utils.hpp"

// GetAudioManager
REGISTER_GAME_FUNCTION(
    GetAudioManager,
    "F3 44 0F 10 ? ? ? ? ? F3 44 0F 10 ? ? ? ? ? 44 38 B6 D9 02 00 00 0F 84 ? ? ? ? E8 ? ? ? ?",
    __fastcall, void*);

// AudioManagerFMOD::SetMusicVol
REGISTER_GAME_FUNCTION(AudioManagerFMODSetMusicVol,
                       "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 89 A8 00", __fastcall, void,
                       void* this_, float musicVol);

// AudioManagerFMOD::Preload
REGISTER_GAME_FUNCTION(AudioManagerFMODPreload,
                       "40 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 30 FF FF FF 48 81 EC "
                       "D0 01 00 00 48 C7 44 24 38 FE FF FF FF",
                       __fastcall, void, void*, void*, bool, bool, bool, bool)

// AudioManagerFMOD::Play
REGISTER_GAME_FUNCTION(AudioManagerFMODPlay,
                       "40 55 53 56 57 41 54 41 56 41 57 48 8D 6C 24 E9 48 81 EC A0 00 00 00 48 C7",
                       __fastcall, void*, void* this_, std::string fName, bool bLooping,
                       bool bIsMusic, bool bAddBasePath, bool bForceStreaming)

class AudioStutterPatch : public patch::BasePatch
{
  public:
    void apply() const override
    {
        auto& game = game::GameHarness::get();
        // Hook AudioManagerFMOD::Preload.
        game.hookFunctionPatternDirect(pattern::AudioManagerFMODPreload, AudioManagerFMODPreload,
                                       &real::AudioManagerFMODPreload);
    }

    static void __fastcall AudioManagerFMODPreload(void* this_, void* unk2, bool bLooping,
                                                   bool bIsMusic, bool bAddBasePath,
                                                   bool bForceStreaming)
    {
        // Current assumption is PC client had streaming disabled because of Seth's attempt to
        // utilize extra memory resources in order to save performance and cache audio files.
        // However, in practice, the Growtopia client fails to properly utilize said caching and as
        // such creates an annoying stutter. This forces every call to AudioManagerFMOD::Preload to
        // use file streaming to fix said stutter.
        // nit: This approach can cause audio cutoff for those with the hearing to notice it.
        bForceStreaming = true;
        real::AudioManagerFMODPreload(this_, unk2, bLooping, bIsMusic, bAddBasePath,
                                      bForceStreaming);
    }
};
REGISTER_USER_GAME_PATCH(AudioStutterPatch, audio_stutter_fix);

class StartMusicSliderBackport : public patch::BasePatch
{
  public:
    void apply() const override
    {
        // Your ears will thank you.

        // This patch hooks to AudioManagerFMOD's SetMusicVol and Play functions.
        // In SetMusicVol, we check if we last played theme.ogg to determine which variable to
        // override the call with.
        // In Play, we check if to-be-played track name is theme.ogg for same reason.

        // We store the start volume value in same place the future version does, inside App
        // VariantDB's "start_vol" key.

        // nit / FIXME: Due to AudioManagerFMOD weirdness, the game will still play the title screen
        // theme silently even if the volume is set to near zero.

        auto& game = game::GameHarness::get();

        real::GetAudioManager = utils::resolveRelativeCall<GetAudioManager_t>(
            game.findMemoryPattern<uint8_t*>(pattern::GetAudioManager) + 31);
        real::AudioManagerFMODSetMusicVol = game.findMemoryPattern<AudioManagerFMODSetMusicVol_t>(
            pattern::AudioManagerFMODSetMusicVol);

        // We re-use "start_vol" variable from newer client. Modern client users get to keep their
        // preference.
        Variant* pVariant = real::GetApp()->GetVar("start_vol");
        if (pVariant->GetType() == Variant::TYPE_UNUSED)
            pVariant->Set(0.33f);

        auto& optionsMgr = game::OptionsManager::get();
        optionsMgr.addSliderOption("qol", "Audio", "start_vol", "Start Music",
                                   &StartVolumeSliderCallback);

        // Hook.
        game.hookFunctionPatternDirect(pattern::AudioManagerFMODPlay, AudioManagerFMODPlay,
                                       &real::AudioManagerFMODPlay);
        game.hookFunction(real::AudioManagerFMODSetMusicVol, AudioManagerFMODSetMusicVol,
                          &real::AudioManagerFMODSetMusicVol);
    }

    static float getStartVol()
    {
        // We'll have to set it to 0.01f as FMOD doesn't seem to like audio level 0.00f. In order to
        // not cause glitchiness for option slider, we'll let the variable itself go down to 0.00f.
        float fStartVol = real::GetApp()->GetVar("start_vol")->GetFloat();
        if (fStartVol <= 0.01f)
            fStartVol = 0.01f;
        return fStartVol;
    }

    static void StartVolumeSliderCallback(Variant* pVariant)
    {
        // We'll save the start_vol, our own detour of setmusicvol will calculate adequate volume
        // where needed.
        real::GetApp()->GetVar("start_vol")->Set(pVariant->GetFloat());
        AudioManagerFMODSetMusicVol(real::GetAudioManager(), 0.0f);
    }

    static void __fastcall AudioManagerFMODSetMusicVol(void* this_, float volume)
    {
        // If you exited to OnlineScreen, it was still as loud as ever. You could also change
        // "music_vol" slider and it'd affect theme.ogg. This fixes that.
        // nit: Create a struct for AudioManager/AudioManagerFMOD
        std::string* lastTrackName =
            reinterpret_cast<std::string*>(reinterpret_cast<uint8_t*>(this_) + 8);
        if (lastTrackName->find("/theme.ogg") != std::string::npos)
        {
            real::AudioManagerFMODSetMusicVol(this_, volume * getStartVol());
            return;
        }
        real::AudioManagerFMODSetMusicVol(this_, volume);
    }

    static void* __fastcall AudioManagerFMODPlay(void* this_, std::string fName, bool bLooping,
                                                 bool bIsMusic, bool bAddBasePath,
                                                 bool bForceStreaming)
    {
        void* ret = real::AudioManagerFMODPlay(this_, fName, bLooping, bIsMusic, bAddBasePath,
                                               bForceStreaming);
        // Our detour will calculate appropriate volume level depending on audio file played.
        if (bIsMusic)
            AudioManagerFMODSetMusicVol(this_, real::GetApp()->GetVar("music_vol")->GetFloat());
        return ret;
    }
};
REGISTER_USER_GAME_PATCH(StartMusicSliderBackport, start_music_slider_backport);