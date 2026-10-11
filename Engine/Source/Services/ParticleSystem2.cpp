#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/GfxSingletonManager.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRange.h"

// STATIC: 0x00D27CD0
static int32_t s_particleRenderEnable = 1;

// STATIC: 0x00D27CDC
static C3Vector s_particleNormal(0.0f, 0.0f, 1.0f);

// STATIC: 0x00D27CE8
static C44Matrix s_particleXform(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

// STATIC: 0x00D27D28
static C33Matrix s_billboardXform(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

// STATIC: 0x00D27D4C
static int32_t s_fixedOne = 0x10000;

// STATIC: 0x00D27D50
static int32_t s_fixedMinLife = 655;

// STATIC: 0x00D27D54
static const float s_cornerOffsets[4][2] = { { -1.0f, 1.0f }, { -1.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, -1.0f } };

// STATIC: 0x00D27D74
static const float s_cornerTexCoords[4][2] = { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } };

// STATIC: 0x01008B40
uint32_t CParticleEmitter2::s_initCount;

// STATIC: 0x01008B44
CGxBuf* CParticleEmitter2::s_indexBuf;

// STATIC: 0x01008B48
CGxPool* CParticleEmitter2::s_indexPool;

// STATIC: 0x01008B4C
float CParticleEmitter2::s_viewDist;

// STATIC: 0x01008B50
float CParticleEmitter2::s_rndTable[128];

// STATIC: 0x01008D50
uint8_t CParticleEmitter2::s_noIndexBuffer;

// STATIC: 0x01008D54
static int32_t s_particleBounds;

// STATIC: 0x01008D58
std::priority_queue<CSortableParticleRecord> CParticleEmitter2::s_pq;

// STATIC: 0x01008B30
void* ParticleSystemManager::sm_projectParam;

// STATIC: 0x01008B34
int32_t (*ParticleSystemManager::sm_projectCallback)(const C3Vector&, float&, void*);

// OFFSET: 0x00B199A0/0x1408E0150
static uint32_t PartTrackSearch(const M2Array<fixed16>& times, float t) {
    uint32_t lo = 0;
    uint32_t hi = times.m_count - 1;

    if (times.m_count == 1) {
        return 0;
    }

    while (1) {
        uint32_t mid = (hi + lo) >> 1;

        if (times[mid].m_value * 0.000030518509f <= t) {
            lo = mid + 1;

            if (mid + 1 >= times.m_count - 1 || times[mid + 1].m_value * 0.000030518509f > t) {
                return mid;
            }
        } else {
            hi = mid - 1;
        }

        if (lo >= hi) {
            return lo;
        }
    }
}

// OFFSET: 0x00B19A10/0x1408E0200
static float PartTrackKeys(const M2Array<fixed16>& times, float t, uint32_t& key0, uint32_t& key1) {
    if (times.m_count == 2) {
        key0 = 0;
        key1 = 1;
        return t;
    }

    if (times.m_count == 3) {
        float mid = times[1].m_value * 0.000030518509f;

        if (t >= mid) {
            key0 = 1;
            key1 = 2;
            return (t - mid) / (1.0f - mid);
        }

        key0 = 0;
        key1 = 1;
        return t / mid;
    }

    key0 = PartTrackSearch(times, t);
    key1 = key0 + 1;

    return (t - times[key0].m_value * 0.000030518509f) / (0.000030518509f * (times[key1].m_value - times[key0].m_value));
}

// OFFSET: 0x00B19AF0/0x1408E0330
static C2Vector __stdcall PartTrackSample(const M2PartTrack<C2Vector>& track, float t) {
    C2Vector result;

    if (track.m_values.m_count == 1) {
        result = track.m_values[0];
        return result;
    }

    uint32_t key0;
    uint32_t key1;
    float blend = PartTrackKeys(track.m_times, t, key0, key1);

    result.x = (track.m_values[key1].x - track.m_values[key0].x) * blend + track.m_values[key0].x;
    result.y = blend * (track.m_values[key1].y - track.m_values[key0].y) + track.m_values[key0].y;

    return result;
}

// OFFSET: 0x00B19B80
static float __stdcall PartTrackSample(const M2PartTrack<fixed16>& track, float t) {
    if (track.m_values.m_count == 1) {
        return track.m_values[0].m_value * 0.000030518509f;
    }

    uint32_t key0;
    uint32_t key1;
    float blend = PartTrackKeys(track.m_times, t, key0, key1);

    return blend * (0.000030518509f * track.m_values[key1].m_value - track.m_values[key0].m_value * 0.000030518509f) + track.m_values[key0].m_value * 0.000030518509f;
}

// OFFSET: 0x00B19C00
static int32_t __stdcall PartTrackSample(const M2PartTrack<uint16_t>& track, float t) {
    if (track.m_values.m_count == 1) {
        return track.m_values[0];
    }

    uint32_t key0;
    uint32_t key1;
    float blend = PartTrackKeys(track.m_times, t, key0, key1);

    return static_cast<int32_t>(CMath::fint_(blend * (track.m_values[key1] - track.m_values[key0]) + track.m_values[key0]));
}

// OFFSET: 0x00B1EFB0
static uint32_t PopIndex(TSGrowableArray<uint32_t>& array) {
    uint32_t value = *array.Top();
    array.SetCount(array.Count() - 1);

    return value;
}

// INLINED
static inline void SetVertex(CGxVertexPCT& vertex, const C3Vector& position, const CImVector& color, float u, float v) {
    vertex.p = position;
    vertex.c = color;
    vertex.tc.x = u;
    vertex.tc.y = v;
}

// INLINED
static inline void SetVertex(CGxVertexPNCT& vertex, const C3Vector& position, const CImVector& color, float u, float v) {
    vertex.m_position = position;
    vertex.m_normal = s_particleNormal;
    vertex.m_color = color;
    vertex.m_texture.x = u;
    vertex.m_texture.y = v;
}

// INLINED
static inline const C3Vector& VertexPosition(const CGxVertexPCT& vertex) {
    return vertex.p;
}

// INLINED
static inline const C3Vector& VertexPosition(const CGxVertexPNCT& vertex) {
    return vertex.m_position;
}

// INLINED
bool operator<(const CSortableParticleRecord& l, const CSortableParticleRecord& r) {
    return l.m_depth < r.m_depth;
}

// INLINED
template <>
CParticle2* CParticleEmitter2::NextParticle<CParticle2>(uint32_t index) {
    return &this->m_particles[this->m_liveList[index]];
}

// INLINED
template <>
CParticle2* CParticleEmitter2::NextParticle<CParticle2_Model>(uint32_t index) {
    return &this->m_particleModels[this->m_liveList[index]];
}

// INLINED
template <>
CParticle2* CParticleEmitter2::NextParticle<CSortableParticleRecord>(uint32_t index) {
    CParticle2* particle = CParticleEmitter2::s_pq.top().m_particle;
    CParticleEmitter2::s_pq.pop();
    return particle;
}

// PROVEN: 6a61881b IDENTICAL/NONE
// OFFSET: 0x00B13EF0
void CParticleEmitter2::SetModel(CM2Scene* scene, const char* name) {
    if (scene && name && *name) {
        this->m_scene = scene;
        this->m_model = scene->CreateModel(name, 0, 3);

        if (this->m_model) {
            this->m_params.m_hasModel = 1;
        }
    }
}

// PROVEN: 9226a371 IDENTICAL/NONE
// OFFSET: 0x00903F20
CAaBox& CParticleEmitter2::WorldBounds() {
    return this->m_worldBounds;
}

// PROVEN: 27cb204e IDENTICAL/DIFFERENT
// OFFSET: 0x00B15230/0x1408DB0B0
void CParticleEmitter2::SetMaterial(const CParticleMat& material, CTexture* texture) {
    if (texture) {
        TextureAddRef(texture);
    }

    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    this->m_texture = texture;
    this->m_params.m_materialBlend = material.m_blendMode;
    this->m_params.m_materialFlags = material.m_flags;
}

// PROVEN: dd36dcb2 IDENTICAL/UNPROVEN
// OFFSET: 0x00B13F40/0x1408D9DA0
void CParticleEmitter2::SetTexture(CTexture* texture) {
    if (texture) {
        TextureAddRef(texture);
    }

    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    this->m_texture = texture;
}

// OFFSET: 0x00B14110/0x1408D9FA0
void CParticleEmitter2::SetTextureDimensions(uint32_t rows, uint32_t cols) {
    if ((rows & (rows - 1)) || (cols & (cols - 1)) || !rows || !cols) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    this->m_textureRows = rows;
    this->m_textureCols = cols;
    this->m_textureCellShift = -1;

    for (uint32_t i = cols; i; i >>= 1) {
        this->m_textureCellShift++;
    }

    this->m_textureCellWidth = 1.0f / this->m_textureCols;
    this->m_textureCellHeight = 1.0f / rows;
}

// PROVEN: 2991d213 IDENTICAL/IDENTICAL
// OFFSET: 0x00B141A0/0x1408DA020
void CParticleEmitter2::SetParticleStyle(int32_t head, int32_t tail, float tailLength, int32_t clampTail) {
    if (head) {
        this->m_flags |= 0x4;
    } else {
        this->m_flags &= ~0x4;
    }

    if (tail) {
        this->m_flags |= 0x8;
    } else {
        this->m_flags &= ~0x8;
    }

    if (clampTail) {
        this->m_flags |= 0x20000;
    } else {
        this->m_flags &= ~0x20000;
    }

    this->m_vertsPerParticle = 0;
    this->m_indicesPerParticle = 0;
    this->m_params.m_tailLength = tailLength;

    if (this->m_flags & 0x4) {
        this->m_vertsPerParticle = 4;
        this->m_indicesPerParticle = 6;
    }

    if (this->m_flags & 0x8) {
        this->m_vertsPerParticle += 4;
        this->m_indicesPerParticle += 6;
    }
}

// PROVEN: 9c0ba018 IDENTICAL/IDENTICAL
// OFFSET: 0x00B14240/0x1408DA0B0
void CParticleEmitter2::SetZsource(float zsource) {
    this->m_params.m_zsource = zsource;

    if (fabs(zsource) < 0.001f) {
        this->m_params.m_zsource = 0.0f;
    }
}

// OFFSET: 0x00B14270
void CParticleEmitter2::SetFollowParams(float speed1, float scale1, float speed2, float scale2) {
    float speedDelta = speed2 - speed1;

    if (fabs(speedDelta) < 0.00000023841858f) {
        this->m_followSlope = 0.0f;
        this->m_followIntercept = 0.0f;
        return;
    }

    float slope = (scale2 - scale1) / speedDelta;
    this->m_followSlope = slope;
    this->m_followIntercept = scale1 - speed1 * slope;
}

// PROVEN: d815f24e IDENTICAL/IDENTICAL
// OFFSET: 0x00B142E0/0x1408DA130
void CParticleEmitter2::SetChooseRandomTexture(int32_t choose) {
    if (this->m_textureRows * this->m_textureCols > 1 && choose) {
        this->m_flags |= 0x100000;
    } else {
        this->m_flags &= ~0x100000;
    }
}

// PROVEN: 6a30d9a2 IDENTICAL/IDENTICAL
// OFFSET: 0x00B14320/0x1408DA160
CParticleEmitter2* CParticleEmitter2::AddRef() {
    this->m_refCount++;

    return this;
}

// OFFSET: 0x00B14330
void CParticleEmitter2::DecRef() {
    if (this->m_refCount-- == 1) {
        delete this;
    }
}

// OFFSET: 0x00B14660/0x1408DA4E0
void CParticleEmitter2::Init() {
    if (!CParticleEmitter2::s_initCount) {
        if (CParticleEmitter2::s_noIndexBuffer) {
            CParticleEmitter2::s_initCount++;
            CParticleEmitter2::s_indexPool = NULL;
            CParticleEmitter2::s_indexBuf = NULL;
            return;
        }

        CParticleEmitter2::s_indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Static, sizeof(uint16_t) * 0x1FFF8, GxPoolHintBit_Unk2, "CParticleEmitter2_idx");
        CParticleEmitter2::s_indexBuf = g_theGxDevicePtr->BufCreate(CParticleEmitter2::s_indexPool, sizeof(uint16_t), 0x1FFF8, 0);
    }

    CParticleEmitter2::s_initCount++;
}

// OFFSET: 0x00B146D0
void CParticleEmitter2::Destroy() {
    if (--CParticleEmitter2::s_initCount) {
        return;
    }

    if (CParticleEmitter2::s_indexBuf) {
        CGxBuf* buf = CParticleEmitter2::s_indexBuf;

        if (buf != g_theGxDevicePtr->BufStream(CParticleEmitter2::s_indexBuf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buf);
        }
    }

    if (CParticleEmitter2::s_indexPool) {
        g_theGxDevicePtr->PoolDestroy(CParticleEmitter2::s_indexPool);
    }
}

// OFFSET: 0x00B14740/0x1408DA5F0
float CParticleEmitter2::CalcVelocity() {
    return (CRandom::sfloat_(this->m_random) * this->m_params.m_variation + 1.0f) * this->m_params.m_speed;
}

// OFFSET: 0x00B19C90
CImVector CParticleEmitter2::InterpolateColorTrack(float t) {
    const M2PartTrack<C3Vector>* track = this->m_params.m_colorTrack;
    CImVector color;

    if (track->m_values.m_count == 1) {
        color.b = static_cast<uint8_t>(CMath::fint_(track->m_values[0].z));
        color.g = static_cast<uint8_t>(CMath::fint_(track->m_values[0].y));
        color.r = static_cast<uint8_t>(CMath::fint_(track->m_values[0].x));
        color.a = 0xFF;
        return color;
    }

    uint32_t key0;
    uint32_t key1;
    float blend = PartTrackKeys(track->m_times, t, key0, key1);

    float r;
    float g;
    float b;

    if (this->m_flags & 0x10) {
        r = (this->m_params.m_replacementColors[key1].x - this->m_params.m_replacementColors[key0].x) * blend + this->m_params.m_replacementColors[key0].x;
        g = (this->m_params.m_replacementColors[key1].y - this->m_params.m_replacementColors[key0].y) * blend + this->m_params.m_replacementColors[key0].y;
        b = blend * (this->m_params.m_replacementColors[key1].z - this->m_params.m_replacementColors[key0].z) + this->m_params.m_replacementColors[key0].z;
    } else {
        const C3Vector& value0 = track->m_values[key0];
        const C3Vector& value1 = track->m_values[key1];
        r = (value1.x - value0.x) * blend + value0.x;
        g = (value1.y - value0.y) * blend + value0.y;
        b = blend * (value1.z - value0.z) + value0.z;
    }

    color.b = static_cast<uint8_t>(CMath::fint_(b));
    color.a = 0xFF;
    color.r = static_cast<uint8_t>(CMath::fint_(r));
    color.g = static_cast<uint8_t>(CMath::fint_(g));

    return color;
}

