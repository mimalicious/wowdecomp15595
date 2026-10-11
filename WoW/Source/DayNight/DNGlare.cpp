#include "WoW/Source/DayNight/DNGlare.h"
#include <cmath>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "Engine/Source/ShaderEffect/SunShafts.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DayNight/DNClouds.h"
#include "WoW/Source/DayNight/DNOverrideSky.h"
#include "WoW/Source/DayNight/DNPlanet.h"

// STATIC: 0x00CEFC74
static uint16_t s_glareIndices[4] = { 0, 2, 1, 3 };

// STATIC: 0x00CEFC80
static C3Vector s_glareVerts[4] = {
    C3Vector(0.0f, -0.5f, 0.5f),
    C3Vector(0.0f, 0.5f, 0.5f),
    C3Vector(0.0f, -0.5f, -0.5f),
    C3Vector(0.0f, 0.5f, -0.5f),
};

// STATIC: 0x00CEFCB0
static C2Vector s_glareTexCoords[4] = {
    C2Vector(0.0f, 0.0f),
    C2Vector(1.0f, 0.0f),
    C2Vector(0.0f, 1.0f),
    C2Vector(1.0f, 1.0f),
};

// STATIC: 0x00CEFCD0
static float s_glowExponent = 10.0f;

// STATIC: 0x00E43264
CGxShader* DayNight::DNGlare::s_pixelShader;

// STATIC: 0x00E43268
int32_t DayNight::GlareBase::s_masterEnable;

// STATIC: 0x00E4326C
uint32_t DayNight::DNGlare::s_refCount;

// STATIC: 0x00E43270
CGxShader* DayNight::DNGlare::s_vertexShader;

// STATIC: 0x00E43274
CGxShader* DayNight::DNGlare::s_queryVertexShader;

// STATIC: 0x00E43278
CGxShader* DayNight::DNGlare::s_queryPixelShader;

// PROVEN: bb7ab595 IDENTICAL/NONE
// OFFSET: 0x0064F7F0/0x1402EEC30
int32_t DayNight::GlareBase::IsAllLoaded() {
    return TextureIsLoaded(this->m_texture);
}

// PROVEN: 435f07f8 IDENTICAL/NONE
// OFFSET: 0x0064F800
void DayNight::GlareBase::LinkToAsyncHead() {
    if (this->m_texture) {
        TextureIncreasePriority(this->m_texture);
    }
}

// PROVEN: 3fa6a7f9 IDENTICAL/NONE
// OFFSET: 0x0064F810
float DayNight::DNSunGlare::GetCloudDensityFade() {
    return 1.0f - DayNight::g_clouds.GetDensity(this->m_position, 1.0f);
}

// PROVEN: cb38ef01 IDENTICAL/NONE
// OFFSET: 0x0064F830
void DayNight::DNGlare::Destroy() {
    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    if (this->m_query) {
        GxQueryDestroy(this->m_query);
    }

    if (--DNGlare::s_refCount) {
        return;
    }

    if (DNGlare::s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(DNGlare::s_vertexShader);
        DNGlare::s_vertexShader = NULL;
    }

    if (DNGlare::s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(DNGlare::s_pixelShader);
        DNGlare::s_pixelShader = NULL;
    }

    if (DNGlare::s_queryVertexShader) {
        g_theGxDevicePtr->ShaderRelease(DNGlare::s_queryVertexShader);
        DNGlare::s_queryVertexShader = NULL;
    }

    if (DNGlare::s_queryPixelShader) {
        g_theGxDevicePtr->ShaderRelease(DNGlare::s_queryPixelShader);
        DNGlare::s_queryPixelShader = NULL;
    }
}

// OFFSET: 0x0064F8D0
float DayNight::DNGlare::GetLiquidDepthFade() {
    if (!DayNight::g_dnInfo.m_liquidStatus) {
        return 1.0f;
    }

    float fade = DayNight::g_dnInfo.m_camDepth * 0.1f;

    if (fade < 0.0f) {
        return 1.0f - 0.0f;
    }

    if (fade >= 1.0f) {
        fade = 1.0f;
    }

    return 1.0f - fade;
}

