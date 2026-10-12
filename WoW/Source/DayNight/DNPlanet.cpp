#include "WoW/Source/DayNight/DNPlanet.h"
#include <cstring>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DayNight/DayNight.h"

// STATIC: 0x00CEFCD4/0x140B903E0
static uint16_t s_quadIndices[4] = { 0, 1, 2, 3 };

// STATIC: 0x00CEFCDC/0x140B903E8
static uint16_t s_clipIndices[6] = { 0, 1, 4, 5, 2, 3 };

// STATIC: 0x00E4327C/0x140D2C020
uint32_t DayNight::DNPlanet::s_refCount;

// STATIC: 0x00E43280/0x140D2C028
CGxShader* DayNight::DNPlanet::s_vertexShader;

// STATIC: 0x00E43284/0x140D2C030
CGxShader* DayNight::DNPlanet::s_pixelShader;

// OFFSET: 0x00650C80/0x1402F0670
int32_t DayNight::DNPlanet::IsAllLoaded() {
    return TextureIsLoaded(this->m_texture);
}

// OFFSET: 0x00650C90/0x1402F0680
void DayNight::DNPlanet::LinkToAsyncHead() {
    if (this->m_texture) {
        TextureIncreasePriority(this->m_texture);
    }
}

// OFFSET: 0x00650CA0/0x1402F06A0
void DayNight::DNPlanet::Destroy() {
    if (this->m_texture) {
        TextureRelease(this->m_texture);
    }

    if (--DNPlanet::s_refCount) {
        return;
    }

    if (DNPlanet::s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(DNPlanet::s_vertexShader);
        DNPlanet::s_vertexShader = NULL;
    }

    if (DNPlanet::s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(DNPlanet::s_pixelShader);
        DNPlanet::s_pixelShader = NULL;
    }
}

// OFFSET: 0x00650D10/0x1402F0710
void DayNight::DNPlanet::GenGeometry(C3Vector* verts, C2Vector* texCoords, CImVector* colors, uint16_t* indices, uint32_t& vertCount, uint32_t& indexCount) {
    static C3Vector s_verts[6] = {
        C3Vector(0.0f, -0.5f, 0.5f),
        C3Vector(0.0f, 0.5f, 0.5f),
        C3Vector(0.0f, -0.5f, -0.5f),
        C3Vector(0.0f, 0.5f, -0.5f),
        C3Vector(0.0f, -0.5f, 99.0f),
        C3Vector(0.0f, 0.5f, 99.0f),
    };

    static C2Vector s_texCoords[6] = {
        C2Vector(0.0f, 0.0f),
        C2Vector(1.0f, 0.0f),
        C2Vector(0.0f, 1.0f),
        C2Vector(1.0f, 1.0f),
        C2Vector(0.0f, 99.0f),
        C2Vector(1.0f, 99.0f),
    };

    for (uint32_t i = 0; i < 6; i++) {
        verts[i].x = s_verts[i].x * this->m_scale;
        verts[i].y = s_verts[i].y * this->m_scale;
        verts[i].z = s_verts[i].z * this->m_scale;
        texCoords[i] = s_texCoords[i];
        colors[i] = this->m_color;
    }

    vertCount = 4;
    memcpy(indices, s_quadIndices, sizeof(s_quadIndices));
    indexCount = 4;
}

