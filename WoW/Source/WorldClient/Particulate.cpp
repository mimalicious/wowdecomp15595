#include "WoW/Source/WorldClient/Particulate.h"
#include <cmath>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D02ADC
static const float s_sinkSpeed = -0.02f;

// STATIC: 0x00D02AE0
static const float s_riseSpeed = 0.02f;

// STATIC: 0x00D02AEC
static const float s_particleCorners[4][3] = { { -0.5f, 0.5f, 0.0f }, { -0.5f, -0.5f, 0.0f }, { 0.5f, 0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f } };

// STATIC: 0x00D02B20
static const float s_particleTexCoords[25][8] = {
    { 0.0f, 0.0f, 0.0f, 0.19921875f, 0.19921875f, 0.0f, 0.19921875f, 0.19921875f },
    { 0.19921875f, 0.0f, 0.19921875f, 0.19921875f, 0.3984375f, 0.0f, 0.3984375f, 0.19921875f },
    { 0.3984375f, 0.0f, 0.3984375f, 0.19921875f, 0.59765625f, 0.0f, 0.59765625f, 0.19921875f },
    { 0.59765625f, 0.0f, 0.59765625f, 0.19921875f, 0.796875f, 0.0f, 0.796875f, 0.19921875f },
    { 0.0f, 0.19921875f, 0.0f, 0.3984375f, 0.19921875f, 0.19921875f, 0.19921875f, 0.3984375f },
    { 0.19921875f, 0.19921875f, 0.19921875f, 0.3984375f, 0.3984375f, 0.19921875f, 0.3984375f, 0.3984375f },
    { 0.3984375f, 0.19921875f, 0.3984375f, 0.3984375f, 0.59765625f, 0.19921875f, 0.59765625f, 0.3984375f },
    { 0.59765625f, 0.19921875f, 0.59765625f, 0.3984375f, 0.796875f, 0.19921875f, 0.796875f, 0.3984375f },
    { 0.0f, 0.3984375f, 0.0f, 0.59765625f, 0.19921875f, 0.3984375f, 0.19921875f, 0.59765625f },
    { 0.19921875f, 0.3984375f, 0.19921875f, 0.59765625f, 0.3984375f, 0.3984375f, 0.3984375f, 0.59765625f },
    { 0.3984375f, 0.3984375f, 0.3984375f, 0.59765625f, 0.59765625f, 0.3984375f, 0.59765625f, 0.59765625f },
    { 0.59765625f, 0.3984375f, 0.59765625f, 0.59765625f, 0.796875f, 0.3984375f, 0.796875f, 0.59765625f },
    { 0.796875f, 0.0f, 0.796875f, 0.19921875f, 0.99609375f, 0.0f, 0.99609375f, 0.19921875f },
    { 0.796875f, 0.19921875f, 0.796875f, 0.3984375f, 0.99609375f, 0.19921875f, 0.99609375f, 0.3984375f },
    { 0.796875f, 0.3984375f, 0.796875f, 0.59765625f, 0.99609375f, 0.3984375f, 0.99609375f, 0.59765625f },
    { 0.796875f, 0.59765625f, 0.796875f, 0.796875f, 0.99609375f, 0.59765625f, 0.99609375f, 0.796875f },
    { 0.796875f, 0.796875f, 0.796875f, 0.99609375f, 0.99609375f, 0.796875f, 0.99609375f, 0.99609375f },
    { 0.0f, 0.59765625f, 0.0f, 0.796875f, 0.19921875f, 0.59765625f, 0.19921875f, 0.796875f },
    { 0.19921875f, 0.59765625f, 0.19921875f, 0.796875f, 0.3984375f, 0.59765625f, 0.3984375f, 0.796875f },
    { 0.3984375f, 0.59765625f, 0.3984375f, 0.796875f, 0.59765625f, 0.59765625f, 0.59765625f, 0.796875f },
    { 0.59765625f, 0.59765625f, 0.59765625f, 0.796875f, 0.796875f, 0.59765625f, 0.796875f, 0.796875f },
    { 0.0f, 0.796875f, 0.0f, 0.99609375f, 0.19921875f, 0.796875f, 0.19921875f, 0.99609375f },
    { 0.19921875f, 0.796875f, 0.19921875f, 0.99609375f, 0.3984375f, 0.796875f, 0.3984375f, 0.99609375f },
    { 0.3984375f, 0.796875f, 0.3984375f, 0.99609375f, 0.59765625f, 0.796875f, 0.59765625f, 0.99609375f },
    { 0.59765625f, 0.796875f, 0.59765625f, 0.99609375f, 0.796875f, 0.796875f, 0.796875f, 0.99609375f },
};

