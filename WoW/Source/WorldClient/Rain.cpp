#include "WoW/Source/WorldClient/Rain.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D017C8
static float s_motionBlurMaxAngle = 45.0f;

// STATIC: 0x00D017CC/0x140BB1308
static float s_motionBlurMaxSpeed = 30.0f;

// STATIC: 0x00D017D0
static float s_windLead = 1.75f;

struct PTVertex {
    /* 0000 */ C3Vector p;
    /* 000C */ C2Vector tc;
};

// OFFSET: 0x007485E0/0x140427720
float Rain::GetDropStretch() const {
    const C3Vector& velocity = this->m_weather->m_velocity;
    float stretch = 2.0f;

    if (fabs(velocity.z * velocity.z + velocity.x * velocity.x + velocity.y * velocity.y) >= 0.001f) {
        return 0.0f;
    }

    return stretch;
}

// OFFSET: 0x00748640
void Rain::RealizePacket(DropPacket* packet) {
    Drop::Vertex* vertex = reinterpret_cast<Drop::Vertex*>(g_theGxDevicePtr->BufLock(packet->m_buf));

    for (uint32_t i = 0; i < packet->m_count; i++) {
        const Drop& drop = packet->m_buffer.m_particles[i];
        CImVector corner(0);

        for (uint32_t j = 0; j < 3; j++) {
            vertex->m_pos = drop.m_pos;
            vertex->m_velocity = drop.m_velocity;
            corner.a = j;

            if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
                CImVector swizzled(0);
                swizzled.a = j;
                vertex->m_corner = swizzled;
            } else {
                vertex->m_corner = corner;
            }

            vertex->m_startTime = drop.m_startTime;
            vertex->m_endTime = drop.m_endTime;
            vertex++;
        }
    }

    CGxBuf* buf = packet->m_buf;
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x00748740
void Rain::RealizePacket(PatterPacket* packet) {
    Patter::Vertex* vertex = reinterpret_cast<Patter::Vertex*>(g_theGxDevicePtr->BufLock(packet->m_buf));

    for (uint32_t i = 0; i < packet->m_count; i++) {
        const Patter& patter = packet->m_buffer.m_particles[i];
        CImVector corner(0);

        for (uint32_t j = 0; j < 3; j++) {
            vertex->m_pos = patter.m_pos;
            corner.a = j;

            if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
                CImVector swizzled(0);
                swizzled.a = j;
                vertex->m_corner = swizzled;
            } else {
                vertex->m_corner = corner;
            }

            vertex->m_startTime = patter.m_startTime;
            vertex->m_endTime = patter.m_endTime;
            vertex++;
        }
    }

    CGxBuf* buf = packet->m_buf;
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x007497B0/0x140428E60
void Rain::GetMotionBlurMatrix(C33Matrix& matrix) const {
    const C3Vector& velocity = this->m_weather->m_velocity;
    float vx = velocity.x;
    float vy = velocity.y;
    float speedSq = vy * vy + vx * vx;
    float t = sqrt(speedSq) / s_motionBlurMaxSpeed;
    t = t < 0.0f ? 0.0f : t >= 1.0f ? 1.0f : t;
    float angle = s_motionBlurMaxAngle * t;

    if (fabs(speedSq) < 0.001f) {
        return;
    }

    float inv = 1.0f / sqrt(speedSq);
    C3Vector dir(vx * inv, vy * inv, inv * 0.0f);
    C3Vector axis(dir.y - dir.z * 0.0f, dir.z * 0.0f - dir.x, dir.x * 0.0f - dir.y * 0.0f);

    if (fabs(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z) < 0.001f) {
        return;
    }

    float scale = 1.0f / sqrt(axis.y * axis.y + axis.x * axis.x + axis.z * axis.z);
    axis.x = axis.x * scale;
    axis.y = axis.y * scale;
    axis.z = scale * axis.z;
    matrix.Rotate(angle * 0.017453292f, axis, true);
}

