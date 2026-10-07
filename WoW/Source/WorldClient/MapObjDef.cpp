#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapObjRender.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// OFFSET: 0x0075EDD0
void CMapObjDefGroup::WaterDestroy() {
    if (this->m_liquidInstance) {
        this->m_liquidInstance->Release();
        this->m_liquidInstance = NULL;
    }
}

// OFFSET: 0x0075EE50/0x140442E60
bool CMapObjDef::TestAABox(const C3Vector& start, const C3Vector& end) {
    return CWorldMath::VectorIntersectAABox2(this->m_bbox, start, end);
}

// OFFSET: 0x0075EF10
void CMapObjDefGroup::ExtendBounds(const CAaBox& box) {
    this->m_bbox |= box;
    this->m_mapObjDef->m_bbox |= box;
}

// OFFSET: 0x0075FBE0/0x140443DC0
void CMapObjDefGroup::ClearRefs() {
    CMapBaseObjLink* link = this->m_doodadLinks.Head();

    while (link) {
        CMapBaseObjLink* next = this->m_doodadLinks.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
        CMap::FreeBaseObjLink(link);
        doodadDef->Purge();
        link = next;
    }

    link = this->m_entityLinks.Head();

    while (link) {
        CMapBaseObjLink* next = this->m_entityLinks.Next(link);
        CMap::FreeBaseObjLink(link);
        link = next;
    }

    CMapObjDef* mapObjDef = this->m_mapObjDef;
    this->m_flags &= ~0x8;
    mapObjDef->m_mapObj->GetGroupBounds(this->m_portalBox, this->m_groupNum);
    CWorldMath::TransformAABox(mapObjDef->m_mat, this->m_portalBox, this->m_bbox);
    this->m_portalBox = this->m_bbox;

    for (uint32_t i = 0; i < this->m_batchMaterials.m_count; i++) {
        if (this->m_batchMaterials.m_data[i]) {
            CMapObjRender::UnregisterMaterial(this->m_batchMaterials.m_data[i]);
        }
    }

    if (this->m_batchMaterials.m_data) {
        SMemFree(this->m_batchMaterials.m_data, __FILE__, __LINE__, 0);
    }

    this->m_batchMaterials.m_alloc = 0;
    this->m_batchMaterials.m_count = 0;
    this->m_batchMaterials.m_data = NULL;
    this->m_materialState = 0;
}

// OFFSET: 0x0075FD10
void CMapObjDefGroup::CreateDoodadRefs() {
    CMapObjDef* mapObjDef = this->m_mapObjDef;
    CMapObj* mapObj = mapObjDef->m_mapObj;
    CMapObjGroup* group = mapObj->GetGroup(this->m_groupNum, 0);

    for (uint32_t i = 0; i < group->m_doodadRefCount; i++) {
        uint16_t doodadIndex = group->m_doodadRefs[i];
        int32_t doodadSet = mapObj->GetDoodadSet(doodadIndex);
        uint16_t extraSet = 0;

        if (doodadSet && doodadSet != static_cast<int32_t>(mapObjDef->m_doodadSet)) {
            uint32_t j = 0;

            while (doodadSet != mapObjDef->m_extraDoodadSets[j]) {
                if (++j >= 3) {
                    break;
                }
            }

            if (j >= 3) {
                continue;
            }

            extraSet = static_cast<uint16_t>(doodadSet);

            if (!extraSet) {
                continue;
            }
        }

        SMODoodadDef& def = mapObj->m_doodadDefs[doodadIndex];
        CMapDoodadDef* doodadDef = mapObjDef->m_worldMap->CreateDoodadDef(doodadIndex, def, mapObj->m_doodadNames + (def.m_nameIndex & 0xFFFFFF), mapObjDef->m_hashval + 1, mapObjDef->m_mat, extraSet);
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(doodadDef, this);
        doodadDef->m_parentLinkList.LinkToTail(link);

        if (extraSet) {
            this->m_doodadLinks.LinkToHead(link);
        } else {
            this->m_doodadLinks.LinkToTail(link);
        }

        if (!(this->m_flags & 0x2) || (doodadDef->m_flags & 0x1)) {
            doodadDef->m_flags = (doodadDef->m_flags & ~0x3) | 0x1;
        } else {
            doodadDef->m_flags |= 0x2;
        }

        if (doodadDef->m_flags & 0x40) {
            doodadDef->LinkToChunk(this);
        }
    }

    this->m_flags |= 0x8;
}

