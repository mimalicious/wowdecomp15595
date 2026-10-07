#ifndef WOW_SOURCE_DAYNIGHT_DNPLANET_H
#define WOW_SOURCE_DAYNIGHT_DNPLANET_H

#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxShader;
class CTexture;

namespace DayNight {
    class DNPlanet {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;

        /* 0000 */ C3Vector m_position;
        /* 000C */ CImVector m_color;
        /* 0010 */ CTexture* m_texture;
        /* 0014 */ float m_scale;
        /* 0018 */ float m_baseScale;
        /* 001C */ float m_period;

        void ClipGeometry(C3Vector* verts, C2Vector* texCoords, CImVector* colors, uint16_t* indices, uint32_t& vertCount, uint32_t& indexCount);
        void Destroy();
        void GenGeometry(C3Vector* verts, C2Vector* texCoords, CImVector* colors, uint16_t* indices, uint32_t& vertCount, uint32_t& indexCount);
        void Initialize(const char* fileName);
        int32_t IsAllLoaded();
        void LinkToAsyncHead();
        void Render();
    };
}

#endif
