#ifndef WOW_SOURCE_GAME_GAMECLIENT_FFXEFFECTS_H
#define WOW_SOURCE_GAME_GAMECLIENT_FFXEFFECTS_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class EffectColorShift : public FFX::Effect {
    public:
    virtual ~EffectColorShift();
    virtual void SetParameters(int32_t count, const int32_t* params);

    EffectColorShift();
};

class EffectDeath : public FFX::Effect {
    public:
    virtual ~EffectDeath();
    virtual void SetParameters(int32_t count, const int32_t* params);

    EffectDeath();
};

class EffectNetherWorld : public FFX::Effect {
    public:
    virtual ~EffectNetherWorld();
    virtual void End();

    EffectNetherWorld();
};

class EffectSwirlingFog : public FFX::Effect {
    public:
    /* 0018 */ FFX::Tex m_fogTex;
    /* 0038 */ FFX::Tex m_seedTex;

    virtual ~EffectSwirlingFog();
    virtual void SetParameters(int32_t count, const int32_t* params);

    EffectSwirlingFog();
};

class EffectUnconscious : public FFX::Effect {
    public:
    /* 0018 */ bool m_fadingOut;
    /* 0019 */ bool m_reportFade;

    virtual ~EffectUnconscious();
    virtual void Begin();
    virtual void SetParameters(int32_t count, const int32_t* params);
    virtual float GetFadeAmount();

    EffectUnconscious();
    bool FadedOut();
    bool FadingOut() const;
    void SetGlowBlur(uint8_t glow, uint8_t blur);
};

#endif
