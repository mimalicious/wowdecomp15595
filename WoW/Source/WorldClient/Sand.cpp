#include "WoW/Source/WorldClient/Sand.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D017D4
static float s_sandWindLead = 1.75f;

// OFFSET: 0x00748830/0x140427A30
void Sand::RealizePacket(GrainPacket* packet) {
    Grain* vertex = reinterpret_cast<Grain*>(g_theGxDevicePtr->BufLock(packet->m_buf));

    for (uint32_t i = 0; i < packet->m_count; i++) {
        vertex[i] = packet->m_buffer.m_particles[i];
    }

    CGxBuf* buf = packet->m_buf;
    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
}

// OFFSET: 0x0074A8A0/0x14042A160
void Sand::InitGrain(Grain& grain, float time) {
    C3Vector size(this->m_bounds.t.x - this->m_bounds.b.x, this->m_bounds.t.y - this->m_bounds.b.y, this->m_bounds.t.z - this->m_bounds.b.z);
    const C3Vector& camPos = CWorldScene::s_defaultWorldView->m_camPos;
    float rz = CRandom::ufloat_(g_rndSeed);
    float ry = CRandom::ufloat_(g_rndSeed);
    float rx = CRandom::ufloat_(g_rndSeed);
    grain.m_pos.x = (rx * 0.15000001f + 0.85000002f) * size.x;
    grain.m_pos.y = (ry - 0.5f) * size.y;
    grain.m_pos.z = (rz - 0.5f) * size.z;
    float angle = (CRandom::ufloat_(g_rndSeed) - 0.5f) * 0.34906584f - 1.5700001f;
    float c = cos(angle);
    float s = sin(angle);
    float speed = (CRandom::ufloat_(g_rndSeed) - 0.5f) * 0.66666669f + 18.666666f;
    float rise = (CRandom::ufloat_(g_rndSeed) - 0.5f) * 0.16666667f + 0.83333331f;
    grain.m_velocity.x = speed * s;
    grain.m_velocity.y = speed * c;
    grain.m_velocity.z = rise;
    C44Matrix rotation = C44Matrix::RotationAroundZ(-this->m_weather->m_facing);
    grain.m_velocity = rotation * grain.m_velocity;
    grain.m_pos = rotation * grain.m_pos;
    const C3Vector& wind = this->m_weather->m_velocity;
    grain.m_pos.x = grain.m_pos.x + wind.x * s_sandWindLead;
    grain.m_pos.y = grain.m_pos.y + wind.y * s_sandWindLead;
    grain.m_pos.z = grain.m_pos.z + wind.z * s_sandWindLead;
    grain.m_pos.x = grain.m_pos.x + camPos.x;
    grain.m_pos.y = camPos.y + grain.m_pos.y;
    grain.m_pos.z = camPos.z + grain.m_pos.z;
    float lead = (CRandom::ufloat_(g_rndSeed) - 0.5f) * 0.30000001f + 3.2f;
    float height = this->m_weather->m_hmMgr->Query(grain.m_pos);

    if (height < grain.m_pos.z) {
        C3Vector farPos(grain.m_pos.x + grain.m_velocity.x * lead, grain.m_pos.y + grain.m_velocity.y * lead, grain.m_pos.z + grain.m_velocity.z * lead);
        C3Vector hit(0.0f, 0.0f, 0.0f);
        this->m_weather->m_hmMgr->Query(grain.m_pos, farPos, hit);
        grain.m_startTime = time;
        float vz = grain.m_velocity.z;
        float vy = grain.m_velocity.y;
        float vx = grain.m_velocity.x;
        float dx = grain.m_pos.x - hit.x;
        float dy = grain.m_pos.y - hit.y;
        float dz = grain.m_pos.z - hit.z;
        grain.m_endTime = sqrt(dz * dz + dy * dy + dx * dx) / sqrt(vz * vz + vy * vy + vx * vx) + time;
    } else {
        grain.m_startTime = 0.0f;
        grain.m_endTime = 0.0f;
    }
}

// OFFSET: 0x0074AE00/0x14042A720
void Sand::UpdatePacket(uint32_t count) {
    GrainPacket* packet = this->m_grains.m_current;
    float tick = FrameTime::GetTickTimeSec();
    uint32_t available = 6144 - packet->m_count;

    if (available >= count) {
        available = count;
    }

    for (int32_t i = available; i > 0; i--) {
        float r = CRandom::ufloat_(g_rndSeed);
        Grain& grain = packet->m_buffer.m_particles[packet->m_count];
        this->InitGrain(grain, r * tick + packet->m_buildTime);
        uint32_t lastTime = packet->m_baseTime + static_cast<uint32_t>(grain.m_endTime * 1000.0f);

        if (lastTime <= packet->m_lastTime) {
            lastTime = packet->m_lastTime;
        }

        packet->m_lastTime = lastTime;
        packet->m_count++;
    }

    packet->m_buildTime = tick + packet->m_buildTime;
}

