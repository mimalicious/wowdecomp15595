#ifndef WOW_COMMON_LIGHTNING_H
#define WOW_COMMON_LIGHTNING_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"

class CGxShader;
class CTexture;

struct LightningJoint {
    /* 0000 */ C3Vector m_pos;
    /* 000C */ C3Vector m_velocity;
    /* 0018 */ float m_jumpTimer;
};

class CLightning {
    public:
    static uint32_t DecodeChainEffectsID(uint32_t id);

    /* 0000 */ C3Vector m_srcPos;
    /* 000C */ C3Vector m_dstPos;
    /* 0018 */ CImVector m_color;
    /* 001C */ const SpellChainEffectsRec* m_chainEffects;
    /* 0020 */ TSFixedArray<LightningJoint> m_joints;
    /* 002C */ TSFixedArray<C3Vector> m_points;
    /* 0038 */ TSFixedArray<C3Vector> m_positions;
    /* 0044 */ CGxBatch m_batch;
    /* 0054 */ CAaBox m_bounds;
    /* 006C */ float m_texOffset;
    /* 0070 */ CTexture* m_texture;
    /* 0074 */ void (*m_coordUpdate)(void*, uint32_t, C3Vector*, C3Vector*);
    /* 0078 */ void* m_coordUpdateParam;
    /* 007C */ uint32_t m_flags;
    /* 0080 */ float m_waveAngle;
    /* 0084 */ float m_waveSpin;
    /* 0088 */ float m_wavePhase;
    /* 008C */ float m_arcAngle;
    /* 0090 */ float m_arcSpin;
    /* 0094 */ float m_flickerTimer;
    /* 0098 */ float m_pulsePos;
    /* 009C */ uint32_t m_pulseOnStart;
    /* 00A0 */ uint32_t m_pulseFadeStart;
    /* 00A4 */ int32_t m_pulseAlphaStep;
    /* 00A8 */ float m_texRandom;

    CLightning();
    ~CLightning();
    void ApplyPulsingToVertexData();
    void ApplyWaveToPosition(C3Vector& position, float t, float length);
    void BuildPoint(C3Vector& point, const C3Vector& base, float t, const LightningJoint& joint, float scale);
    void BuildPoints();
    void BuildPointsUsingHierarchy();
    void Init(const SpellChainEffectsRec* chainEffects);
    void PrecomputeBeamValues();
    void RandomizeJoint(LightningJoint& joint, float radius, float blend);
    void Render(uint32_t index, const C3Vector& cameraPos, int32_t (*cull)(const CAaBox& box));
    void Reset();
    void SetDstPos(const C3Vector& pos);
    void SetSrcPos(const C3Vector& pos);
    void SetTexture(CTexture* texture);
    void Update(float elapsed);
    void UpdateJointCount(float length, float radius);
};

class CLightningManager {
    public:
    /* 0000 */ TSGrowableArray<CLightning*> m_bolts;
    /* 0010 */ TSGrowableArray<uint32_t> m_freeList;
    /* 0020 */ CGxShader* m_vertexShader;
    /* 0024 */ CGxShader* m_pixelShader;

    CLightningManager();
    ~CLightningManager();
    uint32_t Add(void (*coordUpdate)(void*, uint32_t, C3Vector*, C3Vector*), void* param, const SpellChainEffectsRec* chainEffects);
    CLightning* GetTempPtr(uint32_t index);
    void Move(uint32_t index, C3Vector* srcPos, C3Vector* dstPos);
    void Remove(uint32_t index);
    void Render(const C3Vector& cameraPos, int32_t (*cull)(const CAaBox& box));
    void SetBoltVisible(uint32_t index, bool visible);
    void Update(float elapsed);
};

void LightningDestroyBuffers();

#endif
