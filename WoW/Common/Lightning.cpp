#include <cmath>
#include <cstddef>
#include <cstdint>
#include "WoW/Common/Lightning.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"

// STATIC: 0x01008D6C
static CGxPool* s_pool;

// STATIC: 0x01008D70
static CGxBuf* s_buf;

// STATIC: 0x01008D74
static C3Vector s_beamAxisX;

// STATIC: 0x01008D80
static C3Vector s_beamDir;

// STATIC: 0x01008D8C
static C3Vector s_beamAxisZ;

// STATIC: 0x01008D98
static C3Vector s_jointAxisX;

// STATIC: 0x01008DA4
static C3Vector s_jointAxisY;

// STATIC: 0x01008DB0
static C3Vector s_jointAxisZ;

// STATIC: 0x01008DBC
static C3Vector s_waveOffset;

// STATIC: 0x01008DC8
static C3Vector s_arcOffset;

// STATIC: 0x01008DD4
static CRndSeed s_rndSeed(0);

// OFFSET: 0x00B28860/0x1408F1030
void CLightning::SetTexture(CTexture* texture) {
    if (texture) {
        TextureAddRef(texture);
    }

    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    this->m_texture = texture;
}

// PROVEN: 696f84d8 IDENTICAL/NONE
// OFFSET: 0x00B28890/0x1408F1080
uint32_t CLightning::DecodeChainEffectsID(uint32_t id) {
    uint32_t bits = id & 0xFEFEFEFE;
    uint32_t high = (bits >> 7) & 0x200000;
    high = ((high | (bits & 0x20000000)) >> 3) | (bits & 0x2000000);
    high = ((high >> 4) | (bits & 0x40000000)) >> 3;
    high |= bits & 0x4000000;

    return (high >> 11) | ((((bits >> 4) & 0xFF800000) | bits) & 0xFFFFFF);
}

// OFFSET: 0x00B28A50/0x1408F1300
static float RandomFloat(float min, float max) {
    return (max - min) * CRandom::ufloat_(s_rndSeed) + min;
}

// OFFSET: 0x00B28AA0/0x1408F1360
void CLightning::Reset() {
    this->m_flags &= ~0x5;
    this->m_waveAngle = (this->m_chainEffects->m_maxWaveAngle - this->m_chainEffects->m_minWaveAngle) * CRandom::ufloat_(s_rndSeed) + this->m_chainEffects->m_minWaveAngle;
    this->m_waveSpin = (this->m_chainEffects->m_maxWaveSpin - this->m_chainEffects->m_minWaveSpin) * CRandom::ufloat_(s_rndSeed) + this->m_chainEffects->m_minWaveSpin;

    if (this->m_chainEffects->m_flags & 0x400) {
        this->m_wavePhase = this->m_chainEffects->m_wavePhase;
    } else {
        this->m_wavePhase = CRandom::ufloat_(s_rndSeed) * 6.2831855f;
    }

    this->m_arcAngle = (this->m_chainEffects->m_maxArcAngle - this->m_chainEffects->m_minArcAngle) * CRandom::ufloat_(s_rndSeed) + this->m_chainEffects->m_minArcAngle;
    this->m_arcSpin = (this->m_chainEffects->m_maxArcSpin - this->m_chainEffects->m_minArcSpin) * CRandom::ufloat_(s_rndSeed) + this->m_chainEffects->m_minArcSpin;
    this->m_flickerTimer = (this->m_chainEffects->m_maxFlickerOnDuration - this->m_chainEffects->m_minFlickerOnDuration) * CRandom::ufloat_(s_rndSeed) + this->m_chainEffects->m_minFlickerOnDuration;
    this->m_pulsePos = 0.0f;

    if (this->m_chainEffects->m_pulseSpeed < 0.0f) {
        this->m_flags |= 0x8;
    }

    this->m_texRandom = CRandom::ufloat_(s_rndSeed);
}

// OFFSET: 0x00B28D20
void LightningDestroyBuffers() {
    if (s_buf) {
        GxBufDestroy(s_buf);
        s_buf = NULL;
    }

    if (s_pool) {
        GxPoolDestroy(s_pool);
        s_pool = NULL;
    }
}

// OFFSET: 0x00B28D90/0x1408F1630
void CLightning::RandomizeJoint(LightningJoint& joint, float radius, float blend) {
    float z = CRandom::sfloat_(s_rndSeed);
    float x = CRandom::sfloat_(s_rndSeed);
    float y = joint.m_pos.y * blend;
    float newZ = (joint.m_pos.z - z * radius) * blend + z * radius;
    joint.m_pos.x = (joint.m_pos.x - x * radius) * blend + x * radius;
    joint.m_pos.y = y;
    joint.m_pos.z = newZ;

    float speedZ = CRandom::sfloat_(s_rndSeed);
    float speedX = CRandom::sfloat_(s_rndSeed);
    joint.m_velocity.x = this->m_chainEffects->m_jointMoveSpeed * speedX;
    joint.m_velocity.y = 0.0f;
    joint.m_velocity.z = this->m_chainEffects->m_jointMoveSpeed * speedZ;

    if (this->m_flags & 0x1) {
        joint.m_jumpTimer = CRandom::ufloat_(s_rndSeed) * this->m_chainEffects->m_maxDurationBetweenJointJumps;
    } else {
        joint.m_jumpTimer = (this->m_chainEffects->m_maxDurationBetweenJointJumps - this->m_chainEffects->m_minDurationBetweenJointJumps) * CRandom::ufloat_(s_rndSeed) + this->m_chainEffects->m_minDurationBetweenJointJumps;
    }
}

