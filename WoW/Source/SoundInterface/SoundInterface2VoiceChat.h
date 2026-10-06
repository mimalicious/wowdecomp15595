#ifndef WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2VOICECHAT_H
#define WOW_SOURCE_SOUNDINTERFACE_SOUNDINTERFACE2VOICECHAT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/SoundEngine/SE3.h"

class SI2LoopbackCapture : public SE3CaptureCallbacks {
    public:
    virtual ~SI2LoopbackCapture();
    virtual void ProcessData(int16_t* data, uint32_t samples);
};

#endif
