#include "WoW/Source/WorldClient/MapFootprint.h"
#include <cstring>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPNCT2.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CFacet.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D01AAC/0x140BB16A0
static World::FacetData s_footprintFacets;

// STATIC: 0x00D01AE0
static C3Vector s_footprintNormal(0.0f, 0.0f, 1.0f);

// STATIC: 0x00E759A0/0x140D5F780
static CGxShader* s_footprintPS;

// STATIC: 0x00E759A4/0x140D5F788
static CGxShader* s_footprintVS;

// STATIC: 0x00E759A8/0x140D5F790
static uint32_t s_footprintPlayerNext;

// STATIC: 0x00E759AC/0x140D5F794
static uint32_t s_footprintNext;

// STATIC: 0x00E759B0
static CMapFootprint s_footprints[320];

// STATIC: 0x00E78BB0/0x140D5F798
static TSGrowableArray<CMapFootprintTexture> s_footprintTextures;

// OFFSET: 0x0075D4C0
static CAaBox FootprintBounds(const C2Vector& size, const C3Vector& pos) {
    float radius = size.x;

    if (radius <= size.y) {
        radius = size.y;
    }

    radius = radius * 0.66000003f;
    CAaBox box;
    box.b = pos;
    box.t = pos;
    box.b.x = box.b.x - radius;
    box.b.y = box.b.y - radius;
    box.b.z = box.b.z - radius;
    box.t.x = box.t.x + radius;
    box.t.y = box.t.y + radius;
    box.t.z = box.t.z + radius;
    return box;
}

// OFFSET: 0x0075D550
static void FootprintRotation(C44Matrix& matrix, const C2Vector& size, int32_t flip, float facing) {
    if (flip) {
        matrix.Scale(C3Vector(-1.0f, 1.0f, 1.0f));
    }

    float x = size.x;
    float y = size.y;

    if (y > x) {
        matrix.Scale(C3Vector(1.0f, y / x, 1.0f));
    } else if (x > y) {
        matrix.Scale(C3Vector(x / y, 1.0f, 1.0f));
    }

    matrix.RotateAroundZ(-facing);
}

// OFFSET: 0x0075D730/0x140441050
void CWorldMap::ClearAllFootprints() {
    for (CMapFootprint* footprint = s_footprints; footprint < &s_footprints[320]; footprint++) {
        footprint->m_textureIndex = 0;
        footprint->m_life = 0;
        footprint->m_alpha = 0;
        footprint->m_mapLink.Unlink();
        footprint->m_textureLink.Unlink();
        footprint->m_verts.SetCount(0);
    }
}

// OFFSET: 0x0075D7C0/0x140441130
void CWorldMap::FootprintDestroy() {
    if (s_footprintVS) {
        g_theGxDevicePtr->ShaderRelease(s_footprintVS);
        s_footprintVS = NULL;
    }

    if (s_footprintPS) {
        g_theGxDevicePtr->ShaderRelease(s_footprintPS);
        s_footprintPS = NULL;
    }

    CWorldMap::ClearAllFootprints();

    for (uint32_t i = 0; i < s_footprintTextures.m_count; i++) {
        if (s_footprintTextures.m_data[i].m_texture) {
            TextureRelease(s_footprintTextures.m_data[i].m_texture);
        }
    }
}