// OFFSET: 0x00B28FD0
static void ComputeXZAxesFromYAxis(C3Vector& axisX, const C3Vector& axisY, C3Vector& axisZ) {
    float length = sqrt(axisY.z * axisY.z + axisY.y * axisY.y + axisY.x * axisY.x);

    if (fabs(axisY.z) > length * 0.98f) {
        axisX.x = 1.0f;
        axisX.y = 0.0f;
        axisX.z = 0.0f;
        axisZ.x = 0.0f;
        axisZ.y = 1.0f;
        axisZ.z = 0.0f;
        return;
    }

    axisX.x = axisY.y;
    axisX.z = 0.0f;

    if (fabs(axisX.x) >= 0.00000023841858f) {
        axisX.y = -axisY.x;
        float scale = 1.0f / sqrt(axisX.y * axisX.y + axisX.x * axisX.x);
        axisX.x = axisX.x * scale;
        axisX.y = axisX.y * scale;
        axisX.z = scale * 0.0f;
    } else if (axisY.x <= 0.0f) {
        axisX.y = 1.0f;
    } else {
        axisX.y = -1.0f;
    }

    float scale = 1.0f / (length + 0.0000000099999999f);
    axisZ.x = scale * (axisY.z * axisX.y - axisY.y * axisX.z);
    axisZ.y = scale * (axisX.z * axisY.x - axisY.z * axisX.x);
    axisZ.z = scale * (axisY.y * axisX.x - axisX.y * axisY.x);
}

// OFFSET: 0x00B29150
void CLightning::PrecomputeBeamValues() {
    s_beamDir.x = this->m_dstPos.x - this->m_srcPos.x;
    s_beamDir.y = this->m_dstPos.y - this->m_srcPos.y;
    s_beamDir.z = this->m_dstPos.z - this->m_srcPos.z;
    ComputeXZAxesFromYAxis(s_beamAxisX, s_beamDir, s_beamAxisZ);

    const SpellChainEffectsRec* chainEffects = this->m_chainEffects;

    if (fabs(chainEffects->m_waveHeight) >= 0.00000023841858f && !(chainEffects->m_flags & 0x8)) {
        float c = cos(this->m_waveAngle) * this->m_chainEffects->m_waveHeight;
        float s = sin(this->m_waveAngle) * this->m_chainEffects->m_waveHeight;
        s_waveOffset.x = s_beamAxisX.x * s + s_beamAxisZ.x * c;
        s_waveOffset.y = s_beamAxisX.y * s + s_beamAxisZ.y * c;
        s_waveOffset.z = s_beamAxisX.z * s + s_beamAxisZ.z * c;
    }

    chainEffects = this->m_chainEffects;

    if (fabs(chainEffects->m_arcHeight) >= 0.00000023841858f) {
        float c = cos(this->m_arcAngle) * this->m_chainEffects->m_arcHeight;
        float s = sin(this->m_arcAngle) * this->m_chainEffects->m_arcHeight;
        s_arcOffset.x = s * s_beamAxisX.x + c * s_beamAxisZ.x;
        s_arcOffset.y = s * s_beamAxisX.y + c * s_beamAxisZ.y;
        s_arcOffset.z = s * s_beamAxisX.z + c * s_beamAxisZ.z;
    }
}

// OFFSET: 0x00B29360
void CLightning::ApplyPulsingToVertexData() {
    float dx = this->m_dstPos.x - this->m_srcPos.x;
    uint32_t count = this->m_joints.Count();
    float dy = this->m_dstPos.y - this->m_srcPos.y;
    const SpellChainEffectsRec* chainEffects = this->m_chainEffects;
    float dz = this->m_dstPos.z - this->m_srcPos.z;
    float scale = static_cast<float>(count) / (sqrt(dy * dy + dx * dx + dz * dz) + 0.000099999997f);
    float pulse = this->m_pulsePos * scale;
    float fadeOut = pulse - chainEffects->m_pulseFadeLength * scale;
    float fade = chainEffects->m_pulseFadeLength * scale;
    float on = fadeOut - chainEffects->m_pulseOnLength * scale;
    int32_t end = static_cast<int32_t>(ceil(pulse));
    int32_t fadeOutIndex = static_cast<int32_t>(fadeOut);
    int32_t onIndex = static_cast<int32_t>(on);
    int32_t start = static_cast<int32_t>(on - fade);

    if (static_cast<uint32_t>(start) > count) {
        start = start < 0 ? 0 : count;
    }

    this->m_batch.m_start = 2 * start;

    if (static_cast<uint32_t>(onIndex) > count) {
        onIndex = onIndex < 0 ? 0 : count;
    }

    this->m_pulseOnStart = 2 * onIndex;

    if (static_cast<uint32_t>(fadeOutIndex) > count) {
        fadeOutIndex = fadeOutIndex < 0 ? 0 : count;
    }

    this->m_pulseFadeStart = 2 * fadeOutIndex;

    if (static_cast<uint32_t>(end) > count) {
        end = end < 0 ? 0 : count;
    }

    this->m_batch.m_count = 2 * end - 2 * start;

    if (fade == 0.0f) {
        this->m_pulseAlphaStep = this->m_color.a << 16;
    } else {
        this->m_pulseAlphaStep = static_cast<int32_t>(static_cast<int64_t>(this->m_color.a / fade * 65536.0));
    }
}

// PROVEN: f8be89da IDENTICAL/NONE
// OFFSET: 0x00B294D0/0x1408F1F80
void CLightning::SetSrcPos(const C3Vector& pos) {
    this->m_srcPos = pos;
}

// OFFSET: 0x00B294F0/0x1408F1FA0
void CLightning::SetDstPos(const C3Vector& pos) {
    this->m_dstPos = pos;
}

// OFFSET: 0x00B29510/0x1408F1FC0
CLightning* CLightningManager::GetTempPtr(uint32_t index) {
    if (index == 0xFFFFFFFF || index >= this->m_bolts.Count()) {
        return NULL;
    }

    CLightning* bolt = this->m_bolts.m_data[index];

    if (reinterpret_cast<uintptr_t>(bolt) & 0x1) {
        return NULL;
    }

    return bolt;
}

// OFFSET: 0x00B29540/0x1408F1FE0
void CLightningManager::Move(uint32_t index, C3Vector* srcPos, C3Vector* dstPos) {
    if (srcPos) {
        this->m_bolts.m_data[index]->m_srcPos = *srcPos;

        if (!dstPos) {
            return;
        }
    } else if (!dstPos) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    this->m_bolts.m_data[index]->m_dstPos = *dstPos;
}

