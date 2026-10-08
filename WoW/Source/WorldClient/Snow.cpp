#include "WoW/Source/WorldClient/Snow.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/C3Spline.h"
#include "Engine/Source/Tempest/C3Spline_CatmullRom.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x00748830/0x140427A30
void Snow::RealizePacket(FlakePacket* packet) {
    Flake* vertex = reinterpret_cast<Flake*>(g_theGxDevicePtr->BufLock(packet->m_buf));

    for (uint32_t i = 0; i < packet->m_count; i++) {
        vertex[i] = packet->m_buffer.m_particles[i];
    }

    CGxBuf* buf = packet->m_buf;
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x0074A140
void Snow::InitFlake(Flake& flake, float time, const C3Vector& origin, const C3Vector& wind) {
    C3Vector size(this->m_bounds.t.x - this->m_bounds.b.x, this->m_bounds.t.y - this->m_bounds.b.y, this->m_bounds.t.z - this->m_bounds.b.z);
    float ry = CRandom::ufloat_(g_rndSeed);
    float rx = CRandom::ufloat_(g_rndSeed);
    C3Vector offset((rx - 0.5f) * size.x, (ry - 0.5f) * size.y, size.z * 0.0f);
    float angle = (6.2831855f - this->m_intensity * 5.9341197f) * (CRandom::ufloat_(g_rndSeed) - 0.5f) - 1.5700001f;
    float c = cos(angle);
    float s = sin(angle);
    float baseSpeed = this->m_intensity * 5.9850001f + 0.015f;
    float speed = (CRandom::ufloat_(g_rndSeed) - 0.5f) * this->m_intensity + baseSpeed;
    float fall = -2.0f - this->m_intensity * 3.5f;
    float randomFall = CRandom::ufloat_(g_rndSeed);
    flake.m_velocity.x = speed * s;
    flake.m_velocity.y = speed * c;
    flake.m_velocity.z = randomFall * this->m_intensity * -1.0f + fall;
    float t = -(size.z * 0.5f / flake.m_velocity.z);
    flake.m_pos.x = offset.x - flake.m_velocity.x * t;
    flake.m_pos.y = offset.y - flake.m_velocity.y * t;
    flake.m_pos.z = offset.z - flake.m_velocity.z * t;
    Weather* weather = this->m_weather;
    C3Vector axis;
    CMath::sincos_(weather->m_facing - 1.5707964f, &axis.y, &axis.x);
    float tilt = weather->m_speed * 0.055555556f;

    if (tilt < 0.0f || tilt >= 1.0f) {
        tilt = tilt < 0.0f ? 0.0f : 1.0f;
    }

    C44Matrix rotation = C44Matrix::Rotation(tilt * 1.134464f, axis, false);
    flake.m_pos = rotation * flake.m_pos;
    flake.m_pos.x = flake.m_pos.x + wind.x * 1.75f;
    flake.m_pos.y = flake.m_pos.y + wind.y * 1.75f;
    flake.m_pos.z = flake.m_pos.z + wind.z * 1.75f;
    flake.m_pos.x = flake.m_pos.x + origin.x;
    flake.m_pos.y = origin.y + flake.m_pos.y;
    flake.m_pos.z = origin.z + flake.m_pos.z;
    C3Vector end(flake.m_pos.x + flake.m_velocity.x * t, flake.m_pos.y + flake.m_velocity.y * t, flake.m_velocity.z * t + flake.m_pos.z);

    if (this->m_weather->m_hmMgr->Query(flake.m_pos) < flake.m_pos.z) {
        int32_t onTaxi = this->m_weather->m_onTaxi;
        float scale = onTaxi ? 1.0f : 2.0f;
        C3Vector farPos(flake.m_velocity.x * t * scale + end.x, flake.m_velocity.y * t * scale + end.y, flake.m_velocity.z * t * scale + end.z);
        C3Vector hit(0.0f, 0.0f, 0.0f);

        if (onTaxi || this->m_weather->m_hmMgr->Query(flake.m_pos, farPos, hit)) {
            if (onTaxi) {
                hit = farPos;
            }

            float inv = 1.0f / sqrt(flake.m_velocity.z * flake.m_velocity.z + flake.m_velocity.y * flake.m_velocity.y + flake.m_velocity.x * flake.m_velocity.x);
            float dx = flake.m_pos.x - hit.x;
            float dz = flake.m_pos.z - hit.z;
            float dy = flake.m_pos.y - hit.y;
            float distance = sqrt(dy * dy + (dz * dz + dx * dx)) * inv;
            flake.m_pos.x = hit.x - flake.m_velocity.x * distance;
            flake.m_pos.y = hit.y - flake.m_velocity.y * distance;
            flake.m_pos.z = hit.z - flake.m_velocity.z * distance;
            flake.m_startTime = time;
            float ex = flake.m_pos.x - hit.x;
            float ez = flake.m_pos.z - hit.z;
            float ey = flake.m_pos.y - hit.y;
            flake.m_endTime = inv * sqrt(ez * ez + ex * ex + ey * ey) + time;
            return;
        }
    }

    flake.m_startTime = 0.0f;
    flake.m_endTime = -0.25f;
}

// OFFSET: 0x0074A770
void Snow::UpdatePacket(uint32_t count, const C3Vector& origin, const C3Vector& wind) {
    FlakePacket* packet = this->m_flakes.m_current;
    float tick = FrameTime::GetTickTimeSec();
    uint32_t available = 6144 - packet->m_count;

    if (available >= count) {
        available = count;
    }

    for (int32_t i = available; i > 0; i--) {
        float r = CRandom::ufloat_(g_rndSeed);
        Flake& flake = packet->m_buffer.m_particles[packet->m_count];
        this->InitFlake(flake, r * tick + packet->m_buildTime, origin, wind);
        float end = flake.m_endTime <= 0.0f ? 0.0f : flake.m_endTime;
        uint32_t lastTime = packet->m_baseTime + static_cast<uint32_t>(end * 1000.0f + 250.0f);

        if (lastTime <= packet->m_lastTime) {
            lastTime = packet->m_lastTime;
        }

        packet->m_lastTime = lastTime;
        packet->m_count++;
    }

    packet->m_buildTime = tick + packet->m_buildTime;
}

// OFFSET: 0x0074B7F0
uint32_t Snow::NumActive() {
    uint32_t active = 0;
    uint32_t now = FrameTime::GetCurTimeMs();

    for (FlakePacket* packet = this->m_flakes.m_active.Head(); packet; packet = this->m_flakes.m_active.Next(packet)) {
        if (now < packet->m_lastTime) {
            active++;
        }
    }

    return this->m_mists.NumActive() + active;
}

// OFFSET: 0x0074E380/0x14042E710
void Snow::UpdatePackets() {
    if (this->m_fadeOut) {
        this->m_flakes.m_current = NULL;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    for (FlakePacket* packet = this->m_flakes.m_active.Head(), * next; packet; packet = next) {
        next = this->m_flakes.m_active.Next(packet);

        if (now > packet->m_lastTime) {
            this->m_flakes.m_activeCount--;
            this->m_flakes.m_freeCount++;
            this->m_flakes.m_free.LinkToTail(packet);
            continue;
        }

        if (this->m_fadeOut && this->m_weather->m_fogIntensity <= 0.25f && now <= packet->m_baseTime) {
            this->m_flakes.m_activeCount--;
            this->m_flakes.m_freeCount++;
            this->m_flakes.m_free.LinkToTail(packet);
            continue;
        }

        const C3Vector& camPos = World::GetCamPos();
        float dz = packet->m_origin.z - camPos.z;
        float dy = packet->m_origin.y - camPos.y;
        float dx = packet->m_origin.x - camPos.x;

        if (sqrt(dx * dx + (dy * dy + dz * dz)) > 200.0f) {
            this->m_flakes.m_activeCount--;
            this->m_flakes.m_freeCount++;
            this->m_flakes.m_free.LinkToTail(packet);
        }

        if (packet->m_lastTime - packet->m_baseTime >= 60000) {
            this->m_flakes.m_activeCount--;
            this->m_flakes.m_freeCount++;
            this->m_flakes.m_free.LinkToTail(packet);
            continue;
        }

        if (!packet->m_buf->unk_001C || !packet->m_buf->unk_001D) {
            this->RealizePacket(packet);
        }
    }

    if (this->m_fadeOut) {
        return;
    }

    C3Vector origin = CWorldScene::s_defaultWorldView->m_camPos;
    Weather* weather = this->m_weather;
    C3Vector wind = weather->m_velocity;

    if (weather->m_onTaxi) {
        const C3Spline_CatmullRom* spline = World::GetWeatherSpline();

        if (spline) {
            float elapsed = World::GetWeatherSplineElapsedTime();
            float duration = World::GetWeatherSplineDuration();

            if (duration > 0.0f) {
                float speed = this->m_weather->m_speed;
                float lead = 0.0f;

                if (fabs(speed) >= 0.001f) {
                    const C3Vector& camPos = CWorldScene::s_defaultWorldView->m_camPos;
                    const C3Vector& position = this->m_weather->m_position;
                    lead = sqrt((camPos.x - position.x) * (camPos.x - position.x) + (camPos.y - position.y) * (camPos.y - position.y) + (camPos.z - position.z) * (camPos.z - position.z)) / speed;
                }

                float t = (elapsed - lead + 2350.0f) / duration;
                t = t < 0.0f ? 0.0f : t >= 1.0f ? 1.0f : t;
                C3Vector velocity(0.0f, 0.0f, 0.0f);
                const_cast<C3Spline_CatmullRom*>(spline)->Vel(t, velocity, static_cast<C3Spline::EvalType>(1));
                float lengthSq = velocity.y * velocity.y + velocity.x * velocity.x + velocity.z * velocity.z;

                if (lengthSq > 0.00000023841858f) {
                    float inv = 1.0f / sqrt(lengthSq);
                    velocity.x = velocity.x * inv;
                    velocity.y = velocity.y * inv;
                    velocity.z = inv * velocity.z;
                }

                velocity.x = velocity.x * speed;
                velocity.y = velocity.y * speed;
                velocity.z = velocity.z * speed;
                wind = velocity;

                if (velocity.z < -10.0f) {
                    static float s_expMin = static_cast<float>(CMath::exp2_(1.4426950216293335));
                    static float s_expMax = static_cast<float>(CMath::exp2_(3.2460637986660004));
                    float e = static_cast<float>(CMath::exp2_(velocity.z * -0.1f * 1.442695f));
                    float drop = (e - s_expMin) / (s_expMax - s_expMin);
                    drop = drop < 0.0f ? 0.0f : drop >= 1.0f ? 1.0f : drop;
                    origin.z = origin.z - (this->m_bounds.t.z - this->m_bounds.b.z) * drop;
                }
            }
        }
    }

    float tick = FrameTime::GetTickTimeSec() >= 0.016666999f ? 0.016666999f : FrameTime::GetTickTimeSec();
    uint32_t count = static_cast<uint32_t>(tick * this->m_particlesPerSecond);

    if (!count) {
        return;
    }

    if (!this->m_flakes.m_current) {
        this->m_flakes.Alloc();
        this->m_flakes.m_current->m_baseTime = now + static_cast<uint32_t>(6144.0f / this->m_particlesPerSecond * 1000.0f);
        this->m_flakes.m_current->m_origin = origin;
    }

    this->UpdatePacket(count, origin, wind);
    FlakePacket* packet = this->m_flakes.m_current;

    if (packet->m_count >= 6144 || packet->m_buildTime >= this->m_maxBuildTime || now > packet->m_baseTime) {
        this->RealizePacket(this->m_flakes.m_current);
        this->m_flakes.m_active.LinkToTail(this->m_flakes.m_current);
        this->m_flakes.m_activeCount++;
        this->m_flakes.m_freeCount--;
        this->m_flakes.m_current = NULL;
    }
}

// OFFSET: 0x0074E960
void Snow::RenderPackets() {
    if (!this->m_flakes.m_active.Head()) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(this->m_flakeTexture);

    if (!tex) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    C4Vector color(this->m_weather->m_color);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 8, &color.x, 1);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_vertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_pixelShader->m_permutes[0]);
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
    CRect window;
    window.minY = 0.0f;
    window.minX = 0.0f;
    window.maxY = 0.0f;
    window.maxX = 0.0f;
    g_theGxDevicePtr->CapsWindowSize(window);
    float scale = window.maxX * 0.00078125001f;

    if (scale < 0.40000001f) {
        scale = 0.40000001f;
    } else if (scale >= 1.0f) {
        scale = 1.0f;
    }

    float range = 1.0f / (scale * 50.0f - scale * 0.0f);
    C4Vector pointSize(range * -1.0f, range * (scale * 50.0f), scale * 14.0f, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 7, &pointSize.x, 1);

    if (this->m_geometryShader && this->m_geometryShader->m_permutes[0].Valid()) {
        C4Vector pixel(1.0f / window.maxX, 1.0f / window.maxY, 0.0f, 0.0f);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Geometry, 0, &pixel.x, 1);
        g_theGxDevicePtr->RsSet(GxRs_GeometryShader, &this->m_geometryShader->m_permutes[0]);
    } else {
        float maxSize = g_theGxDevicePtr->m_caps.m_pointScaleMax >= 24.0f ? 24.0f : g_theGxDevicePtr->m_caps.m_pointScaleMax;
        GxRsSet(GxRs_PointScaleMax, maxSize);
        GxRsSet(GxRs_PointSprite, 1);
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    for (FlakePacket* packet = this->m_flakes.m_active.Head(), * next; packet; packet = next) {
        next = this->m_flakes.m_active.Next(packet);

        if (packet->m_baseTime >= now || !packet->m_count) {
            continue;
        }

        C4Vector time(static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001), 0.0f, 0.0f, 0.0f);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 6, &time.x, 1);
        g_theGxDevicePtr->PrimVertexPtr(packet->m_buf, static_cast<EGxVertexBufferFormat>(3));
        CGxBatch batch;
        batch.m_primType = GxPrim_Points;
        batch.m_start = 0;
        batch.m_count = packet->m_count;
        batch.m_minIndex = 0;
        batch.m_maxIndex = 0;
        g_theGxDevicePtr->DrawNonIndexed(batch);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0074EFC0
