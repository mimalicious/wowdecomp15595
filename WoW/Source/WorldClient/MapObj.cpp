#include "WoW/Source/WorldClient/MapObj.h"
#include <cfloat>
#include <cmath>
#include <new>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WorldClient/MapObjRender.h"

// STATIC: 0x00E7567C
int32_t CMapObj::bPoolsDirty;

// STATIC: 0x00E75678
TSObjectAllocPool<CMapObjOccluder>* CMapObj::s_occluderPool;

// STATIC: 0x00E75684
HASHKEY_NONE CMapObj::s_mapObjKey;

// STATIC: 0x00E75688
TSHashTable<CMapObj, HASHKEY_NONE> CMapObj::s_mapObjHash;

// STATIC: 0x00D01750
static float s_minimapTexelSize = 0.5f;

// STATIC: 0x00E756B4
static float s_minimapQuadSize = s_minimapTexelSize * 256.0f;

// STATIC: 0x00E75680
static uint32_t s_minimapQueryTag;

// PROVEN: 90e66194 IDENTICAL/NONE
// OFFSET: 0x007459F0
void CMapObj::DestroyPools() {
    CMapObj::bPoolsDirty = 1;
}

// PROVEN: 346c15fb IDENTICAL/NONE
// OFFSET: 0x00745A00
void CMapObj::UpdatePools() {
    CMapObj::bPoolsDirty = 0;
}

// PROVEN: 71aa0cc3 IDENTICAL/UNPROVEN
// OFFSET: 0x00745A10/0x140423920
void CMapObj::LinkToAsyncHead(void* param) {
    AsyncFileReadLinkToPriority(static_cast<CMapObj*>(param)->m_asyncObject, 1);
}

// PROVEN: bd76315d IDENTICAL/IDENTICAL
// OFFSET: 0x00745A30/0x140423930
void CMapObj::InitPtrs() {
    this->m_header = NULL;
    this->m_textureNames = NULL;
    this->m_groupNames = NULL;
    this->m_groupInfos = NULL;
    this->m_portalVertices = NULL;
    this->m_portalInfos = NULL;
    this->m_portalRefs = NULL;
    this->m_lights = NULL;
    this->m_doodadSets = NULL;
    this->m_doodadNames = NULL;
    this->m_doodadDefs = NULL;
    this->m_fogs = NULL;
    this->m_convexVolumePlanes = NULL;
    this->m_textureNamesSize = 0;
    this->m_groupNamesSize = 0;
    this->m_groupCount = 0;
    this->m_portalVertexCount = 0;
    this->m_portalCount = 0;
    this->m_portalRefCount = 0;
    this->m_lightCount = 0;
    this->m_doodadSetCount = 0;
    this->m_doodadNamesSize = 0;
    this->m_doodadDefCount = 0;
    this->m_fogCount = 0;
    this->m_convexVolumePlaneCount = 0;
}

