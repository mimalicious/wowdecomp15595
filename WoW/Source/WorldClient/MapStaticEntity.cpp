#include "WoW/Source/WorldClient/MapStaticEntity.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/MapChunk.h"

// STATIC: 0x00D01AEC
C3Vector CMapStaticEntity::s_interiorLightDir = C3Vector(-0.30822f, -0.30822f, -0.89999998f);

// OFFSET: 0x00788C90
static void SampleVertexLighting(const CMapChunk* chunk, const C3Vector& pos, int32_t cellY, int32_t cellX, CImVector& color) {
    const uint8_t* lighting = reinterpret_cast<const uint8_t*>(chunk->m_vertexLighting);

    if (!lighting) {
        color = CImVector(0);
        return;
    }

    int32_t fx = static_cast<int32_t>((((pos.x - chunk->m_position.x) - static_cast<float>(cellX) * -4.1666665f) * -0.23999999f) * 255.0f);
    int32_t fy = static_cast<int32_t>((((pos.y - chunk->m_position.y) - static_cast<float>(cellY) * -4.1666665f) * -0.23999999f) * 255.0f);
    const uint8_t* v = lighting + 4 * (cellX + cellY + 16 * cellX);
    int32_t b0 = (v[0] << 8) + ((fx * ((v[4] << 8) - (v[0] << 8))) >> 8);
    int32_t g0 = (v[1] << 8) + ((fx * ((v[5] << 8) - (v[1] << 8))) >> 8);
    int32_t r0 = (v[2] << 8) + ((fx * ((v[6] << 8) - (v[2] << 8))) >> 8);
    int32_t b1 = (v[36] << 8) + ((fx * ((v[40] << 8) - (v[36] << 8))) >> 8);
    int32_t g1 = (v[37] << 8) + ((fx * ((v[41] << 8) - (v[37] << 8))) >> 8);
    int32_t r1 = (v[38] << 8) + ((fx * ((v[42] << 8) - (v[38] << 8))) >> 8);
    color.b = static_cast<uint8_t>((b0 + ((fy * (b1 - b0)) >> 8)) >> 8);
    color.g = static_cast<uint8_t>((g0 + ((fy * (g1 - g0)) >> 8)) >> 8);
    color.r = static_cast<uint8_t>(static_cast<uint16_t>(r0 + ((fy * (r1 - r0)) >> 8)) >> 8);
    color.a = 0;
}

// OFFSET: 0x00788E50
static void QueryVertexLighting(const CMapChunk* chunk, const C3Vector& pos, CImVector& color) {
    int32_t cellY = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.23999999f) & 7;
    int32_t cellX = static_cast<int32_t>(-(pos.x - 17066.666f) * 0.23999999f) & 7;
    SampleVertexLighting(chunk, pos, cellY, cellX, color);
}

// OFFSET: 0x00762800
void CMapStaticEntity::SetSequenceSpeed(float speed) {
    if (this->m_model) {
        this->m_model->SetAnimationSpeed(speed);
    }
}

// OFFSET: 0x00762820
void CMapStaticEntity::AdjustLighting(CM2Lighting* lighting, void* a2, void* a3, void* a4) {
}

// OFFSET: 0x007629B0
void CMapStaticEntity::SelectUnderwater(CM2Lighting* lighting) {
    uint32_t flags = this->m_entityFlags;

    if ((flags & 0x2) && (flags & 0x4)) {
        lighting->m_liquidPlane.n = C3Vector(0.0f, 0.0f, 1.0f);
        lighting->m_liquidPlane.d = -this->m_liquidHeight;
        lighting->m_flags = (lighting->m_flags & ~0x61u) | 0x60;
    } else if ((flags & 0x2) && (flags & 0x1)) {
        lighting->m_liquidPlane.n = C3Vector(0.0f, 0.0f, 1.0f);
        lighting->m_liquidPlane.d = -this->m_liquidHeight;
        lighting->m_flags = (lighting->m_flags & ~0x61u) | 0x40;
    } else {
        lighting->m_flags = (lighting->m_flags & ~0x60u) | 0x20;
    }
}

// OFFSET: 0x00762AB0
void CMapStaticEntity::ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* arg) {
    CMapStaticEntity* entity = static_cast<CMapStaticEntity*>(arg);
    lighting->m_flags |= 0x10;

    if (entity) {
        const DayNight::FogInfo* fog = DayNight::GetFogInfo();
        C3Vector color(fog->m_color.r * 0.0039215689f, fog->m_color.g * 0.0039215689f, fog->m_color.b * 0.0039215689f);
        lighting->SetFog(color, fog->m_start, fog->m_end, fog->m_density);
        entity->SelectLights(lighting);
        entity->SelectUnderwater(lighting);

        if (entity->m_flags & 0x2) {
            lighting->m_flags |= 0x8;
            return;
        }
    } else {
        const DayNight::LightInfo* light = DayNight::GetLightInfo();
        lighting->AddAmbient(C3Vector(light->m_ambientColor.r * 0.0039215689f, light->m_ambientColor.g * 0.0039215689f, light->m_ambientColor.b * 0.0039215689f));
        lighting->AddDiffuse(C3Vector(light->m_directColor.r * 0.0039215689f, light->m_directColor.g * 0.0039215689f, light->m_directColor.b * 0.0039215689f), light->m_direction);
    }

    lighting->m_flags &= ~0x8u;
}

// OFFSET: 0x00762D50
void CMapStaticEntity::QueryExteriorLighting() {
    this->m_exteriorVertexLight = CImVector(0);

    if (g_theGxDevicePtr->m_caps.m_shaderModel < 4) {
        this->m_exteriorLightingValid = 0;
        return;
    }

    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* owner = link->m_ref;

        if (!(owner->m_type & 0x4)) {
            this->m_exteriorLightingValid = 1;
            return;
        }

        CMapChunk* chunk = static_cast<CMapChunk*>(owner);

        if (chunk->m_terrainBox.b.x <= this->m_position.x && this->m_position.x <= chunk->m_terrainBox.t.x && chunk->m_terrainBox.b.y <= this->m_position.y && this->m_position.y <= chunk->m_terrainBox.t.y) {
            QueryVertexLighting(chunk, this->m_position, this->m_exteriorVertexLight);
            this->m_exteriorLightingValid = 1;
            return;
        }
    }
}

// OFFSET: 0x00762E50
CMapStaticEntity::CMapStaticEntity() {
    this->m_sphere.c.x = 0.0f;
    this->m_sphere.c.y = 0.0f;
    this->m_sphere.c.z = 0.0f;
    this->m_sphere.r = 0.0f;
    this->m_bbox.b.x = 0.0f;
    this->m_bbox.b.y = 0.0f;
    this->m_bbox.b.z = 0.0f;
    this->m_bbox.t.x = 0.0f;
    this->m_bbox.t.y = 0.0f;
    this->m_bbox.t.z = 0.0f;
    this->m_interiorAmbient = CImVector(0);
    this->m_interiorDirect = CImVector(0);
    this->m_exteriorVertexLight = CImVector(0);
    this->m_staticFlags &= 0xFFFFFFC4;
    this->m_worldMap = NULL;
    this->m_model = NULL;
    this->unk_0020 = 0;
    this->m_exteriorLightingValid = 0;
    this->m_entityFlags = (this->m_entityFlags & 0xFFF90000) | 0x60000;
    this->m_fadeLevel = 0;
    this->m_culled = 0;
    this->unk_0026 = 0;
    this->unk_0027 = 0;
    this->m_liquidHeight = 0.0f;
    this->m_sizeCategory = 4;
    this->unk_008C = 0;
}
