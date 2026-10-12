#include "WoW/Source/WorldClient/MapObjDef.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapObjRender.h"
#include "WoW/Source/WorldClient/WorldMap.h"

// PROVEN: c6695cb7 IDENTICAL/IDENTICAL
// OFFSET: 0x0075EDD0/0x14048C530
void CMapObjDefGroup::WaterDestroy() {
    if (this->m_liquidInstance) {
        this->m_liquidInstance->Release();
        this->m_liquidInstance = NULL;
    }
}

// PROVEN: 01f72353 IDENTICAL/IDENTICAL
// OFFSET: 0x0075EDF0/0x140442DF0
void CMapObjDefGroup::UpdateBounds(const C44Matrix& matrix) {
    this->m_mapObjDef->m_mapObj->GetGroupBounds(this->m_portalBox, this->m_groupNum);
    CWorldMath::TransformAABox(matrix, this->m_portalBox, this->m_bbox);
    this->m_portalBox = this->m_bbox;
}

// PROVEN: 1ac3adee IDENTICAL/NONE
// OFFSET: 0x00756DF0
CMapObjDefGroup* CMapObjDef::GetDefGroup(uint32_t index) {
    return this->m_defGroups[index];
}

// PROVEN: e9e8d538 IDENTICAL/IDENTICAL
// OFFSET: 0x0075EE50/0x140442E60
bool CMapObjDef::TestAABox(const C3Vector& start, const C3Vector& end) {
    return CWorldMath::VectorIntersectAABox2(this->m_bbox, start, end);
}

// PROVEN: a922f468 IDENTICAL/NONE
// OFFSET: 0x0075EF10/0x140442F60
void CMapObjDefGroup::ExtendBounds(const CAaBox& box) {
    this->m_bbox |= box;
    this->m_mapObjDef->m_bbox |= box;
}

// OFFSET: 0x0075EF50/0x140443010
uint32_t CMapObjDef::GetGroundType(uint32_t groupNum, uint16_t polyIndex) {
    CMapObjGroup* group = this->m_mapObj->GetGroup(groupNum, 0);

    if (group && polyIndex < group->m_polyCount) {
        const SMOPoly* poly = &group->m_polys[polyIndex];

        if (poly) {
            return this->m_mapObj->m_materials[poly->m_materialId].m_groundType;
        }
    }

    return 0xFFFFFFFF;
}

// OFFSET: 0x0075EFB0/0x140443090
void CMapObjDef::UpdateBounds() {
    CAaBox bounds;
    memset(&bounds, 0, sizeof(bounds));
    this->m_mapObj->GetBounds(bounds);
    CWorldMath::TransformAABox(this->m_mat, bounds, this->m_bbox);
}

// OFFSET: 0x0075F080/0x1404431C0
int32_t CMapObjDef::CanLinkTo() {
    return (this->m_flags & 0x40) && this->m_defGroupList.Head() ? 1 : 0;
}

// OFFSET: 0x0075F0A0/0x1404431E0
bool CMapObjDef::GetFacets(const CFrustum& frustum, World::FacetData& facets, uint32_t flags) {
    if (!(this->m_mapObj->m_flags & 0x40) || !this->m_defGroupList.Head()) {
        facets.m_status |= 0x2;
        return true;
    }

    C3Vector corners[8];

    for (uint32_t i = 0; i < 8; i++) {
        corners[i] = frustum.corners[i] * this->m_invMat;
    }

    CFrustum local(corners);

    if (flags & 0x300F0) {
        World::TriData::s_statusFlags = 0;
        World::TriData::s_nBatches = 0;
        World::TriData::s_faceIndexCursor = 0;
        World::TriData::s_indexCursor = 0;
        World::TriData::s_nMatrices = 0;
        World::TriData tris;
        this->m_mapObj->GetTris(local, flags, tris, this);

        if (World::TriData::s_nBatches) {
            World::TriDataToFacetData(tris, facets, 0, 0);
            facets.m_status |= World::TriData::s_statusFlags;
            return true;
        }
    }

    return true;
}

