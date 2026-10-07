#ifndef WOW_SOURCE_WORLDCLIENT_PARTICULATE_H
#define WOW_SOURCE_WORLDCLIENT_PARTICULATE_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CTexture;
class CWorldView;

class Particulate {
    public:
    struct Particle {
        /* 0000 */ C3Vector m_pos;
        /* 000C */ float m_size;
    };

    /* 0000 */ Particle m_particles[4000];
    /* FA00 */ uint32_t m_count;
    /* FA04 */ C3Vector m_lastCamPos;
    /* FA10 */ CTexture* m_texture;
    /* FA14 */ uint8_t m_enabled;
    /* FA18 */ float m_scale;
    /* FA1C */ float m_range;
    /* FA20 */ uint32_t unk_FA20;
    /* FA24 */ uint32_t m_liquidType;
    /* FA28 */ C3Vector m_moveDir;
    /* FA34 */ float m_moveSpeed;
    /* FA38 */ float m_moveTime;
    /* FA3C */ float m_moveAmplitude;

    Particulate(float scale, float range, const char* textureName);
    ~Particulate();
    C3Vector ComputeMovement(float dt);
    void InitMovement();
    void InitParticles(uint32_t liquidType);
    void Render(CWorldView* view);
    void SetScale(float scale);
    void SetTexture(const char* textureName);
    void Update();
};

#endif
