#include "WoW/Source/WorldClient/MapEntity.h"
#include <cmath>
#include <cstring>
#include <functional>
#include <queue>
#include <utility>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

struct SFogQueueLess {
    bool operator()(const std::pair<float, uint32_t>& a, const std::pair<float, uint32_t>& b) const {
        return a.first < b.first;
    }
};

typedef std::priority_queue<std::pair<float, uint32_t>, std::vector<std::pair<float, uint32_t>, Blizzard::Memory::AllocSTL_raw<std::pair<float, uint32_t> > >, SFogQueueLess> FogQueue;

// OFFSET: 0x00733140/0x14040E850
void CMapEntity::UpdateMapObjLiquid(CMapObjDef* mapObjDef, CMapObjGroup* group) {
    C3Vector local = this->m_position * mapObjDef->m_invMat;
    uint32_t liquidType;
    float height;

    if (group->QueryLiquid(local, 0, liquidType, height)) {
        float z = mapObjDef->m_mat.a2 * local.x + mapObjDef->m_mat.b2 * local.y + mapObjDef->m_mat.c2 * height + mapObjDef->m_mat.d2;

        if (z + 0.0099999998f > this->m_position.z) {
            this->m_entityFlags |= 0x1;
            uint8_t transparent = Liquid::IsTransparent(liquidType);
            this->m_entityFlags ^= (this->m_entityFlags ^ (2 * transparent)) & 0x2;
            this->m_liquidHeight = z;
            this->m_liquidType = static_cast<uint16_t>(liquidType);
        }

        if (z > this->m_bbox.b.z && this->m_bbox.t.z > z) {
            this->m_entityFlags |= 0x4;
        }
    }

    if (this->m_model && (mapObjDef->m_flags & 0x20000)) {
        this->m_model->m_fFlags |= 0x4000000;
    }
}

// OFFSET: 0x007332E0/0x14040EA00
void CMapEntity::UpdateTerrainLiquid() {
    C3Vector pos = this->m_position;
    uint32_t liquidType;
    float height;

    if (!this->m_worldMap->QueryLiquidTerrain(pos, liquidType, height)) {
        return;
    }

    if (height + 0.0099999998f > this->m_position.z) {
        this->m_entityFlags |= 0x1;
        uint8_t transparent = Liquid::IsTransparent(liquidType);
        this->m_entityFlags ^= (this->m_entityFlags ^ (2 * transparent)) & 0x2;
        this->m_liquidType = static_cast<uint16_t>(liquidType);
        this->m_liquidHeight = height;
    }

    if (height > this->m_bbox.b.z && this->m_bbox.t.z > height) {
        this->m_entityFlags |= 0x4;
    }
}

// OFFSET: 0x00733390/0x14040EAC0
static void ApproachColor(const CImVector& target, CImVector& current) {
    int32_t dr = target.r - current.r;
    int32_t dg = target.g - current.g;
    int32_t db = target.b - current.b;
    int32_t da = target.a - current.a;

    if (!(dr | dg | db | da)) {
        return;
    }

    int32_t step = static_cast<int32_t>(FrameTime::GetTickTimeSec() * 0.5f * 255.0f);

    if (step < 1) {
        step = 1;
    }

    if (dr > 0) {
        int32_t v = current.r + step;
        current.r = static_cast<uint8_t>(target.r < v ? target.r : v);
    } else if (dr < 0) {
        int32_t v = current.r - step;
        current.r = static_cast<uint8_t>(target.r > v ? target.r : v);
    }

    if (dg > 0) {
        int32_t v = current.g + step;
        current.g = static_cast<uint8_t>(target.g < v ? target.g : v);
    } else if (dg < 0) {
        int32_t v = current.g - step;
        current.g = static_cast<uint8_t>(target.g > v ? target.g : v);
    }

    if (db > 0) {
        int32_t v = current.b + step;
        current.b = static_cast<uint8_t>(target.b < v ? target.b : v);
    } else if (db < 0) {
        int32_t v = current.b - step;
        current.b = static_cast<uint8_t>(target.b > v ? target.b : v);
    }

    if (da > 0) {
        int32_t v = current.a + step;
        current.a = static_cast<uint8_t>(target.a < v ? target.a : v);
    } else if (da < 0) {
        int32_t v = current.a - step;
        current.a = static_cast<uint8_t>(target.a > v ? target.a : v);
    }
}