// OFFSET: 0x0075F890/0x1404439F0
void CMapObjDefGroup::UpdateMatrix(const C44Matrix& matrix) {
    this->UpdateBounds(matrix);

    for (CMapBaseObjLink* link = this->m_doodadLinks.Head(), * next; link; link = next) {
        next = this->m_doodadLinks.Next(link);
        static_cast<CMapDoodadDef*>(link->m_owner)->UpdateMatrixOnly(matrix);
    }

    Liquid::CInstance* instance = this->m_liquidInstance;

    if (instance) {
        instance->m_transform = matrix;
        float centerY = this->m_bbox.t.y + this->m_bbox.b.y;
        float centerZ = this->m_bbox.t.z + this->m_bbox.b.z;
        this->m_liquidInstance->m_bounds.c.x = (this->m_bbox.b.x + this->m_bbox.t.x) * 0.5f;
        this->m_liquidInstance->m_bounds.c.y = centerY * 0.5f;
        this->m_liquidInstance->m_bounds.c.z = centerZ * 0.5f;
        float dz = this->m_bbox.t.z - this->m_bbox.b.z;
        float dy = this->m_bbox.t.y - this->m_bbox.b.y;
        float distSq = dy * dy + dz * dz;
        float dx = this->m_bbox.t.x - this->m_bbox.b.x;
        this->m_liquidInstance->m_bounds.r = sqrt(dx * dx + distSq) * 0.5f;
    }
}

// OFFSET: 0x0075F9D0/0x140443B70
void CMapObjDefGroup::SetSequence(uint32_t sequence, int32_t a2, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->m_doodadLinks.Head(), * next; link; link = next) {
        next = this->m_doodadLinks.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadSet && !doodadDef->m_doodadSet) {
            break;
        }

        if (doodadDef->m_model && doodadDef->m_doodadSet == doodadSet) {
            doodadDef->m_model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
        }
    }
}

// OFFSET: 0x0075FA50
void CMapObjDefGroup::SetSequenceSpeed(float speed, int32_t a2, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->m_doodadLinks.Head(), * next; link; link = next) {
        next = this->m_doodadLinks.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadSet && !doodadDef->m_doodadSet) {
            break;
        }

        if (doodadDef->m_model && doodadDef->m_doodadSet == doodadSet) {
            doodadDef->m_model->SetAnimationSpeed(speed);
        }
    }
}

// OFFSET: 0x0075FAC0/0x140443C60
void CMapObjDefGroup::SetSequenceFinishedCallback(WORLD_SEQUENCE_CALLBACK callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->m_doodadLinks.Head(), * next; link; link = next) {
        next = this->m_doodadLinks.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadSet && !doodadDef->m_doodadSet) {
            break;
        }

        if (doodadDef->m_model && doodadDef->m_doodadSet == doodadSet) {
            doodadDef->m_model->SetSequenceCallback(reinterpret_cast<void*>(callback), param);
        }
    }
}

// OFFSET: 0x0075FB30/0x140443D10
void CMapObjDefGroup::SetEventCallback(WORLD_MODEL_EVENT_CALLBACK callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->m_doodadLinks.Head(), * next; link; link = next) {
        next = this->m_doodadLinks.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadSet && !doodadDef->m_doodadSet) {
            break;
        }

        if (doodadDef->m_model && doodadDef->m_doodadSet == doodadSet) {
            doodadDef->m_model->SetEventCallback(reinterpret_cast<void*>(callback), param);
        }
    }
}

// OFFSET: 0x0075FBA0
bool CMapObjDefGroup::IsAllDrawable() {
    if (!(this->m_flags & 0x8)) {
        return false;
    }

    for (CMapBaseObjLink* link = this->m_doodadLinks.Head(), * next; link; link = next) {
        next = this->m_doodadLinks.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadDef->m_model && !(doodadDef->m_flags & 0x40)) {
            return false;
        }
    }

    return true;
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

// OFFSET: 0x0075FD10/0x140443F50
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

// OFFSET: 0x0075FF20/0x1404441D0
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

// OFFSET: 0x00760020/0x140444360
void CMapObjDef::SetSequence(uint32_t sequence, int32_t a2, uint16_t doodadSet) {
    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
        next = this->m_defGroupList.Next(group);
        group->SetSequence(sequence, a2, doodadSet);
    }
}

// OFFSET: 0x00760070
void CMapObjDef::SetSequenceSpeed(float speed, int32_t a2, uint16_t doodadSet) {
    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
        next = this->m_defGroupList.Next(group);
        group->SetSequenceSpeed(speed, a2, doodadSet);
    }
}

