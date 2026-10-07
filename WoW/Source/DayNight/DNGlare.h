#ifndef WOW_SOURCE_DAYNIGHT_DNGLARE_H
#define WOW_SOURCE_DAYNIGHT_DNGLARE_H

#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CGxQuery;
class CGxShader;
class CTexture;

namespace DayNight {
    class DNPlanet;

    class GlareBase {
        public:
        static int32_t s_masterEnable;

        /* 0004 */ CGxQuery* m_query;
        /* 0008 */ int32_t m_queryIssued;
        /* 000C */ int32_t m_enable;
        /* 0010 */ C3Vector m_position;
        /* 001C */ CImVector m_color;
        /* 0020 */ CTexture* m_texture;

        virtual void Update(float deltaSec) = 0;
        virtual void Render() = 0;
        virtual float IsVisible() = 0;
        virtual int32_t IsAllLoaded();
        virtual void LinkToAsyncHead();
    };

    class DNGlare : public GlareBase {
        public:
        static uint32_t s_refCount;
        static CGxShader* s_vertexShader;
        static CGxShader* s_pixelShader;
        static CGxShader* s_queryVertexShader;
        static CGxShader* s_queryPixelShader;

        /* 0024 */ float m_baseScale;
        /* 0028 */ float m_scale;
        /* 002C */ float m_fadeInRate;
        /* 0030 */ float m_fadeOutRate;
        /* 0034 */ float m_fade;
        /* 0038 */ float m_visibility;
        /* 003C */ int32_t m_skipQuery;
        /* 0040 */ uint8_t unk_0040[48];
        /* 0070 */ uint64_t m_queryPixels;
        /* 0078 */ C2Vector m_alphaCurve[4];
        /* 0098 */ float m_scaleMin;
        /* 009C */ float m_scaleMax;
        /* 00A0 */ float m_minDot;
        /* 00A4 */ float m_alphaMin;
        /* 00A8 */ float m_alphaMax;
        /* 00AC */ float m_glow;
        /* 00B0 */ DNPlanet* m_planet;

        void Update(float deltaSec) override;
        void Render() override;
        float IsVisible() override;
        virtual float GetCloudDensityFade() = 0;
        virtual float GetLiquidDepthFade();

        void Destroy();
        void Initialize(const char* fileName);
        float IsVisibleIntersect();
        float IsVisibleQuery();
    };

    class DNSunGlare : public DNGlare {
        public:
        float GetCloudDensityFade() override;
    };

    class DNMoonGlare : public DNGlare {
        public:
        float GetCloudDensityFade() override;
    };
}

#endif