// OFFSET: 0x00B295A0/0x1408F2040
void CLightningManager::SetBoltVisible(uint32_t index, bool visible) {
    if (index == 0xFFFFFFFF || index >= this->m_bolts.Count()) {
        return;
    }

    CLightning* bolt = this->m_bolts.m_data[index];

    if (reinterpret_cast<uintptr_t>(bolt) & 0x1) {
        return;
    }

    if (visible) {
        bolt->m_flags |= 0x2;
    } else {
        bolt->m_flags &= ~0x2;
    }
}

// OFFSET: 0x00B295E0/0x1408F2070
CLightning::CLightning() {
    this->m_batch.m_primType = GxPrim_TriangleStrip;
    this->m_batch.m_start = 0;
    this->m_batch.m_count = 0;
    this->m_batch.m_minIndex = 0;
    this->m_batch.m_maxIndex = 0;
    this->m_texOffset = 0.0f;
    this->m_texture = NULL;
    this->m_flags = 0;
    this->m_waveAngle = 0.0f;
    this->m_waveSpin = 0.0f;
    this->m_wavePhase = 0.0f;
    this->m_arcAngle = 0.0f;
    this->m_arcSpin = 0.0f;
    this->m_flickerTimer = 0.0f;
    this->m_pulsePos = 0.0f;
    this->m_texRandom = 0.0f;
}

// OFFSET: 0x00B296B0
CLightning::~CLightning() {
    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }
}

// INLINED
void CLightning::BuildPoint(C3Vector& point, const C3Vector& base, float t, const LightningJoint& joint, float scale) {
    point.x = s_jointAxisX.x * joint.m_pos.x * scale + s_jointAxisY.x * t + s_jointAxisZ.x * joint.m_pos.z * scale + base.x;
    point.y = s_jointAxisX.y * joint.m_pos.x * scale + s_jointAxisY.y * t + s_jointAxisZ.y * joint.m_pos.z * scale + base.y;
    point.z = s_jointAxisX.z * joint.m_pos.x * scale + s_jointAxisY.z * t + s_jointAxisZ.z * joint.m_pos.z * scale + base.z;
}

// OFFSET: 0x00B29730
void CLightning::BuildPoints() {
    int32_t last = this->m_joints.Count() - 1;
    this->m_points.m_data[0] = this->m_srcPos;
    this->m_points.m_data[last] = this->m_dstPos;

    this->PrecomputeBeamValues();

    s_jointAxisX = s_beamAxisX;
    s_jointAxisY = s_beamDir;
    s_jointAxisZ = s_beamAxisZ;

    float step = 1.0f / static_cast<float>(last);

    for (int32_t i = 1; i < last; i++) {
        this->BuildPoint(this->m_points.m_data[i], this->m_points.m_data[0], static_cast<float>(i) * step, this->m_joints.m_data[i], 1.0f);
    }
}

// OFFSET: 0x00B29D40
void CLightning::BuildPointsUsingHierarchy() {
    int32_t last = this->m_joints.Count() - 1;
    this->m_points.m_data[0] = this->m_srcPos;
    this->m_points.m_data[last] = this->m_dstPos;

    this->PrecomputeBeamValues();

    s_jointAxisX = s_beamAxisX;
    s_jointAxisY = s_beamDir;
    s_jointAxisZ = s_beamAxisZ;

    const SpellChainEffectsRec* chainEffects = this->m_chainEffects;
    int32_t minorStep = chainEffects->m_jointsPerMinorJoint;
    float majorScale = chainEffects->m_majorJointScale;
    int32_t majorStep = minorStep * chainEffects->m_minorJointsPerMajorJoint;
    float majorT = 1.0f / static_cast<float>(last);

    for (int32_t i = majorStep; i < last; i += majorStep) {
        this->BuildPoint(this->m_points.m_data[i], this->m_points.m_data[0], static_cast<float>(i) * majorT, this->m_joints.m_data[i], majorScale);
    }

    float minorStepF = static_cast<float>(minorStep);
    float minorScale = this->m_chainEffects->m_minorJointScale;
    int32_t next = majorStep >= last ? last : majorStep;

    C3Vector base = this->m_points.m_data[0];
    s_jointAxisY.x = this->m_points.m_data[next].x - base.x;
    s_jointAxisY.y = this->m_points.m_data[next].y - base.y;
    s_jointAxisY.z = this->m_points.m_data[next].z - base.z;
    ComputeXZAxesFromYAxis(s_jointAxisX, s_jointAxisY, s_jointAxisZ);

    float step = minorStepF / static_cast<float>(next);
    float t = step;

    if (minorStep >= last) {
        next = last;
    } else {
        for (int32_t i = minorStep; i < last; i += minorStep) {
            if (i == next) {
                next = i + majorStep;

                if (next >= last) {
                    next = last;
                }

                base = this->m_points.m_data[i];
                s_jointAxisY.x = this->m_points.m_data[next].x - base.x;
                s_jointAxisY.y = this->m_points.m_data[next].y - base.y;
                s_jointAxisY.z = this->m_points.m_data[next].z - base.z;
                ComputeXZAxesFromYAxis(s_jointAxisX, s_jointAxisY, s_jointAxisZ);
                step = minorStepF / static_cast<float>(next - i);
                t = step;
            } else {
                this->BuildPoint(this->m_points.m_data[i], base, t, this->m_joints.m_data[i], minorScale);
                t += step;
            }
        }

        next = minorStep;
    }

    base = this->m_points.m_data[0];
    s_jointAxisY.x = this->m_points.m_data[next].x - base.x;
    s_jointAxisY.y = this->m_points.m_data[next].y - base.y;
    s_jointAxisY.z = this->m_points.m_data[next].z - base.z;
    ComputeXZAxesFromYAxis(s_jointAxisX, s_jointAxisY, s_jointAxisZ);

    step = 1.0f / static_cast<float>(next);
    t = step;

    for (int32_t i = 1; i < last; i++) {
        if (i == next) {
            next = i + minorStep;

            if (next >= last) {
                next = last;
            }

            base = this->m_points.m_data[i];
            s_jointAxisY.x = this->m_points.m_data[next].x - base.x;
            s_jointAxisY.y = this->m_points.m_data[next].y - base.y;
            s_jointAxisY.z = this->m_points.m_data[next].z - base.z;
            ComputeXZAxesFromYAxis(s_jointAxisX, s_jointAxisY, s_jointAxisZ);
            step = 1.0f / static_cast<float>(next - i);
            t = step;
        } else {
            this->BuildPoint(this->m_points.m_data[i], base, t, this->m_joints.m_data[i], 1.0f);
            t += step;
        }
    }
}

