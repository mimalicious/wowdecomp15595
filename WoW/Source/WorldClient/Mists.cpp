#include "WoW/Source/WorldClient/Mists.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// INLINED
Mists::Mist::Mist() {
    this->m_heightCount = 64;
    this->m_startTime = 0;
}

// OFFSET: 0x007484C0/0x140427540
void Mists::Mist::SetHeights(uint32_t count, const float* heights) {
    for (uint32_t i = 0; i < count; i++) {
        this->m_heights[i] = heights[i];
    }

    this->m_heightCount = count;
}

// OFFSET: 0x0074AF00
bool Mists::CreateMist(Mist& mist, uint32_t time) {
    mist.m_startTime = time;
    float angle = (CRandom::ufloat_(g_rndSeed) - 0.5f) * this->m_angleRange + this->m_angle;
    float c = cos(angle);
    float s = sin(angle);
    float speed = (CRandom::ufloat_(g_rndSeed) - 0.5f) * this->m_speedRange + this->m_speed;
    float rise = CRandom::ufloat_(g_rndSeed) - 0.5f;
    mist.m_velocity.x = speed * s;
    mist.m_velocity.y = speed * c;
    mist.m_velocity.z = rise * 0.033333335f + 0.33333334f;
    C44Matrix rotation = C44Matrix::RotationAroundZ(-this->m_weather->m_facing);
    mist.m_velocity = rotation * mist.m_velocity;
    float rz = CRandom::ufloat_(g_rndSeed) - 0.5f;
    float ry = CRandom::ufloat_(g_rndSeed) - 0.5f;
    float rx = CRandom::ufloat_(g_rndSeed) - 0.5f;
    mist.m_pos.x = this->m_extents.x * rx;
    mist.m_pos.y = this->m_extents.y * ry;
    mist.m_pos.z = this->m_extents.z * rz;
    mist.m_pos = rotation * mist.m_pos;
    const C3Vector& camPos = CWorldScene::s_defaultWorldView->m_camPos;
    mist.m_pos.x = mist.m_pos.x + (camPos.x - mist.m_velocity.x * 1.5f);
    mist.m_pos.y = (camPos.y - mist.m_velocity.y * 1.5f) + mist.m_pos.y;
    mist.m_pos.z = (camPos.z - mist.m_velocity.z * 1.5f) + mist.m_pos.z;
    float height = this->m_weather->m_hmMgr->Query(mist.m_pos);

    if (height == FLT_MAX) {
        mist.m_startTime = 0;
        return false;
    }

    float z = mist.m_pos.z;

    if (z <= height) {
        z = height;
    }

    mist.m_pos.z = this->m_size * 0.5f + z;
    mist.m_rise = (CRandom::ufloat_(g_rndSeed) - 0.5f) * 3.3333333f;
    mist.m_endTime = mist.m_startTime + static_cast<uint32_t>(((CRandom::ufloat_(g_rndSeed) - 0.5f) * 0.30000001f + 2.7f) * 1000.0f);
    return true;
}

// OFFSET: 0x0074B3D0/0x14042AD70
Mists::Mist::Mist(const Mist& source) {
    this->SetHeights(source.m_heightCount, source.m_heights);
    this->m_numHeights = source.m_numHeights;
    this->m_pos = source.m_pos;
    this->m_velocity = source.m_velocity;
    this->m_startTime = source.m_startTime;
    this->m_endTime = source.m_endTime;
    this->m_rise = source.m_rise;
}

// OFFSET: 0x0074B8F0
bool Mists::InitMist(Mist& mist, uint32_t time) {
    if (!this->CreateMist(mist, time)) {
        return false;
    }

    double duration = static_cast<double>(mist.m_endTime - mist.m_startTime) * 0.001;
    float vx = mist.m_velocity.x;
    float vy = mist.m_velocity.y;
    float length = sqrt(vx * vx + vy * vy);
    float stepX = 1.0f / length * vx * 1.0416666f;
    float stepY = 1.0f / length * vy * 1.0416666f;
    mist.m_numHeights = static_cast<uint32_t>(length * duration / sqrt(stepX * stepX + stepY * stepY));
    int32_t count = mist.m_numHeights;

    if (count >= 64) {
        count = 64;
    }

    mist.m_numHeights = count;
    C3Vector pos = mist.m_pos;

    for (int32_t i = 0; i < static_cast<int32_t>(mist.m_numHeights); i++) {
        mist.m_heights[i] = this->m_weather->m_hmMgr->Query(pos);
        pos.x = pos.x + stepX;
        pos.y = pos.y + stepY;
        pos.z = pos.z + 1.0f / length * 0.0f * 1.0416666f;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(mist.m_numHeights) - 3; i++) {
        float base = mist.m_heights[i];

        if (mist.m_heights[i + 3] - base > 1.0f || mist.m_heights[i + 2] - base > 0.75f || mist.m_heights[i + 1] - base > 0.5f) {
            double fraction = static_cast<double>(i + 1) / static_cast<double>(static_cast<int32_t>(mist.m_numHeights));
            mist.m_numHeights = i;
            mist.m_endTime = mist.m_startTime + static_cast<uint32_t>(duration * fraction * 1000.0);

            if (!i) {
                mist.m_endTime = 0;
                mist.m_startTime = 0;
            }

            break;
        }
    }

    return true;
}