// OFFSET: 0x00749920
bool Rain::InitDrop(Drop& drop, float time) {
    C3Vector size(this->m_bounds.t.x - this->m_bounds.b.x, this->m_bounds.t.y - this->m_bounds.b.y, this->m_bounds.t.z - this->m_bounds.b.z);
    const C3Vector& camPos = CWorldScene::s_defaultWorldView->m_camPos;
    float ry = CRandom::ufloat_(g_rndSeed);
    float rx = CRandom::ufloat_(g_rndSeed);
    C3Vector offset((rx - 0.5f) * size.x, (ry - 0.5f) * size.y, size.z * 0.0f);
    float spread = this->m_intensity * 0.20943952f + 0.052359879f;
    float angle = (CRandom::ufloat_(g_rndSeed) - 0.5f) * spread - 1.5700001f;
    float c = cos(angle);
    float s = sin(angle);
    float baseSpeed = this->m_intensity * 9.4899998f + 0.0099999998f;
    float speed = (CRandom::ufloat_(g_rndSeed) - 0.5f) * this->m_intensity * 2.0f + baseSpeed;
    float fall = -28.0f - this->m_intensity * 4.0f;
    float randomFall = CRandom::ufloat_(g_rndSeed);
    drop.m_velocity.x = speed * s;
    drop.m_velocity.y = speed * c;
    drop.m_velocity.z = randomFall * this->m_intensity * -2.0f + fall;
    float t = -(size.z * 0.5f / drop.m_velocity.z);
    drop.m_pos.x = offset.x - drop.m_velocity.x * t;
    drop.m_pos.y = offset.y - drop.m_velocity.y * t;
    drop.m_pos.z = offset.z - drop.m_velocity.z * t;
    C3Vector axis;
    CMath::sincos_(this->m_weather->m_facing - 1.5707964f, &axis.y, &axis.x);
    float tilt = this->m_weather->m_speed * 0.055555556f;

    if (tilt < 0.0f || tilt >= 1.0f) {
        tilt = tilt < 0.0f ? 0.0f : 1.0f;
    }

    C44Matrix rotation = C44Matrix::Rotation(tilt * 1.134464f, axis, false);
    drop.m_pos = rotation * drop.m_pos;
    const C3Vector& wind = this->m_weather->m_velocity;
    drop.m_pos.x = drop.m_pos.x + wind.x * s_windLead;
    drop.m_pos.y = wind.y * s_windLead + drop.m_pos.y;
    drop.m_pos.x = drop.m_pos.x + camPos.x;
    drop.m_pos.y = camPos.y + drop.m_pos.y;
    drop.m_pos.z = camPos.z + drop.m_pos.z;
    C3Vector end(drop.m_pos.x + t * drop.m_velocity.x, drop.m_velocity.y * t + drop.m_pos.y, drop.m_velocity.z * t + drop.m_pos.z);

    if (this->m_weather->m_hmMgr->Query(drop.m_pos) >= drop.m_pos.z) {
        return false;
    }

    int32_t onTaxi = this->m_weather->m_onTaxi;
    float scale = onTaxi ? 1.0f : 2.0f;
    C3Vector farPos(t * drop.m_velocity.x * scale + end.x, drop.m_velocity.y * t * scale + end.y, drop.m_velocity.z * t * scale + end.z);
    C3Vector hit(0.0f, 0.0f, 0.0f);

    if (onTaxi) {
        hit = farPos;
    } else if (!this->m_weather->m_hmMgr->Query(drop.m_pos, farPos, hit)) {
        return false;
    }

    float inv = 1.0f / sqrt(drop.m_velocity.z * drop.m_velocity.z + drop.m_velocity.y * drop.m_velocity.y + drop.m_velocity.x * drop.m_velocity.x);
    float dx = drop.m_pos.x - hit.x;
    float dz = drop.m_pos.z - hit.z;
    float dy = drop.m_pos.y - hit.y;
    float distance = sqrt(dy * dy + (dz * dz + dx * dx)) * inv;
    drop.m_pos.x = hit.x - drop.m_velocity.x * distance;
    drop.m_pos.y = hit.y - drop.m_velocity.y * distance;
    drop.m_pos.z = hit.z - drop.m_velocity.z * distance;
    drop.m_startTime = time;
    float ex = drop.m_pos.x - hit.x;
    float ez = drop.m_pos.z - hit.z;
    float ey = drop.m_pos.y - hit.y;
    drop.m_endTime = inv * sqrt(ez * ez + ex * ex + ey * ey) + time;
    return true;
}