// OFFSET: 0x00B2A440
void CLightning::ApplyWaveToPosition(C3Vector& position, float t, float length) {
    const SpellChainEffectsRec* chainEffects = this->m_chainEffects;
    float angle = chainEffects->m_waveFreq * t * length + this->m_wavePhase;
    float envelope = 1.0f - (t - 0.5f) * (t - 0.5f) * 4.0f;
    C3Vector offset;

    if (chainEffects->m_flags & 0x8) {
        float height = chainEffects->m_waveHeight * envelope;
        float s = CMath::sin_(angle) * height;
        float c = CMath::cos_(angle) * height;
        offset.x = s_beamAxisX.x * c + s_beamAxisZ.x * s;
        offset.y = s_beamAxisX.y * c + s_beamAxisZ.y * s;
        offset.z = s_beamAxisX.z * c + s_beamAxisZ.z * s;
    } else {
        float s = sin(angle) * envelope;
        offset.x = s_waveOffset.x * s;
        offset.y = s_waveOffset.y * s;
        offset.z = s_waveOffset.z * s;
    }

    position.x = position.x + offset.x;
    position.y = position.y + offset.y;
    position.z = position.z + offset.z;
}

// OFFSET: 0x00B2A650
void CLightning::Render(uint32_t index, const C3Vector& cameraPos, int32_t (*cull)(const CAaBox& box)) {
    this->m_flags &= ~0x30;

    if (!(this->m_flags & 0x2) || (this->m_flags & 0x4)) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(this->m_texture);

    if (!gxTex) {
        return;
    }

    if (this->m_coordUpdate) {
        C3Vector srcPos = this->m_srcPos;
        C3Vector dstPos = this->m_dstPos;
        this->m_coordUpdate(this->m_coordUpdateParam, index, &srcPos, &dstPos);
        this->m_srcPos = srcPos;
        this->m_dstPos = dstPos;
    }

    if (this->m_chainEffects->m_flags & 0x20) {
        this->ApplyPulsingToVertexData();
    }

    if (this->m_chainEffects->m_flags & 0x2) {
        this->BuildPointsUsingHierarchy();
    } else {
        this->BuildPoints();
    }

    uint32_t pointCount = this->m_points.Count();
    uint32_t vertexCount = 2 * pointCount;
    CGxVertexPCT* vertices = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(s_buf));

    this->m_positions.m_data[0] = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_positions.m_data[1] = C3Vector(0.0f, 0.0f, 0.0f);

    C3Vector prev = this->m_points.m_data[0];
    this->m_bounds.t = prev;
    this->m_bounds.b = prev;

    const SpellChainEffectsRec* chainEffects = this->m_chainEffects;
    bool wave = chainEffects->m_waveHeight != 0.0f;
    bool arc = chainEffects->m_arcHeight != 0.0f;

    float dx = this->m_dstPos.x - this->m_srcPos.x;
    float dy = this->m_dstPos.y - this->m_srcPos.y;
    float dz = this->m_dstPos.z - this->m_srcPos.z;
    float width = chainEffects->m_width;
    float length = sqrt(dz * dz + dy * dy + dx * dx);
    float step = static_cast<float>(1.0 / static_cast<double>(pointCount - 1));

    if (!(chainEffects->m_flags & 0x80)) {
        float t = step;

        for (uint32_t i = 0; i < vertexCount - 2; i += 2) {
            C3Vector point = this->m_points.m_data[(i + 2) >> 1];

            if (wave) {
                this->ApplyWaveToPosition(point, t, length);
            }

            if (arc) {
                float envelope = 1.0f - (t - 0.5f) * (t - 0.5f) * 4.0f;
                point.x = envelope * s_arcOffset.x + point.x;
                point.y = point.y + envelope * s_arcOffset.y;
                point.z = point.z + envelope * s_arcOffset.z;
            }

            C3Vector view;
            view.x = (point.x + prev.x) * 0.5f - cameraPos.x;
            view.y = (point.y + prev.y) * 0.5f - cameraPos.y;
            view.z = (point.z + prev.z) * 0.5f - cameraPos.z;

            C3Vector segment;
            segment.x = point.x - prev.x;
            segment.y = point.y - prev.y;
            segment.z = point.z - prev.z;

            C3Vector normal;
            normal.x = segment.y * view.z - segment.z * view.y;
            normal.y = segment.z * view.x - view.z * segment.x;
            normal.z = view.y * segment.x - segment.y * view.x;

            float normalLength = sqrt(normal.z * normal.z + normal.y * normal.y + normal.x * normal.x);

            if (normalLength > 0.001f) {
                normal.x = normal.x * (1.0f / normalLength);
                normal.y = normal.y * (1.0f / normalLength);
                normal.z = normal.z * (1.0f / normalLength);
            }

            C3Vector offset;
            offset.x = normal.x * width;
            offset.y = normal.y * width;
            offset.z = normal.z * width;

            C3Vector* positions = &this->m_positions.m_data[i];
            positions[0].x = (offset.x + prev.x) * 0.5f + positions[0].x;
            positions[0].z = (offset.z + prev.z) * 0.5f + positions[0].z;
            positions[0].y = (offset.y + prev.y) * 0.5f + positions[0].y;

            positions = &this->m_positions.m_data[i];
            positions[1].x = positions[1].x + (prev.x - offset.x) * 0.5f;
            positions[1].y = (prev.y - offset.y) * 0.5f + positions[1].y;
            positions[1].z = (prev.z - offset.z) * 0.5f + positions[1].z;

            positions = &this->m_positions.m_data[i];
            positions[2].x = (offset.x + point.x) * 0.5f;
            positions[2].y = (offset.y + point.y) * 0.5f;
            positions[2].z = (offset.z + point.z) * 0.5f;

            positions = &this->m_positions.m_data[i];
            positions[3].x = (point.x - offset.x) * 0.5f;
            positions[3].y = (point.y - offset.y) * 0.5f;
            positions[3].z = (point.z - offset.z) * 0.5f;

            prev = point;

            const C3Vector& a = this->m_positions.m_data[i];

            if (this->m_bounds.b.x > a.x) {
                this->m_bounds.b.x = a.x;
            }

            if (this->m_bounds.b.y > a.y) {
                this->m_bounds.b.y = a.y;
            }

            if (this->m_bounds.b.z > a.z) {
                this->m_bounds.b.z = a.z;
            }

            if (a.x > this->m_bounds.t.x) {
                this->m_bounds.t.x = a.x;
            }

            if (a.y > this->m_bounds.t.y) {
                this->m_bounds.t.y = a.y;
            }

            if (a.z > this->m_bounds.t.z) {
                this->m_bounds.t.z = a.z;
            }

            const C3Vector& b = this->m_positions.m_data[i + 1];

            if (this->m_bounds.b.x > b.x) {
                this->m_bounds.b.x = b.x;
            }

            if (this->m_bounds.b.y > b.y) {
                this->m_bounds.b.y = b.y;
            }

            if (this->m_bounds.b.z > b.z) {
                this->m_bounds.b.z = b.z;
            }

            if (b.x > this->m_bounds.t.x) {
                this->m_bounds.t.x = b.x;
            }

            if (b.y > this->m_bounds.t.y) {
                this->m_bounds.t.y = b.y;
            }

            if (b.z > this->m_bounds.t.z) {
                this->m_bounds.t.z = b.z;
            }

            t = t + step;
        }
    } else {
        C3Vector view;
        view.x = (this->m_dstPos.x + this->m_srcPos.x) * 0.5f - cameraPos.x;
        view.y = (this->m_dstPos.y + this->m_srcPos.y) * 0.5f - cameraPos.y;
        view.z = (this->m_dstPos.z + this->m_srcPos.z) * 0.5f - cameraPos.z;

        C3Vector beam;
        beam.x = this->m_dstPos.x - this->m_srcPos.x;
        beam.y = this->m_dstPos.y - this->m_srcPos.y;
        beam.z = this->m_dstPos.z - this->m_srcPos.z;

        C3Vector normal;
        normal.x = beam.y * view.z - beam.z * view.y;
        normal.y = beam.z * view.x - view.z * beam.x;
        normal.z = view.y * beam.x - beam.y * view.x;

        float normalLength = sqrt(normal.z * normal.z + normal.y * normal.y + normal.x * normal.x);

        if (normalLength > 0.001f) {
            normal.x = normal.x * (1.0f / normalLength);
            normal.y = normal.y * (1.0f / normalLength);
            normal.z = normal.z * (1.0f / normalLength);
        }

        C3Vector offset;
        offset.x = normal.x * width;
        offset.y = normal.y * width;
        offset.z = normal.z * width;

        float t = 0.0f;

        for (uint32_t i = 0; i < vertexCount; i += 2) {
            C3Vector point = this->m_points.m_data[i >> 1];

            if (wave) {
                this->ApplyWaveToPosition(point, t, length);
            }

            if (arc) {
                float envelope = 1.0f - (t - 0.5f) * (t - 0.5f) * 4.0f;
                point.x = envelope * s_arcOffset.x + point.x;
                point.y = point.y + envelope * s_arcOffset.y;
                point.z = point.z + envelope * s_arcOffset.z;
            }

            C3Vector* positions = &this->m_positions.m_data[i];
            positions[0].x = offset.x + point.x;
            positions[0].y = offset.y + point.y;
            positions[0].z = offset.z + point.z;

            positions = &this->m_positions.m_data[i];
            positions[1].x = point.x - offset.x;
            positions[1].y = point.y - offset.y;
            positions[1].z = point.z - offset.z;

            const C3Vector& a = this->m_positions.m_data[i];

            if (this->m_bounds.b.x > a.x) {
                this->m_bounds.b.x = a.x;
            }

            if (this->m_bounds.b.y > a.y) {
                this->m_bounds.b.y = a.y;
            }

            if (this->m_bounds.b.z > a.z) {
                this->m_bounds.b.z = a.z;
            }

            if (a.x > this->m_bounds.t.x) {
                this->m_bounds.t.x = a.x;
            }

            if (a.y > this->m_bounds.t.y) {
                this->m_bounds.t.y = a.y;
            }

            if (a.z > this->m_bounds.t.z) {
                this->m_bounds.t.z = a.z;
            }

            const C3Vector& b = this->m_positions.m_data[i + 1];

            if (this->m_bounds.b.x > b.x) {
                this->m_bounds.b.x = b.x;
            }

            if (this->m_bounds.b.y > b.y) {
                this->m_bounds.b.y = b.y;
            }

            if (this->m_bounds.b.z > b.z) {
                this->m_bounds.b.z = b.z;
            }

            if (b.x > this->m_bounds.t.x) {
                this->m_bounds.t.x = b.x;
            }

            if (b.y > this->m_bounds.t.y) {
                this->m_bounds.t.y = b.y;
            }

            if (b.z > this->m_bounds.t.z) {
                this->m_bounds.t.z = b.z;
            }

            t = t + step;
        }
    }

    this->m_positions.m_data[1] = this->m_points.m_data[0];
    this->m_positions.m_data[0] = this->m_positions.m_data[1];
    this->m_positions.m_data[vertexCount - 1] = this->m_points.m_data[pointCount - 1];
    this->m_positions.m_data[vertexCount - 2] = this->m_positions.m_data[vertexCount - 1];

    const C3Vector& end = this->m_positions.m_data[vertexCount - 2];

    if (this->m_bounds.b.x > end.x) {
        this->m_bounds.b.x = end.x;
    }

    if (this->m_bounds.b.y > end.y) {
        this->m_bounds.b.y = end.y;
    }

    if (this->m_bounds.b.z > end.z) {
        this->m_bounds.b.z = end.z;
    }

    if (end.x > this->m_bounds.t.x) {
        this->m_bounds.t.x = end.x;
    }

    if (end.y > this->m_bounds.t.y) {
        this->m_bounds.t.y = end.y;
    }

    if (end.z > this->m_bounds.t.z) {
        this->m_bounds.t.z = end.z;
    }

    if (this->m_batch.m_start == 0) {
        this->m_flags |= 0x10;
    }

    if (this->m_batch.m_count + this->m_batch.m_start >= this->m_positions.Count()) {
        this->m_flags |= 0x20;
    }

    if (this->m_batch.m_count <= 2 || (cull && cull(this->m_bounds))) {
        GxBufUnlock(s_buf, 0);
        return;
    }

    chainEffects = this->m_chainEffects;
    float texScale;

    if (chainEffects->m_flags & 0x100) {
        texScale = length / chainEffects->m_textureLength;
    } else {
        texScale = 1.0f;
    }

    float texOffset = this->m_texRandom + this->m_texOffset;

    if ((chainEffects->m_flags & 0x100) && (chainEffects->m_flags & 0x200)) {
        texOffset = length / chainEffects->m_textureLength + texOffset;
    }

    CGxMatrixStack& stack = g_theGxDevicePtr->m_xforms[GxXform_Tex0];
    stack.m_dirty = 1;
    stack.m_flags[stack.m_level] &= ~CGxMatrixStack::F_Identity;
    stack.m_mtx[stack.m_level] = C44Matrix(texScale, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -texOffset, 0.0f, 0.0f, 1.0f);

    CImVector emissive(this->m_color.b, this->m_color.g, this->m_color.r, 0xFF);
    g_theGxDevicePtr->RsSet(GxRs_MatEmissive, *reinterpret_cast<int32_t*>(&emissive));
    g_theGxDevicePtr->RsSet(GxRs_DepthWrite, static_cast<uint8_t>(this->m_chainEffects->m_blendMode) == 2);
    g_theGxDevicePtr->RsSet(GxRs_BlendingMode, static_cast<int32_t>(static_cast<uint8_t>(this->m_chainEffects->m_blendMode)));
    GxUpdateAlphaRef(GxRs_AlphaRef);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    g_theGxDevicePtr->RsSet(GxRs_TextureShader0, 1);

    uint32_t start = this->m_batch.m_start;
    uint32_t vertexEnd = start + this->m_batch.m_count;
    float texStep = static_cast<float>(2.0 / static_cast<double>(vertexCount - 2));
    float u = texStep * static_cast<float>(start >> 1);
    int32_t alpha = 0;

    CImVector color = this->m_color;
    GxFormatColor(color);

    uint32_t fadeIn = this->m_pulseOnStart;

    if (start == 0) {
        if (fadeIn == 0) {
            fadeIn = 2;
        } else {
            color.a = 0;
        }

        vertices[0].p = this->m_positions.m_data[0];
        vertices[0].c = color;
        vertices[0].tc.x = 0.0f;
        vertices[0].tc.y = 0.5f;
        vertices[1].p = this->m_positions.m_data[1];
        vertices[1].c = color;
        vertices[1].tc.x = 0.0f;
        vertices[1].tc.y = 0.5f;
        alpha = this->m_pulseAlphaStep;
        start = 2;
        u = texStep;
    }

    bool last = vertexEnd == vertexCount;

    if (last) {
        vertexEnd -= 2;
    }

    for (uint32_t i = start; i < this->m_pulseOnStart; i += 2) {
        color.a = static_cast<uint8_t>(alpha >> 16);
        vertices[i].p = this->m_positions.m_data[i];
        vertices[i].c = color;
        vertices[i].tc.x = u;
        vertices[i].tc.y = 0.0f;
        vertices[i + 1].p = this->m_positions.m_data[i + 1];
        vertices[i + 1].c = color;
        vertices[i + 1].tc.x = u;
        vertices[i + 1].tc.y = 1.0f;
        alpha += this->m_pulseAlphaStep;
        u = u + texStep;
    }

    color.a = this->m_color.a;

    for (uint32_t i = fadeIn; i < this->m_pulseFadeStart; i += 2) {
        vertices[i].p = this->m_positions.m_data[i];
        vertices[i].c = color;
        vertices[i].tc.x = u;
        vertices[i].tc.y = 0.0f;
        vertices[i + 1].p = this->m_positions.m_data[i + 1];
        vertices[i + 1].c = color;
        vertices[i + 1].tc.x = u;
        vertices[i + 1].tc.y = 1.0f;
        u = u + texStep;
    }

    alpha = this->m_color.a << 16;

    for (uint32_t i = this->m_pulseFadeStart; i < vertexEnd; i += 2) {
        color.a = static_cast<uint8_t>(alpha >> 16);
        vertices[i].p = this->m_positions.m_data[i];
        vertices[i].c = color;
        vertices[i].tc.x = u;
        vertices[i].tc.y = 0.0f;
        vertices[i + 1].p = this->m_positions.m_data[i + 1];
        vertices[i + 1].c = color;
        vertices[i + 1].tc.x = u;
        vertices[i + 1].tc.y = 1.0f;
        u = u + texStep;
        alpha = alpha <= this->m_pulseAlphaStep ? 0 : alpha - this->m_pulseAlphaStep;
    }

    if (last) {
        color.a = static_cast<uint8_t>(alpha >> 16);
        vertices[vertexCount - 2].p = this->m_positions.m_data[vertexCount - 2];
        vertices[vertexCount - 2].c = color;
        vertices[vertexCount - 2].tc.x = 1.0f;
        vertices[vertexCount - 2].tc.y = 0.5f;
        vertices[vertexCount - 1].p = this->m_positions.m_data[vertexCount - 1];
        vertices[vertexCount - 1].c = color;
        vertices[vertexCount - 1].tc.x = 1.0f;
        vertices[vertexCount - 1].tc.y = 0.5f;
    }

    GxBufUnlock(s_buf, 0);

    if (CShaderEffect::s_enableShaders) {
        CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
        C44Matrix worldMatrix = world.m_mtx[world.m_level];
        C44Matrix viewProj;
        GxXformViewProjNative(viewProj);
        C44Matrix worldViewProj = worldMatrix * viewProj;
        CGxMatrixStack& tex = g_theGxDevicePtr->m_xforms[GxXform_Tex0];
        C44Matrix texMatrix = tex.m_mtx[tex.m_level];
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&worldViewProj), 4);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 4, reinterpret_cast<const float*>(&texMatrix), 4);
    }

    g_theGxDevicePtr->PrimVertexPtr(s_buf, GxVBF_PCT);
    g_theGxDevicePtr->DrawNonIndexed(this->m_batch);
}