// STATIC: 0x00BC9090
static const uint32_t s_particleFrames[5][8] = {
    { 0, 1, 2, 3, 4, 5, 6, 7 },
    { 8, 9, 10, 11, 8, 9, 10, 11 },
    { 12, 12, 12, 12, 12, 12, 12, 12 },
    { 13, 14, 15, 16, 13, 14, 15, 16 },
    { 17, 18, 19, 20, 21, 22, 23, 24 },
};

// OFFSET: 0x0077CC80/0x1404696C0
Particulate::~Particulate() {
    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }
}

// OFFSET: 0x0077CCA0/0x1404696E0
void Particulate::SetScale(float scale) {
    this->m_scale = scale;
}

// OFFSET: 0x0077D300/0x140469CE0
void Particulate::InitParticles(uint32_t liquidType) {
    float minSize = this->m_scale * 0.5f;
    float sizeRange = this->m_scale * 1.5f - minSize;
    float half = this->m_range * 0.5f;

    for (uint32_t i = 0; i < this->m_count; i++) {
        float z = CRandom::ufloat_(g_rndSeed);
        float y = CRandom::ufloat_(g_rndSeed);
        float x = CRandom::ufloat_(g_rndSeed);
        Particle& particle = this->m_particles[i];
        particle.m_pos.x = x * this->m_range - half;
        particle.m_pos.y = y * this->m_range - half;
        particle.m_pos.z = z * this->m_range - half;
        particle.m_size = CRandom::ufloat_(g_rndSeed) * sizeRange + minSize;
    }

    this->m_liquidType = liquidType;
}

// OFFSET: 0x0077D750/0x14046A120
void Particulate::InitMovement() {
    float theta = CRandom::sfloat_(g_rndSeed) * 3.1415927f;
    float phi = CRandom::sfloat_(g_rndSeed) * 3.1415927f;
    float s = sin(theta);
    this->m_moveDir.y = sin(phi) * s;
    this->m_moveDir.x = s * cos(phi);
    this->m_moveDir.z = cos(theta) * 0.25f;

    if (this->m_moveDir.z < 0.0f) {
        this->m_moveDir.z = -this->m_moveDir.z;
    }

    float z = this->m_moveDir.z;
    float x = this->m_moveDir.x;
    float y = this->m_moveDir.y;
    float inv = 1.0f / sqrt(x * x + y * y + z * z);
    this->m_moveDir.x = x * inv;
    this->m_moveDir.y = y * inv;
    this->m_moveDir.z = z * inv;
    this->m_moveTime = 0.0f;
    this->m_moveSpeed = (CRandom::ufloat_(g_rndSeed) + 1.0f) * 0.0125f;
    this->m_moveAmplitude = (CRandom::ufloat_(g_rndSeed) + 1.0f) * 0.0049999999f;
}

// OFFSET: 0x0077D940/0x14046A330
C3Vector Particulate::ComputeMovement(float dt) {
    LiquidTypeRec* rec = g_liquidTypeDB.GetRecord(this->m_liquidType);

    switch (rec->m_particleMovement) {
        case 0: {
            float time = dt + this->m_moveTime;
            float t = this->m_moveSpeed * time;
            this->m_moveTime = time;

            if (t > 0.5f) {
                this->InitMovement();
                t = 0.0f;
            }

            float amount = CMath::sin_(t * 6.2831855f) * this->m_moveAmplitude;
            return C3Vector(this->m_moveDir.x * amount, this->m_moveDir.y * amount, amount * this->m_moveDir.z);
        }

        case 1:
            return C3Vector(0.0f, 0.0f, dt * s_sinkSpeed);

        case 2:
            return C3Vector(0.0f, 0.0f, dt * s_riseSpeed);

        default:
            return C3Vector(0.0f, 0.0f, 0.0f);
    }
}

