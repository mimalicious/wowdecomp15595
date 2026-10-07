#ifndef WOW_SOURCE_DAYNIGHT_DNCLOUDS_H
#define WOW_SOURCE_DAYNIGHT_DNCLOUDS_H

#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxShader;
class CTexture;

namespace DayNight {
    class DNClouds {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;

        static void Callback_GxTex(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels);
        static void __stdcall Collide(const C3Vector& origin, const C3Vector& dir, C3Vector& out);

        /* 0000 */ float m_sharpness;
        /* 0004 */ float m_densityOverride;
        /* 0008 */ uint8_t m_density;
        /* 0009 */ uint8_t m_lod;
        /* 000A */ uint8_t m_fullUpdate;
        /* 000B */ uint8_t m_texIndex;
        /* 000C */ float m_animSpeed;
        /* 0010 */ uint32_t m_rowsPerUpdate;
        /* 0014 */ uint32_t m_row;
        /* 0018 */ EGxTexFormat m_texFormat;
        /* 001C */ uint32_t m_size;
        /* 0020 */ uint32_t m_shift;
        /* 0024 */ uint32_t m_mask;
        /* 0028 */ uint32_t m_octaves;
        /* 002C */ uint32_t m_enabled;
        /* 0030 */ TSFixedArray<CImVector> m_texels;
        /* 003C */ TSFixedArray<uint8_t> m_densities;
        /* 0048 */ TSFixedArray<float> m_prevRow;
        /* 0054 */ TSFixedArray<C3Vector> m_verts;
        /* 0060 */ TSFixedArray<C2Vector> m_texCoords;
        /* 006C */ TSFixedArray<CImVector> m_colors;
        /* 0078 */ TSFixedArray<uint16_t> m_indices;
        /* 0084 */ uint16_t m_nIndices;
        /* 0086 */ uint16_t m_nVerts;
        /* 0088 */ uint16_t m_animFrame;
        /* 008C */ CTexture* m_textures[2];

        DNClouds();
        void Destroy();
        void GenSphere(float radius);
        float GetDensity(const C3Vector& pos, float scale);
        void Initialize();
        void Render();
        void SetDensity(float density);
        void SetFullUpdate();
        void SetLOD(uint32_t lod, uint32_t rowsPerUpdate);
        void SetSharpness(float sharpness);
        void SetupBumpMap(C3Vector& shadeColor, C3Vector& sunColor, C3Vector& baseColor, C3Vector& sunPos, float& intensity);
        void Update();
        void WorldToTexture(const C3Vector& world, C2Vector& uv);
    };
}

#endif