// OFFSET: 0x007600D0/0x1404444D0
void CMapObjDef::SetSequenceFinishedCallback(WORLD_SEQUENCE_CALLBACK callback, uint64_t param, uint16_t doodadSet) {
    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
        next = this->m_defGroupList.Next(group);
        group->SetSequenceFinishedCallback(callback, param, doodadSet);
    }
}

// OFFSET: 0x00760130/0x140444560
void CMapObjDef::SetEventCallback(WORLD_MODEL_EVENT_CALLBACK callback, uint64_t param, uint16_t doodadSet) {
    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
        next = this->m_defGroupList.Next(group);
        group->SetEventCallback(callback, param, doodadSet);
    }
}

// OFFSET: 0x00760190/0x1404445F0
int32_t CMapObjDef::IsAllLoaded() {
    if (!this->m_mapObj->IsAllLoaded()) {
        return 0;
    }

    for (CMapObjDefGroup* group = this->m_defGroupList.Head(); group; group = this->m_defGroupList.Next(group)) {
        if (!(group->m_flags & 0x8)) {
            return 0;
        }

        for (CMapBaseObjLink* link = group->m_doodadLinks.Head(), * next; link; link = next) {
            next = group->m_doodadLinks.Next(link);
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

            if (doodadDef->m_model && !(doodadDef->m_flags & 0x40)) {
                return 0;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00760220
int32_t CMapObjDef::IsAllDrawable() {
    uint32_t flags = this->m_flags;

    if (!(flags & 0x40)) {
        return 0;
    }

    if (!(flags & 0x2000)) {
        for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
            next = this->m_defGroupList.Next(group);

            if (!group->IsAllDrawable()) {
                return 0;
            }
        }

        this->m_flags = flags | 0x2000;
    }

    return 1;
}

// OFFSET: 0x00760290/0x140444730
void CMapObjDef::UpdateMoved() {
    if (!(this->m_flags & 0x4000)) {
        this->m_flags |= 0x200;
    }

    if (this->m_mapObj->m_flags & 0x40) {
        CAaBox bounds;
        memset(&bounds, 0, sizeof(bounds));
        this->m_mapObj->GetBounds(bounds);
        CWorldMath::TransformAABox(this->m_mat, bounds, this->m_bbox);

        for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
            next = this->m_defGroupList.Next(group);
            group->UpdateMatrix(this->m_mat);
        }
    } else {
        this->m_bbox.t = this->m_position;
        this->m_bbox.b = this->m_position;
    }
}

// OFFSET: 0x00760350
void CMapObjDef::SetDoodadEmittersIgnoresDistance(int32_t enable, uint16_t doodadSet) {
    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * nextGroup; group; group = nextGroup) {
        nextGroup = this->m_defGroupList.Next(group);

        for (CMapBaseObjLink* link = group->m_doodadLinks.Head(), * next; link; link = next) {
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
            next = group->m_doodadLinks.Next(link);

            if (!doodadDef->m_doodadSet) {
                break;
            }

            if (doodadDef->m_doodadSet != doodadSet || !doodadDef->m_model) {
                continue;
            }

            for (uint32_t i = 0; ; i++) {
                CM2Model* model = doodadDef->m_model;

                if (!(model->m_fFlags & 0x1)) {
                    model->WaitForLoad(NULL);
                }

                if (i >= model->m_shared->m_data->m_particles.m_count) {
                    break;
                }

                CM2Model* emitterModel = doodadDef->m_model;

                if (!(emitterModel->m_fFlags & 0x1)) {
                    emitterModel->WaitForLoad(NULL);
                }

                CParticleEmitter2* emitter = emitterModel->m_particleEmitters[i];

                if (enable) {
                    emitter->m_flags |= 0x400000;
                } else {
                    emitter->m_flags &= ~0x400000;
                }
            }
        }
    }
}

// OFFSET: 0x00760470/0x1404449A0
void CMapObjDef::LinkDoodadDef(CMapDoodadDef* doodadDef, int32_t testBounds) {
    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
        next = this->m_defGroupList.Next(group);

        if (this->m_mapObj->GetGroupFlags(group->m_groupNum) & 0x410080) {
            continue;
        }

        if (testBounds) {
            const CAaBox& box = doodadDef->m_bbox;

            if (box.t.x < group->m_bbox.b.x || box.t.y < group->m_bbox.b.y || box.t.z < group->m_bbox.b.z || group->m_bbox.t.x < box.b.x || group->m_bbox.t.y < box.b.y || group->m_bbox.t.z < box.b.z) {
                continue;
            }
        }

        CMapBaseObjLink* link = CMap::AllocBaseObjLink(doodadDef, group);
        doodadDef->m_parentLinkList.LinkToTail(link);
        group->m_doodadLinks.LinkToHead(link);
        doodadDef->LinkToChunk(group);
    }
}

