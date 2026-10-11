#include "WoW/Source/WorldClient/MapObj.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Common/Types.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/World.h"

struct SMOShaderInfo {
    uint32_t m_fixedTextureCount;
    uint32_t m_shaderTextureCount;
    uint32_t m_texCoordCount;
    uint32_t m_colorCount;
};

// STATIC: 0x00D03F98
static const SMOShaderInfo s_shaderInfo[15] = {
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 2, 1, 1 },
    { 1, 1, 1, 1 },
    { 1, 2, 1, 1 },
    { 1, 2, 2, 2 },
    { 1, 3, 2, 2 },
    { 1, 2, 1, 2 },
    { 1, 2, 2, 2 },
    { 1, 1, 1, 1 },
    { 1, 3, 2, 2 },
    { 1, 3, 2, 2 },
    { 1, 2, 2, 2 },
    { 1, 1, 1, 1 },
};

// STATIC: 0x00EB3254
static TSGrowableArray<C4Vector> s_tangentScratch;

// PROVEN: 738f2391 IDENTICAL/NONE
// OFFSET: 0x007940D0/0x1404867D0
void CMapObj::DestroyMaterial(uint32_t materialId) {
    CMapObjMaterial* material = &this->m_runtimeMaterials[materialId];

    if (--material->m_refCount > 0) {
        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (material->m_textures[i]) {
            TextureRelease(material->m_textures[i]);
            material->m_textures[i] = NULL;
        }
    }
}

// OFFSET: 0x00794120/0x140486840
void CMapObjGroup::FixColorVertexAlpha() {
    uint32_t firstExterior = this->m_transBatchCount ? this->m_batches[this->m_transBatchCount - 1].m_maxIndex + 1 : 0;

    for (uint32_t i = 0; i < this->m_colorCount; i++) {
        uint8_t* color = reinterpret_cast<uint8_t*>(&this->m_colors[i]);

        if (i < firstExterior) {
            color[2] >>= 1;
            color[1] >>= 1;
            color[0] >>= 1;
        } else {
            uint32_t alpha = color[3];
            uint32_t b = (color[2] + ((alpha * color[2]) >> 6)) >> 1;
            uint32_t g = (color[1] + ((alpha * color[1]) >> 6)) >> 1;
            uint32_t r = (color[0] + ((alpha * color[0]) >> 6)) >> 1;
            color[2] = b > 0xFF ? 0xFF : b;
            color[1] = g > 0xFF ? 0xFF : g;
            color[0] = r > 0xFF ? 0xFF : r;
            color[3] = 0xFF;
        }
    }
}

// OFFSET: 0x007942C0/0x140486A50
void CMapObj::AsyncFailedCallback(CAsyncObject* object) {
    CMapObj* mapObj = static_cast<CMapObj*>(object->m_userArg);

    if (!object->m_cancelled) {
        AsyncFileReadSetStreamingError();
    }

    AsyncFileReadDestroyObject(object);
    mapObj->m_flags |= 0x40000;
    mapObj->m_asyncObject = NULL;
    World::SetLoadFailed(1, mapObj->m_name);
}

