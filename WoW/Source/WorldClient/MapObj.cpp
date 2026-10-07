#include "WoW/Source/WorldClient/MapObj.h"
#include <cfloat>
#include <cmath>
#include <new>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapObjRender.h"

// STATIC: 0x00E7567C
int32_t CMapObj::bPoolsDirty;

// STATIC: 0x00E75678
TSObjectAllocPool<CMapObjOccluder>* CMapObj::s_occluderPool;

// STATIC: 0x00E75684
HASHKEY_NONE CMapObj::s_mapObjKey;

// STATIC: 0x00E75688
TSHashTable<CMapObj, HASHKEY_NONE> CMapObj::s_mapObjHash;

// OFFSET: 0x007459F0
void CMapObj::DestroyPools() {
    CMapObj::bPoolsDirty = 1;
}

// OFFSET: 0x00745A00
void CMapObj::UpdatePools() {
    CMapObj::bPoolsDirty = 0;
}

// OFFSET: 0x00745A10
void CMapObj::LinkToAsyncHead(void* param) {
    AsyncFileReadLinkToPriority(static_cast<CMapObj*>(param)->m_asyncObject, 1);
}

// OFFSET: 0x00745A30
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

// OFFSET: 0x00745AD0
void CMapObj::AsyncFreeCallback(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SMemFree(buffer, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00745B00
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

// OFFSET: 0x00745B60
void CMapObj::ReadGroup(uint32_t groupNum) {
    this->ReadGroup(groupNum, 0);
}

// OFFSET: 0x00745B80
void CMapObj::WaitLoad() {
    AsyncFileReadWait(this->m_asyncObject);
}

// OFFSET: 0x00745C40
void CMapObj::Delete(CMapObj* mapObj) {
    mapObj->m_refCount--;
}

// OFFSET: 0x00745C50
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

// OFFSET: 0x00745D00
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

// OFFSET: 0x00745DD0
int32_t CMapObj::IsGroupLoaded(uint32_t groupNum) {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    return this->m_groups[groupNum]->m_flags & 0x40;
}

// OFFSET: 0x00745DF0
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

// OFFSET: 0x00745E90
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

// OFFSET: 0x00745F30
uint32_t CMapObj::GetGroupFlags(uint32_t groupNum) {
    if (this->m_flags & 0x40) {
        return this->m_groupInfos[groupNum].m_flags;
    }

    return 0;
}

// OFFSET: 0x00745F60
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

// OFFSET: 0x007460B0
int32_t CMapObj::TestGroupBounds(const C3Vector& start, const C3Vector& end, uint32_t groupNum) {
    return (this->m_flags & 0x40) && (this->m_groups[groupNum]->m_flags & 0x40) && CWorldMath::VectorIntersectAABox2(this->m_groupInfos[groupNum].m_bbox, start, end) != 0;
}

// OFFSET: 0x00746100
int32_t CMapObj::TestGroupBounds(const CAaBox& box, uint32_t groupNum, int32_t requireLoaded) {
    if (requireLoaded && (!(this->m_flags & 0x40) || !(this->m_groups[groupNum]->m_flags & 0x40))) {
        return 0;
    }

    const CAaBox& groupBox = this->m_groupInfos[groupNum].m_bbox;
    return groupBox.t.x >= box.b.x && groupBox.t.y >= box.b.y && groupBox.t.z >= box.b.z && box.t.x >= groupBox.b.x && box.t.y >= groupBox.b.y && box.t.z >= groupBox.b.z;
}

// OFFSET: 0x007461A0
int32_t CMapObj::TestGroupBounds(const C3Vector& pos, uint32_t groupNum) {
    if (!(this->m_flags & 0x40) || !(this->m_groups[groupNum]->m_flags & 0x40)) {
        return 0;
    }

    const CAaBox& groupBox = this->m_groupInfos[groupNum].m_bbox;
    return pos.x >= groupBox.b.x && pos.y >= groupBox.b.y && pos.z >= groupBox.b.z && groupBox.t.x >= pos.x && groupBox.t.y >= pos.y && groupBox.t.z >= pos.z;
}

// OFFSET: 0x00746230
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

// OFFSET: 0x007462B0
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

// OFFSET: 0x007462E0
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

// OFFSET: 0x00746460
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

// OFFSET: 0x00747000
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

// OFFSET: 0x00747070
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

// OFFSET: 0x00747C10
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

// OFFSET: 0x00747CE0
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
