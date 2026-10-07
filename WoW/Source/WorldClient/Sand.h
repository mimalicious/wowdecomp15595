#ifndef WOW_SOURCE_WORLDCLIENT_SAND_H
#define WOW_SOURCE_WORLDCLIENT_SAND_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/Mists.h"
#include "WoW/Source/WorldClient/WeatherPacket.h"

class CGxShader;
class Weather;

class Sand {
    public:
    struct Grain {
        typedef Grain Vertex;

        /* 0000 */ C3Vector m_pos;
        /* 000C */ C3Vector m_velocity;
        /* 0018 */ float m_startTime;
        /* 001C */ float m_endTime;
    };

    typedef Packet<Grain, 6144, 1> GrainPacket;

    /* 0000 */ Weather* m_weather;
    /* 0004 */ uint8_t m_useShaders;
    /* 0005 */ uint8_t m_fadeOut;
    /* 0008 */ CAaBox m_bounds;
    /* 0020 */ Mists m_mists;
    /* 0074 */ PacketList<GrainPacket> m_grains;
    /* 0098 */ float m_maxBuildTime;
    /* 009C */ CGxShader* m_vertexShader;
    /* 00A0 */ CGxShader* m_geometryShader;
    /* 00A4 */ CGxShader* m_pixelShader;
    /* 00A8 */ float m_particlesPerSecond;

    Sand(Weather* weather, float width, float height);
    ~Sand();
    void InitGrain(Grain& grain, float time);
    uint32_t NumActive();
    void RealizePacket(GrainPacket* packet);
    void Render();
    void RenderPackets();
    void RenderPacketsSW();
    void UpdatePacket(uint32_t count);
    void UpdatePackets();
};

#endif