// OFFSET: 0x00B19E60/0x1408E0760
void CParticleEmitter2::ProjectParticle(CParticle2& particle) {
    float height;

    if (!ParticleSystemManager::sm_projectCallback(particle.m_position, height, ParticleSystemManager::sm_projectParam)) {
        return;
    }

    float life = particle.m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

    if (life <= 0.001f) {
        life = 0.001f;
    }

    C2Vector scale = PartTrackSample(*this->m_params.m_scaleTrack, particle.m_age / life);

    if (scale.x <= scale.y) {
        particle.m_position.z = scale.y + height;
    } else {
        particle.m_position.z = scale.x + height;
    }
}

// OFFSET: 0x00B19F00/0x1408E0810
void CParticleEmitter2::CreateParticle(CParticle2& particle, float time, const C44Matrix& transform) {
    float age = CRandom::ufloat_(this->m_random) * time;
    particle.m_lifeSeed = fixed16(CRandom::sfloat_(this->m_random));

    float life = particle.m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

    if (life <= 0.001f) {
        life = 0.001f;
    }

    age = fmod(age, life);
    particle.m_age = age >= 0.0f ? age : 0.0f;
    particle.m_randomSeed = static_cast<uint16_t>(CRandom::uint32_(this->m_random));
    particle.m_position.x = 0.0f;
    particle.m_position.y = 0.0f;
    particle.m_position.z = 0.0f;

    if (!(this->m_flags & 0x200)) {
        particle.m_position *= transform;

        if (this->m_flags & 0x40000) {
            this->ProjectParticle(particle);
        }
    }

    float velocity = this->CalcVelocity();
    C3Vector direction = CRandom::C3Vector_(this->m_random);

    particle.m_velocity.x = direction.x * velocity + this->m_parentVelocity.x;
    particle.m_velocity.y = direction.y * velocity + this->m_parentVelocity.y;
    particle.m_velocity.z = velocity * direction.z + this->m_parentVelocity.z;

    particle.m_velocity.FlushDenormals();
}

// OFFSET: 0x00B148C0/0x1408DA660
void CParticleEmitter2::CreateParticle(CParticle2_Model& particle, float time, const C44Matrix& transform) {
    this->CreateParticle(static_cast<CParticle2&>(particle), time, transform);

    if (!(this->m_flags & 0x200)) {
        C33Matrix rotation(transform);
        C3Vector axis(rotation.a0, rotation.a1, rotation.a2);
        float lengthSq = axis.y * axis.y + axis.z * axis.z + axis.x * axis.x;

        if (lengthSq > 0.00000023841858f) {
            float scale = 1.0f / sqrt(lengthSq);
            rotation.a0 = axis.x * scale;
            rotation.a1 = axis.y * scale;
            rotation.a2 = scale * axis.z;
        }

        particle.m_orientation.FromRotationMatrix(rotation);
        reinterpret_cast<C4Vector&>(particle.m_orientation).NormalizeSafe();
    }

    float tumbleZ = CRandom::ufloat_(this->m_random);
    float tumbleY = CRandom::ufloat_(this->m_random);

    particle.m_angularVelocity.x = CRandom::ufloat_(this->m_random) * this->m_tumbleSpanX + this->m_tumbleBaseX;
    particle.m_angularVelocity.y = (tumbleY + 1.0f) * this->m_tumbleSpanY;
    particle.m_angularVelocity.z = (tumbleZ + 1.0f) * this->m_tumbleSpanZ;

    if (this->m_flags & 0x10000) {
        float signZ = (CRandom::uint32_(this->m_random) & 1) ? 1.0f : -1.0f;
        float signY = (CRandom::uint32_(this->m_random) & 1) ? 1.0f : -1.0f;
        float signX = (CRandom::uint32_(this->m_random) & 1) ? 1.0f : -1.0f;

        particle.m_angularVelocity.x = signX * particle.m_angularVelocity.x;
        particle.m_angularVelocity.y = particle.m_angularVelocity.y * signY;
        particle.m_angularVelocity.z = particle.m_angularVelocity.z * signZ;
    }

    if (!particle.m_model) {
        particle.m_model = this->m_scene->CreateModel(this->m_model->m_shared->m_fileName, 0, 3);
    }
}

// OFFSET: 0x00B14B20
int32_t CParticleEmitter2::MoveParticle(CParticle2& particle, float deltaTime) {
    if (this->m_windTime > particle.m_age) {
        particle.m_velocity.x = this->m_windVectorX * deltaTime + particle.m_velocity.x;
        particle.m_velocity.y = this->m_windVectorY * deltaTime + particle.m_velocity.y;
        particle.m_velocity.z = deltaTime * this->m_windVectorZ + particle.m_velocity.z;

        particle.m_velocity.FlushDenormals();
    }

    if ((this->m_flags & 0x80000) && deltaTime + deltaTime < particle.m_age) {
        particle.m_position.x = this->m_burstOffset.x + particle.m_position.x;
        particle.m_position.y = this->m_burstOffset.y + particle.m_position.y;
        particle.m_position.z = this->m_burstOffset.z + particle.m_position.z;
    }

    float moveX = particle.m_velocity.x * deltaTime;
    float moveY = particle.m_velocity.y * deltaTime;
    float moveZ = particle.m_velocity.z * deltaTime;

    particle.m_position.x = moveX + particle.m_position.x;
    particle.m_position.y = moveY + particle.m_position.y;
    particle.m_position.z = moveZ + particle.m_position.z - this->m_params.m_gravity * deltaTime * deltaTime * 0.5f;
    particle.m_velocity.z = particle.m_velocity.z - this->m_params.m_gravity * deltaTime;

    if (0.0f != this->m_drag) {
        float drag = deltaTime * this->m_drag;

        if (drag > 1.0f) {
            drag = 1.0f;
        }

        particle.m_velocity.x = particle.m_velocity.x - drag * particle.m_velocity.x;
        particle.m_velocity.y = particle.m_velocity.y - particle.m_velocity.y * drag;
        particle.m_velocity.z = particle.m_velocity.z - drag * particle.m_velocity.z;
    }

    particle.m_velocity.FlushDenormals();

    if (!(this->m_flags & 0x1000)) {
        return 1;
    }

    float dot;
    float x;

    if (this->m_flags & 0x200) {
        dot = particle.m_position.z * moveZ + particle.m_position.y * moveY;
        x = particle.m_position.x;
    } else {
        x = particle.m_position.x - this->m_xform.d0;
        dot = (particle.m_position.z - this->m_xform.d2) * moveZ + (particle.m_position.y - this->m_xform.d1) * moveY;
    }

    if (dot + x * moveX > 0.0f) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x00B14D50
void CParticleEmitter2::InterpolateAllTracksSimple(const CParticle2& particle, TrackValues* values) {
    int32_t life = static_cast<int32_t>(static_cast<int64_t>(this->m_params.m_life * 65536.0f));
    int32_t age = static_cast<int32_t>(static_cast<int64_t>(particle.m_age * 65536.0f));
    int32_t midTime = static_cast<int32_t>(static_cast<int64_t>(this->m_params.m_simpleMidTime * 65536.0f));
    int32_t alphaScale = static_cast<int32_t>(static_cast<int64_t>(65536.0f * this->m_alphaScale));
    int32_t t = static_cast<int32_t>((static_cast<int64_t>(age) << 16) / std::max(s_fixedMinLife, life));
    const SimpleKey* key = this->m_params.m_simpleKeys;
    int32_t numerator;
    int32_t denominator;

    if (t >= midTime) {
        numerator = t - midTime;
        denominator = s_fixedOne - midTime;
        key++;
    } else {
        numerator = t;
        denominator = midTime;
    }

    int32_t blend = static_cast<int32_t>((static_cast<int64_t>(numerator) << 16) / denominator);
    int32_t color[4];

    for (uint32_t i = 0; i < 4; i++) {
        color[i] = blend * key->m_deltaColor[i] + key->m_color[i];
    }

    int32_t alpha = static_cast<int32_t>((static_cast<int64_t>(alphaScale) * color[3]) >> 16);
    values->m_color.r = static_cast<uint8_t>(color[0] >> 16);
    values->m_color.g = static_cast<uint8_t>(color[1] >> 16);
    values->m_color.a = static_cast<uint8_t>(alpha >> 16);
    values->m_color.b = static_cast<uint8_t>(color[2] >> 16);

    float scale = static_cast<float>(blend) * key->m_deltaScale * 0.000015258789f + key->m_scale;
    values->m_scale.x = scale;
    values->m_scale.y = scale;
    values->m_headCell = static_cast<uint32_t>(blend * key->m_deltaHeadCell + key->m_headCell) >> 16;
    values->m_tailCell = static_cast<uint32_t>(blend * key->m_deltaTailCell + key->m_tailCell) >> 16;
}

// OFFSET: 0x00B1A170
void CParticleEmitter2::InterpolateAllTracks(const CParticle2& particle, TrackValues* values) {
    float life = particle.m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

    if (life <= 0.001f) {
        life = 0.001f;
    }

    float t = particle.m_age / life;
    CRndSeed seed(particle.m_randomSeed);

    values->m_color = this->InterpolateColorTrack(t);
    values->m_color.a = static_cast<uint8_t>(CMath::fint_(PartTrackSample(*this->m_params.m_alphaTrack, t) * this->m_alphaScale * 255.0f));
    values->m_scale = PartTrackSample(*this->m_params.m_scaleTrack, t);
    values->m_headCell = 0;
    values->m_tailCell = 0;

    if (this->m_params.m_headCellTrack->m_times.m_count) {
        values->m_headCell = PartTrackSample(*this->m_params.m_headCellTrack, t);
    } else if (this->m_flags & 0x100000) {
        values->m_headCell = CRandom::dice_(this->m_textureRows * this->m_textureCols, seed);
    }

    if (this->m_params.m_tailCellTrack->m_times.m_count) {
        values->m_tailCell = PartTrackSample(*this->m_params.m_tailCellTrack, t);
    }

    if (this->m_flags & 0x800000) {
        float variationY = CRandom::sfloat_(seed);
        float scaleX = CRandom::sfloat_(seed) * this->m_params.m_scaleVariationX + 1.0f;
        float scaleY = this->m_params.m_scaleVariationY * variationY + 1.0f;

        if (scaleY <= 0.000099999997f) {
            scaleY = 0.000099999997f;
        }

        if (scaleX <= 0.000099999997f) {
            values->m_scale.x = 0.000099999997f * values->m_scale.x;
        } else {
            values->m_scale.x = scaleX * values->m_scale.x;
        }

        values->m_scale.y = scaleY * values->m_scale.y;
    } else {
        float scaleXY = CRandom::sfloat_(seed) * this->m_params.m_scaleVariationX + 1.0f;

        if (scaleXY < 0.000099999997f) {
            scaleXY = 0.000099999997f;
        }

        values->m_scale.x = values->m_scale.x * scaleXY;
        values->m_scale.y = scaleXY * values->m_scale.y;
    }
}

// OFFSET: 0x00B14F20
void CParticleEmitter2::RandomizeSpin(const CParticle2& particle, float* spin, float* spinRate) {
    if (0.0f == this->m_params.m_initialSpinVariation && 0.0f == this->m_params.m_spinVariation) {
        *spin = this->m_params.m_initialSpin;
        *spinRate = this->m_params.m_spin;
        return;
    }

    CRndSeed seed(particle.m_randomSeed);

    if (0.0f == this->m_params.m_initialSpinVariation) {
        *spin = this->m_params.m_initialSpin;
    } else {
        *spin = CRandom::sfloat_(seed) * this->m_params.m_initialSpinVariation + this->m_params.m_initialSpin;
    }

    if (0.0f == this->m_params.m_spinVariation) {
        *spinRate = this->m_params.m_spin;
    } else {
        *spinRate = CRandom::sfloat_(seed) * this->m_params.m_spinVariation + this->m_params.m_spin;
    }
}

// OFFSET: 0x00B15080/0x1408DAEC0
void __stdcall CParticleEmitter2::RenderIndices(CGxBuf* buf) {
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));
    uint16_t* end = indices + 0x1FFF8;
    uint16_t vertex = 0;

    while (indices < end) {
        indices[0] = vertex;
        indices[1] = vertex + 1;
        indices[2] = vertex + 2;
        indices[3] = vertex + 3;
        indices[4] = vertex + 2;
        indices[5] = vertex + 1;
        indices += 6;
        vertex += 4;
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x00B19540
void CParticleEmitter2::RenderParticlesPrep(const C44Matrix* frameOfReference, const C44Matrix& view) {
    C44Matrix translate;
    translate.d0 = -this->m_viewPos.x;
    translate.d1 = -this->m_viewPos.y;
    translate.d2 = -this->m_viewPos.z;

    if (this->m_flags & 0x200) {
        s_particleXform = this->m_xform * translate * view;
    } else if (frameOfReference) {
        s_particleXform = *frameOfReference * translate * view;
    } else {
        s_particleXform = translate * view;
    }

    s_particleNormal.x = view.c0;
    s_particleNormal.y = view.c1;
    s_particleNormal.z = view.c2;

    if (!(this->m_flags & 0x4000)) {
        return;
    }

    s_billboardXform = C33Matrix(s_particleXform);

    if ((this->m_flags & 0x200) && fabs(this->m_xformScale) >= 0.00000023841858f) {
        s_billboardXform.Scale(1.0f / this->m_xformScale);
    }

    this->m_billboardAxis.x = s_billboardXform.c0;
    this->m_billboardAxis.y = s_billboardXform.c1;
    this->m_billboardAxis.z = s_billboardXform.c2;

    float lengthSq = this->m_billboardAxis.z * this->m_billboardAxis.z + this->m_billboardAxis.y * this->m_billboardAxis.y + this->m_billboardAxis.x * this->m_billboardAxis.x;

    if (lengthSq > 0.00000023841858f) {
        float scale = 1.0f / sqrt(lengthSq);
        this->m_billboardAxis.x = this->m_billboardAxis.x * scale;
        this->m_billboardAxis.y = this->m_billboardAxis.y * scale;
        this->m_billboardAxis.z = scale * this->m_billboardAxis.z;
    }
}

// OFFSET: 0x00B15100/0x1408DAF50
void CParticleEmitter2::RenderParticleVertices(CGxBuf* buf, uint32_t vertexCount, uint32_t indexCount) {
    if (!s_particleRenderEnable) {
        return;
    }

    if (!CParticleEmitter2::s_indexBuf->unk_001C || !CParticleEmitter2::s_indexBuf->unk_001D) {
        CParticleEmitter2::RenderIndices(CParticleEmitter2::s_indexBuf);
    }

    C44Matrix identity;
    g_theGxDevicePtr->XformSetView(identity);

    GxPrimVertexPtr(buf, (this->m_params.m_materialFlags & 0x1) ? GxVBF_PNCT : GxVBF_PCT);
    g_theGxDevicePtr->PrimIndexPtr(CParticleEmitter2::s_indexBuf);

    CShaderEffect::SetTexMtx_Identity(0);
    CShaderEffect::SetDefaultShaders(0, 0);
    CShaderEffect::UpdateWorldViewMatrix();

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = vertexCount - 1;
    g_theGxDevicePtr->Draw(batch);
}

// OFFSET: 0x00B1A480
int32_t CParticleEmitter2::RenderParticle(CParticle2_Model& particle, const C44Matrix* frameOfReference) {
    uint32_t twinkle = 0;

    if (this->m_twinkleOnOff < 1.0f || 0.0f != this->m_twinkleScaleSpan) {
        twinkle = (static_cast<uint8_t>(CMath::fint_(this->m_twinkleFPS * particle.m_age)) + (reinterpret_cast<uintptr_t>(&particle) >> 5)) & 0x7F;
    }

    if (this->m_twinkleOnOff < 1.0f && this->m_twinkleOnOff < CParticleEmitter2::s_rndTable[twinkle]) {
        CM2Model* model = particle.m_model;

        if (model->m_attachParent) {
            model->m_fFlags &= ~0x80;
        } else {
            model->m_fFlags &= ~0x8;
        }

        if (model->m_attachParent) {
            model->m_fFlags &= ~0x20000;
        } else {
            model->m_fFlags &= ~0x10000;
        }

        return 0;
    }

    float life = particle.m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

    if (life <= 0.001f) {
        life = 0.001f;
    }

    float t = particle.m_age / life;
    CImVector color = this->InterpolateColorTrack(t);
    float alpha = PartTrackSample(*this->m_params.m_alphaTrack, t) * this->m_alphaScale * 255.0f;
    uint8_t alphaByte = static_cast<uint8_t>(CMath::fint_(alpha));
    C2Vector scale = PartTrackSample(*this->m_params.m_scaleTrack, t);

    float twinkleScale = CParticleEmitter2::s_rndTable[twinkle] * this->m_twinkleScaleSpan + this->m_twinkleScaleBase;
    scale.x = scale.x * twinkleScale;
    scale.y = twinkleScale * scale.y;

    if (!(this->m_flags & 0x200) && (this->m_flags & 0x400)) {
        scale.x = scale.x * this->m_xformScale;
        scale.y = this->m_xformScale * scale.y;
    }

    C44Matrix transform(particle.m_orientation);
    transform.Scale(C3Vector(scale.x, scale.y, (scale.x + scale.y) * 0.5f));
    transform.d0 = particle.m_position.x;
    transform.d1 = particle.m_position.y;
    transform.d2 = particle.m_position.z;

    if (this->m_flags & 0x200) {
        transform *= this->m_xform;
    } else if (frameOfReference) {
        transform *= *frameOfReference;
    }

    CM2Model* model = particle.m_model;
    model->m_fFlags |= 0x8000;
    model->SetWorldTransform(transform);

    model = particle.m_model;
    model->m_diffuseTintR = color.r * 0.0039215689f;
    model->m_diffuseTintG = color.g * 0.0039215689f;
    model->m_diffuseTintB = color.b * 0.0039215689f;
    particle.m_model->m_fadeAlpha = 0.0039215689f * alphaByte;

    model = particle.m_model;

    if (model->m_attachParent) {
        model->m_fFlags |= 0x80;
    } else {
        model->m_fFlags |= 0x8;
    }

    if (model->m_attachParent) {
        model->m_fFlags |= 0x20000;
    } else {
        model->m_fFlags |= 0x10000;
    }

    if (particle.m_model->IsLoaded()) {
        CAaSphere sphere = particle.m_model->GetBoundingSphere();
        sphere.c *= transform;

        float radius = scale.y;

        if (scale.x > scale.y) {
            radius = scale.x;
        }

        sphere.r = radius * sphere.r;
        this->m_worldBounds |= sphere.Box();
    }

    return 1;
}

// PROVEN: 1c81cf74 IDENTICAL/DIFFERENT
// OFFSET: 0x00B13F80/0x1408D9DF0
void CParticleEmitter2::SetParticleColors(const CImVector& color0, const CImVector& color1, const CImVector& color2) {
    this->m_params.m_replacementColors[0].x = color0.r;
    this->m_params.m_replacementColors[0].y = color0.g;
    this->m_params.m_replacementColors[0].z = color0.b;
    this->m_params.m_replacementColors[1].x = color1.r;
    this->m_params.m_replacementColors[1].y = color1.g;
    this->m_params.m_replacementColors[1].z = color1.b;
    this->m_params.m_replacementColors[2].x = color2.r;
    this->m_params.m_replacementColors[2].y = color2.g;
    this->m_params.m_replacementColors[2].z = color2.b;

    this->m_flags |= 0x10;

    if (!(this->m_flags & 0x1000000)) {
        return;
    }

    this->m_params.m_simpleKeys[0].m_color[0] = color0.r << 16;
    this->m_params.m_simpleKeys[0].m_color[1] = color0.g << 16;
    this->m_params.m_simpleKeys[0].m_color[2] = color0.b << 16;
    this->m_params.m_simpleKeys[0].m_deltaColor[0] = color1.r - color0.r;
    this->m_params.m_simpleKeys[0].m_deltaColor[1] = color1.g - color0.g;
    this->m_params.m_simpleKeys[0].m_deltaColor[2] = color1.b - color0.b;

    this->m_params.m_simpleKeys[1].m_color[0] = color0.r << 16;
    this->m_params.m_simpleKeys[1].m_color[1] = color0.g << 16;
    this->m_params.m_simpleKeys[1].m_color[2] = color0.b << 16;
    this->m_params.m_simpleKeys[1].m_deltaColor[0] = color2.r - color1.r;
    this->m_params.m_simpleKeys[1].m_deltaColor[1] = color2.g - color1.g;
    this->m_params.m_simpleKeys[1].m_deltaColor[2] = color2.b - color1.b;
}

// OFFSET: 0x00B15280
void CParticleEmitter2::GetReplacementColors(CImVector& color0, CImVector& color1, CImVector& color2) {
    color0.r = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[0].x));
    color0.g = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[0].y));
    color0.b = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[0].z));
    color1.r = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[1].x));
    color1.g = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[1].y));
    color1.b = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[1].z));
    color2.g = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[2].y));
    color2.r = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[2].x));
    color2.b = static_cast<uint8_t>(CMath::fint_(this->m_params.m_replacementColors[2].z));
}