// OFFSET: 0x0077DA40/0x14046A450
void Particulate::Update() {
    if (!this->m_enabled) {
        return;
    }

    CWorldView* view = CWorldScene::s_defaultWorldView;
    C3Vector delta(this->m_lastCamPos.x - view->m_camPos.x, this->m_lastCamPos.y - view->m_camPos.y, this->m_lastCamPos.z - view->m_camPos.z);
    float half = this->m_range * 0.5f;
    this->m_lastCamPos = view->m_camPos;

    if (delta.y * delta.y + delta.x * delta.x + delta.z * delta.z > this->m_range * this->m_range) {
        this->InitParticles(this->m_liquidType);
        delta = C3Vector(0.0f, 0.0f, 0.0f);
    }

    C3Vector move = this->ComputeMovement(FrameTime::GetTickTimeSec());
    float mz = move.z + delta.z;
    float mx = move.x + delta.x;
    float my = move.y + delta.y;

    for (uint32_t i = 0; i < this->m_count; i++) {
        C3Vector& pos = this->m_particles[i].m_pos;
        pos.x = pos.x + mx;
        pos.y = pos.y + my;
        pos.z = pos.z + mz;

        if (pos.x > half) {
            pos.x = pos.x - this->m_range;
        } else if (!(-half <= pos.x)) {
            pos.x = pos.x + this->m_range;
        }

        if (pos.y > half) {
            pos.y = pos.y - this->m_range;
        } else if (!(-half <= pos.y)) {
            pos.y = pos.y + this->m_range;
        }

        if (pos.z > half) {
            pos.z = pos.z - this->m_range;
        } else if (!(-half <= pos.z)) {
            pos.z = pos.z + this->m_range;
        }
    }
}

// OFFSET: 0x0077E180/0x14046B080
void Particulate::Render(CWorldView* view) {
    if (!this->m_enabled) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(this->m_texture);

    if (!gxTex) {
        return;
    }

    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), 2664);
    CGxVertexPCT* vertex = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(vertexBuf));
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 3996);
    uint16_t* index = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    LiquidTypeRec* rec = g_liquidTypeDB.GetRecord(this->m_liquidType);
    uint32_t color = 0xFFFFFFFF;

    if (rec->m_flags & 0x10) {
        color = g_theGxDevicePtr->m_appRenderStates[GxRs_FogColor].m_value.m_data.i[0];
    }

    uint32_t vertexCount = 0;
    uint32_t frame = 8;
    const C44Matrix& mat = view->m_viewMat;

    for (uint32_t i = 0; i < this->m_count; i++) {
        const Particle& particle = this->m_particles[i];
        const C3Vector& pos = particle.m_pos;
        float x = pos.x * mat.a0 + mat.b0 * pos.y + pos.z * mat.c0;
        float z = pos.x * mat.a2 + pos.z * mat.c2 + pos.y * mat.b2;
        float y = mat.a1 * pos.x + mat.c1 * pos.z + mat.b1 * pos.y;

        if (z > 0.0f && z > x && x > -z && z > y && y > -z) {
            const float* tc = s_particleTexCoords[frame];

            for (uint32_t j = 0; j < 4; j++, vertex++) {
                vertex->p.x = s_particleCorners[j][0] * particle.m_size + x;
                vertex->p.y = s_particleCorners[j][1] * particle.m_size + y;
                vertex->p.z = z;
                vertex->c = CImVector(color);
                vertex->tc.x = tc[2 * j];
                vertex->tc.y = tc[2 * j + 1];
            }

            index[0] = vertexCount;
            index[1] = vertexCount + 1;
            index[2] = vertexCount + 2;
            index[3] = vertexCount + 3;
            index[4] = vertexCount + 2;
            index[5] = vertexCount + 1;
            index += 6;
            vertexCount += 4;

            if ((vertexCount & ~3) >= 2664) {
                break;
            }
        }

        frame = s_particleFrames[rec->m_particleTexSlots][i & 7];
    }

    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;
    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;

    C44Matrix identity;
    GxXformPush(GxXform_World);
    GxXformPush(GxXform_View);
    GxXformSet(GxXform_World, identity);
    GxXformSetView(identity);
    g_theGxDevicePtr->RsPush();
    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    DefaultShaders::SetFogEnabled(0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PCT);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    DefaultShaders::SetDefaultShaders(static_cast<EGxVertexBufferFormat>(22), -1);

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = 6 * (vertexCount >> 2);
    batch.m_minIndex = 0;
    batch.m_maxIndex = vertexCount - 1;
    g_theGxDevicePtr->Draw(batch);
    g_theGxDevicePtr->RsPop();
    GxXformPop(GxXform_View);
    GxXformPop(GxXform_World);
}

// OFFSET: 0x0077EEF0/0x14046C080
void Particulate::SetTexture(const char* textureName) {
    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    CStatus status;
    CGxTexFlags flags;
    flags.m_filter = 3;
    this->m_texture = TextureCreate(textureName, flags, &status, 0, NULL, 2);
    SysMsgAdd(&status);
}

// OFFSET: 0x0077F1A0/0x14046C3D0
Particulate::Particulate(float scale, float range, const char* textureName) {
    this->m_lastCamPos = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_enabled = 0;
    this->m_moveDir = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_texture = NULL;
    this->m_scale = scale;
    this->m_count = 4000;
    this->m_range = range;
    this->SetTexture(textureName);
    this->InitParticles(2);
    this->InitMovement();
}