// OFFSET: 0x0075D840/0x1404411D0
void CWorldMap::NewFootprint(uint32_t textureIndex, const C2Vector& size, const C3Vector& pos, float facing, int32_t flip, uint32_t terrainType, int32_t isPlayer) {
    if (textureIndex >= s_footprintTextures.m_count) {
        return;
    }

    TerrainTypeRec* terrain = g_terrainTypeDB.GetRecord(terrainType);

    if (!terrain || !(terrain->m_flags & 0x3)) {
        return;
    }

    CAaBox box;
    memset(&box, 0, sizeof(box));
    box = FootprintBounds(size, pos);

    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (worldMap && !worldMap->QueryFacets(box, s_footprintFacets, 0x200122)) {
        return;
    }

    if (!s_footprintFacets.facets.m_count) {
        return;
    }

    CMapFootprint* footprint;

    if (isPlayer) {
        footprint = &s_footprints[s_footprintPlayerNext];

        if (++s_footprintPlayerNext >= 64) {
            s_footprintPlayerNext = 0;
        }
    } else {
        footprint = &s_footprints[s_footprintNext];

        if (++s_footprintNext >= 320) {
            s_footprintNext = 64;
        }
    }

    footprint->m_mapLink.Unlink();
    footprint->m_textureIndex = textureIndex;
    footprint->m_life = 6000;
    footprint->m_alpha = 127;

    C44Matrix texMatrix;
    C44Matrix fadeMatrix;
    C44Matrix rotation;
    FootprintRotation(rotation, size, flip, facing);
    ProjectTex2dMakeMatrices(texMatrix, fadeMatrix, box, NULL, &rotation, 0.5f, 1, 0);

    footprint->m_verts.SetCount(3 * s_footprintFacets.facets.m_count);
    CGxVertexPT2* vertex = footprint->m_verts.m_data;

    for (uint32_t i = 0; i < s_footprintFacets.facets.m_count; i++) {
        const CFacet& facet = s_footprintFacets.facets.m_data[i];

        for (uint32_t j = 0; j < 3; j++) {
            const C3Vector& point = facet.v[j];
            vertex->p = point;
            vertex->tc[0] = C2Vector(point * texMatrix);
            vertex->tc[1] = C2Vector(point * fadeMatrix);
            vertex++;
        }
    }

    this->m_footprintList.LinkToTail(footprint);
}

