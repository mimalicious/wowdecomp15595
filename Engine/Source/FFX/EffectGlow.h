#ifndef ENGINE_SOURCE_FFX_EFFECTGLOW_H
#define ENGINE_SOURCE_FFX_EFFECTGLOW_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class EffectGlow : public FFX::Effect {
    public:
    static CVar* s_cvarGlow;

    static void InitCVars();

    /* 0018 */ TSGrowableArray<FFX::Pass*> m_altPasses;
    /* 0028 */ int32_t m_mode;

    virtual ~EffectGlow();
    virtual bool Valid();
    virtual void Execute();
    virtual void SetParameters(int32_t count, const int32_t* params);

    EffectGlow();
};

#endif
