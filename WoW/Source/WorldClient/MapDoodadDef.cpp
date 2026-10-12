#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include <cfloat>
#include <cmath>
#include <cstring>
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapChunkLiquid.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x00733100/0x140475D70
void CMapDoodadDef::StreamingCallback(void* param) {
    static_cast<CMapDoodadDef*>(param)->m_model->LinkToAsyncHead();
}

// OFFSET: 0x00786A20
void CMapDoodadDef::GetLocalMatrix(C44Matrix& matrix) {
    matrix = C44Matrix();
    matrix.Translate(this->m_pos);
    matrix.Rotate(this->m_rotation);
    matrix.Scale(this->m_scale);
}

// OFFSET: 0x00786AC0/0x140475D80
void CMapDoodadDef::SelectLights(CM2Lighting* lighting) {
    if (!this->m_exteriorLightingValid) {
        this->QueryExteriorLighting();
    }

    if (this->m_flags & 0x2) {
        lighting->AddAmbient(C3Vector(this->m_interiorAmbient.r * 0.0039215689f, this->m_interiorAmbient.g * 0.0039215689f, this->m_interiorAmbient.b * 0.0039215689f));
        lighting->AddDiffuse(C3Vector(this->m_interiorDirect.r * 0.0039215689f, this->m_interiorDirect.g * 0.0039215689f, this->m_interiorDirect.b * 0.0039215689f), CMapStaticEntity::s_interiorLightDir);
        lighting->m_flags |= 0x8;
    } else {
        const CM2Light& sun = CWorldMap::s_sunLight;
        const CImVector& vertex = this->m_exteriorVertexLight;

        if (vertex.b | vertex.g | vertex.r) {
            lighting->AddAmbient(C3Vector(vertex.r * 0.001953125f + sun.m_ambColor.x, vertex.g * 0.001953125f + sun.m_ambColor.y, vertex.b * 0.001953125f + sun.m_ambColor.z));
            float x = -sun.m_dir.x * 0.5f + sun.m_dir.x;
            float y = -sun.m_dir.y * 0.5f + sun.m_dir.y;
            float z = (1.0f - sun.m_dir.z) * 0.5f + sun.m_dir.z;
            float inv = 1.0f / sqrtf(z * z + y * y + x * x);
            C3Vector dir(x * inv, y * inv, inv * z);
            lighting->AddDiffuse(C3Vector(vertex.r * 0.001953125f + sun.m_dirColor.x, vertex.g * 0.001953125f + sun.m_dirColor.y, vertex.b * 0.001953125f + sun.m_dirColor.z), dir);
        } else {
            lighting->AddAmbient(sun.m_ambColor);
            lighting->AddDiffuse(sun.m_dirColor, sun.m_dir);
        }

        lighting->m_flags &= ~0x8u;
    }

    const DayNight::FogInfo* fog;

    if (this->m_flags & 0x4) {
        fog = DayNight::GetIntFogInfo();
        lighting->m_flags |= 0x80;
    } else {
        fog = DayNight::GetFogInfo();
    }

    C3Vector color(fog->m_color.r * 0.0039215689f, fog->m_color.g * 0.0039215689f, fog->m_color.b * 0.0039215689f);
    lighting->SetFog(color, fog->m_start, fog->m_end, fog->m_density);
}