// OFFSET: 0x007524F0
Mists::Mists(Weather* weather, float size, const C3Vector& extents, const char* texture, float angle, float angleRange, float speed, float speedRange, uint32_t count) {
    this->m_weather = weather;
    this->m_extents = extents;
    this->m_spawnRate = 0.0f;
    this->m_spawnAccum = 0.0f;
    this->m_size = size;
    this->m_speed = speed;
    this->m_angle = angle;
    this->m_speedRange = speedRange;
    this->m_angleRange = angleRange;
    this->m_fadeOut = 0;
    this->m_texture = NULL;
    this->m_fadeTime = 0.40000001f;
    this->m_mists.SetCount(count);
    CStatus status;
    CGxTexFlags flags;
    flags.m_filter = 4;
    this->m_texture = TextureCreate(texture, flags, &status, 1, NULL, 2);
}

// INLINED
Mists::~Mists() {
    TextureRelease(this->m_texture);
}

// INLINED
uint32_t Mists::NumActive() {
    uint32_t active = 0;
    uint32_t now = FrameTime::GetCurTimeMs();

    for (uint32_t i = 0; i < this->m_mists.Count(); i++) {
        const Mist& mist = this->m_mists[i];

        if (mist.m_startTime && now > mist.m_startTime && now <= mist.m_endTime) {
            active++;
        }
    }

    return active;
}