// OFFSET: 0x00749F50/0x1404296C0
void Rain::UpdatePacket(uint32_t count) {
    PatterPacket* patters = this->m_patters.m_current;
    DropPacket* drops = this->m_drops.m_current;
    float tick = FrameTime::GetTickTimeSec();
    uint32_t available = 1024 - drops->m_count;

    if (available >= count) {
        available = count;
    }

    for (int32_t i = available; i > 0; i--) {
        float r = CRandom::ufloat_(g_rndSeed);
        Drop& drop = drops->m_buffer.m_particles[drops->m_count];
        drops->m_count++;

        if (!this->InitDrop(drop, r * tick + drops->m_buildTime)) {
            drops->m_count--;
            continue;
        }

        uint32_t lastTime = drops->m_baseTime + static_cast<uint32_t>((-2.0f / drop.m_velocity.z + drop.m_endTime) * 1000.0f);

        if (lastTime <= drops->m_lastTime) {
            lastTime = drops->m_lastTime;
        }

        drops->m_lastTime = lastTime;

        if (this->m_weather->m_onTaxi) {
            continue;
        }

        float fall = drop.m_endTime - drop.m_startTime;
        Patter& patter = patters->m_buffer.m_particles[patters->m_count];
        patters->m_count++;
        patter.m_pos.x = drop.m_pos.x + drop.m_velocity.x * fall;
        patter.m_endTime = drop.m_endTime + 0.25f;
        patter.m_pos.y = drop.m_pos.y + drop.m_velocity.y * fall;
        patter.m_startTime = drop.m_endTime;
        patter.m_pos.z = drop.m_pos.z + drop.m_velocity.z * fall;
        uint32_t patterTime = patters->m_baseTime + static_cast<uint32_t>(1000.0f * patter.m_endTime);

        if (patterTime <= patters->m_lastTime) {
            patterTime = patters->m_lastTime;
        }

        patters->m_lastTime = patterTime;
    }

    drops->m_buildTime = tick + drops->m_buildTime;
}

// OFFSET: 0x0074B770
uint32_t Rain::NumActive() {
    uint32_t active = 0;
    uint32_t now = FrameTime::GetCurTimeMs();

    for (DropPacket* packet = this->m_drops.m_active.Head(); packet; packet = this->m_drops.m_active.Next(packet)) {
        if (now < packet->m_lastTime) {
            active++;
        }
    }

    return this->m_mists.NumActive() + active;
}