// OFFSET: 0x007334B0/0x14040EC00
void CMapEntity::UpdateBounds() {
    CM2Model* model = this->m_model;

    if (!(model->m_shared->m_flags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    CAaBox collision = model->m_shared->m_data->m_collisionBounds.m_extent;
    CAaBox box = model->GetBoundingBox();
    C3Vector center;
    center.y = (collision.t.y + collision.b.y) * 0.5f;
    center.z = (collision.t.z + collision.b.z) * 0.5f;
    center.x = (collision.t.x + collision.b.x) * 0.5f;
    this->m_vec2 = center * model->m_worldTransform;

    this->m_sphere = model->GetBoundingSphere();
    float radius = this->m_sphere.r;

    if (radius > 0.000099999997f) {
        const C44Matrix& m = model->m_worldTransform;
        this->m_sphere.r = sqrt(m.a2 * m.a2 + m.a1 * m.a1 + m.a0 * m.a0) * radius;
        this->m_sphere.c = this->m_sphere.c * m;
    }

    box |= collision;

    if (box.t.x > box.b.x && box.t.y > box.b.y && box.t.z > box.b.z) {
        this->m_bbox = CAaBox::Transform(box, model->m_worldTransform);
    }

    if (this->m_entityFlags & 0x1000) {
        this->m_entityFlags |= 0x4000;
    }
}

// OFFSET: 0x007337C0
static void LerpInteriorFog(float* fog, const float* target, float t) {
    uint8_t blend = static_cast<uint8_t>(static_cast<int32_t>(t * 255.0f));

    for (int32_t i = 0; i < 2; i++) {
        fog[0] = (target[0] - fog[0]) * t + fog[0];
        fog[1] = (target[1] - fog[1]) * t + fog[1];
        CImVector color = *reinterpret_cast<const CImVector*>(&target[2]);

        if (blend) {
            reinterpret_cast<CImVector*>(&fog[2])->InterpColor(blend, &color);
        }

        target += 3;
        fog += 3;
    }
}

// OFFSET: 0x00733880/0x14040F0E0
CMapEntity::CMapEntity() {
    this->unk_00D8 = 0;
    this->unk_00C0 = 0;
    this->unk_00C4 = 0;
    this->m_updateCallback = NULL;
    this->m_updateParam = NULL;
    this->m_param64 = 0;
    this->m_param32 = 0;
    this->m_groundType = 0;
    this->m_liquidType = 0;
    this->unk_00BC = -1;
    this->unk_00C8 = 0;
    this->m_cullStamp = -1;
    this->unk_00D4 = 0xFF;
    this->m_type |= MapBaseObj_Type_Entity;
    this->unk_00DC = 1.0f;
    this->unk_00E0 = 1.0f;
}

// OFFSET: [0x00733950, 0x007346E0]
CMapEntity::~CMapEntity() {
    if (reinterpret_cast<CMapEntity*>(CWorldScene::camTargEntity) == this) {
        CWorldScene::camTargEntity = 0;
    }

    if (this->m_model) {
        this->m_model->m_lightingCallback = NULL;
        this->m_model->m_lightingArg = NULL;
        this->m_model->Release();
        this->m_model = NULL;
    }
}

// OFFSET: 0x00733A10/0x14040F2E0
void CMapEntity::SelectLights(CM2Lighting* lighting) {
    float scale = this->unk_00E0;
    C3Vector direct;
    C3Vector dir;
    C3Vector ambient;

    if (!(this->m_flags & 0x2)) {
        const DayNight::LightInfo* info = DayNight::GetLightInfo();
        direct.x = this->m_interiorDirect.r * 0.0039215689f * scale;
        direct.y = this->m_interiorDirect.g * 0.0039215689f * scale;
        direct.z = this->m_interiorDirect.b * 0.0039215689f * scale;
        dir = info->m_direction;
        ambient.x = this->m_interiorAmbient.r * 0.0039215689f;
        ambient.y = this->m_interiorAmbient.g * 0.0039215689f;
        ambient.z = this->m_interiorAmbient.b * 0.0039215689f;
    } else {
        direct.x = this->m_interiorDirect.r * 0.0039215689f * scale;
        direct.y = this->m_interiorDirect.g * 0.0039215689f * scale;
        direct.z = this->m_interiorDirect.b * 0.0039215689f * scale;
        dir = CMapStaticEntity::s_interiorLightDir;
        ambient.x = this->m_interiorAmbient.r * 0.0039215689f;
        ambient.y = this->m_interiorAmbient.g * 0.0039215689f;
        ambient.z = this->m_interiorAmbient.b * 0.0039215689f;

        if (this->m_entityFlags & 0x400) {
            const DayNight::LightInfo* info = DayNight::GetLightInfo();
            float t = this->m_interiorDirect.a * 0.0039215689f;
            C3Vector blended;
            blended.z = (info->m_direction.z - dir.z) * t + dir.z;
            blended.y = (info->m_direction.y - dir.y) * t + dir.y;
            blended.x = (info->m_direction.x - dir.x) * t + dir.x;
            float inv = 1.0f / sqrt(blended.z * blended.z + blended.y * blended.y + blended.x * blended.x);
            dir.x = blended.x * inv;
            dir.y = blended.y * inv;
            dir.z = inv * blended.z;
        }
    }

    if (this->m_staticFlags & 0x20) {
        ambient.x = World::GetCharacterAmbMul() * ambient.x;
        ambient.y = World::GetCharacterAmbMul() * ambient.y;
        ambient.z = World::GetCharacterAmbMul() * ambient.z;

        if (ambient.x >= 1.0f) {
            ambient.x = 1.0f;
        }

        if (ambient.y >= 1.0f) {
            ambient.y = 1.0f;
        }

        if (ambient.z >= 1.0f) {
            ambient.z = 1.0f;
        }
    }

    lighting->AddAmbient(ambient);
    lighting->AddDiffuse(direct, dir);

    const DayNight::FogInfo* fog = (this->m_flags & 0x4) ? DayNight::GetIntFogInfo() : DayNight::GetFogInfo();
    C3Vector fogColor(fog->m_color.r * 0.0039215689f, fog->m_color.g * 0.0039215689f, fog->m_color.b * 0.0039215689f);
    lighting->SetFog(fogColor, fog->m_start, fog->m_end, fog->m_density);
}

// OFFSET: 0x00733DA0
void CMapEntity::AdjustLighting(CMapObjDef* mapObjDef, uint32_t groupNum, const uint16_t* polyIndex, const C3Vector* pos) {
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0;
    bool exterior;
    bool found;

    if (polyIndex) {
        C3Vector local = *pos * mapObjDef->m_invMat;
        found = mapObjDef->m_mapObj->QueryLighting(groupNum, local, *polyIndex, color, exterior) != 0;
    } else {
        C3Vector local = this->m_position * mapObjDef->m_invMat;
        C3Segment seg;
        seg.b = C3Vector(local.x, local.y, local.z + 1.0f);
        seg.t = C3Vector(local.x, local.y, local.z - 12.0f);
        found = mapObjDef->m_mapObj->QueryLighting(groupNum, seg, color, exterior);
    }

    if (!found) {
        return;
    }

    CMapStaticEntity::AdjustLighting(color, *reinterpret_cast<CImVector*>(&this->unk_00D8), 168, *reinterpret_cast<CImVector*>(&this->unk_00D4), 96);

    if (exterior) {
        const DayNight::LightInfo* info = DayNight::GetLightInfo();
        uint8_t alpha = color.a;

        if (alpha) {
            reinterpret_cast<CImVector*>(&this->unk_00D8)->InterpColor(alpha, &info->m_directColor);
            reinterpret_cast<CImVector*>(&this->unk_00D4)->InterpColor(alpha, &info->m_ambientColor);
        }

        this->m_entityFlags |= 0x400;
        reinterpret_cast<CImVector*>(&this->unk_00D8)->a = color.a;
    } else {
        this->m_entityFlags &= ~0x400;
        reinterpret_cast<CImVector*>(&this->unk_00D8)->a = 0;
    }
}

// OFFSET: 0x00734050/0x14040FAA0
void CMapEntity::UpdateLighting() {
    CImVector& ambient = *reinterpret_cast<CImVector*>(&this->unk_00D4);
    CImVector& direct = *reinterpret_cast<CImVector*>(&this->unk_00D8);

    if (!(this->m_flags & 0x2)) {
        const DayNight::LightInfo* info = DayNight::GetLightInfo();
        uint8_t g = this->m_exteriorVertexLight.g;
        uint8_t r = this->m_exteriorVertexLight.r;

        if (r | g | this->m_exteriorVertexLight.b) {
            int32_t hr = r >> 1;
            int32_t hg = g >> 1;
            int32_t hb = this->m_exteriorVertexLight.b >> 1;
            int32_t v = hr + info->m_ambientColor.r;
            ambient.r = static_cast<uint8_t>(v > 255 ? 255 : v);
            v = hg + info->m_ambientColor.g;
            ambient.g = static_cast<uint8_t>(v > 255 ? 255 : v);
            v = hb + info->m_ambientColor.b;
            ambient.b = static_cast<uint8_t>(v > 255 ? 255 : v);
            v = hr + info->m_directColor.r;
            direct.r = static_cast<uint8_t>(v > 255 ? 255 : v);
            v = hg + info->m_directColor.g;
            direct.g = static_cast<uint8_t>(v > 255 ? 255 : v);
            v = hb + info->m_directColor.b;
            direct.b = static_cast<uint8_t>(v > 255 ? 255 : v);
        } else {
            ambient = info->m_ambientColor;
            direct = info->m_directColor;
        }
    }

    ApproachColor(ambient, this->m_interiorAmbient);
    ApproachColor(direct, this->m_interiorDirect);

    if (this->m_entityFlags & 0x10000) {
        this->m_interiorAmbient = ambient;
        this->m_interiorDirect = direct;
        this->m_entityFlags &= ~0x10000;
    }

    float delta = this->unk_00E0 - this->unk_00DC;

    if (delta != 0.0f) {
        float step = FrameTime::GetTickTimeSec() * 0.5f;
        float target = this->unk_00DC;
        float value;
        bool reached;

        if (delta >= 0.0f) {
            value = this->unk_00E0 - step;
            reached = target <= value;
        } else {
            value = this->unk_00E0 + step;
            reached = value <= target;
        }

        this->unk_00E0 = value;

        if (!reached) {
            this->unk_00E0 = target;
        }
    }

    if (!this->m_parentLinkList.Head()) {
        this->m_entityFlags |= 0x4000;
    }
}

// OFFSET: 0x00734710/0x140410610
int32_t CMapEntity::QueryInterior(int32_t a2) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (ref->m_type & 0x4) {
            break;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
        CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (!group) {
            continue;
        }

        if (!(group->m_groupFlags & 0x8)) {
            return 1;
        }

        if (a2 || !(mapObjDef->m_flags & 0x200)) {
            break;
        }
    }

    return 0;
}

// OFFSET: 0x00734790
int32_t CMapEntity::QueryMapObjZoneName(const char*& name) {
    CMapObjDef* mapObjDef;
    CMapObj* mapObj;
    CMapObjDefGroup* defGroup;
    CMapObjGroup* group;

    if (!this->GetMapObjAndGroup(mapObjDef, mapObj, defGroup, group, 1)) {
        return 0;
    }

    name = "";
    const WMOAreaTableRec* rec = SDBWMOAreaTableLookup(mapObjDef->m_mapObj->m_header->m_wmoId, mapObjDef->m_nameSet, static_cast<uint32_t>(-1));

    if (!rec) {
        return 1;
    }

    if (*rec->m_areaName) {
        name = rec->m_areaName;
        return 1;
    }

    uint32_t areaId = rec->m_areaTableID;

    if (areaId || (areaId = this->m_worldMap->QueryAreaIdTerrain(this->m_position)) != 0) {
        AreaTableRec* areaRec = g_areaTableDB.GetRecord(areaId);

        if (areaRec) {
            name = areaRec->m_areaName;
        }
    }

    return 1;
}

// OFFSET: 0x00734840
int32_t CMapEntity::QueryMapObjSubzoneName(const char*& name, uint32_t& areaId) {
    CMapObjDef* mapObjDef;
    CMapObj* mapObj;
    CMapObjDefGroup* defGroup;
    CMapObjGroup* group;

    if (!this->GetMapObjAndGroup(mapObjDef, mapObj, defGroup, group, 1)) {
        return 0;
    }

    if (!defGroup->m_areaName) {
        const WMOAreaTableRec* rec = SDBWMOAreaTableLookup(mapObj->m_header->m_wmoId, mapObjDef->m_nameSet, group->m_wmoGroupId);
        defGroup->m_areaName = rec ? static_cast<const char*>(rec->m_areaName) : "";
    }

    name = defGroup->m_areaName;

    if (defGroup->m_groupNum) {
        areaId = defGroup->m_groupNum;
    }

    return 1;
}

// OFFSET: 0x007348D0/0x140410920
int32_t CMapEntity::QueryMapObjAreaTable(const WMOAreaTableRec*& groupRec, const WMOAreaTableRec*& rootRec, uint32_t& wmoGroupId, int32_t a5) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (ref->m_type & 0x4) {
            break;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
        CMapObj* mapObj = mapObjDef->m_mapObj;
        CMapObjGroup* group = mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (group && (!(group->m_groupFlags & 0x8) || a5 || !(mapObjDef->m_flags & 0x200))) {
            wmoGroupId = group->m_wmoGroupId;
            groupRec = SDBWMOAreaTableLookup(mapObj->m_header->m_wmoId, mapObjDef->m_nameSet, group->m_wmoGroupId);
            const WMOAreaTableRec* root = SDBWMOAreaTableLookup(mapObj->m_header->m_wmoId, mapObjDef->m_nameSet, static_cast<uint32_t>(-1));
            rootRec = root;
            return groupRec && root;
        }
    }

    return 0;
}