// OFFSET: 0x0074B870
uint32_t Sand::NumActive() {
    uint32_t active = 0;
    uint32_t now = FrameTime::GetCurTimeMs();

    for (GrainPacket* packet = this->m_grains.m_active.Head(); packet; packet = this->m_grains.m_active.Next(packet)) {
        if (now < packet->m_lastTime) {
            active++;
        }
    }

    return this->m_mists.NumActive() + active;
}

// OFFSET: 0x0074F7C0/0x14042FE00
void Sand::UpdatePackets() {
    if (this->m_fadeOut) {
        this->m_grains.m_current = NULL;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    for (GrainPacket* packet = this->m_grains.m_active.Head(), * next; packet; packet = next) {
        next = this->m_grains.m_active.Next(packet);

        if (now > packet->m_lastTime) {
            this->m_grains.m_activeCount--;
            this->m_grains.m_freeCount++;
            this->m_grains.m_free.LinkToTail(packet);
            continue;
        }

        if (this->m_fadeOut && this->m_weather->m_fogIntensity <= 0.25f && now <= packet->m_baseTime) {
            this->m_grains.m_activeCount--;
            this->m_grains.m_freeCount++;
            this->m_grains.m_free.LinkToTail(packet);
            continue;
        }

        if (packet->m_lastTime - packet->m_baseTime >= 60000) {
            this->m_grains.m_activeCount--;
            this->m_grains.m_freeCount++;
            this->m_grains.m_free.LinkToTail(packet);
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

    if (!this->m_grains.m_current) {
        this->m_grains.Alloc();
        this->m_grains.m_current->m_baseTime = now + static_cast<uint32_t>(6144.0f / this->m_particlesPerSecond * 1000.0f);
    }

    this->UpdatePacket(count);
    GrainPacket* packet = this->m_grains.m_current;

    if (packet->m_count >= 6144 || packet->m_buildTime >= this->m_maxBuildTime || now > packet->m_baseTime) {
        this->RealizePacket(this->m_grains.m_current);
        this->m_grains.m_active.LinkToTail(this->m_grains.m_current);
        this->m_grains.m_activeCount++;
        this->m_grains.m_freeCount--;
        this->m_grains.m_current = NULL;
    }
}

// OFFSET: 0x0074FA80/0x1404300C0
void Sand::RenderPacketsSW() {
    if (!this->m_grains.m_active.Head()) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    C3Vector camPos = World::GetCamPos();
    uint32_t now = FrameTime::GetCurTimeMs();
    CImVector color = DayNight::GetFogInfo()->m_color;

    for (GrainPacket* packet = this->m_grains.m_active.Head(), * next; packet; packet = next) {
        next = this->m_grains.m_active.Next(packet);

        if (packet->m_baseTime >= now) {
            continue;
        }

        float age = static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001);
        CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPC), packet->m_count);
        CGxVertexPC* vertex = reinterpret_cast<CGxVertexPC*>(g_theGxDevicePtr->BufLock(buf));
        uint32_t vertexCount = 0;

        for (uint32_t i = 0; i < packet->m_count; i++) {
            const Grain& grain = packet->m_buffer.m_particles[i];
            float start = grain.m_startTime;
            float end = grain.m_endTime;

            if (!(start <= age && age <= end)) {
                continue;
            }

            float t = age - start;
            float duration = end - start;

            if (age - start >= 0.0f) {
                if (duration <= t) {
                    t = duration;
                }
            } else {
                t = 0.0f;
            }

            float x = grain.m_pos.x + grain.m_velocity.x * t - camPos.x;
            float y = grain.m_pos.y + grain.m_velocity.y * t - camPos.y;
            float fadeIn = 1.0f - t * 5.0f;
            float z = grain.m_pos.z + t * grain.m_velocity.z - camPos.z;
            fadeIn = fadeIn < 0.0f ? 0.0f : fadeIn >= 1.0f ? 1.0f : 1.0f - t * 5.0f;
            float fadeScale = 1.0f / (duration - (duration - 0.2f));
            float fadeOut = fadeScale * -1.0f * t + fadeScale * duration;
            fadeOut = fadeOut < 0.0f ? 0.0f : fadeOut >= 1.0f ? 1.0f : fadeOut;
            vertex->p.x = x;
            vertex->p.y = y;
            vertex->p.z = z;
            uint8_t alpha = static_cast<uint8_t>(static_cast<int32_t>((1.0f - fadeIn) * fadeOut * 255.0f));
            color.a = alpha;

            if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
                CImVector swizzled;
                swizzled.b = color.r;
                swizzled.g = color.g;
                swizzled.r = color.b;
                swizzled.a = alpha;
                vertex->c = swizzled;
            } else {
                vertex->c = color;
            }

            vertex++;
            vertexCount++;
        }

        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;

        if (vertexCount) {
            g_theGxDevicePtr->PrimVertexPtr(buf, static_cast<EGxVertexBufferFormat>(8));
            CGxBatch batch;
            batch.m_primType = GxPrim_Points;
            batch.m_start = 0;
            batch.m_count = vertexCount;
            batch.m_minIndex = 0;
            batch.m_maxIndex = 0;
            g_theGxDevicePtr->DrawNonIndexed(batch);
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0074FEB0/0x140430610
void Sand::RenderPackets() {
    if (!this->m_grains.m_active.Head()) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Alpha);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    g_theGxDevicePtr->RsSet(GxRs_VertexShader, &this->m_vertexShader->m_permutes[0]);
    g_theGxDevicePtr->RsSet(GxRs_PixelShader, &this->m_pixelShader->m_permutes[0]);
    CRect window;
    window.minY = 0.0f;
    window.minX = 0.0f;
    window.maxY = 0.0f;
    window.maxX = 0.0f;
    g_theGxDevicePtr->CapsWindowSize(window);

    if (this->m_geometryShader && this->m_geometryShader->m_permutes[0].Valid()) {
        C4Vector pixel(1.0f / window.maxX, 1.0f / window.maxY, 0.0f, 0.0f);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Geometry, 0, &pixel.x, 1);
        g_theGxDevicePtr->RsSet(GxRs_GeometryShader, &this->m_geometryShader->m_permutes[0]);
    } else {
        GxRsSet(GxRs_PointScaleMin, 1.0f);
        float maxSize = g_theGxDevicePtr->m_caps.m_pointScaleMax;

        if (maxSize >= 24.0f) {
            maxSize = 24.0f;
        }

        GxRsSet(GxRs_PointScaleMax, maxSize);
        GxRsSet(GxRs_PointSprite, 1);
    }

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
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 3, &eye.x, 1);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 4, &viewProj.a0, 4);
    CRect screen;
    screen.minY = 0.0f;
    screen.minX = 0.0f;
    screen.maxY = 0.0f;
    screen.maxX = 0.0f;
    g_theGxDevicePtr->CapsWindowSize(screen);
    C4Vector pointSize(-0.050000001f, 1.5f, (screen.maxY - screen.minY) * 0.0024999999f, 0.0f);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 9, &pointSize.x, 1);
    C4Vector fogColor(DayNight::GetFogInfo()->m_color);
    g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, &fogColor.x, 1);
    uint32_t now = FrameTime::GetCurTimeMs();

    for (GrainPacket* packet = this->m_grains.m_active.Head(), * next; packet; packet = next) {
        next = this->m_grains.m_active.Next(packet);

        if (packet->m_baseTime >= now || !packet->m_count) {
            continue;
        }

        C4Vector time(static_cast<float>(static_cast<double>(now - packet->m_baseTime) * 0.001), 0.0f, 0.0f, 0.0f);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 8, &time.x, 1);
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