// OFFSET: 0x0075FF20
void CMapObjDefGroup::CreateBatchMaterials() {
    CMapObj* mapObj = this->m_mapObjDef->m_mapObj;
    CMapObjGroup* group = mapObj->GetGroup(this->m_groupNum, 0);
    int32_t unified = mapObj->m_header->m_flags & 0x2;
    uint32_t count = 0;

    for (uint32_t i = 0; i < group->m_batchCount; i++) {
        uint32_t shader = mapObj->m_materials[group->m_batches[i].m_materialId].m_shader;

        if (shader != 10 && shader != 14) {
            count += group->ComputePassCount(unified, i);
        }
    }

    this->m_batchMaterials.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        this->m_batchMaterials.m_data[i] = NULL;
    }
}

// OFFSET: 0x00760A80
CMapObjDefGroup::~CMapObjDefGroup() {
    if (this->m_liquidInstance) {
        this->m_liquidInstance->Release();
        this->m_liquidInstance = NULL;
    }
}

// OFFSET: 0x00760CE0
CMapObjDefGroup::CMapObjDefGroup() {
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->m_portalBox.b.x = 0.0f;
    this->m_portalBox.b.y = 0.0f;
    this->m_portalBox.b.z = 0.0f;
    this->m_portalBox.t.x = 0.0f;
    this->m_portalBox.t.y = 0.0f;
    this->m_portalBox.t.z = 0.0f;
    this->m_type |= 0x20;
    this->m_mapObjDef = NULL;
    this->m_groupNum = -1;
    this->m_frameStamp = 0;
    this->m_cullStamp = 0;
    this->unk_0050 = 0;
    this->m_liquidInstance = NULL;
    this->m_materialState = 0;
}

// OFFSET: 0x00760EF0
void CMapObjDef::CreateGroups() {
    uint32_t count = this->m_mapObj->m_groupCount;
    this->m_defGroups.SetCount(count);

    for (uint32_t i = 0; i < this->m_mapObj->m_groupCount; i++) {
        CMapObjDefGroup* group = CMap::AllocMapObjDefGroup();
        this->m_defGroupList.LinkToTail(group);
        group->m_mapObjDef = this;
        this->m_defGroups[i] = group;
        group->m_groupNum = i;
        this->m_mapObj->GetGroupBounds(group->m_portalBox, i);
        CWorldMath::TransformAABox(this->m_mat, group->m_portalBox, group->m_bbox);
        group->m_portalBox = group->m_bbox;
        group->m_flags |= (this->m_mapObj->GetGroupFlags(i) & 0x48) ? 0x1 : 0x2;
    }
}

// OFFSET: 0x00761060
CMapObjDef::CMapObjDef() {
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->m_lights.m_chunk = 0;
    *reinterpret_cast<uint32_t*>(&this->m_ambColor) = 0;
    this->m_worldMap = NULL;
    this->m_mapObj = NULL;
    this->m_doodadSet = 0;
    this->m_nameSet = 0;
    this->unk_0100 = 0;
    this->m_param64 = 0;
    this->m_type |= 0x10;
    this->m_perturbatorMesh = NULL;
    this->m_gatherStamp = 0;
    this->unk_004C = 0;
    this->m_skipPortal = -1;
}

// OFFSET: 0x00761260
CMapObjDef::~CMapObjDef() {
    if (this->m_perturbatorMesh) {
        Liquid::DestroyPerturbatorMesh(Liquid::PertSurface_Ocean, this->m_perturbatorMesh);
        this->m_perturbatorMesh = NULL;
    }
}

// OFFSET: 0x007613D0
void CMapObjDef::UpdateLoaded() {
    CAaBox box;
    box.b.x = 0.0f;
    box.b.y = 0.0f;
    box.b.z = 0.0f;
    box.t.x = 0.0f;
    box.t.y = 0.0f;
    box.t.z = 0.0f;
    this->m_mapObj->GetBounds(box);
    CWorldMath::TransformAABox(this->m_mat, box, this->m_bbox);
    this->m_ambColor = this->m_mapObj->m_ambColor;
    this->m_lights.SetCount(this->m_mapObj->m_lightCount);

    for (uint32_t i = 0; i < this->m_lights.m_count; i++) {
        this->m_lights.m_data[i] = NULL;
    }

    this->CreateGroups();

    if (this->m_flags & 0x100000) {
        this->m_worldMap->LinkObjectToMapExterior(this, this->m_bbox, 1, 0);
    }

    this->m_flags |= 0x40;
}