// OFFSET: 0x00760550
int32_t CMapObjDef::MoveDoodadDefs(uint16_t doodadSet, CMapObjDef* target, uint16_t targetDoodadSet) {
    if (!(target->m_flags & 0x40) || !target->m_defGroupList.Head()) {
        return 0;
    }

    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * nextGroup; group; group = nextGroup) {
        nextGroup = this->m_defGroupList.Next(group);

        for (CMapBaseObjLink* link = group->m_doodadLinks.Head(), * next; link; link = next) {
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
            uint16_t set = doodadDef->m_doodadSet;
            next = group->m_doodadLinks.Next(link);

            if (!set) {
                break;
            }

            if (set != doodadSet) {
                continue;
            }

            for (CMapBaseObjLink* parentLink = doodadDef->m_parentLinkList.Head(), * nextParent; parentLink; parentLink = nextParent) {
                nextParent = doodadDef->m_parentLinkList.Next(parentLink);
                CMap::FreeBaseObjLink(parentLink);
            }

            target->LinkDoodadDef(doodadDef, 0);
            doodadDef->m_doodadSet = targetDoodadSet;
        }
    }

    return 0;
}

// OFFSET: 0x00760650/0x140444C80
void CMapObjDef::SetDoodadsEnabled(int32_t enable, uint16_t doodadSet) {
    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * nextGroup; group; group = nextGroup) {
        nextGroup = this->m_defGroupList.Next(group);

        for (CMapBaseObjLink* link = group->m_doodadLinks.Head(), * next; link; link = next) {
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
            next = group->m_doodadLinks.Next(link);

            if (!doodadDef->m_doodadSet) {
                break;
            }

            if (doodadDef->m_doodadSet != doodadSet || !doodadDef->m_model) {
                continue;
            }

            if (enable) {
                doodadDef->m_flags &= ~0x90;
                doodadDef->m_entityFlags &= ~0xF;
                this->m_worldMap->LinkDoodadDefToMapObjDefs(doodadDef, this);
            } else {
                doodadDef->m_flags |= 0x90;

                for (CMapBaseObjLink* parentLink = doodadDef->m_parentLinkList.Head(), * nextParent; parentLink; parentLink = nextParent) {
                    nextParent = doodadDef->m_parentLinkList.Next(parentLink);
                    CMapBaseObj* ref = parentLink->m_ref;

                    if (!(ref->m_type & MapBaseObj_Type_MapObjDefGroup) || static_cast<CMapObjDefGroup*>(ref)->m_mapObjDef != this) {
                        CMap::FreeBaseObjLink(parentLink);
                    }
                }
            }
        }
    }
}