// OFFSET: 0x00794310/0x140486AB0
int32_t CMapObj::CreatePointers() {
    uint8_t* data = this->m_data;
    uint8_t* end = this->m_data + this->m_dataSize;

    while (data < end) {
        uint32_t token = reinterpret_cast<uint32_t*>(data)[0];
        uint32_t size = reinterpret_cast<uint32_t*>(data)[1];
        uint8_t* chunk = data + 8;

        switch (token) {
            case 'MVER':
                if (*reinterpret_cast<uint32_t*>(chunk) != 17) {
                    return 0;
                }

                break;

            case 'MOHD':
                this->m_header = reinterpret_cast<SMOHeader*>(chunk);
                break;

            case 'MOTX':
                this->m_textureNames = reinterpret_cast<const char*>(chunk);
                this->m_textureNamesSize = size;
                break;

            case 'MOMT':
                this->m_materials = reinterpret_cast<SMOMaterial*>(chunk);
                this->m_materialCount = size / sizeof(SMOMaterial);
                break;

            case 'MOGN':
                this->m_groupNames = reinterpret_cast<const char*>(chunk);
                this->m_groupNamesSize = size;
                break;

            case 'MOGI':
                this->m_groupInfos = reinterpret_cast<SMOGroupInfo*>(chunk);
                this->m_groupCount = size / sizeof(SMOGroupInfo);
                break;

            case 'MOSB':
                this->m_skyboxName = reinterpret_cast<const char*>(chunk);

                if (!*chunk) {
                    this->m_skyboxName = NULL;
                }

                if (!this->m_skyboxName) {
                    for (uint32_t i = 0; i < this->m_groupCount; i++) {
                        this->m_groupInfos[i].m_flags &= ~0x40000;
                    }
                }

                break;

            case 'MOPV':
                this->m_portalVertices = reinterpret_cast<C3Vector*>(chunk);
                this->m_portalVertexCount = size / sizeof(C3Vector);
                break;

            case 'MOPT':
                this->m_portalInfos = reinterpret_cast<SMOPortalInfo*>(chunk);
                this->m_portalCount = size / sizeof(SMOPortalInfo);

                for (uint32_t i = 0; i < this->m_portalCount; i++) {
                    if (std::isnan(this->m_portalInfos[i].m_plane.d)) {
                        this->m_portalInfos[i].m_plane.n.x = 0.0f;
                        this->m_portalInfos[i].m_plane.n.y = 0.0f;
                        this->m_portalInfos[i].m_plane.n.z = 1.0f;
                        this->m_portalInfos[i].m_plane.d = 800000.0f;
                    }
                }

                break;

            case 'MOPR':
                this->m_portalRefs = reinterpret_cast<SMOPortalRef*>(chunk);
                this->m_portalRefCount = size / sizeof(SMOPortalRef);
                break;

            case 'MOVV':
                this->m_visBlockVertices = reinterpret_cast<C3Vector*>(chunk);
                this->m_visBlockVertexCount = size / sizeof(C3Vector);
                break;

            case 'MOVB':
                this->m_visBlocks = reinterpret_cast<SMOVisibleBlock*>(chunk);
                this->m_visBlockCount = size / sizeof(SMOVisibleBlock);
                break;

            case 'MOLT':
                this->m_lights = reinterpret_cast<SMOLight*>(chunk);
                this->m_lightCount = size / sizeof(SMOLight);
                break;

            case 'MODS':
                this->m_doodadSets = reinterpret_cast<SMODoodadSet*>(chunk);
                this->m_doodadSetCount = size / sizeof(SMODoodadSet);
                break;

            case 'MODN':
                this->m_doodadNames = reinterpret_cast<const char*>(chunk);
                this->m_doodadNamesSize = size;
                break;

            case 'MODD':
                this->m_doodadDefs = reinterpret_cast<SMODoodadDef*>(chunk);
                this->m_doodadDefCount = size / sizeof(SMODoodadDef);
                break;

            case 'MFOG':
                this->m_fogs = reinterpret_cast<SMOFog*>(chunk);
                this->m_fogCount = size / sizeof(SMOFog);
                break;

            case 'MCVP':
                this->m_convexVolumePlanes = reinterpret_cast<C4Plane*>(chunk);
                this->m_convexVolumePlaneCount = size / sizeof(C4Plane);
                break;
        }

        data = chunk + size;
    }

    return data == end;
}

// OFFSET: 0x00794680/0x140486EB0
void CMapObj::CreateMaterials() {
    if (!this->m_materialCount) {
        return;
    }

    this->m_runtimeMaterials = static_cast<CMapObjMaterial*>(SMemAlloc(sizeof(CMapObjMaterial) * this->m_materialCount, __FILE__, __LINE__, 0));

    for (uint32_t i = 0; i < this->m_materialCount; i++) {
        this->m_runtimeMaterials[i].m_refCount = 0;
        this->m_runtimeMaterials[i].m_textures[0] = NULL;
        this->m_runtimeMaterials[i].m_textures[1] = NULL;
        this->m_runtimeMaterials[i].m_textures[2] = NULL;
    }
}

// OFFSET: 0x00794710/0x140486F60
void CMapObj::CreateMaterial(uint32_t materialId) {
    CMapObjMaterial* runtime = &this->m_runtimeMaterials[materialId];
    SMOMaterial* material = &this->m_materials[materialId];

    if (++runtime->m_refCount > 1) {
        return;
    }

    const char* names[3];
    names[0] = this->m_textureNames + material->m_texture1;
    names[1] = this->m_textureNames + material->m_texture2;
    names[2] = this->m_textureNames + material->m_texture3;

    if (!*names[0]) {
        names[0] = "createcrappygreentexture.blp";
    }

    const SMOShaderInfo* info = &s_shaderInfo[material->m_shader];
    uint32_t count = CShaderEffect::s_enableShaders ? info->m_shaderTextureCount : info->m_fixedTextureCount;
    uint32_t i = 0;

    for (; i < count; i++) {
        if (!*names[i]) {
            material->m_shader = 4;
            i = 1;
            break;
        }
    }

    if (i < 3) {
        memset(&names[i], 0, sizeof(names[0]) * (3 - i));
    }

    char specularName[260];

    if (material->m_shader == 8 && names[1]) {
        strcpy(specularName, names[1]);
        strcpy(strrchr(specularName, '.'), "_s.blp");
        names[1] = specularName;
    }

    for (i = 0; i < 3; i++) {
        runtime->m_textures[i] = names[i] ? CMap::LoadTexture(names[i]) : NULL;
    }
}