// OFFSET: 0x0075DCF0/0x140441900
void CWorldMap::RenderFootprints(CWorldView* view) {
    for (CMapFootprint* footprint = this->m_footprintList.Head(), * next; footprint; footprint = next) {
        next = this->m_footprintList.Next(footprint);
        footprint->m_life -= static_cast<int16_t>(FrameTime::GetTickTimeMs());

        if (footprint->m_life > 0) {
            int32_t alpha = (footprint->m_life << 8) / 6000;

            if (alpha >= 127) {
                alpha = 127;
            }

            footprint->m_alpha = static_cast<uint8_t>(alpha);
            CMapFootprintTexture& texture = s_footprintTextures.m_data[footprint->m_textureIndex];
            texture.m_footprints.LinkToTail(footprint);
            texture.m_vertexCount += footprint->m_verts.m_count;
        } else {
            footprint->m_verts.SetCount(0);
            footprint->m_mapLink.Unlink();
        }
    }

    CTexture* fadeTexture = ProjectTex2dGetModFadeTex();
    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Mod);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_DepthTest, 1);
    GxRsSet(GxRs_DepthFunc, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_PolygonOffset, static_cast<int32_t>(GxPolygonOffset_Default));
    g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(fadeTexture));

    C44Matrix world;
    world.d0 = -view->m_camPos.x;
    world.d1 = -view->m_camPos.y;
    world.d2 = -view->m_camPos.z;
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);

    if (CShaderEffect::s_enableShaders) {
        C44Matrix worldTop;
        GxXform(GxXform_World, worldTop);
        C44Matrix viewTop;
        GxXform(GxXform_View, viewTop);
        C44Matrix projection = g_theGxDevicePtr->m_projNative;

        if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
            projection.c0 = projection.c0 * -1.0f;
            projection.c1 = projection.c1 * -1.0f;
            projection.c2 = projection.c2 * -1.0f;
            projection.c3 = projection.c3 * -1.0f;
        }

        C44Matrix worldView = worldTop * viewTop;
        C3Vector sunDir(0.0f, 0.0f, 0.0f);
        CM2Lighting lighting(C3Vector(0.0f, 0.0f, 0.0f));
        lighting.AddLight(&CWorldMap::s_sunLight);

        C4Vector light[4];
        memset(light, 0, sizeof(light));

        if (lighting.GetSunLight(sunDir, *reinterpret_cast<C3Vector*>(&light[1]), *reinterpret_cast<C3Vector*>(&light[2]), *reinterpret_cast<C3Vector*>(&light[3]))) {
            C3Vector dir = C3Vector(-sunDir.x, -sunDir.y, -sunDir.z) * viewTop;
            light[0] = C4Vector(dir.x, dir.y, dir.z, 1.0f);
            light[3].w = 20.0f;
        } else {
            light[1] = C4Vector(1.0f, 1.0f, 1.0f, 1.0f);
            light[0] = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
            light[3] = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
        }

        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&worldView), 4);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 4, reinterpret_cast<const float*>(&projection), 4);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 24, reinterpret_cast<const float*>(light), 4);

        CImVector fogColor = *reinterpret_cast<const CImVector*>(&g_theGxDevicePtr->m_appRenderStates[GxRs_FogColor].m_value.m_data.u[0]);
        C4Vector fog(fogColor);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Pixel, 0, reinterpret_cast<const float*>(&fog), 1);
        GxRsSet(GxRs_VertexShader, s_footprintVS, 0);
        GxRsSet(GxRs_PixelShader, s_footprintPS, 0);
    }

    for (uint32_t i = 0; i < s_footprintTextures.m_count; i++) {
        CMapFootprintTexture& texture = s_footprintTextures.m_data[i];

        if (texture.m_vertexCount) {
            CGxTex* gxTex = TextureGetGxTex(texture.m_texture);

            if (gxTex) {
                CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPNCT2), texture.m_vertexCount);
                CGxVertexPNCT2* vertex = reinterpret_cast<CGxVertexPNCT2*>(g_theGxDevicePtr->BufLock(vertexBuf));
                CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), texture.m_vertexCount);
                uint16_t* index = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
                uint16_t nextIndex = 0;

                for (CMapFootprint* footprint = texture.m_footprints.Head(), * next; footprint; footprint = next) {
                    next = texture.m_footprints.Next(footprint);
                    CImVector color;
                    color.b = 0xFF;
                    color.g = 0xFF;
                    color.r = 0xFF;
                    color.a = footprint->m_alpha;
                    const CGxVertexPT2* source = footprint->m_verts.m_data;

                    for (uint32_t j = 0; j < footprint->m_verts.m_count; j++) {
                        vertex->p = source->p;
                        vertex->n = s_footprintNormal;
                        vertex->c = color;
                        vertex->tc[0] = source->tc[0];
                        vertex->tc[1] = source->tc[1];
                        *index++ = nextIndex++;
                        source++;
                        vertex++;
                    }

                    footprint->m_textureLink.Unlink();
                }

                g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
                vertexBuf->unk_001C = 1;
                g_theGxDevicePtr->BufUnlock(indexBuf, 0);
                indexBuf->unk_001C = 1;
                g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PNCT2);
                g_theGxDevicePtr->PrimIndexPtr(indexBuf);
                g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);

                CGxBatch batch;
                batch.m_primType = GxPrim_Triangles;
                batch.m_start = 0;
                batch.m_count = texture.m_vertexCount;
                batch.m_minIndex = 0;
                batch.m_maxIndex = static_cast<uint16_t>(texture.m_vertexCount - 1);
                g_theGxDevicePtr->Draw(batch);
            }
        }

        texture.m_vertexCount = 0;
    }

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x0075ECB0/0x140442C00
void CWorldMap::FootprintInitialize() {
    CStatus status;
    s_footprintNext = 64;
    s_footprintPlayerNext = 0;

    for (int32_t i = 0; i < 320; i++) {
        CMapFootprint* footprint = &s_footprints[i];
        footprint->m_textureIndex = 0;
        footprint->m_life = 0;
        footprint->m_alpha = 0;
    }

    CGxTexFlags flags;
    s_footprintTextures.SetCount(g_footprintTexturesDB.m_maxID + 1);

    for (int32_t i = g_footprintTexturesDB.m_numRecords; i; ) {
        i--;
        FootprintTexturesRec* rec = g_footprintTexturesDB.GetRecordByIndex(i);
        CMapFootprintTexture& texture = s_footprintTextures.m_data[rec->m_id];

        if (!texture.m_texture) {
            texture.m_texture = TextureCreate(rec->m_footstepFilename, flags, &status, 1, NULL, 2);
        }
    }

    s_footprintVS = NULL;

    if (CShaderEffect::s_enableShaders) {
        s_footprintVS = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "FootPrint");
        s_footprintPS = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "FootPrint");
    }
}