// OFFSET: 0x0074BFE0/0x14042BE20
void Rain::RenderPatterPacketsSW() {
    if (!this->m_patters.m_active.Head()) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(this->m_patterTexture);

    if (!tex) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Mod2x);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    GxRsSet(GxRs_MatDiffuse, *reinterpret_cast<const int32_t*>(&this->m_weather->m_color));
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 1);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_FogStart, 70.0f);
    GxRsSet(GxRs_FogEnd, 75.0f);
    GxRsSet(GxRs_FogColor, static_cast<int32_t>(0x80808080));
    g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    C44Matrix view;
    GxXformView(view);
    C44Matrix world;
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    C3Vector up(view.a1 * 0.16666667f, view.b1 * 0.16666667f, view.c1 * 0.16666667f);
    C3Vector right(view.a0 * 0.083333336f, view.b0 * 0.083333336f, view.c0 * 0.083333336f);
    C3Vector left = -right;
    C3Vector camPos = World::GetCamPos();
    uint32_t now = FrameTime::GetCurTimeMs();

    for (PatterPacket* packet = this->m_patters.m_active.Head(), * next; packet; packet = next) {
        next = this->m_patters.m_active.Next(packet);

        if (packet->m_baseTime >= now) {
            continue;
        }

        float age = static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001);
        CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(PTVertex), 3 * packet->m_count);
        PTVertex* vertex = reinterpret_cast<PTVertex*>(g_theGxDevicePtr->BufLock(buf));
        uint32_t vertexCount = 0;

        for (uint32_t i = 0; i < packet->m_count; i++) {
            const Patter& patter = packet->m_buffer.m_particles[i];

            if (!(patter.m_startTime <= age && age <= patter.m_endTime)) {
                continue;
            }

            float dx = patter.m_pos.x - camPos.x;
            float dy = patter.m_pos.y - camPos.y;
            float dz = patter.m_pos.z - camPos.z;
            float length = sqrt(dz * dz + dy * dy + dx * dx);
            float facing = (-(dy * (1.0f / length)) - dx * (1.0f / length)) * 0.0f + -(dz * (1.0f / length));

            if (facing <= 0.0f) {
                facing = 0.0f;
            }

            C3Vector center;
            center.y = dy - up.y * (facing * 0.5f);
            center.x = dx - facing * 0.5f * up.x;
            center.z = dz - up.z * (facing * 0.5f) + facing * 0.083333336f;
            float u = static_cast<float>(floor((age - patter.m_startTime) * 4.0 * 3.99));
            float v = static_cast<float>(floor((1.0 - facing) * 3.99));
            float row = v * 0.25f;
            float column = u * 0.25f;
            vertex[0].p.x = center.x + left.x;
            vertex[0].p.y = center.y + left.y;
            vertex[0].p.z = left.z + center.z;
            vertex[0].tc.x = column;
            vertex[0].tc.y = row + 0.25f;
            vertex[1].p.x = center.x + up.x;
            vertex[1].p.y = center.y + up.y;
            vertex[1].p.z = up.z + center.z;
            vertex[1].tc.y = row + 0.04296875f;
            vertex[1].tc.x = column + 0.125f;
            vertex[2].p.x = center.x + right.x;
            vertex[2].p.y = center.y + right.y;
            vertex[2].p.z = right.z + center.z;
            vertex[2].tc.x = column + 0.25f;
            vertex[2].tc.y = row + 0.25f;
            vertex += 3;
            vertexCount += 3;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;

        if (vertexCount) {
            g_theGxDevicePtr->PrimVertexPtr(buf, static_cast<EGxVertexBufferFormat>(13));
            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = 0;
            batch.m_count = vertexCount;
            batch.m_minIndex = 0;
            batch.m_maxIndex = vertexCount - 1;
            g_theGxDevicePtr->DrawNonIndexed(batch);
        }
    }

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0074C900/0x14042C880
void Rain::RenderDropPacketsSW() {
    if (!this->m_drops.m_active.Head()) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(this->m_dropTexture);

    if (!tex) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Mod2x);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    GxRsSet(GxRs_MatDiffuse, *reinterpret_cast<const int32_t*>(&this->m_weather->m_color));
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 1);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_FogStart, 70.0f);
    GxRsSet(GxRs_FogEnd, 75.0f);
    GxRsSet(GxRs_FogColor, static_cast<int32_t>(0x80808080));
    g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    C3Vector camPos = World::GetCamPos();
    C44Matrix world;
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    uint32_t now = FrameTime::GetCurTimeMs();
    C33Matrix blur;
    this->GetMotionBlurMatrix(blur);
    float stretch = this->GetDropStretch();

    for (DropPacket* packet = this->m_drops.m_active.Head(), * next; packet; packet = next) {
        next = this->m_drops.m_active.Next(packet);

        if (packet->m_baseTime >= now) {
            continue;
        }

        float age = static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001);
        CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(PTVertex), 3 * packet->m_count);
        PTVertex* vertex = reinterpret_cast<PTVertex*>(g_theGxDevicePtr->BufLock(buf));
        uint32_t vertexCount = 0;

        for (uint32_t i = 0; i < packet->m_count; i++) {
            const Drop& drop = packet->m_buffer.m_particles[i];
            float vz = drop.m_velocity.z;
            float start = drop.m_startTime;
            float end = drop.m_endTime;
            C3Vector rel(drop.m_pos.x - camPos.x, drop.m_pos.y - camPos.y, drop.m_pos.z - camPos.z);
            C2Vector uv0(0.0f, 1.0f);
            C2Vector uv1(1.0f, 1.0f);
            C2Vector uv2(0.5f, 0.0f);
            float tailEnd = end - stretch / vz;

            if (!(start <= age && age <= tailEnd)) {
                continue;
            }

            if (end > age) {
                end = age;
            }

            float headTime = end - start;

            if (tailEnd > age) {
                tailEnd = age;
            }

            float tailTime = tailEnd - start;
            C3Vector head(drop.m_velocity.x * headTime + rel.x, drop.m_velocity.y * headTime + rel.y, vz * headTime + rel.z);
            C3Vector tail(drop.m_velocity.x * tailTime + rel.x, drop.m_velocity.y * tailTime + rel.y, vz * tailTime + rel.z);
            float scale = 1.0f / sqrt(-head.z * -head.z + -head.y * -head.y + -head.x * -head.x);
            C3Vector toCam(-head.x * scale, -head.y * scale, -head.z * scale);
            scale = 1.0f / sqrt(-vz * -vz + -drop.m_velocity.y * -drop.m_velocity.y + -drop.m_velocity.x * -drop.m_velocity.x);
            C3Vector dir(-drop.m_velocity.x * scale, -drop.m_velocity.y * scale, -vz * scale);
            C3Vector side(toCam.y * dir.z - toCam.z * dir.y, toCam.z * dir.x - toCam.x * dir.z, toCam.x * dir.y - toCam.y * dir.x);
            scale = 1.0f / sqrt(side.z * side.z + side.y * side.y + side.x * side.x);
            side.z = side.z * scale;
            side.x = side.x * scale;
            side.y = side.y * scale;
            vertex[0].p.x = side.x * -0.050000001f + head.x;
            vertex[0].p.y = side.y * -0.050000001f + head.y;
            vertex[0].p.z = side.z * -0.050000001f + head.z;
            vertex[0].tc = uv0;
            vertex[1].p.x = side.x * 0.050000001f + head.x;
            vertex[1].p.y = side.y * 0.050000001f + head.y;
            vertex[1].p.z = side.z * 0.050000001f + head.z;
            vertex[1].tc = uv1;
            float bx = dir.x * blur.a0 + dir.z * blur.a2 + dir.y * blur.a1;
            float by = dir.z * blur.b2 + dir.y * blur.b1 + blur.b0 * dir.x;
            float bz = dir.z * blur.c2 + dir.y * blur.c1 + blur.c0 * dir.x;
            vertex[2].p.x = bx * 2.0f + tail.x;
            vertex[2].p.y = by * 2.0f + tail.y;
            vertex[2].p.z = bz * 2.0f + tail.z;
            vertex[2].tc = uv2;
            vertex += 3;
            vertexCount += 3;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;

        if (vertexCount) {
            g_theGxDevicePtr->PrimVertexPtr(buf, static_cast<EGxVertexBufferFormat>(13));
            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = 0;
            batch.m_count = vertexCount;
            batch.m_minIndex = 0;
            batch.m_maxIndex = vertexCount - 1;
            g_theGxDevicePtr->DrawNonIndexed(batch);
        }
    }

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0074D2E0/0x14042D320
void Rain::RenderPatterPackets() {
    if (!this->m_patters.m_active.Head()) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(this->m_patterTexture);

    if (!tex) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Mod2x);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);
    C4Vector fogParams(CShaderEffect::s_fogMul * -0.2f, 15.0f, 1.0f, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, &fogParams.x, 1);
    C4Vector color(this->m_weather->m_color);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 7, &color.x, 1);
    GxRsSet(GxRs_FogColor, static_cast<int32_t>(0x80808080));
    g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_patterShader->m_permutes[0]);
    C44Matrix view;
    GxXformView(view);
    C44Matrix proj;
    proj = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        proj.c0 = proj.c0 * -1.0f;
        proj.c1 = proj.c1 * -1.0f;
        proj.c2 = proj.c2 * -1.0f;
        proj.c3 = proj.c3 * -1.0f;
    }

    const C3Vector& camPos = CWorldScene::s_defaultWorldView->m_camPos;
    C4Vector eye(camPos.x, camPos.y, camPos.z, 1.0f);
    C44Matrix viewProj = (view * proj).Transpose();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, &eye.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 2, &viewProj.a0, 4);
    C4Vector right(view.a0 * 0.083333336f, view.b0 * 0.083333336f, view.c0 * 0.083333336f, 0.0f);
    C4Vector up(view.a1 * 0.16666667f, view.b1 * 0.16666667f, view.c1 * 0.16666667f, 0.0f);
    C4Vector left(-right.x, -right.y, -right.z, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 8, &left.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 9, &up.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 10, &right.x, 1);
    C4Vector uv0(0.0f, 0.25f, 0.0f, 0.0f);
    C4Vector uv1(0.125f, 0.04296875f, 0.0f, 0.0f);
    C4Vector uv2(0.25f, 0.25f, 0.0f, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 11, &uv0.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 12, &uv1.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 13, &uv2.x, 1);
    uint32_t now = FrameTime::GetCurTimeMs();
    DefaultShaders::SetFogEnabled(1);
    DefaultShaders::SetPixelShader(static_cast<EGxVertexBufferFormat>(9), -1);

    for (PatterPacket* packet = this->m_patters.m_active.Head(), * next; packet; packet = next) {
        next = this->m_patters.m_active.Next(packet);

        if (packet->m_baseTime >= now || !packet->m_count) {
            continue;
        }

        C4Vector time(static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001), 0.0f, 0.0f, 0.0f);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 6, &time.x, 1);
        g_theGxDevicePtr->PrimVertexPtr(packet->m_buf, static_cast<EGxVertexBufferFormat>(9));
        CGxBatch batch;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = 3 * packet->m_count;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 0;
        g_theGxDevicePtr->DrawNonIndexed(batch);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0074DA30/0x14042DC20