// OFFSET: 0x00794860/0x140487110
void CMapObjGroup::AsyncFailedCallback(CAsyncObject* object) {
    CMapObjGroup* group = static_cast<CMapObjGroup*>(object->m_userArg);

    if (!object->m_cancelled) {
        AsyncFileReadSetStreamingError();
    }

    AsyncFileReadDestroyObject(object);
    group->m_flags |= 0x40000;
    group->m_asyncObject = NULL;

    char name[260];
    strcpy(name, group->m_parent->m_name);
    char chunk[32];
    SStrPrintf(chunk, sizeof(chunk), " - Chunk %d", group->m_groupIndex);
    strcat(name, chunk);
    World::SetLoadFailed(1, name);
}

// OFFSET: 0x00794910/0x1404871D0
uint32_t CMapObjGroup::GetLiquidType(uint32_t legacyType) {
    if (legacyType >= 21 || !legacyType) {
        return legacyType;
    }

    switch ((legacyType - 1) & 3) {
        case 0:
            return (this->m_groupFlags & 0x80000) ? 14 : 13;

        case 1:
            return 14;

        case 2:
            return 19;

        case 3:
            return 20;
    }

    return legacyType;
}

// OFFSET: 0x00794980/0x140487220
void CMapObjGroup::CreateDataPointers(uint8_t* data, uint8_t* end) {
    CAaBspNode* bspNodes = NULL;
    uint32_t bspNodeCount = 0;
    uint16_t* bspRefs = NULL;
    uint32_t bspRefCount = 0;

    this->m_colors = NULL;
    this->m_texCoords = NULL;

    while (data < end) {
        uint32_t token = reinterpret_cast<uint32_t*>(data)[0];
        uint32_t size = reinterpret_cast<uint32_t*>(data)[1];
        uint8_t* chunk = data + 8;

        switch (token) {
            case 'MOPY':
                this->m_polys = reinterpret_cast<SMOPoly*>(chunk);
                this->m_polyCount = size / sizeof(SMOPoly);
                break;

            case 'MOVI':
                this->m_indices = reinterpret_cast<uint16_t*>(chunk);
                this->m_indexCount = size / sizeof(uint16_t);
                break;

            case 'MORI':
                this->m_stripIndices = reinterpret_cast<uint16_t*>(chunk);
                this->m_stripIndexCount = size / sizeof(uint16_t);
                break;

            case 'MOVT':
                this->m_vertices = reinterpret_cast<C3Vector*>(chunk);
                this->m_vertexCount = size / sizeof(C3Vector);
                break;

            case 'MONR':
                this->m_normals = reinterpret_cast<C3Vector*>(chunk);
                this->m_normalCount = size / sizeof(C3Vector);
                break;

            case 'MOTV':
                if (this->m_texCoords) {
                    this->m_texCoords2 = reinterpret_cast<C2Vector*>(chunk);
                    this->m_texCoord2Count = size / sizeof(C2Vector);
                } else {
                    this->m_texCoords = reinterpret_cast<C2Vector*>(chunk);
                    this->m_texCoordCount = size / sizeof(C2Vector);
                }

                break;

            case 'MOBA':
                this->m_batches = reinterpret_cast<SMOBatch*>(chunk);
                this->m_batchCount = size / sizeof(SMOBatch);
                break;

            case 'MOBS':
                this->m_shadowBatches = reinterpret_cast<SMOBatch*>(chunk);
                this->m_shadowBatchCount = size / sizeof(SMOBatch);
                break;

            case 'MORB':
                this->m_stripBatches = reinterpret_cast<SMOStripBatch*>(chunk);

                if (CMap::enableTriangleStrips) {
                    for (uint32_t i = 0; i < this->m_batchCount; i++) {
                        this->m_batches[i].m_startIndex = this->m_stripBatches[i].m_startIndex;
                        this->m_batches[i].m_count = this->m_stripBatches[i].m_count;
                    }
                }

                break;

            case 'MOLR':
                this->m_lightRefs = reinterpret_cast<uint16_t*>(chunk);
                this->m_lightRefCount = size / sizeof(uint16_t);
                break;

            case 'MODR':
                this->m_doodadRefs = reinterpret_cast<uint16_t*>(chunk);
                this->m_doodadRefCount = size / sizeof(uint16_t);
                break;

            case 'MOCV':
                if (this->m_colors || !(this->m_groupFlags & 0x4)) {
                    this->m_colors2 = reinterpret_cast<CImVector*>(chunk);
                    this->m_color2Count = size / sizeof(CImVector);
                } else {
                    this->m_colors = reinterpret_cast<CImVector*>(chunk);
                    this->m_colorCount = size / sizeof(CImVector);

                    if (!(this->m_parent->m_header->m_flags & 0x8)) {
                        this->FixColorVertexAlpha();
                    }
                }

                break;

            case 'MOTA':
                this->m_tangentStarts = reinterpret_cast<uint16_t*>(chunk);
                this->m_tangents = reinterpret_cast<C4Vector*>(chunk + sizeof(uint16_t) * this->m_batchCount);
                break;

            case 'MOBN':
                bspNodes = reinterpret_cast<CAaBspNode*>(chunk);
                bspNodeCount = size / sizeof(CAaBspNode);
                break;

            case 'MOBR':
                bspRefs = reinterpret_cast<uint16_t*>(chunk);
                bspRefCount = size / sizeof(uint16_t);
                break;

            case 'MLIQ': {
                const uint32_t* header = reinterpret_cast<const uint32_t*>(chunk);
                this->m_liquidVerts.x = header[0];
                this->m_liquidVerts.y = header[1];
                this->m_liquidTiles.x = header[2];
                this->m_liquidTiles.y = header[3];
                this->m_liquidCorner.x = reinterpret_cast<const float*>(header)[4];
                this->m_liquidCorner.y = reinterpret_cast<const float*>(header)[5];
                this->m_liquidCorner.z = reinterpret_cast<const float*>(header)[6];
                this->m_liquidMaterialId = *reinterpret_cast<const uint16_t*>(chunk + 28);

                uint32_t vertexCount = this->m_liquidVerts.x * this->m_liquidVerts.y;
                this->m_liquidVertices = reinterpret_cast<SMOLiquidVert*>(chunk + 30);
                this->m_liquidTileFlags = chunk + 30 + sizeof(SMOLiquidVert) * vertexCount;

                if (!this->m_liquidType) {
                    this->m_liquidType = this->GetLiquidType(this->IdentifyLegacyLiquidType());
                }

                CMapObjGroup::AllocVertArray(this->m_liquidVertArray, vertexCount);
                this->GenLiquidVerts();
                break;
            }
        }

        data = chunk + size;
    }

    if (bspNodes) {
        this->m_bsp.Set(bspNodes, bspNodeCount, bspRefs, bspRefCount, this->m_bbox);
    }
}