// OFFSET: 0x00B2BC20/0x1408F40F0
CLightningManager::CLightningManager() : m_vertexShader(NULL), m_pixelShader(NULL) {
    if (CShaderEffect::s_enableShaders) {
        this->m_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "Lightning");
        this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "Lightning");
    }
}

// OFFSET: 0x00B2BC80
CLightningManager::~CLightningManager() {
    if (this->m_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_vertexShader);
        this->m_vertexShader = NULL;
    }

    if (this->m_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_pixelShader);
        this->m_pixelShader = NULL;
    }

    for (uint32_t i = this->m_bolts.Count(); i; i--) {
        CLightning* bolt = reinterpret_cast<CLightning*>(reinterpret_cast<uintptr_t>(this->m_bolts.m_data[i - 1]) & ~0x1);

        if (bolt) {
            delete bolt;
        }
    }
}

// INLINED
void CLightning::Init(const SpellChainEffectsRec* chainEffects) {
    this->m_chainEffects = chainEffects;
    this->m_color.a = chainEffects->m_alpha;
    this->m_color.r = chainEffects->m_red;
    this->m_color.g = chainEffects->m_green;
    this->m_color.b = chainEffects->m_blue;
    this->m_flags &= ~0x30;
    this->Reset();
}

// OFFSET: 0x00B2BD30/0x1408F4270
uint32_t CLightningManager::Add(void (*coordUpdate)(void*, uint32_t, C3Vector*, C3Vector*), void* param, const SpellChainEffectsRec* chainEffects) {
    uint32_t index;

    if (this->m_freeList.Count()) {
        index = *this->m_freeList.Top();
        this->m_bolts.m_data[index] = reinterpret_cast<CLightning*>(reinterpret_cast<uintptr_t>(this->m_bolts.m_data[index]) & ~0x1);
        this->m_freeList.SetCount(this->m_freeList.Count() - 1);
    } else {
        index = this->m_bolts.Count();
        CLightning* bolt = new (__FILE__, __LINE__) CLightning();
        this->m_bolts.New(bolt);
    }

    this->m_bolts.m_data[index]->Init(chainEffects);

    CLightning* bolt = this->m_bolts.m_data[index];
    bolt->m_coordUpdate = coordUpdate;
    bolt->m_coordUpdateParam = param;

    return index;
}