// PROVEN: d45dc428 IDENTICAL/IDENTICAL
// OFFSET: 0x00B15370/0x1408DB1D0
void CParticleEmitter2::SetTwinkleScale(const CRange& scale) {
    this->m_twinkleScaleBase = scale.m_l;
    this->m_twinkleScaleSpan = scale.m_h - scale.m_l;
}

// PROVEN: 2ded3c5e IDENTICAL/NONE
// OFFSET: 0x00B153A0
void CParticleEmitter2::UpdateXform(const C44Matrix& transform, const C3Vector& viewPos, const C44Matrix* frameOfReference) {
    this->m_viewPos = viewPos;

    if (!frameOfReference || (this->m_flags & 0x200)) {
        this->m_xform = transform;
    } else {
        this->m_xform = transform * frameOfReference->AffineInverse();
    }

    this->m_xformScale = sqrt(transform.a2 * transform.a2 + transform.a1 * transform.a1 + transform.a0 * transform.a0);
}

// OFFSET: 0x00B203A0
void CParticleEmitter2::InternalUpdate(float deltaTime, int32_t child) {
    float time = 0.0f;

    if (deltaTime < 0.0f || (time = deltaTime, deltaTime <= 0.1f)) {
        this->m_burstOffset = this->m_followOffset;
    } else {
        float steps = floor(time * 10.0f);
        time = deltaTime - steps * 0.1f;
        float maxSteps = floor(this->m_params.m_life * 10.0f);

        if (maxSteps < steps) {
            steps = maxSteps;
        }

        uint32_t count = static_cast<uint32_t>(steps);
        float scale = 1.0f / (count + 1);

        this->m_burstOffset = C3Vector(this->m_followOffset.x * scale, this->m_followOffset.y * scale, scale * this->m_followOffset.z);

        if (this->m_params.m_hasModel) {
            this->StepUpdate<CParticle2_Model>(0.1f, child, count);
        } else {
            this->StepUpdate<CParticle2>(0.1f, child, count);
        }
    }

    if (this->m_params.m_hasModel) {
        this->StepUpdate<CParticle2_Model>(time, child, 1);
    } else {
        this->StepUpdate<CParticle2>(time, child, 1);
    }
}

// OFFSET: 0x00B15430/0x1408DB3D0
static void RecursiveEmitterModelLoaded(CM2Model* model, void* arg) {
    CParticleEmitter2* emitter = static_cast<CParticleEmitter2*>(arg);

    if (!model || !emitter) {
        return;
    }

    if (!(model->m_fFlags & 1)) {
        model->WaitForLoad(NULL);
    }

    uint32_t count = 4;

    if (model->m_shared->m_data->m_particles.m_count < 4) {
        if (!(model->m_fFlags & 1)) {
            model->WaitForLoad(NULL);
        }

        count = model->m_shared->m_data->m_particles.m_count;
    }

    for (uint32_t i = 0; i < count; i++) {
        if (!(model->m_fFlags & 1)) {
            model->WaitForLoad(NULL);
        }

        CParticleEmitter2* child = model->m_particleEmitters[i];
        emitter->m_childEmitters[emitter->m_childEmitterCount++] = child;
        child->m_flags |= 0x1;
    }
}

// PROVEN: a0c8586e IDENTICAL/NONE
// OFFSET: 0x00B154D0
void CParticleEmitter2::CreateChildEmittersFromModel(CM2Scene* scene, const char* name) {
    if (!scene) {
        return;
    }

    this->m_childModel = scene->CreateModel(name, 0, 3);

    if (this->m_childModel) {
        this->m_childModel->SetLoadedCallback(RecursiveEmitterModelLoaded, this);
    }
}

// OFFSET: 0x00B15510/0x1408DB500
void CParticleEmitter2::ChangeFrameOfReference(const C44Matrix& matrix) {
    if (!(this->m_flags & 0x200)) {
        uint32_t count = this->m_liveList.Count();

        for (uint32_t i = 0; i < count; i++) {
            uint32_t index = this->m_liveList[i];
            CParticle2* particle = this->m_params.m_hasModel ? static_cast<CParticle2*>(&this->m_particleModels[index]) : &this->m_particles[index];

            particle->m_position *= matrix;

            C3Vector velocity;
            velocity.x = particle->m_velocity.y * matrix.b0 + matrix.c0 * particle->m_velocity.z + matrix.a0 * particle->m_velocity.x;
            velocity.y = matrix.a1 * particle->m_velocity.x + matrix.c1 * particle->m_velocity.z + particle->m_velocity.y * matrix.b1;
            velocity.z = matrix.c2 * particle->m_velocity.z + particle->m_velocity.y * matrix.b2 + particle->m_velocity.x * matrix.a2;
            particle->m_velocity = velocity;
        }
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        this->m_childEmitters[i]->ChangeFrameOfReference(matrix);
    }
}

// OFFSET: 0x00B1A940
void CParticleEmitter2::SetSimpleColorKeys() {
    if (this->m_flags & 0x10) {
        const float* colors = &this->m_params.m_replacementColors[0].x;

        for (uint32_t i = 0; i < 3; i++) {
            this->m_params.m_simpleKeys[0].m_color[i] = static_cast<uint8_t>(CMath::fint_(colors[i])) << 16;
            this->m_params.m_simpleKeys[1].m_color[i] = static_cast<uint8_t>(CMath::fint_(colors[i + 3])) << 16;
            this->m_params.m_simpleKeys[0].m_deltaColor[i] = static_cast<uint8_t>(static_cast<uint32_t>(this->m_params.m_simpleKeys[1].m_color[i] - this->m_params.m_simpleKeys[0].m_color[i]) >> 16);
            this->m_params.m_simpleKeys[1].m_deltaColor[i] = static_cast<uint8_t>(CMath::fint_(colors[i + 6])) - static_cast<uint8_t>(this->m_params.m_simpleKeys[1].m_color[i] >> 16);
        }

        return;
    }

    const M2PartTrack<C3Vector>* track = this->m_params.m_colorTrack;
    this->m_params.m_simpleKeys[0].m_color[0] = static_cast<uint8_t>(CMath::fint_(track->m_values[0].x)) << 16;
    this->m_params.m_simpleKeys[0].m_color[1] = static_cast<uint8_t>(CMath::fint_(track->m_values[0].y)) << 16;
    this->m_params.m_simpleKeys[0].m_color[2] = static_cast<uint8_t>(CMath::fint_(track->m_values[0].z)) << 16;

    switch (this->m_params.m_colorTrack->m_values.m_count) {
        case 1:
            this->m_params.m_simpleKeys[0].m_deltaColor[0] = 0;
            this->m_params.m_simpleKeys[0].m_deltaColor[1] = 0;
            this->m_params.m_simpleKeys[0].m_deltaColor[2] = 0;
            break;

        case 2:
            this->m_params.m_simpleKeys[0].m_deltaColor[0] = static_cast<int32_t>(CMath::fint_((this->m_params.m_colorTrack->m_values[1].x - this->m_params.m_colorTrack->m_values[0].x) * this->m_params.m_simpleMidTime));
            this->m_params.m_simpleKeys[0].m_deltaColor[1] = static_cast<int32_t>(CMath::fint_((this->m_params.m_colorTrack->m_values[1].y - this->m_params.m_colorTrack->m_values[0].y) * this->m_params.m_simpleMidTime));
            this->m_params.m_simpleKeys[0].m_deltaColor[2] = static_cast<int32_t>(CMath::fint_((this->m_params.m_colorTrack->m_values[1].z - this->m_params.m_colorTrack->m_values[0].z) * this->m_params.m_simpleMidTime));
            break;

        case 3:
            this->m_params.m_simpleKeys[0].m_deltaColor[0] = static_cast<int32_t>(CMath::fint_(this->m_params.m_colorTrack->m_values[1].x - this->m_params.m_colorTrack->m_values[0].x));
            this->m_params.m_simpleKeys[0].m_deltaColor[1] = static_cast<int32_t>(CMath::fint_(this->m_params.m_colorTrack->m_values[1].y - this->m_params.m_colorTrack->m_values[0].y));
            this->m_params.m_simpleKeys[0].m_deltaColor[2] = static_cast<int32_t>(CMath::fint_(this->m_params.m_colorTrack->m_values[1].z - this->m_params.m_colorTrack->m_values[0].z));
            break;
    }

    this->m_params.m_simpleKeys[1].m_color[0] = (static_cast<int16_t>(this->m_params.m_simpleKeys[0].m_deltaColor[0]) << 16) + this->m_params.m_simpleKeys[0].m_color[0];
    this->m_params.m_simpleKeys[1].m_color[1] = (static_cast<int16_t>(this->m_params.m_simpleKeys[0].m_deltaColor[1]) << 16) + this->m_params.m_simpleKeys[0].m_color[1];
    this->m_params.m_simpleKeys[1].m_color[2] = (static_cast<int16_t>(this->m_params.m_simpleKeys[0].m_deltaColor[2]) << 16) + this->m_params.m_simpleKeys[0].m_color[2];

    if (this->m_params.m_colorTrack->m_values.m_count == 1) {
        this->m_params.m_simpleKeys[1].m_deltaColor[0] = 0;
        this->m_params.m_simpleKeys[1].m_deltaColor[1] = 0;
        this->m_params.m_simpleKeys[1].m_deltaColor[2] = 0;
    } else if (this->m_params.m_colorTrack->m_values.m_count == 2) {
        this->m_params.m_simpleKeys[1].m_deltaColor[0] = static_cast<int32_t>(CMath::fint_((this->m_params.m_colorTrack->m_values[1].x - this->m_params.m_colorTrack->m_values[0].x) * (1.0f - this->m_params.m_simpleMidTime)));
        this->m_params.m_simpleKeys[1].m_deltaColor[1] = static_cast<int32_t>(CMath::fint_((this->m_params.m_colorTrack->m_values[1].y - this->m_params.m_colorTrack->m_values[0].y) * (1.0f - this->m_params.m_simpleMidTime)));
        this->m_params.m_simpleKeys[1].m_deltaColor[2] = static_cast<int32_t>(CMath::fint_((this->m_params.m_colorTrack->m_values[1].z - this->m_params.m_colorTrack->m_values[0].z) * (1.0f - this->m_params.m_simpleMidTime)));
    } else if (this->m_params.m_colorTrack->m_values.m_count == 3) {
        this->m_params.m_simpleKeys[1].m_deltaColor[0] = static_cast<int32_t>(CMath::fint_(this->m_params.m_colorTrack->m_values[2].x - this->m_params.m_colorTrack->m_values[1].x));
        this->m_params.m_simpleKeys[1].m_deltaColor[1] = static_cast<int32_t>(CMath::fint_(this->m_params.m_colorTrack->m_values[2].y - this->m_params.m_colorTrack->m_values[1].y));
        this->m_params.m_simpleKeys[1].m_deltaColor[2] = static_cast<int32_t>(CMath::fint_(this->m_params.m_colorTrack->m_values[2].z - this->m_params.m_colorTrack->m_values[1].z));
    }
}