void Snow::RenderPacketsSW() {
    if (!this->m_flakes.m_active.Head()) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(this->m_flakeTexture);

    if (!tex) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Fog, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, tex);
    GxRsSet(GxRs_Texture1, static_cast<void*>(NULL));
    C44Matrix view;
    GxXformView(view);
    C3Vector right(view.a0, view.b0, view.c0);
    C3Vector up(view.a1, view.b1, view.c1);
    C3Vector downRight = right - up;
    C3Vector downLeft = -right - up;
    float scale = 1.0f / sqrt(up.x * up.x + up.y * up.y + up.z * up.z);
    C3Vector corner0(up.x * scale, up.y * scale, scale * up.z);
    scale = 1.0f / sqrt(downRight.x * downRight.x + downRight.z * downRight.z + downRight.y * downRight.y);
    C3Vector corner1(downRight.x * scale, downRight.y * scale, scale * downRight.z);
    scale = 1.0f / sqrt(downLeft.x * downLeft.x + downLeft.z * downLeft.z + downLeft.y * downLeft.y);
    corner0.x = corner0.x * 0.083333336f;
    corner0.y = corner0.y * 0.083333336f;
    corner0.z = corner0.z * 0.083333336f;
    corner1.x = corner1.x * 0.083333336f;
    corner1.y = corner1.y * 0.083333336f;
    corner1.z = corner1.z * 0.083333336f;
    C3Vector corner2(scale * downLeft.x * 0.083333336f, downLeft.y * scale * 0.083333336f, downLeft.z * scale * 0.083333336f);
    C3Vector camPos = World::GetCamPos();
    uint32_t now = FrameTime::GetCurTimeMs();
    CImVector color = this->m_weather->m_color;

    if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
        CImVector swizzled;
        swizzled.b = color.r;
        swizzled.g = color.g;
        swizzled.r = color.b;
        swizzled.a = color.a;
        color = swizzled;
    }

    for (FlakePacket* packet = this->m_flakes.m_active.Head(), * next; packet; packet = next) {
        next = this->m_flakes.m_active.Next(packet);

        if (packet->m_baseTime >= now) {
            continue;
        }

        float age = static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001);
        CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), 3 * packet->m_count);
        CGxVertexPCT* vertex = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(buf));
        uint32_t vertexCount = 0;

        for (uint32_t i = 0; i < packet->m_count; i++) {
            static C2Vector s_texCoords[3] = { C2Vector(0.5f, -0.75f), C2Vector(-0.5f, 1.5f), C2Vector(1.5f, 1.5f) };
            const Flake& flake = packet->m_buffer.m_particles[i];
            float start = flake.m_startTime;
            float end = flake.m_endTime;

            if (!(start <= age && age <= end + 0.25f)) {
                continue;
            }

            float clamped = end <= age ? end : age;
            float over = age - end;
            float travel = clamped - start;

            if (age - end < 0.0f) {
                over = 0.0f;
            }

            float x = flake.m_pos.x + flake.m_velocity.x * travel;
            float y = flake.m_pos.y + flake.m_velocity.y * travel;
            float z = flake.m_pos.z + flake.m_velocity.z * travel;
            float fade = 1.0f - over * 4.0f;
            fade = fade < 0.0f ? 0.0f : fade >= 1.0f ? 1.0f : fade;

            if (travel > 1.0f) {
                travel = 1.0f;
            }

            if (fade >= 1.0f) {
                fade = travel;
            }

            float rx = x - camPos.x;
            float ry = y - camPos.y;
            float rz = z - camPos.z;
            color.a = static_cast<uint8_t>(static_cast<int32_t>(fade * 255.0f));
            vertex[0].p = C3Vector(rx + corner0.x, ry + corner0.y, rz + corner0.z);
            vertex[0].c = color;
            vertex[0].tc = s_texCoords[0];
            vertex[1].p = C3Vector(corner1.x + rx, corner1.y + ry, corner1.z + rz);
            vertex[1].c = color;
            vertex[1].tc = s_texCoords[1];
            vertex[2].p = C3Vector(corner2.x + rx, corner2.y + ry, corner2.z + rz);
            vertex[2].c = color;
            vertex[2].tc = s_texCoords[2];
            vertex += 3;
            vertexCount += 3;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;

        if (vertexCount) {
            g_theGxDevicePtr->PrimVertexPtr(buf, static_cast<EGxVertexBufferFormat>(9));
            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = 0;
            batch.m_count = vertexCount;
            batch.m_minIndex = 0;
            batch.m_maxIndex = 0;
            g_theGxDevicePtr->DrawNonIndexed(batch);
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00752330
Snow::~Snow() {
    TextureRelease(this->m_flakeTexture);

    if (this->m_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_vertexShader);
        this->m_vertexShader = NULL;
    }

    if (this->m_geometryShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_geometryShader);
        this->m_geometryShader = NULL;
    }

    if (this->m_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(this->m_pixelShader);
        this->m_pixelShader = NULL;
    }
}