void Rain::RenderDropPackets() {
    if (!this->m_drops.m_active.Head()) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(this->m_dropTexture);

    if (!tex) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Mod2x);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);
    C4Vector fogParams(CShaderEffect::s_fogMul * -0.2f, 15.0f, 1.0f, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, &fogParams.x, 1);
    C4Vector color(this->m_weather->m_color);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 7, &color.x, 1);
    GxRsSet(GxRs_FogColor, static_cast<int32_t>(0x80808080));
    g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_dropShader->m_permutes[0]);
    C44Matrix view;
    GxXformView(view);
    C44Matrix proj;
    proj = g_theGxDevicePtr->m_projNative;

    if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
        proj.c0 = proj.c0 * -1.0f;
        proj.c1 = proj.c1 * -1.0f;
        proj.c2 = proj.c2 * -1.0f;
        proj.c3 = proj.c3 * -1.0f;
    }

    const C3Vector& camPos = CWorldScene::s_defaultWorldView->m_camPos;
    C4Vector eye(camPos.x, camPos.y, camPos.z, 1.0f);
    C44Matrix viewProj = (view * proj).Transpose();
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 1, &eye.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 2, &viewProj.a0, 4);
    C33Matrix blur;
    this->GetMotionBlurMatrix(blur);
    C4Vector blurX(blur.a0, blur.b0, blur.c0, 0.0f);
    C4Vector blurY(blur.a1, blur.b1, blur.c1, 0.0f);
    C4Vector blurZ(blur.a2, blur.b2, blur.c2, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 8, &blurX.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 9, &blurY.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 10, &blurZ.x, 1);
    C4Vector stretch(this->GetDropStretch(), 0.0f, 0.0f, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 11, &stretch.x, 1);
    C4Vector uv0(0.0f, 1.0f, 0.0f, 0.0f);
    C4Vector uv1(1.0f, 1.0f, 0.0f, 0.0f);
    C4Vector uv2(0.5f, 0.0f, 0.0f, 0.0f);
    C4Vector side0(-0.050000001f, -0.050000001f, -0.050000001f, 0.0f);
    C4Vector side1(0.050000001f, 0.050000001f, 0.050000001f, 0.0f);
    C4Vector zero0(0.0f, 0.0f, 0.0f, 0.0f);
    C4Vector zero1(0.0f, 0.0f, 0.0f, 0.0f);
    C4Vector zero2(0.0f, 0.0f, 0.0f, 0.0f);
    C4Vector tail(2.0f, 2.0f, 2.0f, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 12, &uv0.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 13, &uv1.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 14, &uv2.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 15, &side0.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 16, &side1.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 17, &zero0.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 18, &zero1.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 19, &zero2.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 20, &tail.x, 1);
    uint32_t now = FrameTime::GetCurTimeMs();
    DefaultShaders::SetFogEnabled(1);
    DefaultShaders::SetPixelShader(static_cast<EGxVertexBufferFormat>(4), -1);

    for (DropPacket* packet = this->m_drops.m_active.Head(), * next; packet; packet = next) {
        next = this->m_drops.m_active.Next(packet);

        if (packet->m_baseTime >= now || !packet->m_count) {
            continue;
        }

        C4Vector time(static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001), 0.0f, 0.0f, 0.0f);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 6, &time.x, 1);
        g_theGxDevicePtr->PrimVertexPtr(packet->m_buf, static_cast<EGxVertexBufferFormat>(4));
        CGxBatch batch;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = 3 * packet->m_count;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 0;
        g_theGxDevicePtr->DrawNonIndexed(batch);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00751DE0/0x140432CF0
