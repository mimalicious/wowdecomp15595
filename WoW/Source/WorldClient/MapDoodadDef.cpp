#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include <cmath>
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldView.h"

// OFFSET: 0x00733100
void CMapDoodadDef::StreamingCallback(void* param) {
    static_cast<CMapDoodadDef*>(param)->m_model->LinkToAsyncHead();
}

// OFFSET: 0x00786AC0
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

// OFFSET: 0x007874F0
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

// OFFSET: 0x00787900
void CMapDoodadDef::LinkToChunk(CMapBaseObj* owner) {
    if (owner->m_type & 0x4) {
        CMapChunk* chunk = static_cast<CMapChunk*>(owner);
        float top = chunk->m_bbox.t.z;

        if (top <= this->m_bbox.t.z) {
            top = this->m_bbox.t.z;
        }

        CMapArea* area = chunk->m_area;
        chunk->m_bbox.t.z = top;

        if (top <= area->m_bbox.t.z) {
            top = area->m_bbox.t.z;
        }

        area->m_bbox.t.z = top;
        float bottom = chunk->m_bbox.b.z;

        if (area->m_bbox.b.z <= bottom) {
            bottom = area->m_bbox.b.z;
        }

        area->m_bbox.b.z = bottom;
        area->m_chunkBounds[chunk->m_indexX + 16 * chunk->m_indexY] = chunk->m_bbox;
    } else if (owner->m_type & 0x20) {
        static_cast<CMapObjDefGroup*>(owner)->ExtendBounds(this->m_bbox);
    }
}

// OFFSET: 0x007879B0
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

// OFFSET: 0x00787C50
void CMapDoodadDef::UpdateOwners() {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        this->LinkToChunk(link->m_ref);
    }
}

// OFFSET: 0x00787C90
void CMapDoodadDef::Purge() {
    if (this->m_refCount) {
        return;
    }

    this->m_updateLink.Unlink();
    CMap::FreeDoodadDef(this);
}

// OFFSET: 0x00787CE0
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
    this->unk_00EC = 0;
    this->m_type |= 0x80;
    this->m_scale = 0.0f;
    this->m_cullStamp = 0xFFFFFFFF;
    this->m_fade = 0xFF00;
}

// OFFSET: 0x00788030
void CMapDoodadDef::UpdateLoaded() {
    this->UpdateBounds();
    this->UpdateOwners();

    CM2Model* model = this->m_model;
    uint32_t billboards = model->IsLoaded() ? model->m_shared->m_billboardBoneCount : 0;
    this->m_flags |= 0x40;
    this->m_staticFlags ^= (this->m_staticFlags ^ (4 * (billboards != 0))) & 0x4;
}