// OFFSET: 0x00B2BE10/0x1408F43B0
void CLightningManager::Remove(uint32_t index) {
    this->m_bolts.m_data[index] = reinterpret_cast<CLightning*>(reinterpret_cast<uintptr_t>(this->m_bolts.m_data[index]) | 0x1);
    this->m_freeList.Add(1, &index);
}

// OFFSET: 0x00B2BFA0/0x1408F44A0
void CLightning::UpdateJointCount(float length, float radius) {
    int32_t count;

    if (this->m_chainEffects->m_flags & 0x1) {
        count = this->m_chainEffects->m_jointCount;
    } else {
        count = static_cast<int32_t>(static_cast<int64_t>(length / this->m_chainEffects->m_avgSegLen)) + 2;
    }

    count++;

    if (count > 1000) {
        count = 1000;
    } else if (count < 2) {
        count = 2;
    }

    int32_t oldCount;

    if (this->m_flags & 0x1) {
        oldCount = this->m_joints.Count();
    } else {
        oldCount = 0;
    }

    if (oldCount != count) {
        this->m_joints.SetCount(count);
        this->m_points.SetCount(count);
        this->m_positions.SetCount(2 * count);
    }

    if (s_pool) {
        if (static_cast<uint32_t>(s_pool->m_size) < static_cast<uint32_t>(48 * count)) {
            g_theGxDevicePtr->PoolSizeSet(s_pool, 48 * count);
            g_theGxDevicePtr->BufSizeSet(s_buf, sizeof(CGxVertexPCT), 2 * count);
        }
    } else {
        s_pool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, GxPoolUsage_Stream, 48 * count, GxPoolHintBit_Unk0, "Lightning");
        s_buf = g_theGxDevicePtr->BufCreate(s_pool, sizeof(CGxVertexPCT), 2 * count, 0);
    }

    this->m_batch.m_start = 0;
    this->m_batch.m_count = 2 * count;
    this->m_pulseOnStart = 0;
    this->m_pulseFadeStart = 2 * count;
    this->m_pulseAlphaStep = 0;

    for (int32_t i = oldCount; i < count; i++) {
        this->RandomizeJoint(this->m_joints.m_data[i], radius, 0.0f);
    }

    this->m_flags |= 0x1;
}