// OFFSET: 0x00750560/0x140430DB0
void Mists::Render() {
    CGxTex* gxTex = TextureGetGxTex(this->m_texture);

    if (!gxTex) {
        return;
    }

    const DayNight::FogInfo* fog = DayNight::GetFogInfo();
    float half = this->m_size * 0.5f;
    C44Matrix view;
    GxXformView(view);
    C3Vector right(view.a0 * half, view.b0 * half, view.c0 * half);
    C3Vector up(view.a1 * half, view.b1 * half, view.c1 * half);
    C3Vector corners[4];
    corners[0] = -right + up;
    corners[1] = up + right;
    corners[2] = -up + right;
    corners[3] = -up + -right;
    C2Vector texCoords[4] = { C2Vector(0.0f, 0.0f), C2Vector(1.0f, 0.0f), C2Vector(1.0f, 1.0f), C2Vector(0.0f, 1.0f) };
    uint32_t now = FrameTime::GetCurTimeMs();
    uint32_t tickMs = FrameTime::GetTickTimeMs();
    uint32_t prev = now - tickMs;
    float dt = FrameTime::GetTickTimeSec();
    float dt2 = dt * dt;
    float accum = FrameTime::GetTickTimeSec() * this->m_spawnRate + this->m_spawnAccum;
    this->m_spawnAccum = accum;
    float maxAccum = static_cast<float>(this->m_mists.Count());
    this->m_spawnAccum = accum <= maxAccum ? accum : maxAccum;
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), 4 * this->m_mists.Count());
    CGxVertexPCT* vertex = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(vertexBuf));
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), 6 * this->m_mists.Count());
    uint16_t* index = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    uint16_t vertexCount = 0;

    for (uint32_t i = 0; i < this->m_mists.Count(); i++) {
        Mist& mist = this->m_mists[i];

        if (mist.m_startTime && now > mist.m_endTime) {
            mist.m_startTime = 0;
        }

        if (this->m_fadeOut && now <= mist.m_startTime) {
            mist.m_startTime = 0;
        }

        if (!mist.m_startTime && this->m_spawnAccum >= 1.0f && !this->m_fadeOut) {
            this->m_spawnAccum = this->m_spawnAccum - 1.0f;
            uint32_t offset = static_cast<uint32_t>(CRandom::ufloat_(g_rndSeed) * static_cast<double>(tickMs));
            this->InitMist(mist, prev + offset);
        }

        if (!mist.m_startTime) {
            continue;
        }

        float lifetime = static_cast<float>(static_cast<double>(mist.m_endTime - mist.m_startTime) * 0.001);
        float rise = mist.m_rise * dt2 * 0.5f;
        mist.m_pos.x = mist.m_pos.x + (mist.m_velocity.x * dt + rise);
        mist.m_pos.y = mist.m_velocity.y * dt + rise + mist.m_pos.y;
        mist.m_pos.z = mist.m_velocity.z * dt + rise + mist.m_pos.z;
        float age = static_cast<float>(static_cast<double>(now - mist.m_startTime) * 0.001);
        float prevAge = prev <= mist.m_startTime ? 0.0f : static_cast<float>(static_cast<double>(prev - mist.m_startTime) * 0.001);
        float t = 1.0f / lifetime * age >= 1.0f ? 1.0f : 1.0f / lifetime * age;
        uint32_t numHeights = mist.m_numHeights;
        float position = static_cast<float>(numHeights) * (prevAge * (1.0f / lifetime));
        int32_t i0 = static_cast<int32_t>(position);
        int32_t i1 = static_cast<int32_t>(static_cast<float>(numHeights) * t);

        if (i1 == i0) {
            i1 = i0 + 1;
        }

        if (i1 >= static_cast<int32_t>(numHeights - 1)) {
            i1 = numHeights - 1;
        }

        float z = mist.m_pos.z;
        float ground = (mist.m_heights[i1] - mist.m_heights[i0]) * (position - static_cast<float>(i0)) + mist.m_heights[i0] + this->m_size * 0.5f;

        if (ground > z) {
            mist.m_rise = mist.m_rise + 1.6666666f;
            float dz = ground - z;

            if (this->m_size * 0.25f <= dz) {
                dz = this->m_size * 0.25f;
            }

            mist.m_pos.z = z + dz;
        }

        float fadeTime = this->m_fadeTime;
        CImVector color = fog->m_color;
        const C3Vector& camPos = CWorldScene::s_defaultWorldView->m_camPos;
        float fadeIn = 1.0f / fadeTime * -1.0f * age + 1.0f / fadeTime * fadeTime;
        fadeIn = fadeIn < 0.0f ? 0.0f : fadeIn >= 1.0f ? 1.0f : fadeIn;
        float fadeScale = 1.0f / (lifetime - (lifetime - fadeTime));
        float fadeOut = fadeScale * -1.0f * age + fadeScale * lifetime;
        fadeOut = fadeOut < 0.0f ? 0.0f : fadeOut >= 1.0f ? 1.0f : fadeOut;
        float alpha = (1.0f - fadeIn) * fadeOut;

        for (uint32_t j = 0; j < 4; j++) {
            float x = corners[j].x + mist.m_pos.x - camPos.x;
            float y = corners[j].y + mist.m_pos.y - camPos.y;
            float zz = corners[j].z + mist.m_pos.z - camPos.z;
            float nearFade = 1.5f - sqrt(zz * zz + y * y + x * x) * 0.083333336f;
            nearFade = 0.0f <= nearFade ? (nearFade >= 1.0f ? 1.0f : nearFade) : 0.0f;
            color.a = static_cast<uint8_t>(static_cast<int32_t>((1.0f - nearFade) * alpha * 255.0f));
            vertex->p.x = x;
            vertex->p.y = y;
            vertex->p.z = zz;

            if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
                CImVector swizzled;
                swizzled.b = color.r;
                swizzled.g = color.g;
                swizzled.r = color.b;
                swizzled.a = color.a;
                vertex->c = swizzled;
            } else {
                vertex->c = color;
            }

            vertex->tc = texCoords[j];
            vertex++;
        }

        index[1] = vertexCount + 1;
        index[0] = vertexCount;
        index[3] = vertexCount;
        index[2] = vertexCount + 2;
        index[4] = vertexCount + 2;
        index[5] = vertexCount + 3;
        vertexCount += 4;
        index += 6;
    }

    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;
    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;

    if (!vertexCount) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    C44Matrix world;
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    DefaultShaders::SetFogEnabled(0);
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, static_cast<EGxVertexBufferFormat>(9));
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);
    DefaultShaders::SetDefaultShaders(static_cast<EGxVertexBufferFormat>(22), -1);
    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = (6 * vertexCount) >> 2;
    batch.m_minIndex = 0;
    batch.m_maxIndex = vertexCount - 1;
    g_theGxDevicePtr->Draw(batch);
    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}