// OFFSET: 0x006510D0/0x1402F0AD0
void DayNight::DNPlanet::ClipGeometry(C3Vector* verts, C2Vector* texCoords, CImVector* colors, uint16_t* indices, uint32_t& vertCount, uint32_t& indexCount) {
    vertCount = 0;

    float height = this->m_position.z - DayNight::g_dnInfo.m_camPos.z;
    float top = verts[0].z + height;
    float bottom = verts[2].z + height;

    if (top > 0.0f && bottom > 0.0f) {
        vertCount = 4;
    } else {
        if (top < 0.0f && bottom < 0.0f) {
            return;
        }

        vertCount = 4;
        float t = top / (top - bottom);
        float z = (verts[2].z - verts[0].z) * t + verts[0].z;
        verts[2].z = z;
        verts[3].z = z;
        texCoords[2].y = t;
        texCoords[3].y = t;
    }

    float fadeTop = (verts[0].z + height) - 0.40000001f;
    float fadeBottom = (verts[2].z + height) - 0.40000001f;

    if (fadeTop > 0.001f && fadeBottom < 0.001f) {
        vertCount = 6;
        indexCount = 6;
        memcpy(indices, s_clipIndices, sizeof(s_clipIndices));

        float t = fadeTop / (fadeTop - fadeBottom);
        float z = (verts[2].z - verts[0].z) * t + verts[0].z;
        float v = (texCoords[2].y - texCoords[0].y) * t + texCoords[0].y;
        verts[4].z = z;
        verts[5].z = z;
        texCoords[4].y = v;
        texCoords[5].y = v;
    }

    for (uint32_t i = 0; i < vertCount; i++) {
        float fade = (verts[i].z + height) - 0.40000001f;

        if (fade < 0.001f) {
            float alpha = (0.40000001f - -fade) * 2.5f;

            if (alpha < 0.0f) {
                alpha = 0.0f;
            } else if (alpha >= 1.0f) {
                alpha = 1.0f;
            }

            colors[i].a = static_cast<uint8_t>(static_cast<int32_t>(alpha * 255.0f));
        }
    }
}

// OFFSET: 0x006512C0/0x1402F0CD0
void DayNight::DNPlanet::Render() {
    C3Vector verts[6];
    C2Vector texCoords[6];
    CImVector colors[6];
    uint16_t indices[8];
    uint32_t vertCount;
    uint32_t indexCount;

    this->GenGeometry(verts, texCoords, colors, indices, vertCount, indexCount);
    this->ClipGeometry(verts, texCoords, colors, indices, vertCount, indexCount);

    if (!vertCount) {
        return;
    }

    CGxTex* texture = TextureGetGxTex(this->m_texture);

    if (!texture) {
        return;
    }

    g_theGxDevicePtr->RsPush();

    C44Matrix view;
    GxXformView(view);

    C3Vector forward(view.a2, view.b2, view.c2);
    C44Matrix world;
    DayNight::Billboard(forward, world);
    world.d0 = this->m_position.x - DayNight::g_dnInfo.m_camPos.x;
    world.d1 = this->m_position.y - DayNight::g_dnInfo.m_camPos.y;
    world.d2 = this->m_position.z - DayNight::g_dnInfo.m_camPos.z;

    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);

    if (DNPlanet::s_vertexShader) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &DNPlanet::s_vertexShader->m_permutes[0]);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &DNPlanet::s_pixelShader->m_permutes[0]);

        C44Matrix proj;
        GxXformProjNative(proj);
        C44Matrix transform = world * view * proj;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
    } else {
        g_theGxDevicePtr->XformPush(GxXform_World);
        g_theGxDevicePtr->m_xforms[GxXform_World].Identity();
        g_theGxDevicePtr->XformSetView(world * view);
        GxRsSet(GxRs_ColorOp0, 0);
        GxRsSet(GxRs_AlphaOp0, 0);
    }

    GxPrimVertexPtr(vertCount, verts, sizeof(C3Vector), NULL, 0, colors, sizeof(CImVector), texCoords, sizeof(C2Vector), NULL, 0);
    GxPrimIndexPtr(indexCount, indices);

    CGxBatch batch;
    batch.m_primType = GxPrim_TriangleStrip;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = vertCount - 1;
    g_theGxDevicePtr->Draw(batch);

    if (!DNPlanet::s_vertexShader) {
        GxXformPop(GxXform_World);
        g_theGxDevicePtr->XformSetView(view);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00651910/0x1402F15A0
void DayNight::DNPlanet::Initialize(const char* fileName) {
    CStatus status;
    this->m_texture = TextureCreate(fileName, CGxTexFlags(), &status, 0, NULL, 2);

    if (!DNPlanet::s_refCount++ && g_theGxDevicePtr->m_caps.m_shaderModel >= 3) {
        DNPlanet::s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "DNPlanet");
        DNPlanet::s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "DNPlanet");
    }
}
