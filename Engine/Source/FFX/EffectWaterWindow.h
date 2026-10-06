#ifndef ENGINE_SOURCE_FFX_EFFECTWATERWINDOW_H
#define ENGINE_SOURCE_FFX_EFFECTWATERWINDOW_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/FFX/FFX.h"

class EffectWaterWindow : public FFX::Effect {
    public:
    virtual ~EffectWaterWindow();

    EffectWaterWindow(uint32_t type);
};

class EffectSubmarineWindow : public FFX::Effect {
    public:
    virtual ~EffectSubmarineWindow();

    EffectSubmarineWindow(uint32_t type);
};

#endif