// OFFSET: 0x00745AD0/0x1404239E0
void CMapObj::AsyncFreeCallback(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00745B00/0x140423A20
uint32_t CMapObj::CreateWmoIgnoreFlags(uint32_t flags) {
    if (flags & 0x80) {
        return 0x2CA;
    }

    uint32_t ignore = (flags & 0x10) ? 0xC6 : 0xEE;

    if (flags & 0x20) {
        ignore &= 0xDB;
    }

    if (!(flags & 0x40)) {
        ignore &= ~0x2;
    }

    if (!(flags & 0x4000)) {
        ignore &= ~0x40;
    }

    if (flags & 0x1000000) {
        if (flags & 0x40000000) {
            return 0xFFFFFCDB;
        }

        return ignore | 0x100;
    }

    return ignore;
}

// PROVEN: 893420cb IDENTICAL/UNPROVEN
// OFFSET: 0x00745B60/0x140423A70
void CMapObj::ReadGroup(uint32_t groupNum) {
    this->ReadGroup(groupNum, 0);
}

// PROVEN: be75624a IDENTICAL/IDENTICAL
// OFFSET: 0x00745B80/0x140423A80
void CMapObj::WaitLoad() {
    AsyncFileReadWait(this->m_asyncObject);
}

// OFFSET: 0x00745B90
static uint32_t MinimapOutcode(const CAaBox& box, const C3Vector& point) {
    float d0 = point.x - box.b.x;
    float d1 = point.y - box.b.y;
    float d2 = point.z - box.b.z;
    float d3 = box.t.x - point.x;
    float d4 = box.t.y - point.y;
    float d5 = box.t.z - point.z;
    return ((reinterpret_cast<uint32_t&>(d4) >> 27) & 0x10) | ((reinterpret_cast<uint32_t&>(d3) >> 28) & 0x8) | ((reinterpret_cast<uint32_t&>(d2) >> 29) & 0x4) | ((reinterpret_cast<uint32_t&>(d1) >> 30) & 0x2) | (reinterpret_cast<uint32_t&>(d0) >> 31) | ((reinterpret_cast<uint32_t&>(d5) >> 26) & 0x20);
}

// OFFSET: 0x00745C40/0x140423A90
void CMapObj::Delete(CMapObj* mapObj) {
    mapObj->m_refCount--;
}

// OFFSET: 0x00745C50/0x140423AA0
void CMapObj::Init() {
    this->m_minimapGroup = 0xFFFF;
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_data = NULL;
    this->m_dataSize = 0;
    this->m_asyncObject = NULL;
    this->m_materials = NULL;
    this->m_runtimeMaterials = NULL;
    this->m_materialCount = 0;
    this->m_bbox.t.z = 0.0f;
    this->m_mapObjGroupCount = 0;
    this->m_flushTimer = FLT_MAX;
    this->InitPtrs();
}

// OFFSET: 0x00745D00/0x140423B60
void CMapObj::Clear() {
    int32_t freeData = 1;

    if (this->m_asyncObject) {
        freeData = AsyncFileReadCancel(this->m_asyncObject, CMapObj::AsyncFreeCallback);
        this->m_asyncObject = NULL;
    }

    for (uint32_t i = 0; i < this->m_mapObjGroupCount; i++) {
        CMap::FreeMapObjGroup(this->m_groups[i]);
        this->m_groups[i] = NULL;
    }

    this->m_mapObjGroupCount = 0;

    if (this->m_materialCount) {
        if (this->m_runtimeMaterials) {
            SMemFree(this->m_runtimeMaterials, "delete[]", -1, 0);
        }

        this->m_runtimeMaterials = NULL;
        this->m_materialCount = 0;
    }

    this->InitPtrs();

    if (this->m_data) {
        if (freeData) {
            SMemFree(this->m_data, __FILE__, __LINE__, 0);
        }

        this->m_data = NULL;
    }

    this->m_dataSize = 0;
}

// PROVEN: 42688913 IDENTICAL/IDENTICAL
// OFFSET: 0x00745DD0/0x140423D10
int32_t CMapObj::IsGroupLoaded(uint32_t groupNum) {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    return this->m_groups[groupNum]->m_flags & 0x40;
}

// OFFSET: 0x00745DF0/0x140423D30
void CMapObj::GetBounds(CAaBox& box) {
    if (this->m_flags & 0x40) {
        box = this->m_bbox;
        return;
    }

    box.b.x = 0.0f;
    box.b.y = 0.0f;
    box.t.x = 0.0f;
    box.b.z = 0.0f;
    box.t.y = 0.0f;
    box.t.z = 0.0f;
}

// OFFSET: 0x00745E90/0x140423DD0
void CMapObj::GetGroupBounds(CAaBox& box, uint32_t groupNum) {
    if (this->m_flags & 0x40) {
        box = this->m_groupInfos[groupNum].m_bbox;
        return;
    }

    box.b.x = 0.0f;
    box.b.y = 0.0f;
    box.t.x = 0.0f;
    box.b.z = 0.0f;
    box.t.y = 0.0f;
    box.t.z = 0.0f;
}

// OFFSET: 0x00745F30/0x140423E70
uint32_t CMapObj::GetGroupFlags(uint32_t groupNum) {
    if (this->m_flags & 0x40) {
        return this->m_groupInfos[groupNum].m_flags;
    }

    return 0;
}

// OFFSET: 0x00745F60/0x140423E90
int32_t CMapObj::TestBounds(const CAaBox& box) {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    return this->m_bbox.t.x >= box.b.x && this->m_bbox.t.y >= box.b.y && this->m_bbox.t.z >= box.b.z && box.t.x >= this->m_bbox.b.x && box.t.y >= this->m_bbox.b.y && box.t.z >= this->m_bbox.b.z;
}

// OFFSET: 0x00745FF0
int32_t CMapObj::TestBounds(const C3Vector& pos) {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    return pos.x >= this->m_bbox.b.x && pos.y >= this->m_bbox.b.y && pos.z >= this->m_bbox.b.z && this->m_bbox.t.x >= pos.x && this->m_bbox.t.y >= pos.y && this->m_bbox.t.z >= pos.z;
}

// OFFSET: 0x00746080
int32_t CMapObj::TestBounds(const C3Vector& start, const C3Vector& end) {
    return (this->m_flags & 0x40) && CWorldMath::VectorIntersectAABox2(this->m_bbox, start, end) != 0;
}

// OFFSET: 0x007460B0/0x140423F90
int32_t CMapObj::TestGroupBounds(const C3Vector& start, const C3Vector& end, uint32_t groupNum) {
    return (this->m_flags & 0x40) && (this->m_groups[groupNum]->m_flags & 0x40) && CWorldMath::VectorIntersectAABox2(this->m_groupInfos[groupNum].m_bbox, start, end) != 0;
}

// OFFSET: 0x00746100/0x140423FE0
int32_t CMapObj::TestGroupBounds(const CAaBox& box, uint32_t groupNum, int32_t requireLoaded) {
    if (requireLoaded && (!(this->m_flags & 0x40) || !(this->m_groups[groupNum]->m_flags & 0x40))) {
        return 0;
    }

    const CAaBox& groupBox = this->m_groupInfos[groupNum].m_bbox;
    return groupBox.t.x >= box.b.x && groupBox.t.y >= box.b.y && groupBox.t.z >= box.b.z && box.t.x >= groupBox.b.x && box.t.y >= groupBox.b.y && box.t.z >= groupBox.b.z;
}

// OFFSET: 0x007461A0/0x140424070
int32_t CMapObj::TestGroupBounds(const C3Vector& pos, uint32_t groupNum) {
    if (!(this->m_flags & 0x40) || !(this->m_groups[groupNum]->m_flags & 0x40)) {
        return 0;
    }

    const CAaBox& groupBox = this->m_groupInfos[groupNum].m_bbox;
    return pos.x >= groupBox.b.x && pos.y >= groupBox.b.y && pos.z >= groupBox.b.z && groupBox.t.x >= pos.x && groupBox.t.y >= pos.y && groupBox.t.z >= pos.z;
}

// OFFSET: 0x00746230/0x1404240F0
int32_t CMapObj::TestConvexVolume(const C3Vector& pos) {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    for (uint32_t i = 0; i < this->m_convexVolumePlaneCount; i++) {
        const C4Plane& plane = this->m_convexVolumePlanes[i];

        if (plane.n.y * pos.y + plane.n.z * pos.z + pos.x * plane.n.x + plane.d > 0.0f) {
            return 0;
        }
    }

    return 1;
}

// PROVEN: 80205287 IDENTICAL/IDENTICAL
// OFFSET: 0x007462B0/0x140424180
CMapObjGroup* CMapObj::GetGroup(uint32_t groupNum, int32_t loaded) {
    if (!(this->m_flags & 0x40)) {
        return NULL;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40) && !loaded) {
        return NULL;
    }

    return group;
}

