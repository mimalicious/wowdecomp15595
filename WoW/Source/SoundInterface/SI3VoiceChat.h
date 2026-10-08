#ifndef WOW_SOURCE_SOUNDINTERFACE_SI3VOICECHAT_H
#define WOW_SOURCE_SOUNDINTERFACE_SI3VOICECHAT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/SoundEngine/SE3.h"

class SI3LoopbackCapture : public SE3CaptureCallbacks {
    public:
    virtual ~SI3LoopbackCapture();
    virtual void ProcessData(int16_t* data, uint32_t samples);
};

#endif