// OFFSET: 0x00B1AE10
void CParticleEmitter2::SetSimpleAlphaKeys() {
    this->m_params.m_simpleKeys[0].m_color[3] = static_cast<uint8_t>(CMath::fint_(this->m_params.m_alphaTrack->m_values[0].m_value * 0.000030518509f * 255.0f)) << 16;

    const M2PartTrack<fixed16>* track = this->m_params.m_alphaTrack;

    if (track->m_values.m_count == 1) {
        this->m_params.m_simpleKeys[0].m_deltaColor[3] = 0;
    } else if (track->m_values.m_count == 2) {
        this->m_params.m_simpleKeys[0].m_deltaColor[3] = static_cast<int32_t>(CMath::fint_(static_cast<int16_t>(track->m_values[1].m_value - track->m_values[0].m_value) * this->m_params.m_simpleMidTime * 0.0077822199f));
    } else if (track->m_values.m_count == 3) {
        this->m_params.m_simpleKeys[0].m_deltaColor[3] = static_cast<int32_t>(CMath::fint_(static_cast<int16_t>(track->m_values[1].m_value - track->m_values[0].m_value) * 0.0077822199f));
    }

    this->m_params.m_simpleKeys[1].m_color[3] = (static_cast<int16_t>(this->m_params.m_simpleKeys[0].m_deltaColor[3]) << 16) + this->m_params.m_simpleKeys[0].m_color[3];

    track = this->m_params.m_alphaTrack;

    if (track->m_values.m_count == 1) {
        this->m_params.m_simpleKeys[1].m_deltaColor[3] = 0;
    } else if (track->m_values.m_count == 2) {
        int16_t delta = track->m_values[1].m_value - track->m_values[0].m_value;
        this->m_params.m_simpleKeys[1].m_deltaColor[3] = static_cast<int32_t>(CMath::fint_((1.0f - this->m_params.m_simpleMidTime) * delta * 0.0077822199f));
    } else if (track->m_values.m_count == 3) {
        int16_t delta = track->m_values[2].m_value - track->m_values[1].m_value;
        this->m_params.m_simpleKeys[1].m_deltaColor[3] = static_cast<int32_t>(CMath::fint_(delta * 0.0077822199f));
    }
}

// OFFSET: 0x00B1AF70
void CParticleEmitter2::SetSimpleScaleKeys() {
    this->m_params.m_simpleKeys[0].m_scale = this->m_params.m_scaleTrack->m_values[0].x;

    const M2PartTrack<C2Vector>* track = this->m_params.m_scaleTrack;
    float delta = 0.0f;

    switch (track->m_values.m_count) {
        case 1:
            this->m_params.m_simpleKeys[0].m_deltaScale = 0.0f;
            break;

        case 2:
            this->m_params.m_simpleKeys[0].m_deltaScale = (track->m_values[1].x - track->m_values[0].x) * this->m_params.m_simpleMidTime;
            break;

        case 3:
            this->m_params.m_simpleKeys[0].m_deltaScale = track->m_values[1].x - track->m_values[0].x;
            break;
    }

    this->m_params.m_simpleKeys[1].m_scale = this->m_params.m_simpleKeys[0].m_deltaScale + this->m_params.m_simpleKeys[0].m_scale;

    track = this->m_params.m_scaleTrack;

    if (track->m_values.m_count == 1) {
        this->m_params.m_simpleKeys[1].m_deltaScale = delta;
    } else if (track->m_values.m_count == 2) {
        this->m_params.m_simpleKeys[1].m_deltaScale = (1.0f - this->m_params.m_simpleMidTime) * (track->m_values[1].x - track->m_values[0].x);
    } else if (track->m_values.m_count == 3) {
        delta = track->m_values[2].x - track->m_values[1].x;
        this->m_params.m_simpleKeys[1].m_deltaScale = delta;
    }
}

// OFFSET: 0x00B1B060
void CParticleEmitter2::SetSimpleHeadCellKeys() {
    const M2PartTrack<uint16_t>* track = this->m_params.m_headCellTrack;

    if (!track->m_values.m_count) {
        this->m_params.m_simpleKeys[0].m_headCell = 0;
        this->m_params.m_simpleKeys[0].m_deltaHeadCell = 0;
        this->m_params.m_simpleKeys[1].m_headCell = 0;
        this->m_params.m_simpleKeys[1].m_deltaHeadCell = 0;
        return;
    }

    this->m_params.m_simpleKeys[0].m_headCell = static_cast<int16_t>(track->m_values[0]) << 16;

    switch (this->m_params.m_headCellTrack->m_values.m_count) {
        case 1:
            this->m_params.m_simpleKeys[0].m_deltaHeadCell = 0;
            break;

        case 2:
            this->m_params.m_simpleKeys[0].m_deltaHeadCell = static_cast<int32_t>(CMath::fint_((this->m_params.m_headCellTrack->m_values[1] - this->m_params.m_headCellTrack->m_values[0]) * this->m_params.m_simpleMidTime));
            break;

        case 3:
        case 4:
            this->m_params.m_simpleKeys[0].m_deltaHeadCell = this->m_params.m_headCellTrack->m_values[1] - this->m_params.m_headCellTrack->m_values[0];
            break;
    }

    this->m_params.m_simpleKeys[1].m_headCell = (static_cast<int16_t>(this->m_params.m_simpleKeys[0].m_deltaHeadCell) << 16) + this->m_params.m_simpleKeys[0].m_headCell;

    switch (this->m_params.m_headCellTrack->m_values.m_count) {
        case 1:
            this->m_params.m_simpleKeys[1].m_deltaHeadCell = 0;
            break;

        case 2:
            this->m_params.m_simpleKeys[1].m_deltaHeadCell = static_cast<int32_t>(CMath::fint_((1.0f - this->m_params.m_simpleMidTime) * (this->m_params.m_headCellTrack->m_values[1] - this->m_params.m_headCellTrack->m_values[0])));
            break;

        case 3:
            this->m_params.m_simpleKeys[1].m_deltaHeadCell = this->m_params.m_headCellTrack->m_values[2] - this->m_params.m_headCellTrack->m_values[1];
            break;

        case 4:
            this->m_params.m_simpleKeys[1].m_deltaHeadCell = this->m_params.m_headCellTrack->m_values[3] - this->m_params.m_headCellTrack->m_values[2];
            break;
    }
}

// OFFSET: 0x00B1B1F0
void CParticleEmitter2::SetSimpleTailCellKeys() {
    const M2PartTrack<uint16_t>* track = this->m_params.m_tailCellTrack;

    if (!track->m_values.m_count) {
        this->m_params.m_simpleKeys[0].m_tailCell = 0;
        this->m_params.m_simpleKeys[0].m_deltaTailCell = 0;
        this->m_params.m_simpleKeys[1].m_tailCell = 0;
        this->m_params.m_simpleKeys[1].m_deltaTailCell = 0;
        return;
    }

    this->m_params.m_simpleKeys[0].m_tailCell = static_cast<int16_t>(track->m_values[0]) << 16;

    switch (this->m_params.m_tailCellTrack->m_values.m_count) {
        case 1:
            this->m_params.m_simpleKeys[0].m_deltaTailCell = 0;
            break;

        case 2:
            this->m_params.m_simpleKeys[0].m_deltaTailCell = static_cast<int32_t>(CMath::fint_((this->m_params.m_tailCellTrack->m_values[1] - this->m_params.m_tailCellTrack->m_values[0]) * this->m_params.m_simpleMidTime));
            break;

        case 3:
        case 4:
            this->m_params.m_simpleKeys[0].m_deltaTailCell = this->m_params.m_tailCellTrack->m_values[1] - this->m_params.m_tailCellTrack->m_values[0];
            break;
    }

    this->m_params.m_simpleKeys[1].m_tailCell = (static_cast<int16_t>(this->m_params.m_simpleKeys[0].m_deltaTailCell) << 16) + this->m_params.m_simpleKeys[0].m_tailCell;

    switch (this->m_params.m_tailCellTrack->m_values.m_count) {
        case 1:
            this->m_params.m_simpleKeys[1].m_deltaTailCell = 0;
            break;

        case 2:
            this->m_params.m_simpleKeys[1].m_deltaTailCell = static_cast<int32_t>(CMath::fint_((1.0f - this->m_params.m_simpleMidTime) * (this->m_params.m_tailCellTrack->m_values[1] - this->m_params.m_tailCellTrack->m_values[0])));
            break;

        case 3:
            this->m_params.m_simpleKeys[1].m_deltaTailCell = this->m_params.m_tailCellTrack->m_values[2] - this->m_params.m_tailCellTrack->m_values[1];
            break;

        case 4:
            this->m_params.m_simpleKeys[1].m_deltaTailCell = this->m_params.m_tailCellTrack->m_values[3] - this->m_params.m_tailCellTrack->m_values[2];
            break;
    }
}

// OFFSET: 0x00B157E0
CParticleEmitter2::SimpleKey::SimpleKey() {
    this->m_deltaColor[0] = 0;
    this->m_deltaColor[1] = 0;
    this->m_deltaColor[2] = 0;
    this->m_deltaColor[3] = 0;
}

