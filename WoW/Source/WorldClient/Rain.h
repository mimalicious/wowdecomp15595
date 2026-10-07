#ifndef WOW_SOURCE_WORLDCLIENT_RAIN_H
#define WOW_SOURCE_WORLDCLIENT_RAIN_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/Mists.h"
#include "WoW/Source/WorldClient/WeatherPacket.h"

class C33Matrix;
class CGxShader;
class CTexture;
class Weather;

class Rain {
    public:
    struct Drop {
        struct Vertex {
            /* 0000 */ C3Vector m_pos;
            /* 000C */ C3Vector m_velocity;
            /* 0018 */ CImVector m_corner;
            /* 001C */ float m_startTime;
            /* 0020 */ float m_endTime;
        };

        /* 0000 */ C3Vector m_pos;
        /* 000C */ C3Vector m_velocity;
        /* 0018 */ float unk_0018;
        /* 001C */ float m_startTime;
        /* 0020 */ float m_endTime;

        Drop() : unk_0018(0.0f) {
        }
    };

    struct Patter {
        struct Vertex {
            /* 0000 */ C3Vector m_pos;
            /* 000C */ CImVector m_corner;
            /* 0010 */ float m_startTime;
            /* 0014 */ float m_endTime;
        };

        /* 0000 */ C3Vector m_pos;
        /* 000C */ float unk_000C;
        /* 0010 */ float m_startTime;
        /* 0014 */ float m_endTime;

        Patter() : unk_000C(0.0f) {
        }
    };

    typedef Packet<Drop, 1024, 3> DropPacket;
    typedef Packet<Patter, 1024, 3> PatterPacket;

    /* 0000 */ Weather* m_weather;
    /* 0004 */ uint8_t m_useShaders;
    /* 0005 */ uint8_t m_fadeOut;
    /* 0008 */ CAaBox m_bounds;
    /* 0020 */ float m_maxBuildTime;
    /* 0024 */ float m_particlesPerSecond;
    /* 0028 */ float m_intensity;
    /* 002C */ CTexture* m_dropTexture;
    /* 0030 */ CGxShader* m_dropShader;
    /* 0034 */ CTexture* m_patterTexture;
    /* 0038 */ CGxShader* m_patterShader;
    /* 003C */ Mists m_mists;
    /* 0090 */ PacketList<DropPacket> m_drops;
    /* 00B4 */ PacketList<PatterPacket> m_patters;

    Rain(Weather* weather, float width, float height);
    ~Rain();
    float GetDropStretch() const;
    void GetMotionBlurMatrix(C33Matrix& matrix) const;
    bool InitDrop(Drop& drop, float time);
    uint32_t NumActive();
    void RealizePacket(DropPacket* packet);
    void RealizePacket(PatterPacket* packet);
    void Render();
    void RenderDropPackets();
    void RenderDropPacketsSW();
    void RenderPatterPackets();
    void RenderPatterPacketsSW();
    void UpdatePacket(uint32_t count);
    void UpdatePackets();
};

#endif