// OFFSET: 0x00786E70/0x140476210
int32_t CMapDoodadDef::SelectUnderwater(CMapObjDefGroup* group) {
    CMapObjDef* mapObjDef = group->m_mapObjDef;
    CMapObjGroup* objGroup = mapObjDef->m_mapObj->GetGroup(group->m_groupNum, 0);

    if (!objGroup) {
        return 0;
    }

    C3Vector local = C3Vector(this->m_position.x, this->m_position.y, this->m_bbox.b.z) * mapObjDef->m_invMat;
    uint32_t liquidType;
    float height;

    if (objGroup->QueryLiquid(local, 0, liquidType, height) && height > local.z) {
        this->m_entityFlags |= 0x1;
        this->m_entityFlags ^= (this->m_entityFlags ^ (2 * static_cast<uint8_t>(Liquid::IsTransparent(liquidType)))) & 0x2;
        uint32_t flags = this->m_entityFlags;
        float worldHeight = mapObjDef->m_mat.a2 * local.x + mapObjDef->m_mat.b2 * local.y + mapObjDef->m_mat.c2 * height + mapObjDef->m_mat.d2;

        if (worldHeight > this->m_liquidHeight) {
            float top = this->m_bbox.t.z;
            this->m_liquidHeight = worldHeight;

            if (top >= worldHeight) {
                this->m_entityFlags = flags | 0x4;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00787000
void CMapDoodadDef::UpdateMatrixOnly(const C44Matrix& matrix) {
    this->m_position = this->m_pos * matrix;
    C44Matrix transform;
    this->GetLocalMatrix(transform);
    transform *= matrix;
    CAaBox bounds;
    memset(&bounds, 0, sizeof(bounds));
    CAaBox collision;
    memset(&collision, 0, sizeof(collision));
    CM2Model* model = this->m_model;
    C3Vector center;
    float radius;

    if (model->IsLoaded()) {
        bounds = model->GetBoundingBox();

        if (!(model->m_shared->m_flags & 0x1)) {
            model->WaitForLoad(NULL);
        }

        collision = model->m_shared->m_data->m_collisionBounds.m_extent;
        CAaSphere sphere = model->GetBoundingSphere();
        center = sphere.c;
        radius = sphere.r;
    } else {
        memset(&bounds, 0, sizeof(bounds));
        center = C3Vector(0.0f, 0.0f, 0.0f);
        collision = model->m_shared->m_boundingBox;
        radius = 0.0f;
    }

    this->m_sphere.c = center * transform;
    this->m_sphere.r = this->m_scale * radius;

    if (bounds.t.x < bounds.b.x || bounds.t.y < bounds.b.y || bounds.t.z < bounds.b.z) {
        this->m_bbox.b = this->m_position;
        this->m_bbox.t = this->m_position;
    } else {
        CWorldMath::TransformAABox(transform, bounds, this->m_bbox);
    }

    if ((collision.t.x == collision.b.x && collision.t.y == collision.b.y && collision.t.z == collision.b.z) || collision.t.x < collision.b.x || collision.t.y < collision.b.y || collision.t.z < collision.b.z) {
        this->m_modelBox.b = C3Vector(FLT_MAX, FLT_MAX, FLT_MAX);
        this->m_modelBox.t = C3Vector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    } else {
        CWorldMath::TransformAABox(transform, collision, this->m_modelBox);
    }

    this->m_model->m_fFlags |= 0x4000;
    this->m_model->SetWorldTransform(transform);

    if (this->m_soundEmitter) {
        SE3::EmitterManagerSendMessage_Set3DPosition(this->m_soundEmitter, this->m_position);
    }
}

// OFFSET: 0x007874F0/0x140476AB0
void CMapDoodadDef::UpdateBounds() {
    CAaBox modelBox = this->m_model->GetBoundingBox();
    CAaSphere sphere = this->m_model->GetBoundingSphere();
    CM2Model* model = this->m_model;

    if (!(model->m_shared->m_flags & 0x1)) {
        model->WaitForLoad(NULL);
    }

    CAaBox dataBox = this->m_model->m_shared->m_data->m_collisionBounds.m_extent;
    const C44Matrix& mat = this->m_model->m_worldTransform;

    this->m_sphere.c.x = mat.c0 * sphere.c.z + mat.a0 * sphere.c.x + mat.b0 * sphere.c.y + mat.d0;
    this->m_sphere.c.y = mat.b1 * sphere.c.y + mat.c1 * sphere.c.z + sphere.c.x * mat.a1 + mat.d1;
    this->m_sphere.c.z = mat.b2 * sphere.c.y + mat.c2 * sphere.c.z + mat.a2 * sphere.c.x + mat.d2;
    this->m_sphere.r = this->m_scale * sphere.r;

    if (modelBox.t.x < modelBox.b.x || modelBox.t.y < modelBox.b.y || modelBox.t.z < modelBox.b.z) {
        this->m_bbox.b = this->m_position;
        this->m_bbox.t = this->m_position;
    } else {
        this->m_bbox = CAaBox::Transform(modelBox, mat);
    }

    if ((dataBox.t.x == dataBox.b.x && dataBox.t.y == dataBox.b.y && dataBox.t.z == dataBox.b.z) || dataBox.t.x < dataBox.b.x || dataBox.t.y < dataBox.b.y || dataBox.t.z < dataBox.b.z) {
        this->m_modelBox.b.x = 3.402823466e38f;
        this->m_modelBox.b.y = 3.402823466e38f;
        this->m_modelBox.b.z = 3.402823466e38f;
        this->m_modelBox.t.x = -3.402823466e38f;
        this->m_modelBox.t.y = -3.402823466e38f;
        this->m_modelBox.t.z = -3.402823466e38f;
    } else {
        this->m_modelBox = CAaBox::Transform(dataBox, mat);
    }

    float cx = (dataBox.t.x + dataBox.b.x) * 0.5f;
    float cz = (dataBox.t.z + dataBox.b.z) * 0.5f;
    float cy = (dataBox.t.y + dataBox.b.y) * 0.5f;
    this->m_vec2.x = cx * mat.a0 + mat.c0 * cz + mat.b0 * cy + mat.d0;
    this->m_vec2.y = mat.c1 * cz + mat.b1 * cy + cx * mat.a1 + mat.d1;
    this->m_vec2.z = mat.a2 * cx + cz * mat.c2 + cy * mat.b2 + mat.d2;

    float size = this->m_bbox.t.x - this->m_bbox.b.x;

    if (size <= this->m_bbox.t.y - this->m_bbox.b.y) {
        size = this->m_bbox.t.y - this->m_bbox.b.y;
    }

    if (size <= this->m_bbox.t.z - this->m_bbox.b.z) {
        size = this->m_bbox.t.z - this->m_bbox.b.z;
    }

    this->m_sizeCategory = 0;

    do {
        if (CWorldView::s_fadeStart[this->m_sizeCategory] >= size) {
            break;
        }

        this->m_sizeCategory++;
    } while (this->m_sizeCategory < 4);

    if (!(World::GetEnables() & 0x4000) && this->m_sizeCategory < 3) {
        this->m_model->m_flags &= ~0x20u;
    }
}

// OFFSET: 0x00787900/0x140476FA0
void CMapDoodadDef::LinkToChunk(CMapBaseObj* owner) {
    if (owner->m_type & 0x4) {
        CMapChunk* chunk = static_cast<CMapChunk*>(owner);
        float top = chunk->m_bbox.t.z;

        if (!(top > this->m_bbox.t.z)) {
            top = this->m_bbox.t.z;
        }

        CMapArea* area = chunk->m_area;
        chunk->m_bbox.t.z = top;

        if (!(top > area->m_bbox.t.z)) {
            top = area->m_bbox.t.z;
        }

        area->m_bbox.t.z = top;
        float bottom = chunk->m_bbox.b.z;

        if (!(area->m_bbox.b.z > bottom)) {
            bottom = area->m_bbox.b.z;
        }

        area->m_bbox.b.z = bottom;
        area->m_chunkBounds[chunk->m_indexX + 16 * chunk->m_indexY] = chunk->m_bbox;
    } else if (owner->m_type & 0x20) {
        static_cast<CMapObjDefGroup*>(owner)->ExtendBounds(this->m_bbox);
    }
}

// OFFSET: [0x007879B0/0x140477070, 0x00787DC0]
CMapDoodadDef::~CMapDoodadDef() {
    if (this->m_model) {
        this->m_model->SetSequenceCallback(NULL, 0);
        this->m_model->SetEventCallback(NULL, 0);
        this->m_model->m_lightingCallback = NULL;
        this->m_model->m_lightingArg = NULL;
        this->m_model->Release();
        this->m_model = NULL;
    }

    this->m_updateLink.Unlink();
}

// OFFSET: 0x00787A40
void CMapDoodadDef::QueryInteriorLighting(const CMapObjDef* mapObjDef, uint32_t groupNum, const uint16_t* polyIndex, const C3Vector* pos) {
    CImVector color(0);
    bool exterior;
    bool found;

    if (polyIndex) {
        C3Vector local = *pos * mapObjDef->m_invMat;
        found = mapObjDef->m_mapObj->QueryLighting(groupNum, local, *polyIndex, color, exterior) != 0;
    } else {
        C3Segment seg;
        seg.b = C3Vector(this->m_pos.x, this->m_pos.y, this->m_pos.z + 1.5f);
        seg.t = C3Vector(this->m_pos.x, this->m_pos.y, this->m_pos.z - 1760.0f);
        found = mapObjDef->m_mapObj->QueryLighting(groupNum, seg, color, exterior);
    }

    if (!found) {
        return;
    }

    CMapStaticEntity::AdjustLighting(color, this->m_interiorDirect, 168, this->m_interiorAmbient, 96);

    if (!exterior) {
        this->m_entityFlags &= ~0x400;
        return;
    }

    const DayNight::LightInfo* info = DayNight::GetLightInfo();
    uint8_t alpha = color.a;

    if (alpha) {
        this->m_interiorDirect.InterpColor(alpha, &info->m_directColor);
    }

    if (alpha) {
        this->m_interiorAmbient.InterpColor(alpha, &info->m_ambientColor);
    }

    this->m_entityFlags |= 0x400;
    this->m_interiorDirect.a = color.a;
}

// OFFSET: 0x00787C50
void CMapDoodadDef::UpdateOwners() {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        this->LinkToChunk(link->m_ref);
    }
}

// OFFSET: 0x00787C90/0x1404773F0
void CMapDoodadDef::Purge() {
    if (this->m_refCount) {
        return;
    }

    this->m_updateLink.Unlink();
    CMap::FreeDoodadDef(this);
}

// OFFSET: 0x00787CE0/0x140477470
CMapDoodadDef::CMapDoodadDef() {
    this->m_modelBox.b.x = 0.0f;
    this->m_modelBox.b.y = 0.0f;
    this->m_modelBox.b.z = 0.0f;
    this->m_modelBox.t.x = 0.0f;
    this->m_modelBox.t.y = 0.0f;
    this->m_modelBox.t.z = 0.0f;
    this->m_rotation.x = 0.0f;
    this->m_rotation.y = 0.0f;
    this->m_rotation.z = 0.0f;
    this->m_rotation.w = 1.0f;
    this->m_doodadSet = 0;
    this->m_type |= 0x80;
    this->m_scale = 0.0f;
    this->m_cullStamp = 0xFFFFFFFF;
    this->m_fade = 0xFF00;
}

// OFFSET: 0x00787DF0/0x1404775A0
int32_t CMapDoodadDef::SelectUnderwater() {
    CMap* map = this->m_worldMap->GetMap(this->m_position);

    if (!map) {
        return 0;
    }

    float fx = -(this->m_position.x - 17066.666f) * 0.23999999f;
    int32_t x = static_cast<int32_t>(fx);
    int32_t y = static_cast<int32_t>(-(this->m_position.y - 17066.666f) * 0.23999999f);
    float fy = -(this->m_position.y - 17066.666f) * 0.23999999f;
    CMap::CMapAreaTableEntry* entry = map->m_areaTable.Get((y >> 7) & 0x3F, (static_cast<int32_t>(fx) >> 7) & 0x3F);

    if (!entry || !(entry->m_flags & 0x2) || !entry->m_area) {
        return 0;
    }

    CMapChunk* chunk = entry->m_area->m_chunks[((y >> 3) & 0xF) + 16 * ((x >> 3) & 0xF)];

    if (!chunk) {
        return 0;
    }

    C2Vector frac(fy - static_cast<float>(y), fx - static_cast<float>(x));
    C2iVector cell = { y & 7, static_cast<int32_t>(fx) & 7 };
    float height;

    for (CChunkLiquid* liquid = chunk->m_liquidList.Head(); liquid; liquid = chunk->m_liquidList.Next(liquid)) {
        if (liquid->TileExists(cell.x, cell.y) && liquid->GetHeight(frac, cell, height) && height > this->m_bbox.b.z && height > this->m_liquidHeight) {
            this->m_staticFlags &= ~0x10;
            this->m_entityFlags |= 0x1;
            uint8_t transparent = static_cast<uint8_t>(Liquid::IsTransparent(liquid->m_liquidType));
            float top = this->m_bbox.t.z;
            this->m_entityFlags ^= (this->m_entityFlags ^ (2 * transparent)) & 0x2;
            uint32_t flags = this->m_entityFlags;
            this->m_liquidHeight = height;

            if (top >= height) {
                this->m_entityFlags = flags | 0x4;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00787FB0
void CMapDoodadDef::SelectUnderwater(CM2Lighting* lighting) {
    uint32_t flags = this->m_entityFlags;

    if (!(flags & 0x8)) {
        this->m_entityFlags = (flags & ~0xF) | 0x8;
        this->m_liquidHeight = -FLT_MAX;

        for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
            next = this->m_parentLinkList.Next(link);
            CMapBaseObj* ref = link->m_ref;

            if ((ref->m_type & 0x20) && !this->SelectUnderwater(static_cast<CMapObjDefGroup*>(ref))) {
                this->m_entityFlags &= ~0x8;
                break;
            }
        }

        if (!(this->m_flags & 0x2)) {
            this->SelectUnderwater();
        }
    }

    CMapStaticEntity::SelectUnderwater(lighting);
}

// OFFSET: 0x00788030/0x1404778F0
void CMapDoodadDef::UpdateLoaded() {
    this->UpdateBounds();
    this->UpdateOwners();

    CM2Model* model = this->m_model;
    uint32_t billboards = model->IsLoaded() ? model->m_shared->m_billboardBoneCount : 0;
    this->m_flags |= 0x40;
    this->m_staticFlags ^= (this->m_staticFlags ^ (4 * (billboards != 0))) & 0x4;
}