// PROVEN: 8fb0fc77 DIFFERENT/IDENTICAL
// OFFSET: 0x00B15800/0x1408DB830
int32_t CParticleEmitter2::HasLiveParticles() {
    if (this->m_liveList.Count()) {
        return 1;
    }

    uint32_t count = this->m_childEmitterCount;

    for (uint32_t i = 0; i < count; i++) {
        if (this->m_childEmitters[i]->HasLiveParticles()) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00B15850
uint32_t CParticleEmitter2::GetNumParticleModels() {
    uint32_t count = 0;

    if (this->m_params.m_hasModel == 1) {
        count = this->m_liveList.Count();
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        count += this->m_childEmitters[i]->GetNumParticleModels();
    }

    return count;
}

// OFFSET: 0x00B15890
CM2Model* CParticleEmitter2::GetParticleModelInternal(uint32_t& index) {
    if (this->m_params.m_hasModel == 1) {
        uint32_t count = this->m_liveList.Count();

        if (index < count) {
            return this->m_particleModels[this->m_liveList[index]].m_model;
        }

        index -= count;
    }

    uint32_t childCount = this->m_childEmitterCount;

    for (uint32_t i = 0; i < childCount; i++) {
        CM2Model* model = this->m_childEmitters[i]->GetParticleModelInternal(index);

        if (model) {
            return model;
        }
    }

    return NULL;
}

// OFFSET: 0x00B158F0/0x1408DB9B0
uint32_t CParticleEmitter2::GetNumBatches() {
    uint32_t count = this->m_batchFlags & 0x1;

    if (count >= this->m_liveList.Count()) {
        count = this->m_liveList.Count();
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        count += this->m_childEmitters[i]->GetNumBatches();
    }

    return count;
}

// OFFSET: 0x00B15940/0x1408DBA20
uint32_t CParticleEmitter2::GetNumPrimitives() {
    uint32_t count = this->m_liveList.Count() * this->m_indicesPerParticle / 3;

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        count += this->m_childEmitters[i]->GetNumPrimitives();
    }

    return count;
}

// OFFSET: 0x00B198A0/0x1408E0000
CParticleEmitter2::~CParticleEmitter2() {
    if (this->m_texture) {
        TextureRelease(this->m_texture);
        this->m_texture = NULL;
    }

    if (this->m_model) {
        this->m_model->Release();
        this->m_model = NULL;
    }

    if (this->m_childModel) {
        this->m_childModel->Release();
        this->m_childModel = NULL;
    }

    delete[] this->m_params.m_simpleKeys;
}

// OFFSET: 0x00B19980/0x1408E0130
void CParticleEmitter2::SetEmissionRate(float rate) {
    if (rate <= 0.0f) {
        this->m_params.m_emissionRate = 0.0f;
    } else {
        this->m_params.m_emissionRate = rate;
    }
}

// OFFSET: 0x00B192F0
int32_t CParticleEmitter2::MoveParticle(CParticle2_Model& particle, float deltaTime) {
    float speed = sqrt(particle.m_angularVelocity.z * particle.m_angularVelocity.z + particle.m_angularVelocity.y * particle.m_angularVelocity.y + particle.m_angularVelocity.x * particle.m_angularVelocity.x);

    if (speed > 0.000099999997f) {
        float halfAngle = speed * deltaTime * 0.5f;
        float scale = sin(halfAngle) * (1.0f / speed);

        C4Quaternion rotation;
        rotation.x = particle.m_angularVelocity.x * scale;
        rotation.y = particle.m_angularVelocity.y * scale;
        rotation.z = scale * particle.m_angularVelocity.z;
        rotation.w = cos(halfAngle);

        const C4Quaternion& q = particle.m_orientation;
        C4Quaternion result;
        result.x = q.w * rotation.x + rotation.z * q.y + rotation.w * q.x - q.z * rotation.y;
        result.y = rotation.w * q.y + q.w * rotation.y + q.z * rotation.x - rotation.z * q.x;
        result.z = rotation.z * q.w + rotation.w * q.z + q.x * rotation.y - rotation.x * q.y;
        result.w = rotation.w * q.w - q.x * rotation.x - q.y * rotation.y - rotation.z * q.z;
        particle.m_orientation = result;
    }

    return this->MoveParticle(static_cast<CParticle2&>(particle), deltaTime);
}

// OFFSET: [0x00B15AE0, 0x00B16940, 0x00B177A0, 0x00B18510, 0x00B1B810, 0x00B1C670, 0x00B1D4D0, 0x00B1E240, 0x00B20720, 0x00B21620, 0x00B22520, 0x00B23330]
template <class V, class P, bool simple>
void CParticleEmitter2::BuildVertices(V* vertices, uint32_t count) {
    this->m_renderedParticles = 0;

    for (uint32_t i = 0; i < count; i++) {
        CParticle2* particle = this->NextParticle<P>(i);
        uint32_t twinkle = 0;

        if (this->m_twinkleOnOff < 1.0f || 0.0f != this->m_twinkleScaleSpan) {
            twinkle = (static_cast<uint8_t>(reinterpret_cast<uintptr_t>(particle) >> 5) + static_cast<uint8_t>(CMath::fint_(this->m_twinkleFPS * particle->m_age))) & 0x7F;
        }

        if (this->m_twinkleOnOff < CParticleEmitter2::s_rndTable[twinkle]) {
            continue;
        }

        TrackValues values;

        if (simple) {
            this->InterpolateAllTracksSimple(*particle, &values);
        } else {
            this->InterpolateAllTracks(*particle, &values);
        }

        float spin;
        float spinRate;
        this->RandomizeSpin(*particle, &spin, &spinRate);

        CImVector color = values.m_color;

        if (g_theGxDevicePtr->Caps().m_colorFormat == GxCF_rgba) {
            color.b = values.m_color.r;
            color.r = values.m_color.b;
        }

        float twinkleScale = CParticleEmitter2::s_rndTable[twinkle] * this->m_twinkleScaleSpan + this->m_twinkleScaleBase;
        C2Vector scale(twinkleScale * values.m_scale.x, twinkleScale * values.m_scale.y);

        if (this->m_flags & 0x400) {
            scale.x = this->m_xformScale * scale.x;
            scale.y = this->m_xformScale * scale.y;
        }

        C3Vector center = particle->m_position * s_particleXform;
        uint32_t flags = this->m_flags;

        if (flags & 0x4) {
            uint32_t column = values.m_headCell & (this->m_textureCols - 1);
            float u = column * this->m_textureCellWidth;
            float v = (values.m_headCell >> this->m_textureCellShift) * this->m_textureCellHeight;
            C3Vector axisX;
            C3Vector axisY;

            if ((flags & 0x200000) && particle->m_velocity.x * particle->m_velocity.x + particle->m_velocity.y * particle->m_velocity.y + particle->m_velocity.z * particle->m_velocity.z > 0.00000023841858f) {
                C3Vector reverse(-particle->m_velocity.x, -particle->m_velocity.y, -particle->m_velocity.z);
                C3Vector direction = TransformDirection(reverse, s_particleXform);
                C2Vector flat(direction);
                float directionSq = direction.z * direction.z + direction.y * direction.y + direction.x * direction.x;
                float flatSq = flat.y * flat.y + flat.x * flat.x;
                float inv = 0.0f;

                if (flatSq > 0.00000023841858f) {
                    inv = 1.0f / sqrt(flatSq);
                }

                flat.x = flat.x * inv;
                flat.y = flat.y * inv;
                C2Vector size = scale;

                if (inv > 0.00000023841858f) {
                    size.x = 1.0f / inv / sqrt(directionSq) * scale.x;
                }

                axisX = C3Vector(size.x * flat.x, size.x * flat.y, 0.0f);
                axisY = C3Vector(-flat.y * size.y, size.y * flat.x, 0.0f);
            } else if (!(flags & 0x4000)) {
                if (0.0f == this->m_params.m_spin && 0.0f == this->m_params.m_spinVariation) {
                    axisX = C3Vector(scale.x, 0.0f, 0.0f);
                    axisY = C3Vector(0.0f, scale.y, 0.0f);
                } else {
                    float angle = particle->m_age * spinRate + spin;

                    if ((flags & 0x10000) && (reinterpret_cast<uintptr_t>(particle) & 0x20)) {
                        angle = -angle;
                    }

                    float cosAngle = cos(angle);
                    float sinAngle = sin(angle);
                    axisX = C3Vector(cosAngle * scale.x, sinAngle * scale.x, 0.0f);
                    axisY = C3Vector(-(sinAngle * scale.y), cosAngle * scale.y, 0.0f);
                }
            } else {
                axisX = C3Vector(scale.x * s_billboardXform.a0, scale.x * s_billboardXform.a1, scale.x * s_billboardXform.a2);
                axisY = C3Vector(scale.y * s_billboardXform.b0, scale.y * s_billboardXform.b1, scale.y * s_billboardXform.b2);

                if (0.0f != this->m_params.m_spin || 0.0f != this->m_params.m_spinVariation) {
                    float angle = particle->m_age * spinRate + spin;

                    if ((flags & 0x10000) && (reinterpret_cast<uintptr_t>(particle) & 0x20)) {
                        angle = -angle;
                    }

                    C33Matrix rotation = C33Matrix::Rotation(angle, this->m_billboardAxis, true);
                    axisX = axisX * rotation;
                    axisY = axisY * rotation;
                }
            }

            SetVertex(vertices[0], C3Vector(center.x - axisX.x + axisY.x, center.y - axisX.y + axisY.y, center.z - axisX.z + axisY.z), color, u, v);
            SetVertex(vertices[1], C3Vector(center.x - axisX.x - axisY.x, center.y - axisX.y - axisY.y, center.z - axisX.z - axisY.z), color, u, this->m_textureCellHeight + v);
            SetVertex(vertices[2], C3Vector(center.x + axisY.x + axisX.x, center.y + axisY.y + axisX.y, center.z + axisY.z + axisX.z), color, this->m_textureCellWidth + u, v);
            SetVertex(vertices[3], C3Vector(center.x + axisX.x - axisY.x, center.y + axisX.y - axisY.y, center.z + axisX.z - axisY.z), color, this->m_textureCellWidth + u, this->m_textureCellHeight + v);
            vertices += 4;
        }

        if (this->m_flags & 0x8) {
            uint32_t column = values.m_tailCell & (this->m_textureCols - 1);
            float u = column * this->m_textureCellWidth;
            float v = (values.m_tailCell >> this->m_textureCellShift) * this->m_textureCellHeight;
            float tailLength = this->m_params.m_tailLength;

            if ((this->m_flags & 0x20000) && tailLength > particle->m_age) {
                tailLength = particle->m_age;
            }

            C3Vector reverse(-particle->m_velocity.x, -particle->m_velocity.y, -particle->m_velocity.z);
            C3Vector tail = TransformDirection(reverse, s_particleXform) * tailLength;
            C2Vector flat(tail);
            float flatSq = flat.y * flat.y + flat.x * flat.x;

            if (flatSq < 0.00077160494f) {
                for (uint32_t j = 0; j < 4; j++) {
                    SetVertex(vertices[j], C3Vector(s_cornerOffsets[j][0] * scale.x + center.x, s_cornerOffsets[j][1] * scale.y + center.y, center.z), color, s_cornerTexCoords[j][0] * this->m_textureCellWidth + u, s_cornerTexCoords[j][1] * this->m_textureCellHeight + v);
                }
            } else {
                float inv = 1.0f / sqrt(flatSq);
                float perpX = inv * scale.x * flat.x;
                float perpY = inv * scale.y * flat.y;
                C3Vector end(tail.x + center.x, tail.y + center.y, tail.z + center.z);

                SetVertex(vertices[0], C3Vector(center.x - perpY, perpX + center.y, center.z), color, u, v);
                SetVertex(vertices[1], C3Vector(perpY + center.x, center.y - perpX, center.z), color, u, this->m_textureCellHeight + v);
                SetVertex(vertices[2], C3Vector(end.x - perpY, end.y + perpX, end.z), color, this->m_textureCellWidth + u, v);
                SetVertex(vertices[3], C3Vector(end.x + perpY, end.y - perpX, end.z), color, this->m_textureCellWidth + u, this->m_textureCellHeight + v);
            }

            vertices += 4;
        }

        this->m_renderedParticles++;
    }
}

// OFFSET: 0x00B1B380/0x1408E2200
void CParticleEmitter2::DetermineIfSimple() {
    uint32_t colorKeys = this->m_params.m_colorTrack->m_times.m_count;

    if (colorKeys > 3 || !colorKeys) {
        return;
    }

    uint32_t alphaKeys = this->m_params.m_alphaTrack->m_times.m_count;

    if (alphaKeys > 3 || !alphaKeys) {
        return;
    }

    const M2PartTrack<C2Vector>* scaleTrack = this->m_params.m_scaleTrack;
    uint32_t scaleKeys = scaleTrack->m_times.m_count;

    if (scaleKeys > 3 || !scaleKeys) {
        return;
    }

    uint32_t headKeys = this->m_params.m_headCellTrack->m_times.m_count;

    if (headKeys > 4 || (headKeys == 4 && this->m_params.m_headCellTrack->m_times[1].m_value != this->m_params.m_headCellTrack->m_times[2].m_value)) {
        return;
    }

    uint32_t tailKeys = this->m_params.m_tailCellTrack->m_times.m_count;

    if (tailKeys > 4 || (tailKeys == 4 && this->m_params.m_tailCellTrack->m_times[1].m_value != this->m_params.m_tailCellTrack->m_times[2].m_value)) {
        return;
    }

    if (0.0f != this->m_params.m_lifeVariation || (!headKeys && (this->m_flags & 0x100000))) {
        return;
    }

    if (this->m_params.m_scaleVariationX != 0.0f || this->m_params.m_scaleVariationY != 0.0f) {
        return;
    }

    for (uint32_t i = 0; i < scaleTrack->m_values.m_count; i++) {
        if (CMath::fnotequal(scaleTrack->m_values[i].x, scaleTrack->m_values[i].y)) {
            return;
        }
    }

    int16_t midTime = 0;

    if (colorKeys == 3) {
        midTime = this->m_params.m_colorTrack->m_times[1].m_value;
    } else if (alphaKeys == 3) {
        midTime = this->m_params.m_alphaTrack->m_times[1].m_value;
    } else if (scaleKeys == 3) {
        midTime = this->m_params.m_scaleTrack->m_times[1].m_value;
    } else if (headKeys >= 3) {
        midTime = this->m_params.m_headCellTrack->m_times[1].m_value;
    } else if (tailKeys >= 3) {
        midTime = this->m_params.m_tailCellTrack->m_times[1].m_value;
    }

    if (alphaKeys == 3 && this->m_params.m_alphaTrack->m_times[1].m_value != midTime) {
        return;
    }

    if (scaleKeys == 3 && this->m_params.m_scaleTrack->m_times[1].m_value != midTime) {
        return;
    }

    if (headKeys >= 3 && this->m_params.m_headCellTrack->m_times[1].m_value != midTime) {
        return;
    }

    if (tailKeys >= 3 && this->m_params.m_tailCellTrack->m_times[1].m_value != midTime) {
        return;
    }

    this->m_params.m_simpleKeys = new (__FILE__, __LINE__) SimpleKey[2];

    if (!this->m_params.m_simpleKeys) {
        return;
    }

    this->m_flags |= 0x1000000;
    this->m_params.m_simpleMidTime = midTime * 0.000030518509f;

    this->SetSimpleColorKeys();
    this->SetSimpleAlphaKeys();
    this->SetSimpleScaleKeys();
    this->SetSimpleHeadCellKeys();
    this->SetSimpleTailCellKeys();
}

// OFFSET: 0x00B1F020
void CParticleEmitter2::EmitParticle(float time, const C44Matrix& transform) {
    uint32_t index = PopIndex(this->m_freeList);
    this->m_liveList.New(index);

    if (this->m_params.m_hasModel) {
        this->CreateParticle(this->m_particleModels[index], time, transform);
    } else {
        this->CreateParticle(this->m_particles[index], time, transform);
    }
}

// OFFSET: 0x00B1F660
void CParticleEmitter2::EmitNewParticles(float deltaTime, const C44Matrix& transform) {
    float distanceScale = 1.0f;

    if (!(this->m_flags & 0x400000)) {
        distanceScale = 1.0f - (CParticleEmitter2::s_viewDist - 50.0f) * 0.02f;

        if (distanceScale < 0.25f) {
            distanceScale = 0.25f;
        } else if (distanceScale >= 1.0f) {
            distanceScale = 1.0f;
        }
    }

    float rate = CRandom::sfloat_(this->m_random) * this->m_params.m_emissionRateVariation + this->m_params.m_emissionRate;
    rate = ParticleSystemManager::GetScaler() * rate * distanceScale;

    if ((this->m_flags & 0x40) && (this->m_flags & 0x2)) {
        int32_t burst = static_cast<int32_t>(rate);

        while (this->m_freeList.Count() && burst--) {
            this->EmitParticle(0.0f, transform);
        }

        this->m_flags &= ~0x40;
    }

    if ((this->m_flags & 0x3) != 0x3) {
        return;
    }

    uint32_t emitted = 0;
    this->m_emitAccumulator = rate * deltaTime + this->m_emitAccumulator;

    if (this->m_flags & 0x2000) {
        C44Matrix& xform = const_cast<C44Matrix&>(transform);
        C3Vector position(xform.d0, xform.d1, xform.d2);
        C3Vector delta(position.x - this->m_parentPosition.x, position.y - this->m_parentPosition.y, position.z - this->m_parentPosition.z);
        int32_t count = static_cast<int32_t>(this->m_emitAccumulator + 0.5f);

        if (this->m_freeList.Count()) {
            while (count--) {
                float t = CRandom::ufloat_(this->m_random);
                xform.d0 = delta.x * t + this->m_parentPosition.x;
                xform.d1 = delta.y * t + this->m_parentPosition.y;
                xform.d2 = t * delta.z + this->m_parentPosition.z;

                this->EmitParticle(deltaTime, transform);
                emitted++;

                if (!this->m_freeList.Count()) {
                    break;
                }
            }
        }

        xform.d0 = position.x;
        xform.d1 = position.y;
        xform.d2 = position.z;
    } else {
        int32_t count = static_cast<int32_t>(this->m_emitAccumulator + 0.5f);

        if (this->m_freeList.Count()) {
            while (count--) {
                this->EmitParticle(deltaTime, transform);
                emitted++;

                if (!this->m_freeList.Count()) {
                    break;
                }
            }
        }
    }

    this->m_emitAccumulator = this->m_emitAccumulator - emitted;
}

// OFFSET: 0x00B1F970
int32_t CParticleEmitter2::UpdateLiveParticle(float deltaTime, CParticle2* particle, uint32_t index) {
    C3Vector position = particle->m_position;
    int32_t alive;

    if (this->m_params.m_hasModel) {
        alive = this->MoveParticle(*static_cast<CParticle2_Model*>(particle), deltaTime);
    } else {
        alive = this->MoveParticle(*particle, deltaTime);
    }

    if (!alive) {
        this->DestroyParticle(*particle);
        this->m_freeList.New(this->m_liveList[index]);
        this->m_liveList[index] = *this->m_liveList.Top();
        PopIndex(this->m_liveList);
        return 0;
    }

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        CParticleEmitter2* child = this->m_childEmitters[i];
        float d0 = this->m_xform.d0;
        float d1 = this->m_xform.d1;
        float d2 = this->m_xform.d2;

        this->m_xform.d0 = particle->m_position.x;
        this->m_xform.d1 = particle->m_position.y;
        this->m_xform.d2 = particle->m_position.z;

        if (child->m_flags & 0x800) {
            child->m_parentVelocity = particle->m_velocity;
        }

        if (child->m_flags & 0x2000) {
            child->m_parentPosition = position;
        }

        child->EmitNewParticles(deltaTime, this->m_xform);

        this->m_xform.d0 = d0;
        this->m_xform.d1 = d1;
        this->m_xform.d2 = d2;
    }

    return 1;
}

