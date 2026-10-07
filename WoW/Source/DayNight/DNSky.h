#ifndef WOW_SOURCE_DAYNIGHT_DNSKY_H
#define WOW_SOURCE_DAYNIGHT_DNSKY_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxShader;

namespace DayNight {
    class CurrentLight;

    class DNSky {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;

        /* 0000 */ TSFixedArray<C3Vector> m_verts;
        /* 000C */ TSFixedArray<CImVector> m_colors;
        /* 0018 */ TSFixedArray<uint16_t> m_indices;
        /* 0024 */ int32_t m_thetaTess;
        /* 0028 */ uint16_t m_nVerts;
        /* 002A */ uint16_t m_nIndices;
        /* 002C */ float unk_002C;

        void Destroy();
        void GenSphere(float radius);
        void Initialize();
        void Render();
        void SetColors(const CurrentLight& light);
    };
}

#endif