// PROVEN: 979541c2 IDENTICAL/NONE
// OFFSET: 0x007523D0
void Snow::Render() {
    this->UpdatePackets();

    if (CWorldScene::s_defaultWorldView->m_exteriorPortalView.m_maxViewDepth < 0.0f) {
        return;
    }

    if (this->m_useShaders) {
        this->RenderPackets();
    } else {
        this->RenderPacketsSW();
    }

    this->m_mists.Render();
}

// OFFSET: 0x00752940/0x140433970
Snow::Snow(Weather* weather, float width, float height) : m_mists(weather, 12.0f, C3Vector(44.0f, 44.0f, 25.0f), "textures\\Weather\\SnowMist01.blp", -1.5700001f, 0.34906584f, 9.0f, 3.0f, 128) {
    this->m_weather = weather;
    this->m_useShaders = 1;
    this->m_fadeOut = 0;
    this->m_flakeTexture = NULL;
    this->m_vertexShader = NULL;
    this->m_geometryShader = NULL;
    this->m_pixelShader = NULL;
    this->m_intensity = 0.0f;
    float count = this->m_useShaders ? 14000.0f : 1300.0f;
    float density = Weather::s_densityScale * 0.0f;
    this->m_particlesPerSecond = density * count;
    this->m_maxBuildTime = count * 0.00016276042f;
    float mists = this->m_useShaders ? 48.0f : 24.0f;
    this->m_mists.m_spawnRate = density * mists;
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    this->m_bounds.b = C3Vector(-halfWidth, -halfWidth, -halfHeight);
    this->m_bounds.t = C3Vector(halfWidth, halfWidth, halfHeight);
    this->m_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "snowpoint");
    this->m_geometryShader = g_theGxDevicePtr->ShaderCreate(GxSh_Geometry, "snowpoint");
    this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "snowpoint");
    CStatus status;
    CGxTexFlags flags;
    flags.m_filter = 4;
    this->m_flakeTexture = TextureCreate(this->m_weather->m_precipTexture, flags, &status, 1, NULL, 2);

    if (!this->m_vertexShader || !this->m_vertexShader->m_permutes[0].Valid() || (!g_theGxDevicePtr->m_caps.m_pointSprite && (!this->m_geometryShader || !this->m_geometryShader->m_permutes[0].Valid())) || !this->m_pixelShader || !this->m_pixelShader->m_permutes[0].Valid()) {
        this->m_useShaders = 0;
    }
}