// OFFSET: 0x007349B0/0x140410A40
int32_t CMapEntity::QueryMapObjFileName(const char*& fileName) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (ref->m_type & 0x4) {
            fileName = NULL;
            return 0;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
        CMapObj* mapObj = mapObjDef->m_mapObj;
        CMapObjGroup* group = mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (group && (!(group->m_groupFlags & 0x8) || !(mapObjDef->m_flags & 0x200))) {
            fileName = mapObj->m_name;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00734A50
bool CMapEntity::QueryMapObjMinimap(const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads, bool a5, bool a6) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        CMapBaseObj* ref = link->m_ref;
        next = this->m_parentLinkList.Next(link);

        if (ref->m_type & 0x4) {
            return false;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
        uint32_t groupFlags = mapObjDef->m_mapObj->GetGroupFlags(defGroup->m_groupNum);

        if ((mapObjDef->m_flags & 0x200) && (groupFlags & 0x8)) {
            continue;
        }

        if (!(groupFlags & 0x48)) {
            a6 = false;
        }

        CAaBox local;
        memset(&local, 0, sizeof(local));
        CWorldMath::TransformAABox(mapObjDef->m_invMat, box, local);
        return mapObjDef->m_mapObj->QueryMapObjMinimap(defGroup->m_groupNum, local, quads, maxQuads, a5, a6);
    }

    return false;
}

// OFFSET: 0x00734B40/0x140410C50
bool CMapEntity::QueryMapObjIDs(uint32_t& wmoId, uint32_t& nameSet, uint32_t& groupId) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        CMapBaseObj* ref = link->m_ref;
        next = this->m_parentLinkList.Next(link);

        if (ref->m_type & 0x4) {
            break;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
        CMapObj* mapObj = mapObjDef->m_mapObj;
        uint32_t groupFlags = mapObj->GetGroupFlags(defGroup->m_groupNum);

        if (!(mapObjDef->m_flags & 0x200) || !(groupFlags & 0x8)) {
            wmoId = mapObj->m_header->m_wmoId;
            nameSet = mapObjDef->m_mapObj->m_hashval;
            groupId = defGroup->m_groupNum;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00734BE0
bool CMapEntity::QueryMapObjMatrix(C44Matrix* matrix, C44Matrix* invMatrix) {
    CMapObjDef* mapObjDef;

    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; ; link = next) {
        if (!link) {
            return false;
        }

        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (ref->m_type & 0x4) {
            return false;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        mapObjDef = defGroup->m_mapObjDef;

        if (!(mapObjDef->m_flags & 0x200)) {
            break;
        }

        CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (group && !(group->m_groupFlags & 0x8)) {
            break;
        }
    }

    if (matrix) {
        *matrix = mapObjDef->m_mat;
    }

    if (invMatrix) {
        *invMatrix = mapObjDef->m_invMat;
    }

    return true;
}

// OFFSET: 0x00734C70/0x140410F20
void CMapEntity::UpdateLiquid() {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (!(ref->m_type & MapBaseObj_Type_MapObjDefGroup)) {
            continue;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjGroup* group = defGroup->m_mapObjDef->m_mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (group && !(group->m_groupFlags & 0x8)) {
            this->UpdateMapObjLiquid(defGroup->m_mapObjDef, group);
            return;
        }
    }

    this->UpdateTerrainLiquid();
}

// OFFSET: 0x00734CE0/0x140410FC0
void CMapEntity::UpdateMapObjLiquid() {
    CMapObjDef* mapObjDef;
    CMapObj* mapObj;
    CMapObjDefGroup* defGroup;
    CMapObjGroup* group;

    if (this->GetMapObjAndGroup(mapObjDef, mapObj, defGroup, group, 0)) {
        this->UpdateMapObjLiquid(mapObjDef, group);
    }
}

// OFFSET: 0x00734D20/0x140411040
void CMapEntity::QueryLiquidSounds(TSGrowableArray<World::LiquidQueryResult>& results, uint32_t* distance) {
    if (!(this->m_staticFlags & 0x8)) {
        return;
    }

    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (ref->m_type & MapBaseObj_Type_MapObjDefGroup) {
            CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
            C3Vector local = this->m_position * defGroup->m_mapObjDef->m_invMat;
            defGroup->m_mapObjDef->m_mapObj->QueryLiquidSounds(defGroup->m_groupNum, defGroup->m_groupNum, 0, distance, local, results);
        }
    }
}

// OFFSET: 0x00734E60/0x1404111B0
void CMapEntity::UpdatePortals() {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (!(ref->m_type & MapBaseObj_Type_MapObjDefGroup)) {
            continue;
        }

        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
        CMapObj* mapObj = mapObjDef->m_mapObj;
        CMapObjGroup* group = mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (!group) {
            continue;
        }

        if (this->m_staticFlags & 0x2) {
            mapObjDef->m_skipPortal = -1;
            continue;
        }

        const SMOPortalInfo* portals = mapObj->m_portalInfos;
        const C3Vector* portalVerts = mapObj->m_portalVertices;
        const SMOPortalRef* portalRefs = mapObj->m_portalRefs;

        if (!portals || !portalRefs || !portalVerts) {
            continue;
        }

        const SMOPortalRef* portalRef = &portalRefs[group->m_portalStart];
        const C44Matrix& m = mapObjDef->m_mat;

        for (uint32_t i = 0; i < group->m_portalCount; i++, portalRef++) {
            const SMOPortalInfo& portal = portals[portalRef->m_portalIndex];
            uint32_t count = portal.m_count;
            C3Vector sum(0.0f, 0.0f, 0.0f);

            for (uint32_t k = 0; k < count; k++) {
                const C3Vector& v = portalVerts[portal.m_startVertex + k];
                sum.x = v.x * m.a0 + v.z * m.c0 + v.y * m.b0 + m.d0 + sum.x;
                sum.y = v.x * m.a1 + v.z * m.c1 + v.y * m.b1 + m.d1 + sum.y;
                sum.z = v.x * m.a2 + v.z * m.c2 + v.y * m.b2 + m.d2 + sum.z;
            }

            float inv = 1.0f / static_cast<float>(static_cast<int32_t>(count));
            float cy = sum.y * inv;
            float cz = sum.z * inv;
            float cx = inv * sum.x;

            if (cx > this->m_bbox.b.x && cy > this->m_bbox.b.y && cz > this->m_bbox.b.z
                && this->m_bbox.t.x > cx && this->m_bbox.t.y > cy && this->m_bbox.t.z > cz) {
                mapObjDef->m_skipPortal = portalRef->m_portalIndex;
                return;
            }
        }
    }
}

// OFFSET: 0x007354A0/0x1404117F0
void CMapEntity::EnablePortals(int32_t enable) {
    this->m_staticFlags = this->m_staticFlags ^ ((this->m_staticFlags ^ (2 * (enable != 0))) & 0x2) | 0x1;
    this->UpdatePortals();
}

// OFFSET: 0x00735560/0x140411900
void CMapEntity::Update() {
    if (this->m_model) {
        int32_t status = this->m_model->GetAsyncRenderStatusRecursive();

        if (status != 2) {
            const C3Vector& pos = World::GetGuardBandPosition();
            float dx = pos.x - this->m_position.x;
            float dy = pos.y - this->m_position.y;
            uint32_t priority = AsyncFileReadComputePriority(3, status, 1, dx * dx + dy * dy + (pos.z - this->m_position.z) * (pos.z - this->m_position.z));
            AsyncFileReadAddStreamingObject(CMapDoodadDef::StreamingCallback, this, priority);
        }

        if (!(this->m_flags & 0x40) && this->m_model->IsLoadedRecursive()) {
            this->m_flags |= 0x40;

            if (this->m_flags & 0x80000) {
                this->UpdateBounds();
            }
        }
    }

    if (!(this->m_flags & 0x10000)) {
        this->m_entityFlags |= 0x60000;
        this->UpdateLighting();

        if ((this->m_entityFlags & 0x4000) || this->m_worldMap->unk_010C) {
            this->m_worldMap->UpdateEntity(this);

            if (this->m_staticFlags & 0x1) {
                this->UpdatePortals();
            }
        }

        return;
    }

    this->m_entityFlags |= 0x4000;
}

// OFFSET: 0x007356D0/0x140411A10
int32_t CMapEntity::QueryCameraFog(SMOFog* fog, uint8_t& found, float& distance) {
    CWorldView* view = CWorldScene::s_defaultWorldView;
    CMapObjDef* mapObjDef = view->m_viewerMapObjDef;

    if (!mapObjDef) {
        return 0;
    }

    CMapObj* mapObj = mapObjDef->m_mapObj;

    if (!mapObj || mapObj->m_fogCount == 1) {
        return 0;
    }

    C3Vector local = view->m_camPos * mapObjDef->m_invMat;
    found = 0;

    for (uint32_t i = 0; i < view->m_viewerGroups.Count(); i++) {
        CMapObjGroup* group = mapObj->GetGroup(view->m_viewerGroups.m_data[i], 0);

        if (!group || (group->m_groupFlags & 0x48)) {
            continue;
        }

        found = 1;
        float dist = mapObj->DistFromClosestExtPortal(group, local, 1);

        if (distance > dist) {
            distance = dist;
        }
    }

    if (!found) {
        distance = 0.0f;
    }

    *fog = mapObj->m_fogs[0];

    static FogQueue s_fogQueue;

    CMapObjGroup* group = mapObj->GetGroup(view->m_viewerGroups.m_data[0], 0);

    if (group) {
        for (uint32_t k = 0; k < 4; k++) {
            uint8_t index = group->m_fogs[k];

            if (!index) {
                continue;
            }

            const SMOFog& entry = mapObj->m_fogs[index];
            float dz = entry.m_pos.z - local.z;
            float dy = entry.m_pos.y - local.y;
            float dx = entry.m_pos.x - local.x;
            float dist = sqrt(dx * dx + (dy * dy + dz * dz));

            if (entry.m_end > dist && !(entry.m_flags & 0x1)) {
                s_fogQueue.push(std::pair<float, uint32_t>(dist, index));
            }
        }
    }

    while (!s_fogQueue.empty()) {
        std::pair<float, uint32_t> top = s_fogQueue.top();
        s_fogQueue.pop();

        const SMOFog& entry = mapObj->m_fogs[top.second];
        float dist = top.first;

        if (dist < 0.0f) {
            dist = 0.0f;
        } else if (entry.m_end <= dist) {
            dist = entry.m_end;
        }

        float t = 1.0f;

        if (entry.m_start <= dist) {
            t = 1.0f - (dist - entry.m_start) / (entry.m_end - entry.m_start);
        }

        LerpInteriorFog(&fog->m_fogEnd, &entry.m_fogEnd, t);

        if (s_fogQueue.empty()) {
            fog->m_flags = entry.m_flags;
        }
    }

    return 1;
}