// OFFSET: [0x00B1FD60, 0x00B1FE70]
template <class P>
void CParticleEmitter2::KillExpiredParticles() {
    uint32_t i = 0;

    while (i < this->m_liveList.Count()) {
        CParticle2* particle = this->NextParticle<P>(i);
        float life = particle->m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

        if (life <= 0.001f) {
            life = 0.001f;
        }

        if (particle->m_age < life) {
            i++;
        } else {
            this->DestroyParticle(*particle);
            this->m_freeList.New(this->m_liveList[i]);
            this->m_liveList[i] = *this->m_liveList.Top();
            PopIndex(this->m_liveList);
        }
    }
}

// OFFSET: 0x00B20510
void CParticleEmitter2::SetLifeSpan(float life) {
    int32_t longer = life >= this->m_params.m_life;
    this->m_params.m_life = life;

    if (longer) {
        return;
    }

    if (this->m_params.m_hasModel) {
        this->KillExpiredParticles<CParticle2_Model>();
    } else {
        this->KillExpiredParticles<CParticle2>();
    }
}

// OFFSET: [0x00B1FB90, 0x00B20550]
template <class P>
void CParticleEmitter2::StepUpdate(float deltaTime, int32_t child, uint32_t count) {
    if (deltaTime <= 0.0f) {
        return;
    }

    if ((this->m_flags & 0x3) == 0x3 || ((this->m_flags & 0x40) && (this->m_flags & 0x2))) {
        this->Sync();
    }

    for (; count; count--) {
        if (!child) {
            this->EmitNewParticles(deltaTime, this->m_xform);
        }

        uint32_t i = 0;

        while (i < this->m_liveList.Count()) {
            CParticle2* particle = this->NextParticle<P>(i);
            particle->m_age = particle->m_age + deltaTime;

            float life = particle->m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

            if (life <= 0.001f) {
                life = 0.001f;
            }

            if (particle->m_age < life) {
                if (this->UpdateLiveParticle(deltaTime, particle, i)) {
                    i++;
                }
            } else {
                this->DestroyParticle(*particle);
                this->m_freeList.New(this->m_liveList[i]);
                this->m_liveList[i] = *this->m_liveList.Top();
                PopIndex(this->m_liveList);
            }
        }

        for (uint32_t j = 0; j < this->m_childEmitterCount; j++) {
            this->m_childEmitters[j]->InternalUpdate(deltaTime, 1);
        }
    }
}

// INLINED
CParticleEmitter2::Params::Params() {
    this->m_hasModel = 0;
    this->m_emissionRate = 0.0f;
    this->m_emissionRateVariation = 0.0f;
    this->m_life = 0.0f;
    this->m_lifeVariation = 0.0f;
    this->m_tailLength = 1.0f;
    this->m_speed = 0.0f;
    this->m_gravity = 0.0f;
    this->m_variation = 0.1f;
    this->m_zsource = 0.0f;
    this->m_initialSpin = 0.0f;
    this->m_initialSpinVariation = 0.0f;
    this->m_spin = 0.0f;
    this->m_spinVariation = 0.0f;
    this->m_materialBlend = 0;
    this->m_materialFlags |= 0x7;
    this->m_colorTrack = NULL;
    this->m_alphaTrack = NULL;
    this->m_scaleTrack = NULL;
    this->m_scaleVariationX = 0.0f;
    this->m_scaleVariationY = 0.0f;
    this->m_headCellTrack = NULL;
    this->m_tailCellTrack = NULL;
    this->m_simpleMidTime = 0.0f;
    this->m_simpleKeys = NULL;
}

// OFFSET: 0x00B1F090/0x1408E6570
CParticleEmitter2::CParticleEmitter2() : m_random(0) {
    this->m_emitAccumulator = 0.0f;
    this->m_refCount = 1;
    this->m_textureCellShift = 0;
    this->m_textureCellWidth = 1.0f;
    this->m_textureCellHeight = 1.0f;
    this->m_renderedParticles = 0;
    this->m_emitterType = 0;
    this->m_childEmitterCount = 4;
    this->m_model = NULL;
    this->m_childModel = NULL;
    this->m_vertsPerParticle = 0;
    this->m_indicesPerParticle = 0;
    this->m_alphaScale = 1.0f;
    this->m_texture = NULL;
    this->unk_012C = 0;
    this->m_twinkleFPS = 10.0f;
    this->m_flags = 0x6;
    this->m_textureRows = 1;
    this->m_twinkleOnOff = 1.0f;
    this->m_textureCols = 1;
    this->m_twinkleScaleBase = 1.0f;
    this->m_twinkleScaleSpan = 0.0f;
    this->m_ivelScale = 1.0f;
    this->m_tumbleBaseX = 0.0f;
    this->m_tumbleSpanX = 0.0f;
    this->m_tumbleBaseY = 0.0f;
    this->m_tumbleSpanY = 0.0f;
    this->m_tumbleBaseZ = 0.0f;
    this->m_tumbleSpanZ = 0.0f;
    this->m_drag = 0.0f;
    this->m_windVectorX = 0.0f;
    this->m_windVectorY = 0.0f;
    this->m_windVectorZ = 0.0f;
    this->m_windTime = 0.0f;
    this->m_followIntercept = 0.0f;
    this->m_followSlope = 0.0f;
    this->m_ivelTimer = 0.0f;
    this->m_worldBounds.b.x = 3.402823466e38f;
    this->m_worldBounds.b.y = 3.402823466e38f;
    this->m_worldBounds.b.z = 3.402823466e38f;
    this->m_worldBounds.t.x = -3.402823466e38f;
    this->m_worldBounds.t.y = -3.402823466e38f;
    this->m_worldBounds.t.z = -3.402823466e38f;
    this->unk_0230 = 0;

    this->m_random.SetSeed((rand() << 16) | static_cast<uint16_t>(rand()));
    this->m_childEmitterCount = 0;
}

// OFFSET: 0x00B1F4E0/0x1408E6990
void CParticleEmitter2::SyncReserve(uint32_t count, uint32_t used, uint32_t spare) {
    uint32_t target = count;

    if (count & (count - 1)) {
        target = 2 * count - 1;

        for (uint32_t bits = target & (2 * count - 2); bits; bits &= bits - 1) {
            target = bits;
        }
    }

    if (used + spare >= target) {
        return;
    }

    uint32_t shortfall = target - (used + spare);

    if (this->m_params.m_hasModel) {
        if (shortfall + this->m_particleModels.Count() > this->m_particleModels.m_alloc) {
            this->m_particleModels.ReallocData(shortfall + this->m_particleModels.Count());
        }
    } else if (shortfall + this->m_particles.Count() > this->m_particles.m_alloc) {
        this->m_particles.ReallocData(shortfall + this->m_particles.Count());
    }

    if (shortfall + this->m_liveList.Count() > this->m_liveList.m_alloc) {
        this->m_liveList.ReallocData(shortfall + this->m_liveList.Count());
    }

    if (shortfall + this->m_freeList.Count() > this->m_freeList.m_alloc) {
        this->m_freeList.ReallocData(shortfall + this->m_freeList.Count());
    }
}

// OFFSET: 0x00B1F570/0x1408E6A30
void CParticleEmitter2::SyncAllocation(uint32_t count) {
    if (this->m_params.m_hasModel) {
        uint32_t oldCount = this->m_particleModels.Count();

        if (oldCount >= count) {
            return;
        }

        this->SyncReserve(count, oldCount, this->m_particleModels.m_alloc - this->m_particleModels.Count());
        this->m_particleModels.SetCount(count);

        if (count + this->m_liveList.Count() > this->m_liveList.m_alloc) {
            this->m_liveList.ReallocData(count + this->m_liveList.Count());
        }

        if (count + this->m_freeList.Count() > this->m_freeList.m_alloc) {
            this->m_freeList.ReallocData(count + this->m_freeList.Count());
        }

        for (uint32_t i = oldCount; i != count; i++) {
            uint32_t index = i;
            this->m_freeList.New(index);
        }
    } else {
        uint32_t oldCount = this->m_particles.Count();

        if (oldCount >= count) {
            return;
        }

        this->SyncReserve(count, oldCount, this->m_particles.m_alloc - this->m_particles.Count());
        this->m_particles.SetCount(count);

        if (count + this->m_liveList.Count() > this->m_liveList.m_alloc) {
            this->m_liveList.ReallocData(count + this->m_liveList.Count());
        }

        if (count + this->m_freeList.Count() > this->m_freeList.m_alloc) {
            this->m_freeList.ReallocData(count + this->m_freeList.Count());
        }

        for (uint32_t i = oldCount; i != count; i++) {
            uint32_t index = i;
            this->m_freeList.New(index);
        }
    }
}

// OFFSET: [0x00B1FF80, 0x00B20010]
template <class P>
void CParticleEmitter2::SortParticles() {
    for (uint32_t i = 0; i < this->m_liveList.Count(); i++) {
        CParticle2* particle = this->NextParticle<P>(i);
        CSortableParticleRecord record;
        record.m_depth = s_particleXform.c2 * particle->m_position.z + s_particleXform.b2 * particle->m_position.y + s_particleXform.a2 * particle->m_position.x + s_particleXform.d2;
        record.m_particle = particle;
        CParticleEmitter2::s_pq.push(record);
    }
}

// OFFSET: [0x00B24B50, 0x00B24D70]
template <class V>
void CParticleEmitter2::BuildParticleVertices(V* vertices, uint32_t count, const C44Matrix& transform) {
    if (this->m_flags & 0x20) {
        if (this->m_params.m_hasModel) {
            this->SortParticles<CParticle2_Model>();
        } else {
            this->SortParticles<CParticle2>();
        }

        if (this->m_flags & 0x1000000) {
            this->BuildVertices<V, CSortableParticleRecord, true>(vertices, count);
        } else {
            this->BuildVertices<V, CSortableParticleRecord, false>(vertices, count);
        }
    } else if (this->m_params.m_hasModel) {
        if (this->m_flags & 0x1000000) {
            this->BuildVertices<V, CParticle2_Model, true>(vertices, count);
        } else {
            this->BuildVertices<V, CParticle2_Model, false>(vertices, count);
        }
    } else if (this->m_flags & 0x1000000) {
        this->BuildVertices<V, CParticle2, true>(vertices, count);
    } else {
        this->BuildVertices<V, CParticle2, false>(vertices, count);
    }

    if (!s_particleBounds || !this->m_renderedParticles) {
        return;
    }

    for (uint32_t i = 0; i < this->m_renderedParticles; i++) {
        const C3Vector& position = VertexPosition(vertices[i]);

        if (this->m_worldBounds.b.x > position.x) {
            this->m_worldBounds.b.x = position.x;
        }

        if (this->m_worldBounds.b.y > position.y) {
            this->m_worldBounds.b.y = position.y;
        }

        if (this->m_worldBounds.b.z > position.z) {
            this->m_worldBounds.b.z = position.z;
        }

        if (position.x > this->m_worldBounds.t.x) {
            this->m_worldBounds.t.x = position.x;
        }

        if (position.y > this->m_worldBounds.t.y) {
            this->m_worldBounds.t.y = position.y;
        }

        if (position.z > this->m_worldBounds.t.z) {
            this->m_worldBounds.t.z = position.z;
        }
    }

    this->m_worldBounds = CAaBox::Transform(this->m_worldBounds, transform);
    this->m_worldBounds.b.x = this->m_viewPos.x + this->m_worldBounds.b.x;
    this->m_worldBounds.b.y = this->m_viewPos.y + this->m_worldBounds.b.y;
    this->m_worldBounds.b.z = this->m_viewPos.z + this->m_worldBounds.b.z;
    this->m_worldBounds.t.x = this->m_worldBounds.t.x + this->m_viewPos.x;
    this->m_worldBounds.t.y = this->m_viewPos.y + this->m_worldBounds.t.y;
    this->m_worldBounds.t.z = this->m_viewPos.z + this->m_worldBounds.t.z;
}

// OFFSET: 0x00B24F90/0x1408ECEA0
void CParticleEmitter2::RenderParticles(const C44Matrix* frameOfReference, void* buffer) {
    CGxBuf* buf = NULL;

    if (!s_particleRenderEnable) {
        this->m_batchFlags &= ~0x1;
        this->m_renderedParticles = 0;
        return;
    }

    C44Matrix view;
    CGxDevice* device = g_theGxDevicePtr;
    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];

    this->RenderParticlesPrep(frameOfReference, view);

    if (!TextureGetGxTex(this->m_texture)) {
        this->m_batchFlags &= ~0x1;
        this->m_renderedParticles = 0;
        return;
    }

    uint32_t count = 0x4000 / this->m_vertsPerParticle;

    if (count >= this->m_liveList.Count()) {
        count = this->m_liveList.Count();
    }

    if (!buffer) {
        buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, GxVertexSize((this->m_params.m_materialFlags & 0x1) ? GxVBF_PNCT : GxVBF_PCT), count * this->m_vertsPerParticle);
        buffer = g_theGxDevicePtr->BufLock(buf);
    }

    C44Matrix inverse = view.AffineInverse();

    if (this->m_params.m_materialFlags & 0x1) {
        this->BuildParticleVertices(static_cast<CGxVertexPNCT*>(buffer), count, inverse);
    } else {
        this->BuildParticleVertices(static_cast<CGxVertexPCT*>(buffer), count, inverse);
    }

    if (buf) {
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;

        if (this->m_renderedParticles) {
            this->RenderParticleVertices(buf, this->m_renderedParticles * this->m_vertsPerParticle, this->m_renderedParticles * this->m_indicesPerParticle);
            g_theGxDevicePtr->XformSetView(view);
        }
    }
}