// OFFSET: 0x00794D50/0x140487690
void CMapObj::DistFromClosestExtPortal(uint32_t depth, uint32_t flags, const CMapObjGroup* group, const CMapObjGroup* fromGroup, const C3Vector& pos, float& dist) {
    if (depth > 3) {
        return;
    }

    for (uint32_t i = 0; i < group->m_portalCount; i++) {
        const SMOPortalRef* ref = &this->m_portalRefs[group->m_portalStart + i];
        uint16_t groupIndex = ref->m_groupIndex;
        CMapObjGroup* other = this->m_groups[groupIndex];

        if (!other || other == fromGroup) {
            continue;
        }

        if (flags & this->m_groupInfos[groupIndex].m_flags) {
            const SMOPortalInfo* info = &this->m_portalInfos[ref->m_portalIndex];
            float d = NTempest::DistanceFromPolygon(pos, &this->m_portalVertices[info->m_startVertex], info->m_count, info->m_plane);

            if (d < 25.0f && dist > d) {
                dist = d;
            }
        } else {
            this->DistFromClosestExtPortal(depth + 1, flags, other, group, pos, dist);
        }
    }
}

// OFFSET: 0x00795290/0x140487E30
float CMapObj::DistFromClosestExtPortal(CMapObjGroup* group, const C3Vector& pos, int32_t outdoor) {
    uint32_t flags = 0x8;

    if (outdoor) {
        flags = 0x48;
    }

    float dist = FLT_MAX;
    this->DistFromClosestExtPortal(0, flags, group, NULL, pos, dist);
    return dist;
}

