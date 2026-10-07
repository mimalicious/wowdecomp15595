#include "WoW/Source/DayNight/DNSky.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DayNight/DayNight.h"

// STATIC: 0x00BBB054
static float s_ringAngles[7] = { 0.0f, 0.17f, 0.2f, 0.23f, 0.24f, 0.25f, 1.0f };

// STATIC: 0x00CEFD28
static C2Vector s_highlightTable[6] = {
    C2Vector(0.125f, 0.0f),
    C2Vector(0.27083334f, 1.0f),
    C2Vector(0.29166669f, 0.0f),
    C2Vector(0.85416663f, 0.0f),
    C2Vector(0.89583331f, 1.0f),
    C2Vector(0.99930555f, 0.0f),
};

// STATIC: 0x00CEFD58
static C2Vector s_facingTable[6] = {
    C2Vector(0.125f, 1.0f),
    C2Vector(0.375f, 0.0f),
    C2Vector(0.5f, -0.5f),
    C2Vector(0.625f, -0.69999999f),
    C2Vector(0.75f, -0.5f),
    C2Vector(0.875f, 0.0f),
};

// STATIC: 0x00E43304
uint32_t DayNight::DNSky::s_refCount;

// STATIC: 0x00E43308
CGxShader* DayNight::DNSky::s_vertexShader;

// STATIC: 0x00E4330C
CGxShader* DayNight::DNSky::s_pixelShader;

// INLINED
static void ApplyEclipse(CImVector& color) {
    if (DayNight::g_dnInfo.m_eclipseAmount) {
        CImVector eclipse;
        memcpy(&eclipse, DayNight::g_dnInfo.m_eclipseLightId, sizeof(eclipse));

        if (DayNight::g_dnInfo.m_eclipseAmount) {
            color.InterpColor(DayNight::g_dnInfo.m_eclipseAmount, &eclipse);
        }
    }
}

// PROVEN: 2ca9ae1a IDENTICAL/NONE
// OFFSET: 0x006519C0/0x1402F1680
void DayNight::DNSky::Initialize() {
    if (!DNSky::s_refCount++ && g_theGxDevicePtr->m_caps.m_shaderModel >= 3) {
        DNSky::s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "DNSky");
        DNSky::s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "DNSky");
    }
}

// PROVEN: 0feec97e IDENTICAL/NONE
// OFFSET: 0x00651A10
void DayNight::DNSky::Destroy() {
    if (--DNSky::s_refCount) {
        return;
    }

    if (DNSky::s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(DNSky::s_vertexShader);
        DNSky::s_vertexShader = NULL;
    }

    if (DNSky::s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(DNSky::s_pixelShader);
        DNSky::s_pixelShader = NULL;
    }
}

// OFFSET: 0x00651A60
void DayNight::DNSky::SetColors(const CurrentLight& light) {
    CImVector bands[5];
    CImVector temp;
    float highlight = DayNight::InterpTable(s_highlightTable, 6, DayNight::g_dnInfo.m_dayFraction) * light.m_highlightSky;

    for (uint32_t i = 0; i < 5; i++) {
        bands[i] = *DayNight::InterpColor(&temp, &light.m_colors[4 + i], &light.m_colors[4], highlight);
    }

    CImVector top = light.m_colors[3];
    top = *DayNight::DarkenColor(&temp, &top, 1.0f);

    CImVector* color = this->m_colors.m_data;
    *color++ = top;

    int32_t tess = this->m_thetaTess;
    float step = -1.0f / static_cast<float>(tess);

    for (uint32_t band = 0; band < 4; band++) {
        float facing = DayNight::g_dnInfo.m_facing * 0.15915494f + 0.25f;

        if (facing > 1.0f) {
            facing = facing - 1.0f;
        }

        for (int32_t i = 0; i < tess; i++) {
            if (facing < 0.0f) {
                facing = facing + 1.0f;
            }

            float factor = DayNight::InterpTable(s_facingTable, 6, facing);
            CImVector value;

            if (factor < 0.0f) {
                CImVector dark = *DayNight::InterpColor(&temp, &bands[band], &light.m_colors[3], highlight * 0.69999999f);
                value = *DayNight::InterpColor(&temp, &bands[band], &dark, -factor * highlight);
            } else {
                value = *DayNight::InterpColor(&temp, &light.m_colors[4 + band], &bands[band], (1.0f - factor) * highlight);
            }

            ApplyEclipse(value);
            *color++ = value;

            facing = facing + step;
            tess = this->m_thetaTess;
        }
    }

    CImVector bottom = light.m_colors[8];
    ApplyEclipse(bottom);

    for (int32_t i = 0; i < this->m_thetaTess; i++) {
        *color++ = bottom;
    }

    CImVector last = light.m_colors[8];
    ApplyEclipse(last);
    *color = last;
}