// OFFSET: 0x00B20170
void CParticleEmitter2::PlaceParticleModels(const C44Matrix* frameOfReference) {
    if (!this->m_model) {
        return;
    }

    uint32_t flags = this->m_flags;

    if (!(flags & 0x20)) {
        for (uint32_t i = 0; i < this->m_liveList.Count(); i++) {
            this->RenderParticle(this->m_particleModels[this->m_liveList[i]], frameOfReference);
        }

        return;
    }

    C44Matrix view;
    CGxDevice* device = g_theGxDevicePtr;
    view = device->m_xforms[GxXform_View].m_mtx[device->m_xforms[GxXform_View].m_level];

    C44Matrix xform;
    xform.a2 = 0.0f;
    xform.b2 = 0.0f;
    xform.c2 = 1.0f;
    xform.d2 = 0.0f;

    if (flags & 0x200) {
        xform = this->m_xform * view;
    } else if (frameOfReference) {
        xform = *frameOfReference * view;
    } else {
        xform = view;
    }

    for (uint32_t i = 0; i < this->m_liveList.Count(); i++) {
        CParticle2* particle = &this->m_particles[this->m_liveList[i]];
        CSortableParticleRecord record;
        record.m_depth = particle->m_position.z * xform.c2 + particle->m_position.y * xform.b2 + particle->m_position.x * xform.a2 + xform.d2;
        record.m_particle = particle;
        CParticleEmitter2::s_pq.push(record);
    }

    for (uint32_t i = this->m_liveList.Count(); i; i--) {
        this->RenderParticle(*static_cast<CParticle2_Model*>(CParticleEmitter2::s_pq.top().m_particle), frameOfReference);
        CParticleEmitter2::s_pq.pop();
    }
}

// OFFSET: 0x00B25160/0x1408ED1B0
void CParticleEmitter2::Render(const C44Matrix* frameOfReference, void* buffer, uint32_t first) {
    this->m_worldBounds.b.x = 3.402823466e38f;
    this->m_worldBounds.b.y = 3.402823466e38f;
    this->m_worldBounds.t.x = -3.402823466e38f;
    this->m_worldBounds.b.z = 3.402823466e38f;
    this->m_worldBounds.t.y = -3.402823466e38f;
    this->m_worldBounds.t.z = -3.402823466e38f;

    if (!this->m_liveList.Count()) {
        this->m_batchFlags &= ~0x1;
        this->m_renderedParticles = 0;
        return;
    }

    this->m_batchFlags ^= (first ^ this->m_batchFlags) & 0x1;

    if (!this->m_params.m_hasModel) {
        this->RenderParticles(frameOfReference, buffer);
    }
}

// OFFSET: 0x00B247E0
void CParticleEmitter2::Update(float deltaTime, const C44Matrix& transform, const C3Vector& viewPos, const C44Matrix* frameOfReference) {
    float dz = transform.d2 - viewPos.z;
    float dy = transform.d1 - viewPos.y;
    float dx = transform.d0 - viewPos.x;
    float distanceSq = dx * dx + (dy * dy + dz * dz);

    CParticleEmitter2::s_viewDist = distanceSq <= 0.0f ? 0.0f : sqrt(distanceSq);

    this->m_parentPosition = C3Vector(this->m_xform.d0, this->m_xform.d1, this->m_xform.d2);
    this->UpdateXform(transform, viewPos, frameOfReference);

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        this->m_childEmitters[i]->UpdateXform(transform, viewPos, frameOfReference);
    }

    if (fabs(deltaTime) < 0.00000023841858f) {
        this->m_flags |= 0x100;
        return;
    }

    float zero;

    if (this->m_flags & 0x80000) {
        this->m_followOffset.x = this->m_xform.d0 - this->m_parentPosition.x;
        this->m_followOffset.y = this->m_xform.d1 - this->m_parentPosition.y;
        this->m_followOffset.z = this->m_xform.d2 - this->m_parentPosition.z;

        zero = 0.0f;
        float lengthSq = this->m_followOffset.z * this->m_followOffset.z + this->m_followOffset.y * this->m_followOffset.y + this->m_followOffset.x * this->m_followOffset.x;
        float speed = 0.0f;

        if (lengthSq > 0.0f) {
            speed = sqrt(lengthSq) / deltaTime;
        }

        float follow = speed * this->m_followSlope + this->m_followIntercept;

        if (follow < zero) {
            follow = zero;
        } else if (follow >= 1.0f) {
            follow = 1.0f;
        }

        this->m_followOffset.x = this->m_followOffset.x * follow;
        this->m_followOffset.y = follow * this->m_followOffset.y;
        this->m_followOffset.z = follow * this->m_followOffset.z;
    } else {
        zero = 0.0f;
    }

    if (this->m_flags & 0x800) {
        this->m_ivelTimer = this->m_ivelTimer + deltaTime;

        if (this->m_ivelTimer > 0.033333335f) {
            float rate = this->m_ivelTimer * 29.999998f;
            this->m_ivelTimer = zero;

            if (this->m_liveList.Count()) {
                this->m_parentVelocity = C3Vector(this->m_xform.d0 - this->m_parentPosition.x, this->m_xform.d1 - this->m_parentPosition.y, this->m_xform.d2 - this->m_parentPosition.z);

                float scale = 1.0f / rate * this->m_ivelScale;
                this->m_parentVelocity.x = this->m_parentVelocity.x * scale;
                this->m_parentVelocity.y = scale * this->m_parentVelocity.y;
                this->m_parentVelocity.z = scale * this->m_parentVelocity.z;
            } else {
                this->m_parentVelocity.x = zero;
                this->m_parentVelocity.y = zero;
                this->m_parentVelocity.z = zero;
            }
        }
    }

    this->InternalUpdate(deltaTime, 0);
    this->m_flags |= 0x80;

    if (this->m_params.m_hasModel == 1) {
        this->PlaceParticleModels(frameOfReference);
    }
}

// OFFSET: 0x00B200A0/0x1408E7950
void CParticleEmitter2::Sync() {
    float count = (this->m_params.m_lifeVariation + this->m_params.m_life) * (this->m_params.m_emissionRateVariation + this->m_params.m_emissionRate) * 1.15f;
    this->SyncAllocation(static_cast<uint32_t>(count));

    for (uint32_t i = 0; i < this->m_childEmitterCount; i++) {
        CParticleEmitter2* child = this->m_childEmitters[i];
        uint32_t childCount = static_cast<uint32_t>(count * ((child->m_params.m_lifeVariation + child->m_params.m_life) * (child->m_params.m_emissionRateVariation + child->m_params.m_emissionRate) * 1.15f));

        if (childCount > 0x1000) {
            childCount = 0x1000;
        }

        child->SyncAllocation(childCount);
    }
}

// OFFSET: 0x00B24140/0x1408EBED0
CParticleEmitter2::CParticleEmitter2(const CParticleEmitter2& source, int32_t copyParticles) : m_random(0), m_params(source.m_params) {
    this->m_emitAccumulator = 0.0f;
    this->m_refCount = 1;
    this->m_textureCellShift = source.m_textureCellShift;
    this->m_textureCellWidth = source.m_textureCellWidth;
    this->m_textureCellHeight = source.m_textureCellHeight;
    this->m_priorityPlane = source.m_priorityPlane;
    this->m_emitterType = source.m_emitterType;
    this->m_childEmitterCount = 4;
    this->m_vertsPerParticle = source.m_vertsPerParticle;
    this->m_indicesPerParticle = source.m_indicesPerParticle;
    this->m_textureRows = source.m_textureRows;
    this->m_textureCols = source.m_textureCols;
    this->unk_012C = source.unk_012C;
    this->m_alphaScale = source.m_alphaScale;
    this->m_flags = source.m_flags;
    this->m_twinkleFPS = source.m_twinkleFPS;
    this->m_twinkleOnOff = source.m_twinkleOnOff;
    this->m_twinkleScaleBase = source.m_twinkleScaleBase;
    this->m_twinkleScaleSpan = source.m_twinkleScaleSpan;
    this->m_ivelScale = source.m_ivelScale;
    this->m_tumbleBaseX = source.m_tumbleBaseX;
    this->m_tumbleSpanX = source.m_tumbleSpanX;
    this->m_tumbleBaseY = source.m_tumbleBaseY;
    this->m_tumbleSpanY = source.m_tumbleSpanY;
    this->m_tumbleBaseZ = source.m_tumbleBaseZ;
    this->m_tumbleSpanZ = source.m_tumbleSpanZ;
    this->m_drag = source.m_drag;
    this->m_windVectorX = source.m_windVectorX;
    this->m_windVectorY = source.m_windVectorY;
    this->m_windVectorZ = source.m_windVectorZ;
    this->m_windTime = source.m_windTime;
    this->m_followIntercept = source.m_followIntercept;
    this->m_followSlope = source.m_followSlope;
    this->m_ivelTimer = source.m_ivelTimer;
    this->m_worldBounds.b.x = 0.0f;
    this->m_worldBounds.b.y = 0.0f;
    this->m_worldBounds.b.z = 0.0f;
    this->m_worldBounds.t.x = 0.0f;
    this->m_worldBounds.t.y = 0.0f;
    this->m_worldBounds.t.z = 0.0f;
    this->unk_0230 = 0;

    this->m_random.SetSeed((rand() << 16) | static_cast<uint16_t>(rand()));
    this->m_childEmitterCount = source.m_childEmitterCount;

    for (uint32_t i = 0; i < source.m_childEmitterCount; i++) {
        this->m_childEmitters[i] = ParticleSystemManager::GetInstance()->DuplicateEmitter(source.m_childEmitters[i], 0);
    }

    this->m_model = source.m_model;

    if (this->m_model) {
        this->m_model->m_refCount++;
    }

    this->m_childModel = source.m_childModel;

    if (this->m_childModel) {
        this->m_childModel->m_refCount++;
    }

    TextureAddRef(source.m_texture);

    this->m_texture = source.m_texture;
    this->m_flags &= ~0x80;

    if (!copyParticles || (!source.m_freeList.m_count && !source.m_liveList.m_count)) {
        return;
    }

    this->Sync();
    this->m_freeList.m_count = 0;
    this->m_liveList.m_count = 0;

    for (uint32_t i = 0; i < source.m_freeList.m_count; i++) {
        *this->m_freeList.New() = source.m_freeList.m_data[i];
    }

    uint32_t liveCount = source.m_liveList.m_count;

    if (this->m_params.m_hasModel) {
        for (uint32_t i = 0; i < liveCount; i++) {
            uint32_t index = source.m_liveList.m_data[i];
            *this->m_liveList.New() = index;

            CParticle2_Model& particle = this->m_particleModels.m_data[index];
            const CParticle2_Model& sourceParticle = source.m_particleModels.m_data[index];
            memcpy(&particle, &sourceParticle, offsetof(CParticle2_Model, m_model));
            particle.m_model = sourceParticle.m_model;

            if (particle.m_model) {
                particle.m_model->m_refCount++;
            }
        }

        return;
    }

    for (uint32_t i = 0; i < liveCount; i++) {
        uint32_t index = source.m_liveList.m_data[i];
        *this->m_liveList.New() = index;
        memcpy(&this->m_particles.m_data[index], &source.m_particles.m_data[index], sizeof(CParticle2));
    }
}

// PROVEN: 7bb8d416 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CParticleEmitter2::DestroyParticle(CParticle2& particle) {
}

// OFFSET: 0x00B14840
CParticle2_Model::CParticle2_Model(const CParticle2_Model& source) {
    this->m_age = source.m_age;
    this->m_position = source.m_position;
    this->m_velocity = source.m_velocity;
    this->m_lifeSeed = source.m_lifeSeed;
    this->m_randomSeed = source.m_randomSeed;
    this->m_orientation = source.m_orientation;
    this->m_angularVelocity = source.m_angularVelocity;
    this->m_model = source.m_model;

    if (this->m_model) {
        this->m_model->m_refCount++;
    }
}

// INLINED
CParticle2_Model::CParticle2_Model() {
    this->m_model = NULL;
    this->m_orientation.x = 0.0f;
    this->m_orientation.y = 0.0f;
    this->m_orientation.z = 0.0f;
    this->m_orientation.w = 1.0f;
}

// INLINED
CParticle2_Model::~CParticle2_Model() {
    if (this->m_model) {
        this->m_model->Release();
        this->m_model = NULL;
    }
}

// OFFSET: 0x00B27170
CPlaneParticleEmitter::CPlaneParticleEmitter() {
    this->m_width = 0.0f;
    this->m_height = 0.0f;
    this->m_emitterType = 1;
    this->m_latitude = 0.0f;
    this->m_longitude = 0.0f;
}

// OFFSET: 0x00B271B0
CPlaneParticleEmitter::CPlaneParticleEmitter(const CPlaneParticleEmitter& source, int32_t copyParticles) : CParticleEmitter2(source, copyParticles) {
    this->m_width = source.m_width;
    this->m_height = source.m_height;
    this->m_latitude = source.m_latitude;
    this->m_longitude = source.m_longitude;
}

// PROVEN: c14c8f1d IDENTICAL/DIFFERENT
// OFFSET: 0x00B27210/0x1408E0130
void CPlaneParticleEmitter::SetWidth(float width) {
    this->m_width = width;
}

// PROVEN: 7b4240fb IDENTICAL/NONE
// OFFSET: 0x00B27230
void CPlaneParticleEmitter::SetHeight(float height) {
    this->m_height = height;
}

// PROVEN: 3b916f09 IDENTICAL/NONE
// OFFSET: 0x00B27250
void CPlaneParticleEmitter::SetLatitude(float latitude) {
    this->m_latitude = latitude;
}

// PROVEN: e51bd20e IDENTICAL/DIFFERENT
// OFFSET: 0x00B27370/0x1408EF770
void CPlaneParticleEmitter::SetLongitude(float longitude) {
    this->m_longitude = longitude;
}

// OFFSET: 0x00B27270
CSphereParticleEmitter::CSphereParticleEmitter() {
    this->m_minRadius = 0.0f;
    this->m_maxRadius = 0.0f;
    this->m_emitterType = 2;
    this->m_latitude = 0.0f;
    this->m_longitude = 0.0f;
}

// OFFSET: 0x00B272B0
CSphereParticleEmitter::CSphereParticleEmitter(const CSphereParticleEmitter& source, int32_t copyParticles) : CParticleEmitter2(source, copyParticles) {
    this->m_minRadius = source.m_minRadius;
    this->m_maxRadius = source.m_maxRadius;
    this->m_latitude = source.m_latitude;
    this->m_longitude = source.m_longitude;
    this->m_radiusSpan = source.m_radiusSpan;
}

// PROVEN: 5f90bd03 IDENTICAL/DIFFERENT
// OFFSET: 0x00B27310/0x1408EF730
void CSphereParticleEmitter::SetWidth(float width) {
    this->m_minRadius = width;
    this->m_radiusSpan = this->m_maxRadius - width;
}

// PROVEN: 46d3a505 IDENTICAL/DIFFERENT
// OFFSET: 0x00B27340/0x1408EF750
void CSphereParticleEmitter::SetHeight(float height) {
    this->m_maxRadius = height;
    this->m_radiusSpan = height - this->m_minRadius;
}