// OFFSET: 0x00794E50/0x140487810
void CMapObj::AttenTransVerts(CMapObjGroup* group) {
    group->m_renderFlags |= 0x1;

    if (!group->m_transBatchCount) {
        return;
    }

    uint32_t lastVertex = group->m_batches[group->m_transBatchCount - 1].m_maxIndex;

    for (uint32_t i = 0; i <= lastVertex; i++) {
        const C3Vector& vertex = group->m_vertices[i];
        uint8_t* color = reinterpret_cast<uint8_t*>(&group->m_colors[i]);
        float atten = 0.0f;

        for (uint32_t j = 0; j < group->m_portalCount; j++) {
            const SMOPortalRef* ref = &this->m_portalRefs[group->m_portalStart + j];
            const SMOPortalInfo* portal = &this->m_portalInfos[ref->m_portalIndex];
            const C4Plane& plane = portal->m_plane;
            C3Vector point = vertex;
            float side = plane.n.x * vertex.x + plane.n.y * vertex.y + plane.n.z * vertex.z + plane.d;

            if (side > 0.001f || side < -0.001f) {
                C3Vector end;

                if (side > 0.001f) {
                    end.x = vertex.x - plane.n.x;
                    end.y = vertex.y - plane.n.y;
                    end.z = vertex.z - plane.n.z;
                } else {
                    end.x = vertex.x + plane.n.x;
                    end.y = vertex.y + plane.n.y;
                    end.z = vertex.z + plane.n.z;
                }

                C3Ray ray = C3Ray::FromStartEnd(vertex, end, false);
                NTempest::Intersect(ray, plane, NULL, &point, 0.0099999998f);
            }

            const C3Vector* verts = &this->m_portalVertices[portal->m_startVertex];
            float dist;

            if (NTempest::Intersect(point, verts, portal->m_count, plane.n.MajorAxis())) {
                dist = plane.n.x * vertex.x + plane.n.y * vertex.y + plane.n.z * vertex.z + plane.d;

                if (ref->m_side != 1) {
                    dist *= -1.0f;
                }
            } else {
                dist = NTempest::DistanceFromPolygonEdge(vertex, verts, portal->m_count);
            }

            if (this->m_groupInfos[ref->m_groupIndex].m_flags & 0x48) {
                if (dist < 0.0f) {
                    dist = 0.0f;
                }

                float amount = 1.0f - dist * 0.15000001f;

                if (amount > 0.001f) {
                    atten += amount;
                }
            } else if (dist > -1.0f && dist < 1.0f) {
                atten = 0.0f;
                goto apply;
            }
        }

        if (atten <= 0.001f) {
            atten = 0.0f;
        } else if (atten > 1.0f) {
            atten = 1.0f;
        }

    apply:
        color[2] = static_cast<int32_t>((127.0f - static_cast<float>(color[2])) * atten + static_cast<float>(color[2]));
        color[1] = static_cast<int32_t>((127.0f - static_cast<float>(color[1])) * atten + static_cast<float>(color[1]));
        color[0] = static_cast<int32_t>((127.0f - static_cast<float>(color[0])) * atten + static_cast<float>(color[0]));
        color[3] = static_cast<int32_t>(atten * 255.0f);
    }
}

// OFFSET: 0x007951F0/0x140487D60
int32_t CMapObj::Create() {
    int32_t created = this->CreatePointers();
    this->CreateMaterials();

    if (!created) {
        return 0;
    }

    this->m_ambColor = this->m_header->m_ambColor;
    this->m_bbox = this->m_header->m_bbox;
    this->m_mapObjGroupCount = this->m_groupCount;

    for (uint32_t i = 0; i < this->m_groupCount; i++) {
        this->m_groups[i] = CMap::AllocMapObjGroup();
    }

    this->m_flags |= 0x40;
    return 1;
}

// OFFSET: 0x00795440/0x140488050
void CMapObj::AsyncCallback(CAsyncObject* object) {
    CMapObj* mapObj = static_cast<CMapObj*>(object->m_userArg);
    AsyncFileReadDestroyObject(object);
    mapObj->m_asyncObject = NULL;

    if (!mapObj->Create()) {
        World::SetLoadFailed(1, mapObj->m_name);
    }
}