// OFFSET: 0x00752410/0x140433400
Sand::~Sand() {
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

// OFFSET: 0x007524B0/0x1404334C0
void Sand::Render() {
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

// OFFSET: 0x00752C00/0x140433D20
Sand::Sand(Weather* weather, float width, float height) : m_mists(weather, 12.0f, C3Vector(44.0f, 44.0f, 25.0f), "textures\\Weather\\WeatherMistGrainy01.blp", -1.5700001f, 0.34906584f, 15.0f, 4.5f, 128) {
    this->m_weather = weather;
    this->m_useShaders = 1;
    this->m_fadeOut = 0;
    this->m_vertexShader = NULL;
    float count = this->m_useShaders ? 32000.0f : 6000.0f;
    this->m_maxBuildTime = count * 0.00016276042f;
    float density = Weather::s_densityScale;
    this->m_particlesPerSecond = density * count * 0.0f;
    float mists = this->m_useShaders ? 64.0f : 32.0f;
    this->m_mists.m_spawnRate = density * mists * 0.0f;
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    this->m_bounds.b = C3Vector(-halfWidth, -halfWidth, -halfHeight);
    this->m_bounds.t = C3Vector(halfWidth, halfWidth, halfHeight);
    this->m_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "sand");
    this->m_geometryShader = g_theGxDevicePtr->ShaderCreate(GxSh_Geometry, "sandpoint");
    this->m_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "sandpoint");

    if (!this->m_vertexShader || !this->m_vertexShader->m_permutes[0].Valid() || (!g_theGxDevicePtr->m_caps.m_pointSprite && (!this->m_geometryShader || !this->m_geometryShader->m_permutes[0].Valid())) || !this->m_pixelShader || !this->m_pixelShader->m_permutes[0].Valid()) {
        this->m_useShaders = 0;
    }

    if (g_theGxDevicePtr->m_caps.m_shaderProfiles[1] && !Weather::s_cvarUseWeatherShaders->m_intValue) {
        this->m_useShaders = 0;
    }
}