// PROVEN: 35c617a3 IDENTICAL/IDENTICAL
// OFFSET: 0x007462E0/0x1404241A0
void CMapObj::WaitLoadGroup(uint32_t groupNum) {
    AsyncFileReadWait(this->m_groups[groupNum]->m_asyncObject);
}

// OFFSET: 0x00746300
const char* CMapObj::GetGroupName(uint32_t groupNum) {
    if (!(this->m_flags & 0x40)) {
        return NULL;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40)) {
        return NULL;
    }

    return group->m_groupName;
}

// OFFSET: 0x00746330
SMOGroupInfo* CMapObj::GetGroupInfo(uint32_t groupNum) {
    if (this->m_flags & 0x40) {
        return &this->m_groupInfos[groupNum];
    }

    return NULL;
}

// OFFSET: 0x00746350
int32_t CMapObj::QueryLighting(uint32_t groupNum, const C3Vector& pos, uint16_t polyIndex, CImVector& color, bool& exterior) {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40) || (group->m_groupFlags & 0x48)) {
        return 0;
    }

    return group->QueryLighting(pos, polyIndex, color, exterior);
}

// OFFSET: 0x00746390
int32_t CMapObj::QueryLiquid(uint32_t ignoreFlags, int32_t requireTiles, const C3Vector& pos, uint32_t& liquidType, float& height) {
    for (uint32_t i = 0; i < this->m_groupCount; i++) {
        if (!(this->m_flags & 0x40)) {
            continue;
        }

        const SMOGroupInfo* info = &this->m_groupInfos[i];

        if (!info || (ignoreFlags & info->m_flags)) {
            continue;
        }

        if (pos.x >= info->m_bbox.b.x && pos.y >= info->m_bbox.b.y && pos.z >= info->m_bbox.b.z && info->m_bbox.t.x >= pos.x && info->m_bbox.t.y >= pos.y && info->m_bbox.t.z >= pos.z && (this->m_groups[i]->m_flags & 0x40) && (this->m_groups[i]->m_flags & 0x40) && this->m_groups[i]->QueryLiquid(pos, requireTiles, liquidType, height)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00746460/0x140424350
int32_t CMapObj::GetDoodadSet(uint32_t doodadIndex) {
    if (!(this->m_flags & 0x40) || doodadIndex >= this->m_doodadDefCount) {
        return -1;
    }

    for (uint32_t i = 0; i < this->m_doodadSetCount; i++) {
        const SMODoodadSet* set = &this->m_doodadSets[i];

        if (set->m_count && doodadIndex >= set->m_startIndex && doodadIndex <= set->m_count + set->m_startIndex - 1) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x007464F0/0x1404243E0
bool CMapObj::VectorIntersect(const C3Vector& start, const C3Vector& end, uint32_t flags, uint32_t groupMask, float* t, uint16_t* faceIndex, CMapObjDef* mapObjDef, float* dot, uint32_t* groupNum) {
    if (!(this->m_flags & 0x40)) {
        return false;
    }

    C3Segment seg;
    seg.b = start;
    seg.t = end;
    uint32_t hitFace = 0;
    bool hit = false;
    C3Vector normal(0.0f, 0.0f, 0.0f);
    uint32_t ignoreFlags = CMapObj::CreateWmoIgnoreFlags(flags);
    World::TriData tris;

    for (uint32_t i = 0; i < this->m_groupCount; i++) {
        if (!(this->m_flags & 0x40) || !(this->m_groups[i]->m_flags & 0x40) || (groupMask & this->m_groups[i]->m_groupFlags)) {
            continue;
        }

        if (!CWorldMath::VectorIntersectAABox2(this->m_groupInfos[i].m_bbox, start, end)) {
            continue;
        }

        World::TriData::s_statusFlags = 0;
        World::TriData::s_nBatches = 0;
        World::TriData::s_faceIndexCursor = 0;
        World::TriData::s_indexCursor = 0;
        World::TriData::s_nMatrices = 0;

        if (!this->m_groups[i]->GetTris(seg, t, flags, ignoreFlags, &tris, mapObjDef)) {
            continue;
        }

        const World::TriData::Batch& batch = World::TriData::s_batches[0];

        if (batch.m_faceIndices) {
            hitFace = batch.m_faceIndices[0];

            if (dot) {
                const C3Vector& v0 = batch.m_vertexList[batch.m_indices[0]];
                const C3Vector& v1 = batch.m_vertexList[batch.m_indices[1]];
                const C3Vector& v2 = batch.m_vertexList[batch.m_indices[2]];
                float e2x = v2.x - v0.x;
                float e2z = v2.z - v0.z;
                float e1x = v1.x - v0.x;
                float e1y = v1.y - v0.y;
                float e2y = v2.y - v0.y;
                float e1z = v1.z - v0.z;
                normal.x = e1y * e2z - e1z * e2y;
                normal.y = e1z * e2x - e2z * e1x;
                normal.z = e2y * e1x - e1y * e2x;
                *dot = (seg.t.z - seg.b.z) * normal.z + (seg.t.y - seg.b.y) * normal.y + (seg.t.x - seg.b.x) * normal.x;
            }
        }

        if (groupNum) {
            *groupNum = i;
        }

        hit = true;
    }

    if (hit && faceIndex) {
        *faceIndex = static_cast<uint16_t>(hitFace);
    }

    if (dot) {
        float length = sqrt(normal.z * normal.z + normal.y * normal.y + normal.x * normal.x);

        if (fabs(length) >= 0.00000023841858f) {
            *dot = *dot / length;
        }
    }

    return hit;
}

// OFFSET: 0x007467A0/0x140424880
bool CMapObj::GetFacets(const CAaBox& box, uint32_t flags, World::FacetData& facets, const CMapObjDef* mapObjDef, uint32_t groupNum) {
    uint32_t ignoreFlags = CMapObj::CreateWmoIgnoreFlags(flags);
    World::TriData::s_statusFlags = 0;
    World::TriData::s_nBatches = 0;
    World::TriData::s_faceIndexCursor = 0;
    World::TriData::s_indexCursor = 0;
    World::TriData::s_nMatrices = 0;

    const CAaBox& groupBox = this->m_groupInfos[groupNum].m_bbox;

    if (box.t.x < groupBox.b.x || box.t.y < groupBox.b.y || box.t.z < groupBox.b.z || groupBox.t.x < box.b.x || groupBox.t.y < box.b.y || groupBox.t.z < box.b.z) {
        return true;
    }

    if (!(this->m_flags & 0x40)) {
        return false;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40)) {
        return false;
    }

    if (!(group->m_groupFlags & 0x80)) {
        World::TriData tris;
        group->GetTris(box, flags, ignoreFlags, tris, mapObjDef);

        if (World::TriData::s_nBatches) {
            World::TriDataToFacetData(tris, facets, mapObjDef->m_param64, 0);
        }
    }

    return true;
}

// OFFSET: 0x007468A0/0x1404249E0
bool CMapObj::GetTris(const CAaBox& box, uint32_t flags, World::TriData& tris, const CMapObjDef* mapObjDef) {
    bool result = false;
    uint32_t ignoreFlags = CMapObj::CreateWmoIgnoreFlags(flags);

    for (uint32_t i = 0; i < this->m_groupCount; i++) {
        const CAaBox& groupBox = this->m_groupInfos[i].m_bbox;

        if (box.t.x < groupBox.b.x || box.t.y < groupBox.b.y || box.t.z < groupBox.b.z || groupBox.t.x < box.b.x || groupBox.t.y < box.b.y || groupBox.t.z < box.b.z) {
            continue;
        }

        if (!(this->m_flags & 0x40) || !(this->m_groups[i]->m_flags & 0x40)) {
            continue;
        }

        uint32_t groupFlags = this->m_groups[i]->m_groupFlags;

        if (!(groupFlags & 0x80) && (!(flags & 0x40000) || (groupFlags & 0x8))) {
            result |= this->m_groups[i]->GetTris(box, flags, ignoreFlags, tris, mapObjDef);
        }
    }

    return result;
}

// OFFSET: 0x00746990/0x140424B50
bool CMapObj::GetTris(const C3Segment& seg, float& t, uint32_t flags, World::TriData& tris, const CMapObjDef* mapObjDef) {
    bool result = false;
    uint32_t ignoreFlags = CMapObj::CreateWmoIgnoreFlags(flags);

    for (uint32_t i = 0; i < this->m_groupCount; i++) {
        if (CWorldMath::VectorIntersectAABox2(this->m_groupInfos[i].m_bbox, seg.b, seg.t) && (this->m_flags & 0x40) && (this->m_groups[i]->m_flags & 0x40)) {
            result |= this->m_groups[i]->GetTris(seg, &t, flags, ignoreFlags, &tris, mapObjDef);
        }
    }

    return result;
}

// OFFSET: 0x00746A40/0x140424C80
bool CMapObj::GetTris(const CFrustum& frustum, uint32_t flags, World::TriData& tris, const CMapObjDef* mapObjDef) {
    bool result = false;
    uint32_t ignoreFlags = CMapObj::CreateWmoIgnoreFlags(flags);
    CAaBox bounds = CAaBox::Bounding(frustum.corners, 8);

    for (uint32_t i = 0; i < this->m_groupCount; i++) {
        const CAaBox& groupBox = this->m_groupInfos[i].m_bbox;

        if (bounds.t.x < groupBox.b.x || bounds.t.y < groupBox.b.y || bounds.t.z < groupBox.b.z || groupBox.t.x < bounds.b.x || groupBox.t.y < bounds.b.y || groupBox.t.z < bounds.b.z) {
            continue;
        }

        if ((this->m_flags & 0x40) && (this->m_groups[i]->m_flags & 0x40) && !(this->m_groups[i]->m_groupFlags & 0x80)) {
            result |= this->m_groups[i]->GetTris(frustum, flags, ignoreFlags, tris, mapObjDef);
        }
    }

    return result;
}

// OFFSET: 0x00746B30/0x140424E00
bool CMapObj::Intersect(const C3Vector& start, const C3Vector& end, float& t, uint32_t flags, uint32_t ignoreFlags, uint32_t groupNum, uint32_t& faceIndex) {
    if (!(this->m_flags & 0x40) || !(this->m_groups[groupNum]->m_flags & 0x40)) {
        return false;
    }

    C3Segment seg;
    seg.b = start;
    seg.t = end;
    return this->m_groups[groupNum]->Intersect(seg, t, flags, ignoreFlags, faceIndex);
}

// OFFSET: 0x00746BA0
bool CMapObj::VectorIntersectPortals(uint32_t groupNum, const C3Segment& seg, float& t, uint32_t* groups) {
    if (!(this->m_flags & 0x40)) {
        return false;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40)) {
        return false;
    }

    C3Vector dir(seg.t.x - seg.b.x, seg.t.y - seg.b.y, seg.t.z - seg.b.z);
    float length = sqrt(dir.z * dir.z + dir.x * dir.x + dir.y * dir.y);
    float maxT = length * t;
    float inv = 1.0f / length;
    C3Ray ray(seg.b, C3Vector(dir.x * inv, dir.y * inv, dir.z * inv));
    const SMOPortalRef* ref = &this->m_portalRefs[group->m_portalStart];
    bool found = false;

    for (uint32_t i = 0; i < group->m_portalCount; i++, ref++) {
        const SMOPortalInfo* info = &this->m_portalInfos[ref->m_portalIndex];

        if (!(this->m_flags & 0x40) || !(this->m_groups[ref->m_groupIndex]->m_flags & 0x40)) {
            continue;
        }

        const C4Plane& plane = info->m_plane;
        C3Vector hit;
        float hitT;

        if (!NTempest::Intersect(ray, plane, &hitT, &hit, 0.1f)) {
            continue;
        }

        if (hitT < 0.0f || hitT > maxT) {
            continue;
        }

        if (!NTempest::Intersect(hit, &this->m_portalVertices[info->m_startVertex], info->m_count, plane.n.MajorAxis())) {
            continue;
        }

        maxT = hitT;
        found = true;

        if (plane.n.y * seg.b.y + plane.n.z * seg.b.z + seg.b.x * plane.n.x + plane.d < 0.0f) {
            if (ref->m_side <= 0) {
                groups[0] = groupNum;
                groups[1] = ref->m_groupIndex;
                continue;
            }
        } else if (ref->m_side > 0) {
            groups[0] = groupNum;
            groups[1] = ref->m_groupIndex;
            continue;
        }

        groups[0] = ref->m_groupIndex;
        groups[1] = groupNum;
    }

    if (found) {
        t = maxT * inv;
    }

    return found;
}

// OFFSET: 0x00746E00
bool CMapObj::ClosestPortal(uint32_t groupNum, const C3Vector& pos, float* dist, uint32_t* closestGroup) {
    if (!(this->m_flags & 0x40)) {
        return false;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40)) {
        return false;
    }

    bool found = false;

    for (uint32_t i = 0; i < group->m_portalCount; i++) {
        const SMOPortalRef* ref = &this->m_portalRefs[i + group->m_portalStart];
        const SMOPortalInfo* info = &this->m_portalInfos[ref->m_portalIndex];
        float d = NTempest::DistanceFromPolygon(pos, &this->m_portalVertices[info->m_startVertex], info->m_count, info->m_plane);

        if (*dist > d) {
            *dist = d;
            *closestGroup = ref->m_groupIndex;
            found = true;
        }
    }

    return found;
}

// OFFSET: 0x00746F10/0x140425330
bool CMapObj::QueryLighting(uint32_t groupNum, const C3Segment& seg, CImVector& color, bool& exterior) {
    float t = 1.0f;

    if (!(this->m_flags & 0x40)) {
        return false;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40) || (group->m_groupFlags & 0x48)) {
        return false;
    }

    World::TriData::s_statusFlags = 0;
    World::TriData::s_nBatches = 0;
    World::TriData::s_faceIndexCursor = 0;
    World::TriData::s_indexCursor = 0;
    World::TriData::s_nMatrices = 0;
    World::TriData tris;

    if (!group->GetTris(seg, &t, 0, 8, &tris, NULL)) {
        return false;
    }

    uint16_t polyIndex = World::TriData::s_batches[0].m_faceIndices[0];
    C3Vector pos(seg.b.x + (seg.t.x - seg.b.x) * t, seg.b.y + (seg.t.y - seg.b.y) * t, seg.b.z + (seg.t.z - seg.b.z) * t);
    return group->QueryLighting(pos, polyIndex, color, exterior) != 0;
}

// OFFSET: 0x00747000/0x140425480
CMapObj::CMapObj() {
    this->m_mapObjGroupCount = 512;
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    *reinterpret_cast<uint32_t*>(&this->m_ambColor) = 0;
    this->Init();
}

// OFFSET: 0x00747070/0x1404254F0
CMapObj::~CMapObj() {
    this->Clear();
}

// OFFSET: 0x007470B0
int32_t CMapObj::IsAllLoaded() {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    for (uint32_t i = 0; i < this->m_groupCount; i++) {
        if (!(this->m_groups[i]->m_flags & 0x40)) {
            return 0;
        }
    }

    for (uint32_t i = 0; i < this->m_materialCount; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            CTexture* texture = this->m_runtimeMaterials[i].m_textures[j];

            if (texture && !TextureGetGxTex(texture)) {
                return 0;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00747150
static void GetMinimapQuads(const CAaBox& groupBox, const CAaBox& box, uint32_t groupNum, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads) {
    float centerZ = (groupBox.t.z + groupBox.b.z) * 0.5f;
    float sizeX = (groupBox.t.x - groupBox.b.x) * (1.0f / s_minimapQuadSize);
    float sizeY = (groupBox.t.y - groupBox.b.y) * (1.0f / s_minimapQuadSize);
    float invLog2 = 1.0f / log10f(2.0f);
    float levelY = invLog2 * log10f(sizeY * 256.0f);
    int32_t resY;

    if (static_cast<uint32_t>(1 << static_cast<int32_t>(ceilf(levelY))) < 0x100 && static_cast<uint32_t>(1 << static_cast<int32_t>(ceilf(levelY))) < 0x20) {
        resY = 32;
    } else if (static_cast<uint32_t>(1 << static_cast<int32_t>(ceilf(levelY))) >= 0x100) {
        resY = 256;
    } else {
        resY = 1 << static_cast<int32_t>(ceilf(levelY));
    }

    int32_t resX = 32;
    float levelX = log10f(sizeX * 256.0f) * invLog2;

    if (static_cast<uint32_t>(1 << static_cast<int32_t>(ceilf(levelX))) >= 0x100 || static_cast<uint32_t>(1 << static_cast<int32_t>(ceilf(levelX))) >= 0x20) {
        if (static_cast<uint32_t>(1 << static_cast<int32_t>(ceilf(levelX))) >= 0x100) {
            resX = 256;
        } else {
            resX = 1 << static_cast<int32_t>(ceilf(levelX));
        }
    }

    int32_t countY = static_cast<int32_t>(ceilf(sizeY));
    int32_t countX = static_cast<int32_t>(ceilf(sizeX));
    float stepX = static_cast<float>(resX) * s_minimapTexelSize;
    float stepY = static_cast<float>(resY) * s_minimapTexelSize;

    for (int32_t y = 0; y < countY; y++) {
        float offsetY = static_cast<float>(y) * stepY;

        for (int32_t x = 0; x < countX; x++) {
            float minX = static_cast<float>(x) * stepX + groupBox.b.x;
            float minY = offsetY + groupBox.b.y;
            float maxX = minX + stepX;
            float maxY = minY + stepY;

            static float s_border = s_minimapTexelSize * 2.0f;

            if (!x) {
                minX = minX - s_border;
            }

            if (!y) {
                minY = minY - s_border;
            }

            if (x == countX - 1) {
                maxX = maxX + s_border;
            }

            if (y == countY - 1) {
                maxY = maxY + s_border;
            }

            if (maxX >= box.b.x && maxY >= box.b.y && box.t.x >= minX && box.t.y >= minY && quads.m_count < maxQuads) {
                World::MinimapQuad* quad = new (&quads.m_data[quads.m_count++]) World::MinimapQuad();
                quad->m_box.b.x = minX;
                quad->m_box.b.y = minY;
                quad->m_box.b.z = centerZ;
                quad->m_box.t.x = maxX;
                quad->m_box.t.y = maxY;
                quad->m_groupNum = groupNum;
                quad->m_x = x;
                quad->m_y = y;
                quad->m_box.t.z = centerZ;
            }
        }
    }
}

// OFFSET: 0x00747560
void CMapObj::QueryMapObjMinimapGroup(uint32_t groupNum, uint32_t fromGroup, const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads, bool interior, uint32_t flagsMatch, uint32_t depth) {
    if (!(this->m_flags & 0x40)) {
        return;
    }

    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40)) {
        return;
    }

    if ((interior ? (group->m_groupFlags & 0x48) : (group->m_groupFlags & 0x8)) != flagsMatch) {
        return;
    }

    if (group->unk_0110 == s_minimapQueryTag) {
        return;
    }

    group->unk_0110 = s_minimapQueryTag;

    if (group->m_bbox.t.x < box.b.x || group->m_bbox.t.y < box.b.y || box.t.x < group->m_bbox.b.x || box.t.y < group->m_bbox.b.y) {
        return;
    }

    ::GetMinimapQuads(group->m_bbox, box, groupNum, quads, maxQuads);

    uint32_t groupId = group->m_wmoGroupId;

    if (groupId == 23015 || groupId == 23016 || !group->m_portalCount || !CWorldParam::cvar_minimapPortalMax || depth >= static_cast<uint32_t>(CWorldParam::cvar_minimapPortalMax->m_intValue)) {
        return;
    }

    const SMOPortalRef* ref = &this->m_portalRefs[group->m_portalStart];

    for (uint32_t i = 0; i < group->m_portalCount; i++, ref++) {
        uint32_t other = ref->m_groupIndex;

        if (other == 0xFFFF || other == fromGroup || (this->GetGroupFlags(other) & 0x80)) {
            continue;
        }

        const SMOPortalInfo& portal = this->m_portalInfos[ref->m_portalIndex];
        uint32_t outcode = 0xFFFFFFFF;

        if (!portal.m_count) {
            continue;
        }

        const C3Vector* vertex = &this->m_portalVertices[portal.m_startVertex];

        for (uint32_t v = portal.m_count; v; v--) {
            outcode &= MinimapOutcode(box, *vertex);
            vertex++;
        }

        if (outcode) {
            continue;
        }

        if ((this->m_flags & 0x40) && (this->m_groups[other]->m_flags & 0x40)) {
            uint32_t otherId = this->m_groups[other]->m_wmoGroupId;

            if (otherId == 23015 || otherId == 23016) {
                continue;
            }
        }

        this->QueryMapObjMinimapGroup(other, groupNum, box, quads, maxQuads, interior, flagsMatch, ++depth);
    }
}

// OFFSET: 0x00747770
void CMapObj::GetMinimapQuads(const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads) {
    CAaBox bounds = this->m_bbox;
    ::GetMinimapQuads(bounds, box, this->m_groupCount, quads, maxQuads);
}

// OFFSET: 0x007477E0
void CMapObj::ClearCache() {
    CMapObj* mapObj = CMapObj::s_mapObjHash.Head();

    while (mapObj) {
        CMapObj* next = CMapObj::s_mapObjHash.Next(mapObj);

        if (!mapObj->m_refCount) {
            CMapObj::s_mapObjHash.Unlink(mapObj);
            CMap::FreeMapObj(mapObj);
        }

        mapObj = next;
    }
}

// OFFSET: 0x00747840
bool CMapObj::QueryMapObjMinimap(uint32_t groupNum, const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads, bool interior, bool wholeObject) {
    s_minimapQueryTag++;

    if (!(this->m_flags & 0x40)) {
        return false;
    }

    CAaBox query = box;
    CMapObjGroup* group = this->m_groups[groupNum];

    if (!(group->m_flags & 0x40)) {
        return false;
    }

    query.t.z = group->m_bbox.t.z;

    if (wholeObject) {
        this->GetMinimapQuads(query, quads, maxQuads);
        return true;
    }

    uint32_t flagsMatch = interior ? (this->m_groupInfos[groupNum].m_flags & 0x40) : 8;
    this->QueryMapObjMinimapGroup(groupNum, groupNum, query, quads, maxQuads, interior, flagsMatch, 0);
    return true;
}

// OFFSET: 0x00747C10/0x140426820
void CMapObj::Initialize() {
    CMapObjRender::Initialize();
    CMapObj::bPoolsDirty = 1;
    void* mem = SMemAlloc(sizeof(TSObjectAllocPool<CMapObjOccluder>), __FILE__, __LINE__, 0);
    CMapObj::s_occluderPool = mem ? new (mem) TSObjectAllocPool<CMapObjOccluder>("MAPOBJOCC", 128) : NULL;
}

// OFFSET: 0x00747C90
CMapObjOccluder* CMapObj::AllocOccluder() {
    return CMapObj::s_occluderPool->New("", 0);
}

// OFFSET: 0x00747CB0
void CMapObj::Destroy() {
    CMapObj::ClearCache();
    CMapObjGroup::Destroy();
    CMapObjRender::Destroy();

    if (CMapObj::s_occluderPool) {
        CMapObj::s_occluderPool->~TSObjectAllocPool<CMapObjOccluder>();
        SMemFree(CMapObj::s_occluderPool, "delete", -1, 0);
        CMapObj::s_occluderPool = NULL;
    }
}

// OFFSET: 0x00747CE0/0x1404269E0
void CMapObj::FreeOccluder(CMapObjOccluder* occluder) {
    CMapObj::s_occluderPool->Delete(occluder, "", 0);
}

// OFFSET: 0x00748360
CMapObj* CMapObj::Create(const char* name) {
    uint32_t hashval = SStrHash(name, 0, 0);
    CMapObj* mapObj = CMapObj::s_mapObjHash.Ptr(hashval, CMapObj::s_mapObjKey);

    if (!mapObj) {
        mapObj = CMap::AllocMapObj();

        mapObj->Read(name);

        CMapObj::s_mapObjHash.Insert(mapObj, hashval, CMapObj::s_mapObjKey);
    }

    mapObj->m_refCount++;
    return mapObj;
}