Rain::~Rain() {
    TextureRelease(this->m_dropTexture);
    TextureRelease(this->m_patterTexture);

    if (this->m_dropShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_dropShader);
        this->m_dropShader = NULL;
    }

    if (this->m_patterShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_patterShader);
        this->m_patterShader = NULL;
    }
}

// OFFSET: 0x00751E80/0x140432DA0
void Rain::UpdatePackets() {
    if (this->m_fadeOut) {
        this->m_drops.m_current = NULL;
        this->m_patters.m_current = NULL;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    for (DropPacket* packet = this->m_drops.m_active.Head(), * next; packet; packet = next) {
        next = this->m_drops.m_active.Next(packet);

        if (now > packet->m_lastTime) {
            this->m_drops.m_activeCount--;
            this->m_drops.m_freeCount++;
            this->m_drops.m_free.LinkToTail(packet);
            continue;
        }

        if (this->m_fadeOut && this->m_weather->m_fogIntensity <= 0.25f && now <= packet->m_baseTime) {
            this->m_drops.m_activeCount--;
            this->m_drops.m_freeCount++;
            this->m_drops.m_free.LinkToTail(packet);
            continue;
        }

        if (packet->m_lastTime - packet->m_baseTime >= 60000) {
            this->m_drops.m_activeCount--;
            this->m_drops.m_freeCount++;
            this->m_drops.m_free.LinkToTail(packet);
            continue;
        }

        if (!packet->m_buf->unk_001C || !packet->m_buf->unk_001D) {
            this->RealizePacket(packet);
        }
    }

    for (PatterPacket* packet = this->m_patters.m_active.Head(), * next; packet; packet = next) {
        next = this->m_patters.m_active.Next(packet);

        if (now > packet->m_lastTime) {
            this->m_patters.m_activeCount--;
            this->m_patters.m_freeCount++;
            this->m_patters.m_free.LinkToTail(packet);
            continue;
        }

        if (!packet->m_buf->unk_001C || !packet->m_buf->unk_001D) {
            this->RealizePacket(packet);
        }
    }

    if (this->m_fadeOut) {
        return;
    }

    float tick = FrameTime::GetTickTimeSec() >= 0.016666999f ? 0.016666999f : FrameTime::GetTickTimeSec();
    uint32_t count = static_cast<uint32_t>(tick * this->m_particlesPerSecond);

    if (!count) {
        return;
    }

    if (!this->m_drops.m_current) {
        this->m_drops.Alloc();
        this->m_drops.m_current->m_baseTime = now + static_cast<uint32_t>(1024.0f / this->m_particlesPerSecond * 1000.0f);
        this->m_patters.Alloc();
        this->m_patters.m_current->m_baseTime = this->m_drops.m_current->m_baseTime;
    }

    this->UpdatePacket(count);
    DropPacket* drops = this->m_drops.m_current;

    if (drops->m_count >= 1024 || drops->m_buildTime >= this->m_maxBuildTime || now > drops->m_baseTime) {
        this->RealizePacket(this->m_drops.m_current);
        this->m_drops.m_active.LinkToTail(this->m_drops.m_current);
        this->m_drops.m_activeCount++;
        this->m_drops.m_freeCount--;
        this->m_drops.m_current = NULL;
        this->RealizePacket(this->m_patters.m_current);
        this->m_patters.m_active.LinkToTail(this->m_patters.m_current);
        this->m_patters.m_activeCount++;
        this->m_patters.m_freeCount--;
        this->m_patters.m_current = NULL;
    }
}

// PROVEN: bf9f2627 IDENTICAL/UNPROVEN
// OFFSET: 0x007522E0/0x140433270
void Rain::Render() {
    this->UpdatePackets();

    if (CWorldScene::s_defaultWorldView->m_exteriorPortalView.m_maxViewDepth < 0.0f) {
        return;
    }

    if (this->m_useShaders) {
        this->RenderDropPackets();
        this->RenderPatterPackets();
    } else {
        this->RenderDropPacketsSW();
        this->RenderPatterPacketsSW();
    }

    this->m_mists.Render();
}

// OFFSET: 0x00752630/0x140433570
Rain::Rain(Weather* weather, float width, float height) : m_mists(weather, 12.0f, C3Vector(44.0f, 44.0f, 25.0f), "textures\\Weather\\SnowMist01.blp", -1.5700001f, 0.34906584f, 5.0f, 1.2f, 128) {
    this->m_weather = weather;
    this->m_useShaders = 1;
    this->m_fadeOut = 0;
    this->m_maxBuildTime = 0.0f;
    this->m_particlesPerSecond = 0.0f;
    this->m_dropTexture = NULL;
    this->m_dropShader = NULL;
    this->m_patterTexture = NULL;
    this->m_patterShader = NULL;
    this->m_intensity = 0.0f;
    float count = this->m_useShaders ? 35000.0f : 6500.0f;
    float density = Weather::s_densityScale * 0.0f;
    this->m_particlesPerSecond = density * count;
    this->m_maxBuildTime = count * 0.0009765625f;
    float mists = this->m_useShaders ? 38.0f : 18.0f;
    this->m_mists.m_spawnRate = density * mists;
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    this->m_bounds.b = C3Vector(-halfWidth, -halfWidth, -halfHeight);
    this->m_bounds.t = C3Vector(halfWidth, halfWidth, halfHeight);
    this->m_dropShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "rain");
    CStatus status;
    CGxTexFlags flags;
    flags.m_filter = 4;
    this->m_dropTexture = TextureCreate(this->m_weather->m_precipTexture, flags, &status, 1, NULL, 2);
    this->m_patterShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "patter");
    this->m_patterTexture = TextureCreate("textures\\Weather\\RainDropSplash01.blp", flags, &status, 1, NULL, 2);

    if (!this->m_dropShader || !this->m_dropShader->m_permutes[0].Valid() || !this->m_patterShader || !this->m_patterShader->m_permutes[0].Valid()) {
        this->m_useShaders = 0;
    }

    if (g_theGxDevicePtr->m_caps.m_shaderProfiles[1] && !Weather::s_cvarUseWeatherShaders->m_intValue) {
        this->m_useShaders = 0;
    }
}
