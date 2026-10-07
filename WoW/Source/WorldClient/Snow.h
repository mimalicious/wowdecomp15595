#ifndef WOW_SOURCE_WORLDCLIENT_SNOW_H
#define WOW_SOURCE_WORLDCLIENT_SNOW_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Source/WorldClient/Mists.h"
#include "WoW/Source/WorldClient/WeatherPacket.h"

class CGxShader;
class CTexture;
class Weather;

class Snow {
    public:
    struct Flake {
        typedef Flake Vertex;

        /* 0000 */ C3Vector m_pos;
        /* 000C */ C3Vector m_velocity;
        /* 0018 */ float m_startTime;
        /* 001C */ float m_endTime;
    };

    typedef Packet<Flake, 6144, 1> FlakePacket;

    /* 0000 */ Weather* m_weather;
    /* 0004 */ uint8_t m_useShaders;
    /* 0005 */ uint8_t m_fadeOut;
    /* 0008 */ CAaBox m_bounds;
    /* 0020 */ CTexture* m_flakeTexture;
    /* 0024 */ CGxShader* m_vertexShader;
    /* 0028 */ CGxShader* m_geometryShader;
    /* 002C */ CGxShader* m_pixelShader;
    /* 0030 */ float m_maxBuildTime;
    /* 0034 */ float m_particlesPerSecond;
    /* 0038 */ float m_intensity;
    /* 003C */ Mists m_mists;
    /* 0090 */ PacketList<FlakePacket> m_flakes;

    Snow(Weather* weather, float width, float height);
    ~Snow();
    void InitFlake(Flake& flake, float time, const C3Vector& origin, const C3Vector& wind);
    uint32_t NumActive();
    void RealizePacket(FlakePacket* packet);
    void Render();
    void RenderPackets();
    void RenderPacketsSW();
    void UpdatePacket(uint32_t count, const C3Vector& origin, const C3Vector& wind);
    void UpdatePackets();
};

#endif