// PROVEN: 4a09dccc IDENTICAL/DIFFERENT
// OFFSET: 0x00B27370/0x1408EF770
void CSphereParticleEmitter::SetLatitude(float latitude) {
    this->m_latitude = latitude;
}

// PROVEN: a1921b28 IDENTICAL/DIFFERENT
// OFFSET: 0x00B27390/0x1408EF780
void CSphereParticleEmitter::SetLongitude(float longitude) {
    this->m_longitude = longitude;
}

// PROVEN: 43da2e20 IDENTICAL/DIFFERENT
// OFFSET: 0x00B273B0/0x1408EF790
void CSplineParticleEmitter::SetEmissionRate(float rate) {
    this->m_ratePerUnit = rate;
    this->m_params.m_emissionRate = rate * this->m_end;
}

// PROVEN: cc428db1 IDENTICAL/DIFFERENT
// OFFSET: 0x00B273E0/0x1408EF7B0
void CSplineParticleEmitter::SetSpline(const C3Vector* points, uint32_t count) {
    this->m_spline.SetPoints(points, count);
}

// PROVEN: 0ec91914 IDENTICAL/NONE
// OFFSET: 0x00B273F0
CParticleEmitter2* CPlaneParticleEmitter::Clone(int32_t copyParticles) {
    void* mem = SMemAlloc(sizeof(CPlaneParticleEmitter), __FILE__, __LINE__, 0);

    return mem ? new (mem) CPlaneParticleEmitter(*this, copyParticles) : NULL;
}

// PROVEN: f1267c0b IDENTICAL/DIFFERENT
// OFFSET: 0x00B27430/0x1408EF870
CParticleEmitter2* CSphereParticleEmitter::Clone(int32_t copyParticles) {
    void* mem = SMemAlloc(sizeof(CSphereParticleEmitter), __FILE__, __LINE__, 0);

    return mem ? new (mem) CSphereParticleEmitter(*this, copyParticles) : NULL;
}

// OFFSET: 0x00B27470
CPlaneParticleEmitter::~CPlaneParticleEmitter() {
}

// OFFSET: 0x00B274A0
void CPlaneParticleEmitter::CreateParticle(CParticle2& particle, float time, const C44Matrix& transform) {
    float age = CRandom::ufloat_(this->m_random) * time;
    particle.m_lifeSeed = fixed16(CRandom::sfloat_(this->m_random));

    float life = particle.m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

    if (life <= 0.001f) {
        life = 0.001f;
    }

    particle.m_age = fmod(age, life);
    particle.m_randomSeed = static_cast<uint16_t>(CRandom::uint32_(this->m_random));

    float heightScale = CRandom::sfloat_(this->m_random);
    float widthScale = CRandom::sfloat_(this->m_random);

    particle.m_position.x = widthScale * this->m_width * 0.5f;
    particle.m_position.y = 0.5f * (this->m_height * heightScale);
    particle.m_position.z = 0.0f;

    float speed = this->CalcVelocity();
    C3Vector velocity;

    if (0.0f == this->m_params.m_zsource) {
        float latitude = CRandom::sfloat_(this->m_random) * this->m_latitude;
        float longitude = CRandom::sfloat_(this->m_random) * this->m_longitude;
        float cosLatitude = cos(latitude);
        float sinLatitude = sin(latitude);
        float cosLongitude = cos(longitude);
        float sinLongitude = sin(longitude);

        velocity.x = cosLongitude * sinLatitude * speed;
        velocity.y = sinLatitude * sinLongitude * speed;
        velocity.z = speed * ((cosLongitude + 1.0f - cosLongitude) * cosLatitude);
    } else {
        C3Vector direction(particle.m_position.x, particle.m_position.y, particle.m_position.z - this->m_params.m_zsource);
        float scale = speed / sqrt(direction.y * direction.y + direction.x * direction.x + direction.z * direction.z);
        velocity.x = direction.x * scale;
        velocity.y = direction.y * scale;
        velocity.z = scale * direction.z;
    }

    if (this->m_flags & 0x200) {
        particle.m_velocity = velocity;
    } else {
        particle.m_velocity.x = transform.b0 * velocity.y + transform.c0 * velocity.z + transform.a0 * velocity.x;
        particle.m_velocity.y = transform.b1 * velocity.y + transform.a1 * velocity.x + transform.c1 * velocity.z;
        particle.m_velocity.z = velocity.z * transform.c2 + (velocity.y * transform.b2 + velocity.x * transform.a2);
        particle.m_position *= transform;

        if (this->m_flags & 0x40000) {
            this->ProjectParticle(particle);
        }
    }

    if (this->m_flags & 0x800) {
        float scale = CRandom::sfloat_(this->m_random) * this->m_params.m_variation + 1.0f;
        particle.m_velocity.x = this->m_parentVelocity.x * scale + particle.m_velocity.x;
        particle.m_velocity.y = this->m_parentVelocity.y * scale + particle.m_velocity.y;
        particle.m_velocity.z = scale * this->m_parentVelocity.z + particle.m_velocity.z;
    }
}

// OFFSET: 0x00B27A20
CSphereParticleEmitter::~CSphereParticleEmitter() {
}

// OFFSET: 0x00B27A50
void CSphereParticleEmitter::CreateParticle(CParticle2& particle, float time, const C44Matrix& transform) {
    float age = CRandom::ufloat_(this->m_random) * time;
    particle.m_lifeSeed = fixed16(CRandom::sfloat_(this->m_random));

    float life = particle.m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

    if (life <= 0.001f) {
        life = 0.001f;
    }

    particle.m_age = fmod(age, life);
    particle.m_randomSeed = static_cast<uint16_t>(CRandom::uint32_(this->m_random));

    float radius = CRandom::ufloat_(this->m_random) * this->m_radiusSpan + this->m_minRadius;
    float latitude = CRandom::sfloat_(this->m_random) * this->m_latitude;
    float longitude = CRandom::sfloat_(this->m_random) * this->m_longitude;
    float cosLatitude = cos(latitude);
    float sinLatitude = sin(latitude);
    float cosLongitude = cos(longitude);
    float sinLongitude = sin(longitude);

    C3Vector direction(cosLongitude * cosLatitude, cosLatitude * sinLongitude, sinLatitude);

    particle.m_position.x = direction.x * radius;
    particle.m_position.y = direction.y * radius;
    particle.m_position.z = radius * sinLatitude;

    C3Vector velocity;

    if (0.0f != this->m_params.m_zsource) {
        direction = C3Vector(particle.m_position.x, particle.m_position.y, particle.m_position.z - this->m_params.m_zsource);
        float lengthSq = direction.y * direction.y + direction.x * direction.x + direction.z * direction.z;

        if (lengthSq > 0.00000023841858f) {
            float scale = 1.0f / sqrt(lengthSq);
            direction.x = direction.x * scale;
            direction.y = direction.y * scale;
            direction.z = direction.z * scale;
        }

        velocity = direction;
    } else if (this->m_flags & 0x8000) {
        velocity = C3Vector(0.0f, 0.0f, 1.0f);
    } else {
        velocity = direction;
    }

    float speed = this->CalcVelocity();
    velocity.x = velocity.x * speed;
    velocity.y = velocity.y * speed;
    velocity.z = speed * velocity.z;

    if (this->m_flags & 0x200) {
        particle.m_velocity = velocity;
    } else {
        particle.m_velocity.x = transform.c0 * velocity.z + transform.b0 * velocity.y + transform.a0 * velocity.x;
        particle.m_velocity.y = transform.c1 * velocity.z + transform.b1 * velocity.y + transform.a1 * velocity.x;
        particle.m_velocity.z = velocity.y * transform.b2 + velocity.z * transform.c2 + velocity.x * transform.a2;
        particle.m_position *= transform;

        if (this->m_flags & 0x40000) {
            this->ProjectParticle(particle);
        }
    }

    if (this->m_flags & 0x800) {
        float scale = CRandom::sfloat_(this->m_random) * this->m_params.m_variation + 1.0f;
        particle.m_velocity.x = this->m_parentVelocity.x * scale + particle.m_velocity.x;
        particle.m_velocity.y = this->m_parentVelocity.y * scale + particle.m_velocity.y;
        particle.m_velocity.z = scale * this->m_parentVelocity.z + particle.m_velocity.z;
    }
}

// OFFSET: 0x00B27F90/0x1408F0580
void CSplineParticleEmitter::SetWidth(float width) {
    if (width < 0.0f) {
        this->m_start = 0.0f;
    } else if (width < 1.0f) {
        this->m_start = width;
    } else {
        this->m_start = 1.0f;
    }
}

// OFFSET: 0x00B27FC0/0x1408F05B0
void CSplineParticleEmitter::SetHeight(float height) {
    float end = 0.0f;

    if (height >= 0.0f) {
        end = height;

        if (height >= 1.0f) {
            end = 1.0f;
        }
    }

    if (fabs(end - this->m_end) >= 0.00000023841858f) {
        this->m_end = end;
        this->m_emitAtEnd = 1;
        this->m_params.m_emissionRate = end * this->m_ratePerUnit;
    }
}

// PROVEN: 30258723 IDENTICAL/DIFFERENT
// OFFSET: 0x00B27370/0x1408EF770
void CSplineParticleEmitter::SetLatitude(float latitude) {
    this->m_latitude = latitude;
}

// PROVEN: f55f2b60 IDENTICAL/DIFFERENT
// OFFSET: 0x00B27390/0x1408EF780
void CSplineParticleEmitter::SetLongitude(float longitude) {
    this->m_longitude = longitude;
}

// OFFSET: 0x00B28030/0x1408F0610
void CSplineParticleEmitter::CreateParticle(CParticle2& particle, float time, const C44Matrix& transform) {
    float age = CRandom::ufloat_(this->m_random) * time;
    particle.m_lifeSeed = fixed16(CRandom::sfloat_(this->m_random));

    float life = particle.m_lifeSeed.m_value * this->m_params.m_lifeVariation * 0.000030518509f + this->m_params.m_life;

    if (life <= 0.001f) {
        life = 0.001f;
    }

    particle.m_age = fmod(age, life);
    particle.m_randomSeed = static_cast<uint16_t>(CRandom::uint32_(this->m_random));

    float t;

    if (this->m_emitAtEnd) {
        t = this->m_end;
        this->m_emitAtEnd = 0;
    } else {
        t = (this->m_end - this->m_start) * CRandom::ufloat_(this->m_random) + this->m_start;
    }

    this->m_spline.Pos(t, particle.m_position, C3Spline::EVAL_ARCLENGTH);

    C3Vector velocity(0.0f, 0.0f, this->CalcVelocity());

    if (0.0f == this->m_params.m_zsource) {
        if (0.0f != this->m_latitude) {
            C3Vector up(0.0f, 0.0f, 1.0f);
            C3Vector tangent(0.0f, 0.0f, 0.0f);
            this->m_spline.Vel(t, tangent, C3Spline::EVAL_ARCLENGTH);

            float scale = 1.0f / sqrt(tangent.z * tangent.z + tangent.y * tangent.y + tangent.x * tangent.x);
            tangent.x = tangent.x * scale;
            tangent.y = tangent.y * scale;
            tangent.z = scale * tangent.z;

            if (fabs(tangent.z + (tangent.x + tangent.y) * 0.0f) > 0.89999998f) {
                up.z = 1.0f;
            }

            C33Matrix rotation = C33Matrix::Rotation(CRandom::sfloat_(this->m_random) * this->m_latitude, tangent, true);
            C3Vector direction = up * rotation;

            if (0.0f != this->m_longitude) {
                C3Vector offset(direction.x * this->m_longitude, direction.y * this->m_longitude, this->m_longitude * direction.z);
                float distance = CRandom::ufloat_(this->m_random);
                particle.m_position.x = offset.x * distance + particle.m_position.x;
                particle.m_position.y = offset.y * distance + particle.m_position.y;
                particle.m_position.z = distance * offset.z + particle.m_position.z;
            }

            velocity = C3Vector(direction.x * velocity.z, direction.y * velocity.z, velocity.z * direction.z);
        }
    } else {
        C3Vector direction(particle.m_position.x, particle.m_position.y, particle.m_position.z - this->m_params.m_zsource);
        float scale = velocity.z / sqrt(direction.y * direction.y + direction.x * direction.x + direction.z * direction.z);
        velocity = C3Vector(direction.x * scale, direction.y * scale, scale * direction.z);
    }

    if (this->m_flags & 0x200) {
        particle.m_velocity = velocity;
    } else {
        particle.m_velocity.x = transform.a0 * velocity.x + transform.b0 * velocity.y + transform.c0 * velocity.z;
        particle.m_velocity.y = transform.b1 * velocity.y + transform.a1 * velocity.x + transform.c1 * velocity.z;
        particle.m_velocity.z = velocity.z * transform.c2 + (velocity.x * transform.a2 + velocity.y * transform.b2);
        particle.m_position *= transform;

        if (this->m_flags & 0x40000) {
            this->ProjectParticle(particle);
        }
    }

    if (this->m_flags & 0x800) {
        float scale = CRandom::sfloat_(this->m_random) * this->m_params.m_variation + 1.0f;
        particle.m_velocity.x = this->m_parentVelocity.x * scale + particle.m_velocity.x;
        particle.m_velocity.y = this->m_parentVelocity.y * scale + particle.m_velocity.y;
        particle.m_velocity.z = scale * this->m_parentVelocity.z + particle.m_velocity.z;
    }
}

// OFFSET: 0x00B28600/0x1408F0D00
CSplineParticleEmitter::CSplineParticleEmitter() {
    this->m_start = 0.0f;
    this->m_end = 0.0f;
    this->m_latitude = 0.0f;
    this->m_emitterType = 3;
    this->m_longitude = 0.0f;
    this->m_emitAtEnd = 0;
    this->m_ratePerUnit = 0.0f;
}

// OFFSET: 0x00B28670/0x1408F0D80
CSplineParticleEmitter::~CSplineParticleEmitter() {
}

// OFFSET: 0x00B287A0/0x1408F0F20
CSplineParticleEmitter::CSplineParticleEmitter(const CSplineParticleEmitter& source, int32_t copyParticles) : CParticleEmitter2(source, copyParticles) {
    this->m_start = source.m_start;
    this->m_end = source.m_end;
    this->m_latitude = source.m_latitude;
    this->m_longitude = source.m_longitude;
    this->m_ratePerUnit = source.m_ratePerUnit;
    this->m_emitAtEnd = source.m_emitAtEnd;
    this->m_spline = source.m_spline;
}

// PROVEN: 24447780 IDENTICAL/DIFFERENT
// OFFSET: 0x00B28820/0x1408F0FD0
CParticleEmitter2* CSplineParticleEmitter::Clone(int32_t copyParticles) {
    void* mem = SMemAlloc(sizeof(CSplineParticleEmitter), __FILE__, __LINE__, 0);

    return mem ? new (mem) CSplineParticleEmitter(*this, copyParticles) : NULL;
}