// OFFSET: 0x00651DE0
void DayNight::DNSky::GenSphere(float radius) {
    this->m_thetaTess = 24;
    this->m_verts.SetCount(168);
    this->m_colors.SetCount(168);
    this->m_indices.SetCount(300);

    C3Vector* vert = this->m_verts.m_data;
    uint16_t* index = this->m_indices.m_data;
    float prevPhi = 0.0f;
    uint16_t prevStart = 0;
    float zOffset = -cos(0.78539819f);

    for (int32_t ring = 0; ring < 7; ring++) {
        float phi = s_ringAngles[ring] * 3.1415927f;
        float cosPhi = CMath::cos_(phi);
        float sinPhi = CMath::sin_(phi);
        uint16_t start = static_cast<uint16_t>(vert - this->m_verts.m_data);
        bool top = CMath::fequal(phi, 0.0f);

        for (int32_t i = 0; i < 24; i++) {
            vert->z = cosPhi * radius + zOffset;
            double theta = static_cast<double>(i) * 0.041666668 * 6.2831855;
            vert->x = sin(theta) * sinPhi * radius;
            vert->y = sinPhi * cos(theta) * radius;
            vert++;

            if (top || CMath::fequal(phi, 3.1415927f)) {
                break;
            }
        }

        if (ring > 0) {
            bool prevTop = CMath::fequal(prevPhi, 0.0f);
            bool bottom = CMath::fequal(phi, 3.1415927f);

            for (uint32_t i = 0; i < 25; i++) {
                *index++ = (prevTop ? 0 : i % 24) + prevStart;
                *index++ = (bottom ? 0 : i % 24) + start;
            }
        }

        prevPhi = phi;
        prevStart = start;
    }

    this->m_nVerts = static_cast<uint16_t>(vert - this->m_verts.m_data);
    this->m_nIndices = 300;
}

// OFFSET: 0x00652130
void DayNight::DNSky::Render() {
    C44Matrix world;
    world.Scale(6.6666665f);

    g_theGxDevicePtr->RsPush();

    if (DNSky::s_vertexShader) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &DNSky::s_vertexShader->m_permutes[0]);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &DNSky::s_pixelShader->m_permutes[0]);

        C44Matrix view;
        GxXformView(view);
        C44Matrix proj;
        GxXformProjNative(proj);
        C44Matrix transform = world * view * proj;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
    } else {
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    }

    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_BlendingMode, 3);
    GxUpdateAlphaRef(GxRs_AlphaRef);

    GxPrimVertexPtr(this->m_nVerts, this->m_verts.m_data, sizeof(C3Vector), NULL, 0, this->m_colors.m_data, sizeof(CImVector), NULL, 0, NULL, 0);
    GxPrimIndexPtr(this->m_nIndices, this->m_indices.m_data);

    CGxBatch batch;
    batch.m_primType = GxPrim_TriangleStrip;
    batch.m_start = 0;
    batch.m_count = this->m_nIndices;
    batch.m_minIndex = 0;
    batch.m_maxIndex = this->m_nVerts - 1;
    g_theGxDevicePtr->Draw(batch);

    if (!DNSky::s_vertexShader) {
        GxXformPop(GxXform_World);
    }

    g_theGxDevicePtr->RsPop();
}