// OFFSET: 0x0064F930
float DayNight::DNMoonGlare::GetCloudDensityFade() {
    float density = DayNight::g_clouds.GetDensity(this->m_position, 1.0f);
    return 1.0f - fabs(density - 0.5f + density - 0.5f);
}

// OFFSET: 0x0064F960/0x1402EEDF0
void DayNight::DNGlare::Update(float deltaSec) {
    DNInfo& info = DayNight::g_dnInfo;

    this->m_visibility = 1.0f;

    C3Vector dir = this->m_position - info.m_camPos;
    float scale = 1.0f / sqrt(dir.y * dir.y + dir.z * dir.z + dir.x * dir.x);
    dir.x = dir.x * scale;
    dir.y = dir.y * scale;
    dir.z = dir.z * scale;

    this->m_visibility = this->GetCloudDensityFade() * this->m_visibility;
    float visibility = this->GetLiquidDepthFade() * this->m_visibility;
    this->m_visibility = visibility;

    float skybox = 0.0f;

    if (info.m_skybox && info.m_skyboxWeight > 0.0f) {
        skybox = info.m_skyboxWeight;
    } else {
        if (info.m_blendSkybox[0] && info.m_blendSkyboxWeight[0] > 0.0f) {
            skybox = info.m_blendSkyboxWeight[0];
        }

        if (info.m_blendSkybox[1] && info.m_blendSkyboxWeight[1] > skybox) {
            skybox = info.m_blendSkyboxWeight[1];
        }

        if (info.m_blendSkybox[2] && info.m_blendSkyboxWeight[2] > skybox) {
            skybox = info.m_blendSkyboxWeight[2];
        }
    }

    this->m_visibility = (1.0f - skybox) * visibility;
    this->m_visibility = DayNight::InterpTable(this->m_alphaCurve, 4, info.m_dayFraction) * this->m_visibility;

    if (this->m_visibility != 0.0f) {
        this->m_visibility = this->IsVisible() * this->m_visibility;
    }

    float target = this->m_visibility;
    float fade = this->m_fade;

    if (target > fade) {
        float value = this->m_fadeInRate * deltaSec + fade;
        this->m_fade = target > value ? value : target;
    } else if (fade > target) {
        float value = fade - this->m_fadeOutRate * deltaSec;
        this->m_fade = value > target ? value : target;
    }

    float dot = info.m_camDir.y * dir.y + info.m_camDir.z * dir.z + info.m_camDir.x * dir.x;

    if (this->m_minDot > dot) {
        dot = this->m_minDot;
    }

    float t = (dot - this->m_minDot) / (1.0f - this->m_minDot);
    this->m_scale = ((this->m_scaleMax - this->m_scaleMin) * t + this->m_scaleMin) * this->m_baseScale;

    float current = this->m_fade;
    float alpha = ((this->m_alphaMax - this->m_alphaMin) * t + this->m_alphaMin) * current * (this->m_color.a * 0.0039215689f) * 255.0f;
    this->m_color.a = static_cast<uint8_t>(CMath::fint_(alpha));
    this->m_glow = pow(dot, s_glowExponent) * current;
}

// OFFSET: 0x0064FC20
float DayNight::DNGlare::IsVisibleIntersect() {
    DNInfo& info = DayNight::g_dnInfo;

    float dx = this->m_position.x - info.m_camPos.x;
    float dy = this->m_position.y - info.m_camPos.y;
    float dz = this->m_position.z - info.m_camPos.z;
    float scale = 1.0f / sqrt(dz * dz + dx * dx + dy * dy);
    dx = dx * scale;
    dy = scale * dy;
    dz = dz * scale;

    C3Vector start(info.m_camPos.x + dx * 0.5f, info.m_camPos.y + dy * 0.5f, dz * 0.5f + info.m_camPos.z);
    C3Vector end(info.m_farClip * dx + info.m_camPos.x, info.m_farClip * dy + info.m_camPos.y, info.m_farClip * dz + info.m_camPos.z);
    C3Vector hit(0.0f, 0.0f, 0.0f);
    float dist = 1.0f;

    if (DayNight::g_intersectCallback && DayNight::g_intersectCallback(start, end, &hit, &dist)) {
        return 0.0f;
    }

    return 1.0f;
}