// OFFSET: 0x00B2C120/0x1408F4660
void CLightning::Update(float elapsed) {
    if (!(this->m_flags & 0x2)) {
        return;
    }

    if (this->m_chainEffects->m_flags & 0x10) {
        this->m_flickerTimer -= elapsed;

        if (this->m_flags & 0x4) {
            if (this->m_flickerTimer > 0.0f) {
                return;
            }

            this->Reset();
        } else if (this->m_flickerTimer <= 0.0f) {
            this->m_flickerTimer = RandomFloat(this->m_chainEffects->m_minFlickerOffDuration, this->m_chainEffects->m_maxFlickerOffDuration);
            this->m_flags |= 0x4;
        }
    }

    const SpellChainEffectsRec* chainEffects = this->m_chainEffects;

    if (chainEffects->m_texCoordScale == 0.0f) {
        this->m_texOffset = 0.0f;
    } else {
        this->m_texOffset = static_cast<float>(fmod(static_cast<double>(chainEffects->m_texCoordScale * elapsed + this->m_texOffset), 1.0));
    }

    float dz = this->m_dstPos.z - this->m_srcPos.z;
    float dy = this->m_dstPos.y - this->m_srcPos.y;
    float dx = this->m_dstPos.x - this->m_srcPos.x;
    float length = sqrt(dx * dx + (dy * dy + dz * dz));

    if (length > 10000.0f) {
        length = 10000.0f;
    }

    float radius = chainEffects->m_noiseScale * length + chainEffects->m_jointOffsetRadius;
    this->UpdateJointCount(length, radius);

    uint32_t jointCount = this->m_joints.Count();

    for (uint32_t i = 1; i < jointCount; i++) {
        LightningJoint& joint = this->m_joints.m_data[i];
        joint.m_jumpTimer -= elapsed;

        if ((this->m_chainEffects->m_flags & 0x4) && joint.m_jumpTimer < 0.0f) {
            this->RandomizeJoint(joint, radius, this->m_chainEffects->m_jointSmoothness);
        } else {
            float scale = (1.0f - this->m_chainEffects->m_jointSmoothness) * elapsed;
            joint.m_pos.x = joint.m_pos.x + joint.m_velocity.x * scale;
            joint.m_pos.y = joint.m_pos.y + joint.m_velocity.y * scale;
            joint.m_pos.z = joint.m_pos.z + joint.m_velocity.z * scale;
        }
    }

    chainEffects = this->m_chainEffects;
    this->m_waveAngle = this->m_waveSpin * elapsed + this->m_waveAngle;
    this->m_wavePhase = this->m_wavePhase - chainEffects->m_waveSpeed * elapsed;
    this->m_arcAngle = this->m_arcSpin * elapsed + this->m_arcAngle;

    if (!(chainEffects->m_flags & 0x20)) {
        return;
    }

    this->m_pulsePos = chainEffects->m_pulseSpeed * elapsed + this->m_pulsePos;

    if (!(this->m_flags & 0x8) && !(chainEffects->m_flags & 0x40)) {
        return;
    }

    float pulseLength = chainEffects->m_pulseFadeLength * 2.0f + chainEffects->m_pulseOnLength;

    if (this->m_pulsePos - pulseLength < length) {
        if (this->m_pulsePos < 0.0f) {
            this->Reset();
            this->m_flags &= ~0x8;
            this->m_pulsePos = pulseLength + length;
        }
    } else {
        this->Reset();
        this->m_flags &= ~0x8;
    }
}

