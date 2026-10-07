#ifndef WOW_SOURCE_DAYNIGHT_DNSTARS_H
#define WOW_SOURCE_DAYNIGHT_DNSTARS_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CM2Model;
class CM2Scene;

namespace DayNight {
    class DNStars {
        public:
        /* 0000 */ CM2Scene* m_scene;
        /* 0004 */ CM2Model* m_model;
        /* 0008 */ CImVector m_color;
        /* 000C */ C3Vector m_position;
        /* 0018 */ uint32_t m_lastTimeMs;

        void Destroy();
        void Initialize();
        int32_t IsAllLoaded();
        void Render(bool interior);
        void Update();
    };
}

#endif