// OFFSET: 0x00760760/0x140444DD0
bool CMapObjDef::GetFacets(const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags) {
    CAaBox local;
    memset(&local, 0, sizeof(local));
    CWorldMath::TransformAABox(this->m_invMat, box2, local);

    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
        next = this->m_defGroupList.Next(group);

        if (group->m_flags & 0x40) {
            if (box2.t.x < group->m_bbox.b.x || box2.t.y < group->m_bbox.b.y || box2.t.z < group->m_bbox.b.z || group->m_bbox.t.x < box2.b.x || group->m_bbox.t.y < box2.b.y || group->m_bbox.t.z < box2.b.z) {
                continue;
            }

            if ((flags & 0x300F0) && this->m_mapObj->TestGroupBounds(local, group->m_groupNum, 0)) {
                this->m_mapObj->GetFacets(local, flags, facets, this, group->m_groupNum);
            }

            if ((flags & 0xF) && !CMap::GetDoodadDefFacets(group->m_doodadLinks, box, box2, facets, flags, this->m_param64)) {
                return false;
            }

            if ((flags & 0xF00000) && !CMap::GetEntityFacets(group->m_entityLinks, box, box2, facets, flags)) {
                return false;
            }
        } else {
            CAaBox groupBox;
            groupBox.b.x = group->m_bbox.b.x;
            groupBox.b.y = group->m_bbox.b.y;
            groupBox.b.z = group->m_bbox.b.z - 0.5f;
            groupBox.t.x = group->m_bbox.t.x;
            groupBox.t.y = group->m_bbox.t.y;
            groupBox.t.z = group->m_bbox.t.z + 1.0f;

            if (box2.t.x >= groupBox.b.x && box2.t.y >= groupBox.b.y && box2.t.z >= groupBox.b.z && groupBox.t.x >= box2.b.x && groupBox.t.y >= box2.b.y && groupBox.t.z >= box2.b.z) {
                if (box.t.x > group->m_bbox.b.x && box.t.y > groupBox.b.y && box.t.z > group->m_bbox.b.z && groupBox.t.x > box.b.x && groupBox.t.y > box.b.y && group->m_bbox.t.z > box.b.z) {
                    facets.m_status |= 0x2;
                    return false;
                }

                if (flags & 0x80000000) {
                    World::AddAaBoxFacets(groupBox, this->m_param64, facets, 1);
                }
            }
        }
    }

    return true;
}

// OFFSET: [0x00760A80/0x1404451E0, 0x00760DA0/0x140445790]
CMapObjDefGroup::~CMapObjDefGroup() {
    if (this->m_liquidInstance) {
        this->m_liquidInstance->Release();
        this->m_liquidInstance = NULL;
    }
}

// OFFSET: 0x00760BA0/0x1404453C0
void CMapObjDef::UpdatePos(const C3Vector& pos, float facing, float pitch, float roll) {
    this->m_position = pos;
    this->m_mat = C44Matrix();
    this->m_mat.d0 = pos.x;
    this->m_mat.d1 = pos.y;
    this->m_mat.d2 = pos.z;
    this->m_mat.RotateAroundZ(facing);
    this->m_mat.RotateAroundY(pitch);
    this->m_mat.RotateAroundX(roll);
    this->m_invMat = this->m_mat.AffineInverse();
    this->UpdateMoved();
}

// OFFSET: 0x00760C90
void CMapObjDef::UpdateMatrix(const C44Matrix& matrix) {
    this->m_position.x = matrix.d0;
    this->m_position.y = matrix.d1;
    this->m_position.z = matrix.d2;
    this->m_mat = matrix;
    this->m_invMat = this->m_mat.AffineInverse();
    this->UpdateMoved();
}

// OFFSET: 0x00760CE0/0x1404456B0
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
    this->m_areaName = NULL;
    this->m_liquidInstance = NULL;
    this->m_materialState = 0;
}

// OFFSET: 0x00760DD0/0x1404457E0
void CMapObjDef::Purge(int32_t unlinkWorld) {
    if (this->m_refCount) {
        return;
    }

    for (CMapObjDefGroup* group = this->m_defGroupList.Head(), * next; group; group = next) {
        next = this->m_defGroupList.Next(group);
        group->ClearRefs();
        group->m_mapObjDefLink.Unlink();
        CMap::FreeMapObjDefGroup(group);
    }

    if (this->m_defGroups.m_count == 0xFFFFFFFF) {
        this->m_defGroups.Dynamic().m_count = 0;
    } else {
        this->m_defGroups.m_count = 0;
    }

    CMapObj::Delete(this->m_mapObj);
    this->m_mapObj = NULL;

    if (unlinkWorld) {
        this->m_worldMap->m_mapObjDefHash.Unlink(this);
    }

    this->m_link30.Unlink();
    CMap::FreeMapObjDef(this);
}

// OFFSET: 0x00760EF0/0x140445950
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

// OFFSET: 0x00761060/0x140445B20
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

// OFFSET: [0x00761260/0x140445D30, 0x007614C0/0x140446020]
CMapObjDef::~CMapObjDef() {
    if (this->m_perturbatorMesh) {
        Liquid::DestroyPerturbatorMesh(Liquid::PertSurface_Ocean, this->m_perturbatorMesh);
        this->m_perturbatorMesh = NULL;
    }
}

// OFFSET: 0x007613D0/0x140445F50
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
        this->m_worldMap->LinkMapObjDef(this);
    }

    this->m_flags |= 0x40;
}