// OFFSET: 0x00B2C400/0x1408F4980
void CLightningManager::Update(float elapsed) {
    for (uint32_t i = this->m_bolts.Count(); i;) {
        i--;

        if (!(reinterpret_cast<uintptr_t>(this->m_bolts.m_data[i]) & 0x1)) {
            this->m_bolts.m_data[i]->Update(elapsed);
        }
    }
}

// OFFSET: 0x00B2C440
void CLightningManager::Render(const C3Vector& cameraPos, int32_t (*cull)(const CAaBox& box)) {
    g_theGxDevicePtr->XformPush(GxXform_World);
    g_theGxDevicePtr->XformPush(GxXform_Tex0);

    C3Vector move(-cameraPos.x, -cameraPos.y, -cameraPos.z);
    CGxMatrixStack& world = g_theGxDevicePtr->m_xforms[GxXform_World];
    world.m_dirty = 1;
    world.m_flags[world.m_level] &= ~CGxMatrixStack::F_Identity;
    world.m_mtx[world.m_level].Translate(move);

    g_theGxDevicePtr->RsPush();
    g_theGxDevicePtr->RsSet(GxRs_MatDiffuse, static_cast<int32_t>(0xFF000000));
    g_theGxDevicePtr->RsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_Fog, 0);
    g_theGxDevicePtr->RsSet(GxRs_ColorOp0, 0);
    g_theGxDevicePtr->RsSet(GxRs_AlphaOp0, 0);
    g_theGxDevicePtr->RsSet(GxRs_ColorWrite, 15);

    if (CShaderEffect::s_enableShaders) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_vertexShader->m_permutes[0]);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_pixelShader->m_permutes[0]);
    }

    for (int32_t layer = 0; layer < 4; layer++) {
        for (uint32_t i = this->m_bolts.Count(); i;) {
            i--;

            CLightning* bolt = this->m_bolts.m_data[i];

            if (reinterpret_cast<uintptr_t>(bolt) & 0x1) {
                continue;
            }

            if (bolt->m_chainEffects && bolt->m_chainEffects->m_renderLayer == layer) {
                bolt->Render(i, cameraPos, cull);
            }
        }
    }

    g_theGxDevicePtr->RsPop();

    CGxMatrixStack& worldStack = g_theGxDevicePtr->m_xforms[GxXform_World];

    if (worldStack.m_level) {
        worldStack.m_level--;
    }

    worldStack.m_dirty = 1;

    CGxMatrixStack& texStack = g_theGxDevicePtr->m_xforms[GxXform_Tex0];
    texStack.m_dirty = 1;

    if (texStack.m_level) {
        texStack.m_level--;
    }
}