// OFFSET: 0x0064FD70
float DayNight::DNGlare::IsVisibleQuery() {
    DNInfo& info = DayNight::g_dnInfo;

    if (this->m_queryIssued) {
        uint32_t available = 0;
        GxQueryGetParam(this->m_query, GxQueryParam_Unk1, &available);

        if (available) {
            uint32_t pixels = 0;

            if (GxQueryGetData(this->m_query, &pixels)) {
                this->m_queryPixels = pixels;
                this->m_queryIssued = 0;
            }
        }
    }

    if (!this->m_queryIssued) {
        GxQueryBegin(this->m_query);
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_ColorWrite, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Culling, 0);
    DefaultShaders::SetFogEnabled(0);

    C3Vector verts[6];
    C2Vector texCoords[6];
    CImVector colors[6];
    uint16_t indices[8];
    uint32_t vertCount;
    uint32_t indexCount;
    this->m_planet->GenGeometry(verts, texCoords, colors, indices, vertCount, indexCount);

    float minX;
    float maxX;
    float minY;
    float maxY;
    float minZ;
    float maxZ;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(minX, maxX, minY, maxY, 0.99902344f, 1.0f);

    C44Matrix view;
    GxXformView(view);

    C3Vector forward(view.a2, view.b2, view.c2);
    C44Matrix world;
    DayNight::Billboard(forward, world);
    world.d0 = this->m_position.x - info.m_camPos.x;
    world.d1 = this->m_position.y - info.m_camPos.y;
    world.d2 = this->m_position.z - info.m_camPos.z;

    C44Matrix proj;
    GxXformProjNative(proj);

    C44Matrix viewProj = view * proj;
    C44Matrix transform = world * viewProj;

    if (DNGlare::s_queryVertexShader) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &DNGlare::s_queryVertexShader->m_permutes[0]);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &DNGlare::s_queryPixelShader->m_permutes[0]);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
    } else {
        g_theGxDevicePtr->XformPush(GxXform_World);
        g_theGxDevicePtr->m_xforms[GxXform_World].Identity();
        g_theGxDevicePtr->XformSetView(viewProj);
    }

    if (!this->m_queryIssued) {
        GxPrimVertexPtr(vertCount, verts, sizeof(C3Vector), NULL, 0, colors, sizeof(CImVector), NULL, 0, NULL, 0);
        GxPrimIndexPtr(indexCount, indices);

        CGxBatch batch;
        batch.m_primType = GxPrim_TriangleStrip;
        batch.m_start = 0;
        batch.m_count = indexCount;
        batch.m_minIndex = 0;
        batch.m_maxIndex = vertCount - 1;
        g_theGxDevicePtr->Draw(batch);
    }

    float screenFade = 0.0f;

    if (SunShafts::IsEnabled()) {
        float x = (transform.a0 + transform.b0 + transform.c0) * 0.0f + transform.d0;
        float y = (transform.a1 + transform.b1 + transform.c1) * 0.0f + transform.d1;
        float w = (transform.a3 + transform.b3 + transform.c3) * 0.0f + transform.d3;

        if (w > 0.000001f) {
            x = x * (1.0f / w);
            y = (1.0f / w) * y;
        }

        float edgeX = fabs(x) - 0.80000001f;
        float edgeY = fabs(y) - 0.80000001f;
        float edge = edgeX <= edgeY ? edgeY : edgeX;

        if (edge > 0.0f) {
            if (edge >= 0.2f) {
                edge = 0.2f;
            }
        } else {
            edge = 0.0f;
        }

        screenFade = 1.0f - edge * 5.0f;
    }

    C3Vector screen[6];
    GxuXformCalc3dScreenCoords(transform, vertCount, verts, screen);
    float area = fabs(screen[1].x - screen[0].x) * fabs(screen[2].y - screen[0].y);

    if (!DNGlare::s_queryVertexShader) {
        GxXformPop(GxXform_World);
        g_theGxDevicePtr->XformSetView(view);
    }

    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    g_theGxDevicePtr->RsPop();

    if (!this->m_queryIssued) {
        GxQueryEnd(this->m_query);
        this->m_queryIssued = 1;
    }

    if (this->m_skipQuery) {
        this->m_skipQuery = 0;
        return 0.0f;
    }

    if (area > 1.0f) {
        float ratio = static_cast<float>(static_cast<double>(this->m_queryPixels) / area);

        if (ratio > 1.0f) {
            ratio = 1.0f;
        }

        return (1.0f - ratio) * screenFade + ratio;
    }

    return 0.0f;
}

