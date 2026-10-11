#include "WoW/Source/WorldClient/MapStaticEntity.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"

// STATIC: 0x00D01AEC
C3Vector CMapStaticEntity::s_interiorLightDir = C3Vector(-0.30822f, -0.30822f, -0.89999998f);

// OFFSET: 0x00734650
int32_t CMapStaticEntity::GetMapObjAndGroup(CMapObjDef*& mapObjDef, CMapObj*& mapObj, CMapObjDefGroup*& defGroup, CMapObjGroup*& group, int32_t skipMoving) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(), * next; link; link = next) {
        next = this->m_parentLinkList.Next(link);
        CMapBaseObj* ref = link->m_ref;

        if (!(ref->m_type & 0x20)) {
            continue;
        }

        defGroup = static_cast<CMapObjDefGroup*>(ref);
        mapObjDef = defGroup->m_mapObjDef;

        if (skipMoving && (mapObjDef->m_flags & 0x200)) {
            continue;
        }

        mapObj = mapObjDef->m_mapObj;
        group = mapObj->GetGroup(defGroup->m_groupNum, 0);

        if (group) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: [0x00762DE0, 0x00762F40]
CMapStaticEntity::~CMapStaticEntity() {
    if (this->m_soundEmitter) {
        SE3::EmitterMangerSendMessage_DestroyEmitter(this->m_soundEmitter);
    }
}

// PROVEN: 60d6bccf IDENTICAL/NONE
// OFFSET: 0x00762800
void CMapStaticEntity::SetSequenceSpeed(float speed) {
    if (this->m_model) {
        this->m_model->SetAnimationSpeed(speed);
    }
}

// PROVEN: ccf96f9c IDENTICAL/NONE
// OFFSET: 0x00762820
void CMapStaticEntity::AdjustLighting(CMapObjDef* mapObjDef, uint32_t groupNum, const uint16_t* polyIndex, const C3Vector* pos) {
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

// OFFSET: 0x00762830/0x140447880
void CMapStaticEntity::ModelEventCallback(CM2Model* model, uint32_t a2, uint32_t a3, uint32_t a4, const C3Vector& a5, uint32_t a6, uint64_t param) {
    CMapStaticEntity* entity = reinterpret_cast<CMapStaticEntity*>(static_cast<uintptr_t>(param));

    switch (a3) {
        case 'ESD$': {
            if (entity->m_soundEmitter) {
                SE3::EmitterMangerSendMessage_DestroyEmitter(entity->m_soundEmitter);
            }

            break;
        }

        case 'LSD$': {
            if (!SE3::EmitterManagerEmitterExists(entity->m_soundEmitter) && !SE3::IsTooCloseToExistingSoundKit(a4, a5)) {
                SE3SoundKitProperties properties;
                properties.m_bit0 = 1;
                properties.m_fadeInTime = 3000;
                properties.m_maxAudibleBehavior = 1;
                properties.m_priority = 128;
                properties.m_usage = 1;
                SE3SoundChunk* chunk = SE3::EmitterManagerGetOrCreateSoundChunk(SE3::EmitterManagerGetSoundChunkHandle(a5));
                SE3::EmitterManagerSendMessage_CreateEmitter(&entity->m_soundEmitter, chunk, a4, a5, &properties, NULL, 0);
            }

            break;
        }

        case 'OSD$': {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit(a4, &a5, &properties, NULL, __FILE__, 53, "CMapStaticEntity::ModelEventCallback", 0, NULL);
            break;
        }

        default: {
            if ((entity->m_type & 0x200) && World::GetUserModelEventCallback()) {
                World::GetUserModelEventCallback()(model, a2, a3, a4, a5, a6, static_cast<CMapEntity*>(entity)->m_param64);
            }

            break;
        }
    }
}

// OFFSET: 0x00762AB0/0x140447BC0
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

// OFFSET: 0x00762C00
void CMapStaticEntity::AdjustLighting(const CImVector& color, CImVector& direct, uint8_t maxDirect, CImVector& ambient, uint8_t maxAmbient) {
    uint8_t brightest = color.r > color.g ? color.r : color.g;

    if (color.b > brightest) {
        brightest = color.b;
    }

    if (!brightest) {
        brightest = 1;
    }

    direct = color;

    if (brightest < maxDirect) {
        C3Vector rgb(static_cast<float>(direct.r) * 0.0039215689f, static_cast<float>(direct.g) * 0.0039215689f, static_cast<float>(color.b) * 0.0039215689f);
        C3Vector hsv(0.0f, 0.0f, 0.0f);
        RGBtoHSV(rgb, hsv);
        hsv.z = static_cast<float>(maxDirect) / static_cast<float>(brightest) * hsv.z;
        HSVtoRGB(hsv, rgb);
        CImVector adjusted(maxDirect, 0, 0, 0);
        adjusted = rgb;
        direct = adjusted;
    }

    ambient = color;

    if (brightest > maxAmbient) {
        ambient.ScaleRGB(static_cast<uint32_t>(static_cast<int64_t>(static_cast<double>(maxAmbient) * 255.0 / static_cast<double>(brightest))));
    }
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
            CMap::QueryLightingColorTerrain(chunk, this->m_position, this->m_exteriorVertexLight);
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
    this->m_shadowFrame[0] = 0;
    this->m_shadowFrame[1] = 0;
    this->m_shadowFrame[2] = 0;
    this->unk_0027 = 0;
    this->m_liquidHeight = 0.0f;
    this->m_sizeCategory = 4;
    this->m_soundEmitter = 0;
}
