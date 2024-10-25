#pragma once

#include "boost/signals/trackable.hpp"
#include "loopingsound.hpp"
#include "variant.hpp"
#include "vec.hpp"


class Background : public boost::signals::trackable
{
  public:
    Background();
    virtual ~Background();

    virtual void Render(Vec2f screenSize, bool bInWorld);
    virtual void Init(bool);
    virtual void Update();
    virtual void RenderForeground(Vec2f, float);
    virtual void SetScale(Vec2f);
    virtual void UpdateSFXVolume(float vol);

    void OnEnterForeground(VariantList*);
    void OnEnterBackground(VariantList*);

    void OnAudioDisabled();
    void OnAudioEnabled();

    // Seems to be used to signify if the weather has snow or other kind of falling particle
    bool m_bParticles = false;

    // Used for the weather change animation/fade
    float m_fadeTime = 1.0f;

    // They're all floats, but no idea what they're used for yet.
    // For BackgroundNight it was "0.0, 0.0, 1.0, 1.0"
    // Changing them affected nothing.
    float m_unk1;
    float m_unk2;
    float m_unk3;
    float m_unk4;

    Rectf m_renderRect;
    LoopingSound m_loopingSound;
    float m_loopingSoundVol = 0.25f;
};
