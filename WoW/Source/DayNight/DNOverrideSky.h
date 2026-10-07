#ifndef WOW_SOURCE_DAYNIGHT_DNOVERRIDESKY_H
#define WOW_SOURCE_DAYNIGHT_DNOVERRIDESKY_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CM2Model;
class CM2Scene;

namespace DayNight {
    class DNOverrideSky : public TSHashObject<DNOverrideSky, HASHKEY_STRI> {
        public:
        static CM2Scene* s_scene;
        static uint32_t s_lastTimeMs;

        /* 0018 */ CM2Model* m_model;
        /* 001C */ uint32_t m_animLengthMs;
        /* 0020 */ int32_t m_lastMinute;
        /* 0024 */ uint32_t m_flags;

        DNOverrideSky();
        int32_t IsAllLoaded();
        void LinkToAsyncHead();
    };
}

#endif