// OFFSET: 0x00795480/0x140488090
int32_t CMapObj::Read(const char* name) {
    strcpy(this->m_name, name);

    SFileImpl* file = NULL;

    if (CMap::SafeOpen(name, &file, 0)) {
        strcpy(this->m_name, name);
    } else {
        CMap::SafeOpen("world\\wmo\\Dungeon\\test\\missingwmo.wmo", &file, 1);
        strcpy(this->m_name, "world\\wmo\\Dungeon\\test\\missingwmo.wmo");
    }

    this->m_dataSize = SFileGetFileSize(file, NULL);
    this->m_data = static_cast<uint8_t*>(SMemAlloc(this->m_dataSize, __FILE__, __LINE__, 0));
    AsyncFileReadCreateObject(file, 3, 0, &this->m_asyncObject, this->m_data, this, CMapObj::AsyncCallback, CMapObj::AsyncFailedCallback, 0, 0, 0);

    return 1;
}

// OFFSET: 0x00795560/0x1404881D0
void CMapObjGroup::ComputeTangents(const SMOBatch* batch, const uint16_t* indices, const C3Vector* vertices, const C3Vector* normals, const C2Vector* texCoords, C4Vector* tangents) {
    uint32_t vertexCount = batch->m_maxIndex - batch->m_minIndex + 1;
    uint32_t scratchIndex = 2 * vertexCount;

    if (scratchIndex >= s_tangentScratch.Count()) {
        uint32_t oldCount = s_tangentScratch.Count();
        s_tangentScratch.SetCount(scratchIndex + 1);

        for (uint32_t i = oldCount; i <= scratchIndex; i++) {
            s_tangentScratch.m_data[i] = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    C4Vector* sTangents = s_tangentScratch.m_data;
    C4Vector* sBitangents = s_tangentScratch.m_data + vertexCount;

    for (uint32_t i = batch->m_startIndex; i < batch->m_startIndex + batch->m_count; i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        const C3Vector& p0 = vertices[i0];
        float e1x = vertices[i1].x - p0.x;
        float e1y = vertices[i1].y - p0.y;
        float e1z = vertices[i1].z - p0.z;
        float e2x = vertices[i2].x - p0.x;
        float e2y = vertices[i2].y - p0.y;
        float e2z = vertices[i2].z - p0.z;

        const C2Vector& uv0 = texCoords[i0];
        float du1 = texCoords[i1].x - uv0.x;
        float dv1 = texCoords[i1].y - uv0.y;
        float du2 = texCoords[i2].x - uv0.x;
        float dv2 = texCoords[i2].y - uv0.y;

        float det = dv2 * du1 - dv1 * du2;

        if (std::isnan(det)) {
            det = 0.0f;
        }

        float tx = 1.0f;
        float ty = 0.0f;
        float tz = 0.0f;
        float bx = 0.0f;
        float by = 1.0f;
        float bz = 0.0f;

        if (det != 0.0f) {
            float inv = 1.0f / det;
            tz = (dv2 * e1z - dv1 * e2z) * inv;
            tx = (dv2 * e1x - dv1 * e2x) * inv;
            ty = (dv2 * e1y - dv1 * e2y) * inv;
            bx = (du1 * e2x - du2 * e1x) * inv;
            by = (du1 * e2y - du2 * e1y) * inv;
            bz = (du1 * e2z - du2 * e1z) * inv;
        }

        uint32_t corners[3] = { i0, i1, i2 };

        for (uint32_t c = 0; c < 3; c++) {
            C4Vector& t = sTangents[corners[c] - batch->m_minIndex];
            t.x += tx;
            t.y += ty;
            t.z += tz;
        }

        for (uint32_t c = 0; c < 3; c++) {
            C4Vector& b = sBitangents[corners[c] - batch->m_minIndex];
            b.x += bx;
            b.y += by;
            b.z += bz;
        }
    }

    for (uint32_t v = batch->m_minIndex; v <= batch->m_maxIndex; v++) {
        C4Vector& t = sTangents[v - batch->m_minIndex];
        C4Vector& b = sBitangents[v - batch->m_minIndex];

        if (t.x * t.x + t.y * t.y + t.z * t.z < 0.001f || b.x * b.x + b.y * b.y + b.z * b.z < 0.001f) {
            t.x = 1.0f;
            t.y = 0.0f;
            t.z = 0.0f;
            b.x = 0.0f;
            b.y = 1.0f;
            b.z = 0.0f;
        }

        const C3Vector& n = normals[v];
        float d = t.x * n.x + t.y * n.y + t.z * n.z;
        float ox = t.x - n.x * d;
        float oy = t.y - n.y * d;
        float oz = t.z - n.z * d;
        float handedness = b.z * (t.y * n.x - t.x * n.y) + b.y * (t.x * n.z - t.z * n.x) + b.x * (t.z * n.y - t.y * n.z);
        float w;

        if (static_cast<float>(handedness < 0.0f) != 0.0f) {
            w = -1.0f;
        } else {
            ox = n.y;
            oy = n.x;
            oz = n.z;
            w = 1.0f;
        }

        C4Vector& out = tangents[v - batch->m_minIndex];
        out.w = w;
        float scale = 1.0f / sqrtf(oz * oz + oy * oy + ox * ox);
        out.x = scale * ox;
        out.y = oy * scale;
        out.z = oz * scale;
    }
}

// OFFSET: 0x00795B60
void CMapObjGroup::CreateOccluders() {
    for (uint32_t i = 0; i < this->m_polyCount; i++) {
        const C3Vector* verts[3];
        verts[0] = &this->m_vertices[this->m_indices[3 * i]];
        verts[1] = &this->m_vertices[this->m_indices[3 * i + 1]];
        verts[2] = &this->m_vertices[this->m_indices[3 * i + 2]];

        float average = (verts[0]->z + verts[1]->z + verts[2]->z) * 0.33333334f;
        uint32_t high[3];
        int32_t count = 0;

        if (verts[0]->z > average) {
            high[count++] = 0;
        }

        if (verts[1]->z > average) {
            high[count++] = 1;
        }

        if (verts[2]->z > average) {
            high[count++] = 2;
        }

        if (count >= 2) {
            CMapObjOccluder* occluder = CMapObj::AllocOccluder();
            occluder->m_start = *verts[high[0]];
            occluder->m_end = *verts[high[1]];
            this->m_occluders.LinkToTail(occluder);
        }
    }
}

// OFFSET: 0x00795C70/0x140488A90
void CMapObjGroup::Create() {
    CMapObj* parent = this->m_parent;
    uint8_t* end = this->m_data + this->m_dataSize;
    const SMOGroupHeader* header = reinterpret_cast<const SMOGroupHeader*>(this->m_data + 20);

    this->m_groupName = parent->m_groupNames + header->m_nameOffset;
    this->m_groupFlags = header->m_flags;

    if (!parent->m_skyboxName) {
        this->m_groupFlags &= ~0x40000;
    }

    this->m_bbox = header->m_bbox;
    this->m_portalStart = header->m_portalStart;
    this->m_portalCount = header->m_portalCount;
    this->m_transBatchCount = header->m_transBatchCount;
    this->m_intBatchCount = header->m_intBatchCount;
    this->m_extBatchCount = header->m_extBatchCount;
    memcpy(this->m_fogs, header->m_fogs, sizeof(this->m_fogs));
    this->m_wmoGroupId = header->m_groupId;

    if (parent->m_header->m_flags & 0x4) {
        this->m_liquidType = this->GetLiquidType(header->m_liquidType);
    } else {
        this->m_liquidType = this->GetLiquidType(header->m_liquidType == 15 ? 0 : header->m_liquidType + 1);
    }

    this->CreateDataPointers(reinterpret_cast<uint8_t*>(const_cast<SMOGroupHeader*>(header) + 1), end);

    if (this->m_batches) {
        for (uint32_t i = 0; i < this->m_batchCount; i++) {
            parent->CreateMaterial(this->m_batches[i].m_materialId);
        }
    }

    this->m_renderFlags &= ~0x1;
    this->m_flags |= 0x40;

    if (this->m_parent->m_header->m_flags & 0x1) {
        this->m_renderFlags |= 0x1;
    }

    if (this->m_groupFlags & 0x4000000) {
        this->CreateOccluders();
    } else if (!strcmp(this->m_groupName, "antiportal")) {
        this->CreateOccluders();
        this->m_groupFlags |= 0x4000080;
        this->m_parent->m_groupInfos[this->m_groupIndex].m_flags |= 0x4000080;
        this->m_bsp.Clear();
        this->m_intBatchCount = 0;
        this->m_extBatchCount = 0;
    }

    this->m_renderFlags |= 0x2;
    uint32_t tangentCount = 0;

    if (this->m_batchCount) {
        this->m_indexStart = 0xFFFFFFFF;
        this->m_indexEnd = 0;
        this->m_minVertex = 0xFFFF;
        this->m_maxVertex = 0;

        uint32_t texCoordCount = 0;
        uint32_t colorCount = 0;

        for (uint32_t i = 0; i < this->m_batchCount; i++) {
            const SMOBatch* batch = &this->m_batches[i];
            const SMOMaterial* material = &this->m_parent->m_materials[batch->m_materialId];

            if (material->m_shader == 10 || material->m_shader == 14) {
                tangentCount += batch->m_count;
            }

            const SMOShaderInfo* info = &s_shaderInfo[material->m_shader];

            if (texCoordCount <= info->m_texCoordCount) {
                texCoordCount = info->m_texCoordCount;
            }

            if (colorCount <= info->m_colorCount) {
                colorCount = info->m_colorCount;
            }

            if (material->m_blendMode || (material->m_flags & 0x100)) {
                this->m_renderFlags &= ~0x2;
            } else {
                uint32_t lastIndex = batch->m_count + batch->m_startIndex - 1;

                if (this->m_maxVertex < batch->m_maxIndex) {
                    this->m_maxVertex = batch->m_maxIndex;
                }

                if (this->m_minVertex > batch->m_minIndex) {
                    this->m_minVertex = batch->m_minIndex;
                }

                if (this->m_indexStart > batch->m_startIndex) {
                    this->m_indexStart = batch->m_startIndex;
                }

                if (this->m_indexEnd < lastIndex) {
                    this->m_indexEnd = lastIndex;
                }
            }
        }

        if (texCoordCount == 2) {
            this->m_vertexFormat = colorCount != 2 ? GxVBF_PNCT2 : GxVBF_PNC2T2;
        } else if (colorCount == 2) {
            this->m_vertexFormat = GxVBF_PNC2T;
        } else {
            this->m_vertexFormat = GxVBF_PNCT;
        }

        if (tangentCount && !this->m_tangents) {
            this->m_tangentStarts = static_cast<uint16_t*>(SMemAlloc(sizeof(uint16_t) * this->m_batchCount, __FILE__, __LINE__, 0));
            C4Vector* tangents = static_cast<C4Vector*>(SMemAlloc(sizeof(C4Vector) * tangentCount, __FILE__, __LINE__, 0));

            if (tangents) {
                for (uint32_t i = 0; i < tangentCount; i++) {
                    tangents[i] = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
                }
            }

            this->m_renderFlags |= 0x8;
            this->m_tangents = tangents;
            uint16_t offset = 0;

            for (uint32_t i = 0; i < this->m_batchCount; i++) {
                this->m_tangentStarts[i] = 0xFFFF;
                const SMOBatch* batch = &this->m_batches[i];
                uint32_t shader = this->m_parent->m_materials[batch->m_materialId].m_shader;

                if (shader == 10 || shader == 14) {
                    this->m_tangentStarts[i] = offset;
                    CMapObjGroup::ComputeTangents(batch, this->m_indices, this->m_vertices, this->m_normals, this->m_texCoords, &this->m_tangents[offset]);
                    offset += batch->m_count;
                }
            }
        }
    }

    if (this->m_groupFlags & 0x10000) {
        this->m_groupFlags &= ~0x8;
        this->m_parent->m_groupInfos[this->m_groupIndex].m_flags &= ~0x8;
    }
}

// OFFSET: 0x00796100/0x140489030
void CMapObjGroup::AsyncCallback(CAsyncObject* object) {
    CMapObjGroup* group = static_cast<CMapObjGroup*>(object->m_userArg);
    AsyncFileReadDestroyObject(object);
    group->m_asyncObject = NULL;
    group->Create();
}

// OFFSET: 0x00796130/0x140489060
void CMapObj::ReadGroup(uint32_t groupNum, int32_t wait) {
    CMapObjGroup* group = this->m_groups[groupNum];

    char fileName[256];
    strcpy(fileName, this->m_name);
    SStrPrintf(strrchr(fileName, '.'), 256, "_%03d", groupNum);
    strcat(fileName, ".wmo");

    SFileImpl* file = NULL;
    CMap::SafeOpen(fileName, &file, 1);

    group->m_dataSize = SFileGetFileSize(file, NULL);
    group->m_data = static_cast<uint8_t*>(SMemAlloc(group->m_dataSize, __FILE__, __LINE__, 0));
    group->m_parent = this;
    group->m_groupIndex = groupNum;

    if (!wait) {
        AsyncFileReadCreateObject(file, 3, 0, &group->m_asyncObject, group->m_data, group, CMapObjGroup::AsyncCallback, CMapObjGroup::AsyncFailedCallback, 0, 0, 0);
        return;
    }

    CMap::SafeRead(fileName, file, group->m_data, group->m_dataSize);
    group->Create();
    SFileCloseFile(file);
}