// PROVEN: cc7f6dc2 IDENTICAL/NONE
// OFFSET: 0x006506B0
float DayNight::DNGlare::IsVisible() {
    if (!GlareBase::s_masterEnable || !this->m_enable || !this->m_color.a) {
        return 0.0f;
    }

    if (this->m_query) {
        return this->IsVisibleQuery();
    }

    return this->IsVisibleIntersect();
}

// OFFSET: 0x006506E0/0x1402EFE60
void DayNight::DNGlare::Render() {
    if (!GlareBase::s_masterEnable || !this->m_enable || !this->m_color.a) {
        return;
    }

    CGxTex* texture = TextureGetGxTex(this->m_texture);

    if (!texture) {
        return;
    }

    C44Matrix view;
    GxXformView(view);

    C3Vector forward(view.a2, view.b2, view.c2);
    C44Matrix world;
    DayNight::Billboard(forward, world);
    world.d0 = this->m_position.x - DayNight::g_dnInfo.m_camPos.x;
    world.d1 = this->m_position.y - DayNight::g_dnInfo.m_camPos.y;
    world.d2 = this->m_position.z - DayNight::g_dnInfo.m_camPos.z;

    C3Vector scale(this->m_scale, this->m_scale, this->m_scale);
    world.Scale(scale);

    float minX = 0.0f;
    float maxX = 0.0f;
    float minY = 0.0f;
    float maxY = 0.0f;
    float minZ = 0.0f;
    float maxZ = 0.0f;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(minX, maxX, minY, maxY, 0.99902344f, 1.0f);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, 3);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_DepthTest, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);

    if (DNGlare::s_vertexShader) {
        int32_t shafts = SunShafts::IsEnabled();

        if (shafts) {
            GxRsSet(GxRs_Texture1, SunShafts::GetBuffer());
        }

        GxRsSet(GxRs_VertexShader, DNGlare::s_vertexShader, 0);
        GxRsSet(GxRs_PixelShader, DNGlare::s_pixelShader, shafts);

        C44Matrix proj;
        GxXformProjNative(proj);
        C44Matrix transform = world * view * proj;
        GxShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
    } else {
        GxRsSet(GxRs_ColorOp0, 0);
        GxRsSet(GxRs_AlphaOp0, 0);
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    }

    GxPrimVertexPtr(4, s_glareVerts, sizeof(C3Vector), NULL, 0, &this->m_color, 0, s_glareTexCoords, sizeof(C2Vector), NULL, 0);
    GxPrimIndexPtr(4, s_glareIndices);

    CGxBatch batch;
    batch.m_primType = GxPrim_TriangleStrip;
    batch.m_start = 0;
    batch.m_count = 4;
    batch.m_minIndex = 0;
    batch.m_maxIndex = 3;
    g_theGxDevicePtr->Draw(batch);

    if (!DNGlare::s_vertexShader) {
        GxXformPop(GxXform_World);
    }

    g_theGxDevicePtr->RsPop();
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

// OFFSET: 0x00650B80/0x1402F0520
void DayNight::DNGlare::Initialize(const char* fileName) {
    CStatus status;
    this->m_texture = TextureCreate(fileName, CGxTexFlags(), &status, 1, NULL, 2);

    if (g_theGxDevicePtr->m_caps.m_occlusionQuery) {
        GxQueryCreate(this->m_query, GxQueryType_Occlusion);
    }

    this->m_queryPixels = 0;
    this->m_queryIssued = 0;

    if (!DNGlare::s_refCount++ && g_theGxDevicePtr->m_caps.m_shaderModel >= 3) {
        DNGlare::s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "DNGlare");
        DNGlare::s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "DNGlare");
        DNGlare::s_queryVertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "DNGlareQuery");
        DNGlare::s_queryPixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "DNGlareQuery");
    }
}
