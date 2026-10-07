#include "WoW/Source/WorldClient/WorldMap.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <new>
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/WorldView.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "Engine/Source/Gx/CGxVertexPCT.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CFrustum.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/Particulate.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldScene.h"

// STATIC: 0x00E74340
HASHKEY_NONE CWorldMap::s_mapObjDefKey;

// STATIC: 0x00E71288
float CWorldMap::s_riverDepthCoordTable[256];

// STATIC: 0x00E71688
float CWorldMap::s_oceanDepthCoordTable[256];

// STATIC: 0x00E71A88
float CWorldMap::s_emptyDepthCoordTable[256];

// STATIC: 0x00D00BF8
float* CWorldMap::s_depthCoordTables[2] = { CWorldMap::s_riverDepthCoordTable, CWorldMap::s_oceanDepthCoordTable };

// STATIC: 0x00E71E88
uint32_t CWorldMap::s_queryTag;

// STATIC: 0x00E78BC4
TSGrowableArray<uint32_t> CWorldMap::s_rasterSpans;

// STATIC: 0x00E78BC0
uint32_t CWorldMap::s_rasterSpanCount;

// STATIC: 0x00E78BD4
TSGrowableArray<SMapObjDefVectorIntersect> CWorldMap::s_mapObjDefIntersects;

// STATIC: 0x00BC7E94
static const float s_rippleRiseFraction[2] = { 0.4f, 0.4f };

// STATIC: 0x00D01B20
static const char* s_rippleTextureNames[2] = { "XTextures\\splash\\splash.blp", "XTextures\\splash\\wake.blp" };

// STATIC: 0x00E78BE4
CTexture* CWaterRipple::s_textures[2];

// STATIC: 0x00E78BEC
uint32_t CWaterRipple::s_nextRipple;

// STATIC: 0x00E78BF0
uint32_t CWaterRipple::s_nextPlayerRipple;

// STATIC: 0x00E78BF4
CGxShader* CWaterRipple::s_vertexShader;

// STATIC: 0x00E78BF8
CGxShader* CWaterRipple::s_pixelShader;

// STATIC: 0x00E78BFC
TSFixedArray<CWaterRipple> CWaterRipple::s_ripples;

// STATIC: 0x00E71E8C/0x140D5BB74
uint32_t CWorldMap::s_activeWorldMapIndex;

// STATIC: 0x00E71E98
CM2Light CWorldMap::s_sunLight;

// STATIC: 0x00E71E90/0x140D5BB78
CWorldMap* CWorldMap::s_worldMaps[2];

// STATIC: 0x00D00C38
float CWorldMap::s_oceanDepthScale = 0.58333331f;

// OFFSET: 0x0072C7D0/0x140405920
void CWorldMap::InitializeLiquid() {
    float scale = CWorldMap::s_oceanDepthScale;
    memset(CWorldMap::s_emptyDepthCoordTable, 0, sizeof(CWorldMap::s_emptyDepthCoordTable));
    float invRange = 1.0f / (CWorldMap::s_oceanDepthScale * 255.0f);

    for (uint32_t i = 0; i < 256; i++) {
        float depth = static_cast<float>(i);

        float ocean = invRange * (depth * scale);
        if (ocean >= 1.0f) {
            ocean = 1.0f;
        }

        float river = depth * 0.11111111f * 0.21428572f;
        CWorldMap::s_oceanDepthCoordTable[i] = ocean;

        if (river >= 1.0f) {
            river = 1.0f;
        }

        CWorldMap::s_riverDepthCoordTable[i] = river;
    }

    Liquid::CreateProceduralTextures(1);
    CWaterRipple::Initialize();
}

// OFFSET: 0x0072C9F0/0x140405B50
void CWorldMap::Initialize() {
    CMap::Initialize();
    CWorldMap::InitializeLiquid();
    CWorldMap::FootprintInitialize();
    CWorldMap::s_worldMaps[0] = NULL;
    CWorldMap::s_worldMaps[1] = NULL;
    CWorldMap::s_activeWorldMapIndex = 0;
    CWorldMap::s_queryTag = 0;
}

// OFFSET: 0x0072CA20/0x140405B90
const float* CWorldMap::GetDepthCoordTable(uint32_t liquidType) {
    if (liquidType) {
        LiquidTypeRec* type = g_liquidTypeDB.GetRecord(liquidType);

        if (type) {
            LiquidMaterialRec* material = g_liquidMaterialDB.GetRecord(type->m_materialID);

            if (material && (material->m_lvf == 0 || material->m_lvf == 2 || material->m_lvf == 4)) {
                return CWorldMap::s_depthCoordTables[type->m_int[0]];
            }
        }
    }

    return CWorldMap::s_emptyDepthCoordTable;
}

// OFFSET: 0x0072CAA0/0x140405C20
void CWorldMap::ClearAllChunkBufs() {
    for (CWorldMap** worldMap = CWorldMap::s_worldMaps; worldMap < &CWorldMap::s_worldMaps[2]; worldMap++) {
        if (*worldMap) {
            for (uint32_t i = 0; i < 12; i++) {
                if ((*worldMap)->m_maps[i]) {
                    (*worldMap)->m_maps[i]->ClearChunkBufs();
                }
            }
        }
    }
}

// OFFSET: 0x0072CAE0/0x140405C90
void CWorldMap::ClearAllRenderChunks() {
    for (CWorldMap** worldMap = CWorldMap::s_worldMaps; worldMap < &CWorldMap::s_worldMaps[2]; worldMap++) {
        if (*worldMap) {
            for (uint32_t i = 0; i < 12; i++) {
                if ((*worldMap)->m_maps[i]) {
                    (*worldMap)->m_maps[i]->ClearRenderChunks();
                }
            }
        }
    }
}

// OFFSET: 0x0072CB20
void CWorldMap::ClearAllDetailDoodads() {
    for (CWorldMap** worldMap = CWorldMap::s_worldMaps; worldMap < &CWorldMap::s_worldMaps[2]; worldMap++) {
        if (*worldMap) {
            for (uint32_t i = 0; i < 12; i++) {
                if ((*worldMap)->m_maps[i]) {
                    (*worldMap)->m_maps[i]->ClearChunkDetailDoodads();
                }
            }
        }
    }
}

// PROVEN: 25cfaffc IDENTICAL/NONE
// OFFSET: 0x0072CB60
void CWorldMap::RemoveOtherMaps() {
    for (uint32_t i = 0; i < 12; i++) {
        CMap* map = this->m_maps[i];

        if (map && map->m_mapId != this->m_mapId) {
            map->m_mapFlags = (map->m_mapFlags & ~(CMap::Flag_Current | CMap::Flag_Remove)) | CMap::Flag_Remove;
        }
    }
}

// OFFSET: 0x0072CC20/0x140405D70
int32_t CWorldMap::AddMap(const char* directory, int32_t mapId, int32_t preload) {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i] && this->m_maps[i]->m_mapId == mapId) {
            CMap* map = this->m_maps[i];

            if (map->m_mapFlags & CMap::Flag_Remove) {
                map->m_mapFlags &= ~CMap::Flag_Remove;
            }

            return 1;
        }
    }

    uint32_t index = 0;

    while (this->m_maps[index]) {
        if (++index >= 12) {
            return index != 12;
        }
    }

    void* mem = SMemAlloc(sizeof(CMap), __FILE__, __LINE__, 0);
    this->m_maps[index] = mem ? new (mem) CMap() : NULL;
    this->m_maps[index]->Load(directory, this, mapId, preload);

    return index != 12;
}

// OFFSET: 0x0072CCE0/0x140405E70
void CWorldMap::RemoveMap(int32_t mapId) {
    for (uint32_t i = 1; i < 12; i++) {
        if (this->m_maps[i] && this->m_maps[i]->m_mapId == mapId) {
            CMap* map = this->m_maps[i];
            map->m_mapFlags = (map->m_mapFlags & ~(CMap::Flag_Current | CMap::Flag_Remove)) | CMap::Flag_Remove;
            return;
        }
    }
}

// OFFSET: 0x0072CD20/0x140405EC0
void CWorldMap::SetCurrent(int32_t mapId, int32_t current) {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i] && this->m_maps[i]->m_mapId == mapId) {
            CMap* map = this->m_maps[i];
            map->m_mapFlags ^= (map->m_mapFlags ^ (4 * (current != 0))) & CMap::Flag_Current;
            return;
        }
    }
}

// OFFSET: 0x0072CD70/0x140405F20
void CWorldMap::UpdateEntity(CMapEntity* entity) {
    CM2Model* model = entity->m_model;
    entity->m_flags &= ~0x3;
    entity->m_entityFlags &= ~0x77;
    entity->m_staticFlags &= ~0x18;

    if (model) {
        model->m_fFlags &= ~0x4000000;
    }

    this->LinkStaticEntity(entity);

    if (entity->m_entityFlags & 0x10) {
        entity->UpdateLiquid();
    } else if (entity->m_staticFlags & 0x8) {
        entity->UpdateMapObjLiquid();
    } else {
        entity->UpdateTerrainLiquid();
    }

    uint32_t flags = entity->m_entityFlags;

    if (!(flags & 0x1000) && (flags & 0x4) && entity->m_liquidHeight + 0.0099999998f > entity->m_position.z) {
        uint32_t areaId;

        if (CWorldMap::QueryAreaId(entity, areaId)) {
            const LiquidTypeRec* rec = GetLiquidTypeRecForArea(areaId, entity->m_liquidType);

            if (rec) {
                uint32_t recFlags = rec->m_flags;

                if ((recFlags & 0x4) || entity->m_liquidHeight > entity->m_position.z) {
                    entity->m_entityFlags ^= (entity->m_entityFlags ^ (recFlags << 5)) & 0x20;
                    entity->m_entityFlags ^= (entity->m_entityFlags ^ (rec->m_flags << 5)) & 0x40;
                }
            }
        }
    }

    float lighting;

    if ((entity->m_flags & 0x2) || CShadowCache::GetShadowMode() >= 2 || (entity->m_flags & 0x100) || !this->QueryShadow(entity->m_position)) {
        lighting = 1.0f;
    } else {
        entity->m_staticFlags |= 0x10;
        lighting = 0.69999999f;
    }

    entity->unk_00DC = lighting;

    if (entity->m_soundEmitter) {
        SE3::EmitterManagerSendMessage_Set3DPosition(entity->m_soundEmitter, entity->m_position);
    }
}

// PROVEN: dc043875 IDENTICAL/NONE
// OFFSET: 0x0072D000/0x140406160
void CWorldMap::AddChildMaps(int32_t mapId) {
    for (int32_t i = 0; i < g_mapDB.m_numRecords; i++) {
        MapRec* rec = g_mapDB.GetRecordByIndex(i);

        if (rec && rec->m_id != mapId && rec->unk_004C == mapId && rec->m_pvp != 3) {
            this->AddMap(rec->m_directory, rec->m_id, 0);
        }
    }
}

// OFFSET: 0x0072D070
void CWorldMap::DestroyLiquidPerturbators() {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(); mapObjDef; mapObjDef = this->m_mapObjDefList.Next(mapObjDef)) {
        if (mapObjDef->m_perturbatorMesh) {
            Liquid::DestroyPerturbatorMesh(Liquid::PertSurface_Ocean, mapObjDef->m_perturbatorMesh);
            mapObjDef->m_perturbatorMesh = NULL;
        }
    }
}

// OFFSET: 0x0072D140
uint8_t CWorldMap::GetCurrentMapIndex(int32_t x, int32_t y) {
    CMapTableEntry* entry = this->m_mapTable.Get(x, y);
    uint8_t flags = entry ? entry->m_flags : 0;

    if (!(flags & 1)) {
        return 0xFF;
    }

    uint8_t index = 0;

    for (uint8_t i = 1; i < 12; i++) {
        CMap* map = this->m_maps[i];

        if (map && (map->m_mapFlags & CMap::Flag_Current)) {
            CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.Get(x, y);
            uint32_t areaFlags = areaEntry ? areaEntry->m_flags : 0;

            if (areaFlags & 1) {
                index = i;
            }
        }
    }

    return index;
}

// OFFSET: 0x0072D1C0/0x140406500
void CWorldMap::UpdateEntityLinks(const CAaBox& box) {
    for (CMapEntity* entity = this->m_entityList.Head(); entity; entity = this->m_entityList.Next(entity)) {
        if (box.t.x >= entity->m_bbox.b.x && box.t.y >= entity->m_bbox.b.y && box.t.z >= entity->m_bbox.b.z && entity->m_bbox.t.x >= box.b.x && entity->m_bbox.t.y >= box.b.y && entity->m_bbox.t.z >= box.b.z) {
            entity->m_entityFlags |= 0x4000;
        }
    }
}

// OFFSET: 0x0072D240/0x140406590
uintptr_t CWorldMap::AddEntity(CM2Model* model, const C44Matrix& matrix, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* name, int32_t a9) {
    CMapEntity* entity = CMap::AllocEntity();
    entity->m_worldMap = this;

    if (flags & 0x8) {
        this->m_entityList.LinkToHead(entity);
    } else {
        this->m_entityList.LinkToTail(entity);
    }

    entity->m_type |= 0x200;
    entity->m_entityFlags = (entity->m_entityFlags & 0xFFFFC86C) | (~(flags << 8) & 0x200) | (((flags & 0x4) | (8 * ((flags & 0x10) | (8 * (((flags & 0x18) != 0) | 0x10))))) << 6);
    entity->m_staticFlags = (flags & 0x20) | (entity->m_staticFlags & ~0x38);

    if (flags & 0x40) {
        entity->m_flags |= 0x8000;
    }

    if (!(flags & 0x1)) {
        entity->m_flags |= 0x80;
    }

    entity->unk_00E0 = 1.0f;
    entity->unk_00DC = 1.0f;

    float b = CWorldMap::s_sunLight.m_ambColor.z;

    if (b < 0.0f) {
        b = 0.0f;
    } else if (b >= 1.0f) {
        b = 1.0f;
    }

    uint8_t blue = static_cast<uint8_t>(b * 255.0f);
    float g = CWorldMap::s_sunLight.m_ambColor.y;

    if (g < 0.0f) {
        g = 0.0f;
    } else if (g >= 1.0f) {
        g = 1.0f;
    }

    uint8_t green = static_cast<uint8_t>(g * 255.0f);
    float r = CWorldMap::s_sunLight.m_ambColor.x;

    if (r < 0.0f) {
        r = 0.0f;
    } else if (r >= 1.0f) {
        r = 1.0f;
    }

    CImVector color;
    color.g = green;
    color.b = blue;
    color.r = static_cast<uint8_t>(r * 255.0f);
    color.a = static_cast<uint8_t>(255.0f);
    *reinterpret_cast<CImVector*>(&entity->unk_00D4) = color;
    entity->m_interiorAmbient = color;
    entity->m_model = model;

    if (model) {
        if (!_strnicmp("InvisibleStalker", model->m_shared->m_fileNameWithoutPath, 16)) {
            entity->m_entityFlags |= 0x80;
            entity->m_model->m_fFlags |= 0x2000000;
        }

        entity->m_model->m_lightingCallback = CMapStaticEntity::ModelLightingCallback;
        entity->m_model->m_lightingArg = entity;
        entity->m_model->m_refCount++;
    }

    C3Vector pos(matrix.d0, matrix.d1, matrix.d2);
    entity->m_position = pos;
    entity->m_sphere.c = pos;
    entity->m_bbox.b = entity->m_position;
    entity->m_bbox.t = entity->m_position;
    entity->m_vec2 = entity->m_position;
    entity->m_updateCallback = callback;
    entity->m_updateParam = param;
    entity->m_param64 = param64;
    entity->m_sphere.r = 0.0f;
    entity->m_param32 = param32;
    return reinterpret_cast<uintptr_t>(entity);
}

// OFFSET: 0x0072D4B0/0x140406860
uintptr_t CWorldMap::AddEntity(const char* name, const C44Matrix& matrix, WORLD_LOAD_CALLBACK callback, void* param, uint64_t param64, uint32_t param32, uint32_t flags, const char* a8, int32_t a9) {
    CM2Model* model = CWorldScene::LoadModel(name, matrix, NULL, 0, NULL, NULL, 0, 0, 0, 0);
    CMapEntity* entity = reinterpret_cast<CMapEntity*>(this->AddEntity(model, matrix, callback, param, param64, param32, flags, a8, a9));
    model->Release();
    entity->m_flags |= 0x80000;
    return reinterpret_cast<uintptr_t>(entity);
}

// OFFSET: 0x0072D520
void CWorldMap::LinkDoodadDefToMapObjDefs(CMapDoodadDef* doodadDef, CMapObjDef* exclude) {
    CWorldMap::s_queryTag++;

    int32_t minY = static_cast<int32_t>(-(doodadDef->m_bbox.t.x - 17066.666f) * 0.029999999f) >> 4;
    int32_t maxY = static_cast<int32_t>(-(doodadDef->m_bbox.b.x - 17066.666f) * 0.029999999f) >> 4;
    int32_t minX = static_cast<int32_t>(-(doodadDef->m_bbox.t.y - 17066.666f) * 0.029999999f) >> 4;
    int32_t maxX = static_cast<int32_t>(-(doodadDef->m_bbox.b.y - 17066.666f) * 0.029999999f) >> 4;

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (!entry || !(entry->m_flags & 0x1)) {
                    continue;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.Get(x, y);

            if (!areaEntry || !(areaEntry->m_flags & 0x2) || !areaEntry->m_area) {
                continue;
            }

            CMapArea* area = areaEntry->m_area;

            for (CMapBaseObjLink* link = area->m_linkList.Head(), * next; link; link = next) {
                next = area->m_linkList.Next(link);
                CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

                if (mapObjDef == exclude || mapObjDef->unk_004C == CWorldMap::s_queryTag) {
                    continue;
                }

                mapObjDef->unk_004C = CWorldMap::s_queryTag;

                if (!mapObjDef->CanLinkTo()) {
                    continue;
                }

                if (doodadDef->m_bbox.t.x >= mapObjDef->m_bbox.b.x
                    && doodadDef->m_bbox.t.y >= mapObjDef->m_bbox.b.y
                    && doodadDef->m_bbox.t.z >= mapObjDef->m_bbox.b.z
                    && mapObjDef->m_bbox.t.x >= doodadDef->m_bbox.b.x
                    && mapObjDef->m_bbox.t.y >= doodadDef->m_bbox.b.y
                    && mapObjDef->m_bbox.t.z >= doodadDef->m_bbox.b.z) {
                    mapObjDef->LinkDoodadDef(doodadDef, 1);
                }
            }
        }
    }
}

// OFFSET: 0x0072D8D0
void CWorldMap::WaterReset() {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefHash.Head(); mapObjDef; mapObjDef = this->m_mapObjDefHash.Next(mapObjDef)) {
        for (CMapObjDefGroup* group = mapObjDef->m_defGroupList.Head(); group; group = mapObjDef->m_defGroupList.Next(group)) {
            group->WaterDestroy();
        }
    }

    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i]) {
            this->m_maps[i]->WaterReset();
        }
    }

    this->DestroyLiquidPerturbators();
}

// OFFSET: 0x0072D980
int32_t CWorldMap::GetActiveMapRecId(int32_t x, int32_t y) {
    uint8_t index = this->GetCurrentMapIndex(x, y);

    if (index == 0xFF) {
        return -1;
    }

    CMap* map = this->m_maps[index];

    if (!map) {
        return -1;
    }

    return map->m_mapId;
}

// OFFSET: 0x0072D9C0/0x1404070E0
int32_t CWorldMap::Load(const char* directory, int32_t mapId, int32_t preload) {
    this->m_mapId = mapId;

    if (mapId == 643) {
        this->m_exteriorUnderwater = 1;
    }

    int32_t result = this->AddMap(directory, mapId, 0);

    if (!result) {
        return result;
    }

    this->AddChildMaps(mapId);

    CMap* map = this->m_maps[0];
    map->m_mapFlags |= CMap::Flag_Current;
    this->m_isGlobalMapObj = static_cast<int32_t>(map->m_mapFlags << 31) >> 31;

    CiRect rect = map->m_areaExtents;
    this->m_mapTable.SetRect(rect);

    if (map->m_areaTable.m_data.m_count == this->m_mapTable.m_data.m_count) {
        CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.m_data.m_data;
        CMapTableEntry* entry = this->m_mapTable.m_data.m_data;

        for (uint32_t i = 0; i < this->m_mapTable.m_data.m_count; i++) {
            if (areaEntry->m_flags & 1) {
                entry->m_flags |= 1;
                entry->m_mapIndex = 0;
            }

            areaEntry++;
            entry++;
        }
    }

    DayNight::LoadMap(mapId);
    DayNight::Preload(World::GetGuardBandPosition());
    this->Preload(0);
    CWorldMap::s_sunLight.SetLightType(M2LIGHT_DIRECT);
    CWorldMap::s_sunLight.SetVisible(1);

    void* weatherMem = SMemAlloc(sizeof(Weather), __FILE__, __LINE__, 0);
    this->m_weather = weatherMem ? new (weatherMem) Weather() : NULL;

    void* particulateMem = SMemAlloc(sizeof(Particulate), __FILE__, __LINE__, 0);
    this->m_particulate = particulateMem ? new (particulateMem) Particulate(0.027777778f, 30.0f, "Textures\\WaterPoop02.blp") : NULL;

    World::s_loadProgressCallback = NULL;
    World::s_loadProgressParam = NULL;
    map->m_mapFlags ^= (map->m_mapFlags ^ (2 * (preload != 0))) & CMap::Flag_Preload;
    this->RemoveOtherMaps();

    return 1;
}

// OFFSET: 0x0072DB50
void CWorldMap::Unload() {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i]) {
            this->m_maps[i]->UnloadXXX();

            if (this->m_maps[i]) {
                delete this->m_maps[i];
            }

            this->m_maps[i] = NULL;
        }
    }

    Particulate* particulate = this->m_particulate;

    if (particulate) {
        particulate->~Particulate();
        SMemFree(particulate, "delete", -1, 0);
        this->m_particulate = NULL;
    }

    Weather* weather = this->m_weather;

    if (weather) {
        weather->~Weather();
        SMemFree(weather, "delete", -1, 0);
        this->m_weather = NULL;
    }

    this->m_rippleList.UnlinkAll();
    CMapObj::ClearCache();
    CWorldScene::s_m2Scene->PurgeModelCache();
    TextureGxCacheFlush();
    DayNight::UnloadMap();
    CWorldMap::ClearAllFootprints();
    CWorldScene::OnMapUnload();
    this->DestroyLiquidPerturbators();
}

// OFFSET: 0x0072DC20/0x140407490
void CWorldMap::PurgeMaps() {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i]) {
            this->m_maps[i]->Purge();
        }
    }

    this->m_rippleList.UnlinkAll();
    CMapObj::ClearCache();
    TextureGxCacheFlush();
}

// OFFSET: 0x0072DC60/0x1404074F0
void CWorldMap::UpdateRipples(const C3Vector& pos) {
    if (!Liquid::HasDynamicDisplMap()) {
        return;
    }

    static uint32_t s_lastTime = FrameTime::GetCurTimeMs();
    uint32_t now = FrameTime::GetCurTimeMs();
    uint32_t elapsed = now - s_lastTime;

    if (now == s_lastTime) {
        return;
    }

    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);

        if (!(mapObjDef->m_flags & 0x200)) {
            continue;
        }

        CAaBox bounds = mapObjDef->m_bbox;

        if (!(bounds.b.z <= -3.0f && bounds.t.z >= 3.0f)) {
            continue;
        }

        if (!mapObjDef->m_perturbatorMesh) {
            bounds.t.z = -2.0f;
            bounds.b.z = -3.0f;
            CAaBox localBox;
            memset(&localBox, 0, sizeof(localBox));
            CWorldMath::TransformAABox(mapObjDef->m_invMat, bounds, localBox);

            World::TriData::s_statusFlags = 0;
            World::TriData::s_nBatches = 0;
            World::TriData::s_faceIndexCursor = 0;
            World::TriData::s_indexCursor = 0;
            World::TriData::s_nMatrices = 0;

            World::TriData tris;
            mapObjDef->m_mapObj->GetTris(localBox, 0x40010, tris, mapObjDef);

            if (!World::TriData::s_nBatches) {
                continue;
            }

            Liquid::DisplMapPerturbationMesh* mesh = Liquid::CreatePerturbatorMesh(static_cast<Liquid::EPertSurface>(1));

            if (!mesh) {
                continue;
            }

            uint32_t baseVertex = 0;

            for (uint32_t b = 0; b < World::TriData::s_nBatches; b++) {
                World::TriData::Batch& batch = World::TriData::s_batches[b];
                uint32_t minIndex = batch.m_minVertexIndex;
                uint32_t maxIndex = batch.m_maxVertexIndex;

                if (mesh->m_vertices.Count() - minIndex + maxIndex + 1 >= 5000 || mesh->m_indices.Count() + 3 * batch.m_faceCount >= 7500) {
                    continue;
                }

                for (uint32_t v = minIndex; v <= batch.m_maxVertexIndex; v++) {
                    const C3Vector& vert = batch.m_vertexList[v];
                    C3Vector scaled(vert.x * 0.75f, vert.y * 0.75f, vert.z * 0.75f);
                    mesh->m_vertices.New(scaled);
                }

                const uint16_t* indices = batch.m_indices;

                for (uint32_t f = 0; f < batch.m_faceCount; f++) {
                    uint16_t index = static_cast<uint16_t>(baseVertex + indices[0] - batch.m_minVertexIndex);
                    mesh->m_indices.New(index);
                    index = static_cast<uint16_t>(baseVertex + indices[1] - batch.m_minVertexIndex);
                    mesh->m_indices.New(index);
                    index = static_cast<uint16_t>(baseVertex + indices[2] - batch.m_minVertexIndex);
                    mesh->m_indices.New(index);
                    indices += 3;
                }

                baseVertex = mesh->m_vertices.Count();
            }

            mesh->m_amplitude = 1.0f;
            mesh->m_period = 1000;
            mapObjDef->m_perturbatorMesh = mesh;
            mesh->m_transform = mapObjDef->m_mat;
        }

        Liquid::DisplMapPerturbationMesh* mesh = mapObjDef->m_perturbatorMesh;

        if (mesh) {
            float dx = mesh->m_transform.d0 - mapObjDef->m_mat.d0;
            float dy = mesh->m_transform.d1 - mapObjDef->m_mat.d1;
            float speed = static_cast<float>(sqrt(dx * dx + dy * dy) / static_cast<double>(elapsed) * 100.0);

            if (speed > 1.0f) {
                speed = 1.0f;
            }

            mesh->m_amplitude = speed * 0.80000001f;
            mesh->m_transform = mapObjDef->m_mat;
        }
    }

    if (elapsed < 33) {
        return;
    }

    uint32_t steps = elapsed / 33;

    if (steps > 4) {
        steps = 4;
        s_lastTime = now;
    } else if (steps < 4) {
        s_lastTime += steps + 32 * steps;
    } else {
        s_lastTime = now;
    }

    float intensity = this->m_weather->unk_0008;

    if (this->m_weather->GetType() != 1 || intensity <= 0.2f) {
        return;
    }

    for (uint32_t i = 20 * steps; i; i--) {
        float amplitude = static_cast<float>(rand() % 200) * 0.001f + 0.2f;
        float dx = (static_cast<float>(rand() % 1000) * 0.0020000001f - 1.0f) * 50.0f;
        C3Vector dropPos;
        dropPos.x = pos.x + dx;
        dropPos.y = pos.y + (static_cast<float>(rand() % 1000) * 0.0020000001f - 1.0f) * 50.0f;
        dropPos.z = pos.z;
        Liquid::AddPerturbation(static_cast<Liquid::EPertSurface>(0), dropPos, 0.30000001f, -amplitude);
    }
}

// OFFSET: 0x0072EBF0/0x140408C10
CWorldMap::CWorldMap() {
    for (uint32_t i = 0; i < 12; i++) {
        this->m_maps[i] = NULL;
    }

    this->m_isGlobalMapObj = 0;
    this->m_particulate = NULL;
    this->m_weather = NULL;
    this->unk_010C = 0;
    this->m_exteriorUnderwater = 0;
    this->m_mapId = -1;
}

// OFFSET: 0x0072ED40
CWorldMap::~CWorldMap() {
}

// OFFSET: 0x0072F350/0x140409860
int32_t CWorldMap::LoadWorldMap(const char* directory, int32_t mapId, int32_t preload) {
    for (CWorldMap** worldMap = CWorldMap::s_worldMaps; worldMap < &CWorldMap::s_worldMaps[2]; worldMap++) {
        if (*worldMap && (*worldMap)->m_mapId == mapId) {
            return 0;
        }
    }

    uint32_t index = 0;

    while (CWorldMap::s_worldMaps[index]) {
        if (++index >= 2) {
            break;
        }
    }

    if (index < 2) {
        void* mem = SMemAlloc(sizeof(CWorldMap), __FILE__, __LINE__, 0);
        CWorldMap::s_worldMaps[index] = mem ? new (mem) CWorldMap() : NULL;
        CWorldMap::s_worldMaps[index]->Load(directory, mapId, preload);
    }

    if (index == 2) {
        return 0;
    }

    CWorldMap::s_activeWorldMapIndex = index;
    return 1;
}

// OFFSET: 0x0072F3F0
void CWorldMap::UnloadAllWorldMaps() {
    for (CWorldMap** worldMap = CWorldMap::s_worldMaps; worldMap < &CWorldMap::s_worldMaps[2]; worldMap++) {
        if (*worldMap) {
            (*worldMap)->Unload();
            CWorldMap* map = *worldMap;

            if (map) {
                map->~CWorldMap();
                SMemFree(map, "delete", -1, 0);
            }

            *worldMap = NULL;
        }
    }
}

// OFFSET: 0x0072F560
void CWorldMap::Destroy() {
    CWorldMap::UnloadAllWorldMaps();
    CWorldMap::FootprintDestroy();
    CWaterRipple::Destroy();
    Liquid::DestroyProceduralTextures(1);
    CMap::Destroy();
}

// OFFSET: 0x0072FAD0/0x14040A240
CMapDoodadDef* CWorldMap::CreateDoodadDef(uint32_t index, SMODoodadDef& def, const char* name, uint32_t parentId, const C44Matrix& parentMat, uint16_t doodadSet) {
    HASHKEY_DWORD key;
    key.m_key = parentId;
    CMapDoodadDef* existing = this->m_doodadDefHash.Ptr(index, key);

    if (existing) {
        return existing;
    }

    CMapDoodadDef* doodadDef = CMap::AllocDoodadDef();
    doodadDef->m_worldMap = this;
    HASHKEY_DWORD insertKey;
    insertKey.m_key = parentId;
    this->m_doodadDefHash.Insert(doodadDef, index, insertKey);

    doodadDef->m_pos = def.m_position;
    doodadDef->m_position = doodadDef->m_pos * parentMat;
    doodadDef->m_rotation.x = def.m_rotation[0];
    doodadDef->m_rotation.y = def.m_rotation[1];
    doodadDef->m_rotation.z = def.m_rotation[2];
    doodadDef->m_rotation.w = def.m_rotation[3];
    doodadDef->m_scale = def.m_scale;
    doodadDef->m_doodadSet = doodadSet;

    if (def.m_nameIndex & 0x1000000) {
        doodadDef->m_flags |= 0x800;
    }

    C44Matrix mat;
    mat.Translate(doodadDef->m_pos);
    mat.Rotate(doodadDef->m_rotation);
    mat.Scale(doodadDef->m_scale);
    mat *= parentMat;

    CMapStaticEntity::AdjustLighting(def.m_color, doodadDef->m_interiorDirect, 112, doodadDef->m_interiorAmbient, 96);

    CM2Model* model = CWorldScene::LoadModel(name, mat, CMapStaticEntity::ModelEventCallback, reinterpret_cast<uintptr_t>(doodadDef), CMapStaticEntity::ModelLightingCallback, doodadDef, 0, 1, 1, 0);
    doodadDef->m_model = model;

    if (model->IsLoaded()) {
        doodadDef->UpdateLoaded();
        const CAaBox& bounds = World::mapObjDefBounds;

        if (doodadDef->m_bbox.t.x >= bounds.b.x && doodadDef->m_bbox.t.y >= bounds.b.y && doodadDef->m_bbox.t.z >= bounds.b.z && bounds.t.x >= doodadDef->m_bbox.b.x && bounds.t.y >= doodadDef->m_bbox.b.y && bounds.t.z >= doodadDef->m_bbox.b.z) {
            doodadDef->m_fade = 0;
            CMap::s_doodadDefLoadedList.LinkToTail(doodadDef);
        }

        return doodadDef;
    }

    doodadDef->m_sphere.c = doodadDef->m_position;
    doodadDef->m_bbox.b = doodadDef->m_position;
    doodadDef->m_bbox.t = doodadDef->m_position;
    doodadDef->m_sphere.r = 0.0f;

    CAaBox modelBox = doodadDef->m_model->m_shared->m_boundingBox;

    if ((modelBox.t.x == modelBox.b.x && modelBox.t.y == modelBox.b.y && modelBox.t.z == modelBox.b.z) || modelBox.t.x < modelBox.b.x || modelBox.t.y < modelBox.b.y || modelBox.t.z < modelBox.b.z) {
        doodadDef->m_modelBox.b.x = 3.402823466e38f;
        doodadDef->m_modelBox.b.y = 3.402823466e38f;
        doodadDef->m_modelBox.b.z = 3.402823466e38f;
        doodadDef->m_modelBox.t.x = -3.402823466e38f;
        doodadDef->m_modelBox.t.y = -3.402823466e38f;
        doodadDef->m_modelBox.t.z = -3.402823466e38f;
    } else {
        doodadDef->m_modelBox = CAaBox::Transform(modelBox, mat);
    }

    CMap::s_doodadDefLoadingList.LinkToTail(doodadDef);
    return doodadDef;
}

// OFFSET: 0x0072FF10/0x14040A7C0
CMapObjDef* CWorldMap::CreateMapObjDef(const char* name, C3Vector& pos, float facing, int32_t a4, int32_t a5, uint16_t* a6, float* a7, int32_t a8) {
    int32_t uniqueId = CMap::s_globalUniqueId--;
    CMapObjDef* mapObjDef = CMap::AllocMapObjDef();
    this->m_mapObjDefHash.Insert(mapObjDef, uniqueId, CWorldMap::s_mapObjDefKey);
    mapObjDef->m_type |= 0x200;
    mapObjDef->m_hashval = uniqueId;
    this->m_mapObjDefList.LinkToHead(mapObjDef);

    CMapObj* mapObj = CMapObj::Create(name);
    mapObjDef->m_mapObj = mapObj;

    if (a4 && !(mapObj->m_flags & 0x40)) {
        mapObj->WaitLoad();
    }

    mapObjDef->m_worldMap = this;

    if (a5) {
        mapObjDef->m_flags |= 0x4000;
    }

    mapObjDef->m_doodadSet = 0;
    mapObjDef->m_nameSet = a8;
    mapObjDef->m_position = pos;
    mapObjDef->m_param64 = 0;

    for (int32_t i = 0; i < 3; i++) {
        mapObjDef->m_extraDoodadSets[i] = i < a5 ? a6[i] : 0;
    }

    mapObjDef->m_mat = C44Matrix();
    mapObjDef->m_mat.d0 = pos.x;
    mapObjDef->m_mat.d1 = pos.y;
    mapObjDef->m_mat.d2 = pos.z;
    mapObjDef->m_mat.RotateAroundZ(facing);
    mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();

    if (mapObj->m_flags & 0x40) {
        mapObjDef->UpdateBounds();
    } else if (a7) {
        float r = *a7;
        CAaBox box(C3Vector(r * -1.0f, r * -1.0f, r * -1.0f), C3Vector(r, r, r));
        CWorldMath::TransformAABox(mapObjDef->m_mat, box, mapObjDef->m_bbox);
    } else {
        mapObjDef->m_bbox.b = pos;
        mapObjDef->m_bbox.t = pos;
    }

    mapObjDef->m_lights.m_count = 0;
    return mapObjDef;
}

// OFFSET: 0x00730200/0x14040ABC0
CMapObjDef* CWorldMap::CreateMapObjDef(const char* name, SMMapObjDef& def, C3Vector& offset, int32_t unique) {
    if (unique) {
        CMapObjDef* existing = this->m_mapObjDefHash.Ptr(def.m_uniqueId, CWorldMap::s_mapObjDefKey);

        if (existing) {
            return existing;
        }
    }

    CMapObjDef* mapObjDef = CMap::AllocMapObjDef();

    if (unique) {
        this->m_mapObjDefHash.Insert(mapObjDef, def.m_uniqueId, CWorldMap::s_mapObjDefKey);
    }

    mapObjDef->m_position.x = -def.m_position.z;
    mapObjDef->m_position.y = -def.m_position.x;
    mapObjDef->m_position.z = def.m_position.y;
    mapObjDef->m_position.x = offset.x + mapObjDef->m_position.x;
    mapObjDef->m_position.y = offset.y + mapObjDef->m_position.y;
    mapObjDef->m_position.z = mapObjDef->m_position.z + offset.z;

    float rotX = def.m_rotation.z * 0.017453292f;
    float rotY = def.m_rotation.x * 0.017453292f;
    float rotZ = def.m_rotation.y * 0.017453292f + 3.1415927f;

    mapObjDef->m_worldMap = this;
    mapObjDef->m_doodadSet = def.m_doodadSet;
    mapObjDef->m_nameSet = def.m_nameSet;
    mapObjDef->m_param64 = 0;
    mapObjDef->m_extraDoodadSets[0] = 0;
    mapObjDef->m_extraDoodadSets[1] = 0;
    mapObjDef->m_extraDoodadSets[2] = 0;

    mapObjDef->m_mat.Identity();
    mapObjDef->m_mat.d0 = mapObjDef->m_position.x;
    mapObjDef->m_mat.d1 = mapObjDef->m_position.y;
    mapObjDef->m_mat.d2 = mapObjDef->m_position.z;
    mapObjDef->m_mat.RotateAroundZ(rotZ);
    mapObjDef->m_mat.RotateAroundY(rotY);
    mapObjDef->m_mat.RotateAroundX(rotX);
    mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();

    mapObjDef->m_bbox.b.x = -def.m_extents.t.z;
    mapObjDef->m_bbox.b.y = -def.m_extents.t.x;
    mapObjDef->m_bbox.b.z = def.m_extents.b.y;
    mapObjDef->m_bbox.b.x = mapObjDef->m_bbox.b.x + offset.x;
    mapObjDef->m_bbox.b.y = offset.y + mapObjDef->m_bbox.b.y;
    mapObjDef->m_bbox.b.z = mapObjDef->m_bbox.b.z + offset.z;
    mapObjDef->m_bbox.t.x = -def.m_extents.b.z;
    mapObjDef->m_bbox.t.y = -def.m_extents.b.x;
    mapObjDef->m_bbox.t.z = def.m_extents.t.y;
    mapObjDef->m_bbox.t.x = offset.x + mapObjDef->m_bbox.t.x;
    mapObjDef->m_bbox.t.y = offset.y + mapObjDef->m_bbox.t.y;
    mapObjDef->m_bbox.t.z = mapObjDef->m_bbox.t.z + offset.z;

    mapObjDef->m_lights.m_count = 0;
    mapObjDef->m_mapObj = CMapObj::Create(name);

    return mapObjDef;
}

// OFFSET: 0x0072F650
CMapDoodadDef* CWorldMap::CreateDoodadDef(const char* name, SMDoodadDef& def, C3Vector& offset) {
    HASHKEY_DWORD key;
    key.m_key = 0;
    TSHashTable<CMapDoodadDef, HASHKEY_DWORD>* hash = &this->m_doodadDefHash;
    CMapDoodadDef* existing = hash->Ptr(def.m_uniqueId, key);

    if (existing) {
        return existing;
    }

    CMapDoodadDef* doodadDef = CMap::AllocDoodadDef();
    doodadDef->m_worldMap = this;
    HASHKEY_DWORD insertKey;
    insertKey.m_key = 0;
    hash->Insert(doodadDef, def.m_uniqueId, insertKey);

    doodadDef->m_pos.x = -def.m_position.z;
    doodadDef->m_pos.y = -def.m_position.x;
    doodadDef->m_pos.z = def.m_position.y;
    doodadDef->m_pos.x = doodadDef->m_pos.x + offset.x;
    doodadDef->m_pos.y = offset.y + doodadDef->m_pos.y;
    doodadDef->m_pos.z = offset.z + doodadDef->m_pos.z;
    doodadDef->m_position = doodadDef->m_pos;
    doodadDef->m_scale = static_cast<float>(def.m_scale) * 0.0009765625f;

    if (def.m_flags & 0x1) {
        doodadDef->m_flags |= 0x400;
    }

    float rotZ = def.m_rotation.y * 0.017453292f;
    float rotX = def.m_rotation.z * 0.017453292f;
    float rotY = def.m_rotation.x * 0.017453292f;

    C33Matrix rotation;
    rotation.a0 = 1.0f;
    rotation.a1 = 0.0f;
    rotation.a2 = 0.0f;
    rotation.b0 = 0.0f;
    rotation.b1 = 1.0f;
    rotation.b2 = 0.0f;
    rotation.c0 = 0.0f;
    rotation.c1 = 0.0f;
    rotation.c2 = 1.0f;
    rotation.RotateAroundZ(rotZ + 3.1415927f);
    rotation.RotateAroundY(rotY);
    rotation.RotateAroundX(rotX);
    doodadDef->m_rotation.FromRotationMatrix(rotation);

    C44Matrix mat;
    mat.Translate(doodadDef->m_pos);
    mat.Rotate(doodadDef->m_rotation);
    mat.Scale(doodadDef->m_scale);

    CM2Model* model = CWorldScene::LoadModel(name, mat, CMapStaticEntity::ModelEventCallback, reinterpret_cast<uintptr_t>(doodadDef), CMapStaticEntity::ModelLightingCallback, doodadDef, 0, 1, 1, def.m_uniqueId == 0x5B018D);
    doodadDef->m_model = model;

    if (model->IsLoaded()) {
        doodadDef->UpdateLoaded();
        const CAaBox& bounds = World::mapObjDefBounds;

        if (doodadDef->m_bbox.t.x >= bounds.b.x && doodadDef->m_bbox.t.y >= bounds.b.y && doodadDef->m_bbox.t.z >= bounds.b.z && bounds.t.x >= doodadDef->m_bbox.b.x && bounds.t.y >= doodadDef->m_bbox.b.y && bounds.t.z >= doodadDef->m_bbox.b.z) {
            doodadDef->m_fade = 0;
            CMap::s_doodadDefLoadedList.LinkToTail(doodadDef);
            return doodadDef;
        }
    } else {
        doodadDef->m_sphere.c = doodadDef->m_position;
        doodadDef->m_bbox.b = doodadDef->m_position;
        doodadDef->m_bbox.t = doodadDef->m_position;
        doodadDef->m_sphere.r = 0.0f;

        CAaBox modelBox = doodadDef->m_model->m_shared->m_boundingBox;

        if ((modelBox.t.x == modelBox.b.x && modelBox.t.y == modelBox.b.y && modelBox.t.z == modelBox.b.z) || modelBox.t.x < modelBox.b.x || modelBox.t.y < modelBox.b.y || modelBox.t.z < modelBox.b.z) {
            doodadDef->m_modelBox.b.x = 3.402823466e38f;
            doodadDef->m_modelBox.b.y = 3.402823466e38f;
            doodadDef->m_modelBox.b.z = 3.402823466e38f;
            doodadDef->m_modelBox.t.x = -3.402823466e38f;
            doodadDef->m_modelBox.t.y = -3.402823466e38f;
            doodadDef->m_modelBox.t.z = -3.402823466e38f;
        } else {
            doodadDef->m_modelBox = CAaBox::Transform(modelBox, mat);
        }

        CMap::s_doodadDefLoadingList.LinkToTail(doodadDef);
    }

    return doodadDef;
}

// OFFSET: 0x00730AD0
void CWorldMap::GetQueryRects(const CAaBox& box, CiRect& subChunkRect, CiRect& chunkRect, CiRect& areaRect) {
    float tx = box.t.x;
    float bx = box.b.x;
    float by = box.b.y;
    subChunkRect.minX = static_cast<int32_t>(-(box.t.y - 17066.666f) * 0.23999999f);
    subChunkRect.maxY = static_cast<int32_t>(-(bx - 17066.666f) * 0.23999999f);
    float minY = -(tx - 17066.666f) * 0.23999999f;
    subChunkRect.minY = static_cast<int32_t>(minY);
    subChunkRect.maxX = static_cast<int32_t>(-(by - 17066.666f) * 0.23999999f);
    chunkRect.minY = static_cast<int32_t>(minY) >> 3;
    chunkRect.minX = subChunkRect.minX >> 3;
    chunkRect.maxY = subChunkRect.maxY >> 3;
    chunkRect.maxX = subChunkRect.maxX >> 3;
    areaRect.minY = static_cast<int32_t>(minY) >> 7;
    areaRect.minX = chunkRect.minX >> 4;
    areaRect.maxY = chunkRect.maxY >> 4;
    areaRect.maxX = chunkRect.maxX >> 4;
}

// OFFSET: 0x00730BA0
bool CWorldMap::IntersectMapObjDefs(const C3Segment& seg, float& t, C4Plane& plane, uint32_t flags) {
    bool hit = false;

    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);

        if ((mapObjDef->m_flags & 0x80) || !mapObjDef->m_mapObj) {
            continue;
        }

        C3Segment local;
        local.b = seg.b * mapObjDef->m_invMat;
        local.t = seg.t * mapObjDef->m_invMat;

        World::TriData::s_statusFlags = 0;
        World::TriData::s_nBatches = 0;
        World::TriData::s_faceIndexCursor = 0;
        World::TriData::s_indexCursor = 0;
        World::TriData::s_nMatrices = 0;

        World::TriData tris;

        if (!mapObjDef->m_mapObj->GetTris(local, t, flags, tris, mapObjDef)) {
            continue;
        }

        World::TriData::Batch batch = World::TriData::s_batches[0];
        const C44Matrix& m = *batch.m_matrix;
        C3Vector p2 = batch.m_vertexList[batch.m_indices[2]] * m;
        C3Vector p1 = batch.m_vertexList[batch.m_indices[1]] * m;
        hit = true;
        C3Vector p0 = batch.m_vertexList[batch.m_indices[0]] * m;

        C3Vector a = p2 - p0;
        C3Vector b = p1 - p0;
        plane.n.x = b.y * a.z - b.z * a.y;
        plane.n.y = b.z * a.x - a.z * b.x;
        plane.n.z = a.y * b.x - b.y * a.x;

        float inv = 1.0f / sqrt(plane.n.z * plane.n.z + plane.n.y * plane.n.y + plane.n.x * plane.n.x);
        plane.n.z *= inv;
        plane.n.y *= inv;
        plane.n.x *= inv;
        plane.d = -(plane.n.z * p0.z + plane.n.y * p0.y + plane.n.x * p0.x);
    }

    return hit;
}

// OFFSET: 0x007311A0
bool CWorldMap::QueryMapObjDefFacets(const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags) {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);
        uint32_t defFlags = mapObjDef->m_flags;

        if (defFlags & 0x80) {
            continue;
        }

        if (defFlags & 0x40) {
            if (box2.t.x < mapObjDef->m_bbox.b.x || box2.t.y < mapObjDef->m_bbox.b.y || box2.t.z < mapObjDef->m_bbox.b.z
                || mapObjDef->m_bbox.t.x < box2.b.x || mapObjDef->m_bbox.t.y < box2.b.y || mapObjDef->m_bbox.t.z < box2.b.z) {
                continue;
            }

            if (!mapObjDef->GetFacets(box, box2, facets, flags)) {
                return false;
            }

            continue;
        }

        CAaBox blocker = mapObjDef->m_bbox;
        blocker.b.z = mapObjDef->m_bbox.b.z - 0.5f;
        blocker.t.z = mapObjDef->m_bbox.t.z + 1.0f;

        if (box2.t.x >= blocker.b.x && box2.t.y >= blocker.b.y && box2.t.z >= blocker.b.z
            && blocker.t.x >= box2.b.x && blocker.t.y >= box2.b.y && blocker.t.z >= box2.b.z) {
            if (box.t.x > mapObjDef->m_bbox.b.x && box.t.y > blocker.b.y && box.t.z > mapObjDef->m_bbox.b.z
                && blocker.t.x > box.b.x && blocker.t.y > box.b.y && mapObjDef->m_bbox.t.z > box.b.z) {
                facets.m_status |= 0x2;
                return false;
            }

            if (static_cast<int32_t>(flags) < 0) {
                World::AddAaBoxFacets(blocker, 0, facets, 1);
            }
        }
    }

    return true;
}

// OFFSET: 0x00731390
void CWorldMap::QueryMapObjDefFacets(const CFrustum& frustum, World::FacetData& facets, uint32_t flags) {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);

        if (!(mapObjDef->m_flags & 0x90) && frustum.Cull(mapObjDef->m_bbox)) {
            mapObjDef->GetFacets(frustum, facets, flags);
        }
    }
}

// OFFSET: 0x00731400
int32_t CWorldMap::QueryMapObjDefsHeight(const C3Vector& start, const C3Vector& end, float& t) {
    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);

        if (mapObjDef->m_flags & 0x90) {
            continue;
        }

        if (mapObjDef->TestAABox(start, end) && !CMap::GetHeightMapObjDef(mapObjDef, start, end, t)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00731490
void CWorldMap::QueryMapObjDefTris(const CAaBox& box, World::TriData& tris, uint32_t flags) {
    CAaBox local;
    memset(&local, 0, sizeof(local));

    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);
        uint32_t defFlags = mapObjDef->m_flags;

        if (!(defFlags & 0x10) && (defFlags & 0x40)) {
            CWorldMath::TransformAABox(mapObjDef->m_invMat, box, local);

            if (mapObjDef->m_mapObj->TestBounds(local)) {
                mapObjDef->m_mapObj->GetTris(local, flags, tris, mapObjDef);
            }
        }
    }
}

// OFFSET: 0x00731540/0x140406350
CMap* CWorldMap::GetMap(const C3Vector& pos) {
    if (this->m_isGlobalMapObj) {
        return this->m_maps[0];
    }

    CMapTableEntry* entry = this->m_mapTable.Get(static_cast<int32_t>(-(pos.y - 17066.666f) * 0.001875f), static_cast<int32_t>(-(pos.x - 17066.666f) * 0.001875f));

    if (entry && (entry->m_flags & 0x1)) {
        return this->m_maps[entry->m_mapIndex];
    }

    return NULL;
}

// OFFSET: 0x007315C0
uint32_t CWorldMap::QueryAreaIdTerrain(const C3Vector& pos) {
    CMap* map = this->GetMap(pos);

    if (!map) {
        return 0;
    }

    return map->QueryAreaIdTerrain(pos);
}

// OFFSET: 0x007315F0
int32_t CWorldMap::QueryLiquidTerrain(const C3Vector& pos, uint32_t& liquidType, float& height) {
    CMap* map = this->GetMap(pos);

    if (!map) {
        return 0;
    }

    return map->QueryLiquidTerrain(pos, liquidType, height);
}

// OFFSET: 0x00731620
bool CWorldMap::QueryLiquid(const C3Vector& pos, int32_t nearest, uint32_t& liquidType, float& height) {
    CMap* map = this->GetMap(pos);

    if (!map) {
        return false;
    }

    return map->QueryLiquid(pos, nearest, liquidType, height);
}

// OFFSET: 0x00731660/0x14040C450
bool CWorldMap::QueryShadow(const C3Vector& pos) {
    CMap* map = this->GetMap(pos);

    if (!map) {
        return false;
    }

    return map->QueryShadowTerrain(pos);
}

// OFFSET: 0x00731690
bool CWorldMap::QueryGroundTypeTerrain(const C3Vector& pos, uint32_t& groundType) {
    CMap* map = this->GetMap(pos);

    if (!map) {
        return false;
    }

    return map->QueryGroundTypeTerrain(pos, groundType);
}

// OFFSET: 0x007316C0
static int32_t QueryMapObjDefModels(CMapObjDef* mapObjDef, CM2Model** models, uint32_t maxModels, const CAaBox& box, uint32_t flags) {
    int32_t count = 0;

    for (CMapObjDefGroup* group = mapObjDef->m_defGroupList.Head(), * nextGroup; group; group = nextGroup) {
        nextGroup = mapObjDef->m_defGroupList.Next(group);

        if (!(group->m_bbox.t.x >= box.b.x && group->m_bbox.t.y >= box.b.y && group->m_bbox.t.z >= box.b.z
            && box.t.x >= group->m_bbox.b.x && box.t.y >= group->m_bbox.b.y && box.t.z >= group->m_bbox.b.z)) {
            continue;
        }

        for (CMapBaseObjLink* link = group->m_doodadLinks.Head(), * next; link; link = next) {
            next = group->m_doodadLinks.Next(link);
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);
            int32_t match = 0;

            if (doodadDef->unk_0020 == CWorldMap::s_queryTag) {
                continue;
            }

            uint32_t doodadFlags = doodadDef->m_flags;
            doodadDef->unk_0020 = CWorldMap::s_queryTag;

            if ((doodadFlags & 0x10) || !(doodadFlags & 0x40)) {
                continue;
            }

            if (!(doodadDef->m_bbox.t.x >= box.b.x && doodadDef->m_bbox.t.y >= box.b.y && doodadDef->m_bbox.t.z >= box.b.z
                && box.t.x >= doodadDef->m_bbox.b.x && box.t.y >= doodadDef->m_bbox.b.y && box.t.z >= doodadDef->m_bbox.b.z)) {
                continue;
            }

            if (flags & 0x200122) {
                match = 1;
            }

            if (flags & 0x2000000) {
                match |= doodadFlags & 0x800;
            }

            if (match) {
                models[count] = doodadDef->m_model;
                count++;

                if (count == static_cast<int32_t>(maxModels)) {
                    return count;
                }
            }
        }
    }

    return count;
}

// OFFSET: 0x00731870/0x14040C720
int32_t CWorldMap::QueryFlightBounds(const C3Vector& pos, float& z, int32_t lower) {
    CMap* map = this->GetMap(pos);

    if (!map) {
        return 0;
    }

    return map->GetFlightBounds(pos, z, lower);
}

// OFFSET: 0x007318B0
void CWorldMap::RenderFlightBounds(const C3Vector& pos) {
    CMap* map = this->GetMap(pos);

    if (map) {
        map->RenderFlightBounds(pos);
    }
}

// OFFSET: 0x007318E0
int32_t CWorldMap::QueryTerrainHeight(const C3Vector& pos, float& height) {
    int32_t x = static_cast<int32_t>(-(pos.y - 17066.666f) * 0.23999999f);
    int32_t chunkX = (x >> 3) & 0xF;
    float yf = -(pos.x - 17066.666f) * 0.23999999f;
    int32_t areaX = (x >> 7) & 0x3F;
    int32_t subX = x & 0x7;

    if (this->m_isGlobalMapObj) {
        return 0;
    }

    int32_t y = static_cast<int32_t>(yf);
    CMapTableEntry* entry = this->m_mapTable.Get(areaX, (y >> 7) & 0x3F);

    if (!entry || !(entry->m_flags & 0x1)) {
        return 0;
    }

    CMap* map = this->m_maps[entry->m_mapIndex];

    if (!map) {
        return 0;
    }

    CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.Get(areaX, (y >> 7) & 0x3F);

    if (!areaEntry || !(areaEntry->m_flags & 0x2) || !areaEntry->m_area) {
        return 0;
    }

    CMapChunk* chunk = areaEntry->m_area->m_chunks[chunkX + 16 * ((y >> 3) & 0xF)];

    if (!chunk) {
        return 0;
    }

    float h = 0.0f;

    if (!CMap::GetHeightTerrain(chunk, pos, subX, y & 0x7, 0, h)) {
        return 0;
    }

    height = h;
    return 1;
}

// OFFSET: 0x00731A10/0x14040C9F0
bool CWorldMap::QueryHeight(const C3Vector& start, const C3Vector& end, float& t) {
    CWorldMap::s_queryTag++;

    float xf = -(start.y - 17066.666f) * 0.23999999f;
    float yf = -(start.x - 17066.666f) * 0.23999999f;
    int32_t x = static_cast<int32_t>(xf);
    int32_t y = static_cast<int32_t>(yf);
    int32_t chunkX = (x >> 3) & 0xF;
    int32_t subX = x & 0x7;
    int32_t subY = y & 0x7;

    if (!this->m_isGlobalMapObj) {
        CMapTableEntry* entry = this->m_mapTable.Get((x >> 7) & 0x3F, (y >> 7) & 0x3F);

        if (!entry || !(entry->m_flags & 0x1)) {
            return false;
        }

        CMap* map = this->m_maps[entry->m_mapIndex];

        if (!map) {
            return false;
        }

        CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.Get((x >> 7) & 0x3F, (y >> 7) & 0x3F);

        if (!areaEntry || !(areaEntry->m_flags & 0x2) || !areaEntry->m_area) {
            return false;
        }

        CMapChunk* chunk = areaEntry->m_area->m_chunks[chunkX + 16 * ((y >> 3) & 0xF)];

        if (!chunk) {
            return false;
        }

        if (chunk->m_bbox.t.z > end.z) {
            if (chunk->m_terrainBox.t.z > end.z) {
                float h = 0.0f;

                if (CMap::GetHeightTerrain(chunk, start, subX, subY, 1, h)) {
                    float f = (start.z - h) / (start.z - end.z);

                    if (f >= 0.0f && f <= 1.0f) {
                        t = f;
                    }
                }
            }

            if (chunk->m_liquidBox.t.z > end.z) {
                float h = 0.0f;
                C2Vector point(xf, yf);

                if (chunk->GetLiquidHeight(point, h)) {
                    float f = (start.z - h) / (start.z - end.z);

                    if (f >= 0.0f && f <= 1.0f) {
                        t = f;
                    }
                }
            }

            if (!CMap::GetHeightDoodadDefs(chunk->m_doodadDefLinkList, start, end)) {
                return false;
            }
        }

        if (!CMap::GetHeightMapObjDefs(chunk->m_mapObjDefLinkList, start, end, t)) {
            return false;
        }
    }

    return this->QueryMapObjDefsHeight(start, end, t) != 0;
}

// OFFSET: 0x00731C70
void CWorldMap::QueryTris(const CAaBox& box, World::TriData& tris, uint32_t flags) {
    CWorldMap::s_queryTag++;

    if (flags & 0x300F0) {
        this->QueryMapObjDefTris(box, tris, flags);
    }

    if (!(flags & 0x30100)) {
        return;
    }

    CiRect subChunkRect;
    memset(&subChunkRect, 0, sizeof(subChunkRect));
    CiRect chunkRect(0, 0, 0, 0);
    CiRect areaRect(0, 0, 0, 0);
    CWorldMap::GetQueryRects(box, subChunkRect, chunkRect, areaRect);
    CiRect rect(0, 0, 0, 0);

    for (int32_t y = areaRect.minY; y <= areaRect.maxY; y++) {
        rect.minY = 16 * y <= chunkRect.minY ? chunkRect.minY : 16 * y;
        rect.maxY = 16 * y + 15 >= chunkRect.maxY ? chunkRect.maxY : 16 * y + 15;

        for (int32_t x = areaRect.minX; x <= areaRect.maxX; x++) {
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (!entry || !(entry->m_flags & 0x1)) {
                    continue;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            rect.minX = 16 * x <= chunkRect.minX ? chunkRect.minX : 16 * x;
            rect.maxX = 16 * x + 15 >= chunkRect.maxX ? chunkRect.maxX : 16 * x + 15;
            map->GetTrisTerrain(box, rect, subChunkRect, tris, flags);
        }
    }
}

// OFFSET: 0x00731DE0
void CWorldMap::QueryLiquidSounds(const C3Vector& pos, float radius, TSGrowableArray<World::LiquidQueryResult>& results) {
    CMap* map = this->GetMap(pos);

    if (map) {
        map->QueryLiquidSounds(pos, radius, results);
    }
}

// OFFSET: 0x00731E10
bool CWorldMap::IntersectTerrainSpans(const C3Segment& seg, float& t, C4Plane& plane, uint32_t flags) {
    const uint32_t* span = CWorldMap::s_rasterSpans.m_data;
    uint32_t areaX = ~span[0];
    uint32_t chunkX = areaX;
    uint32_t areaY = ~span[1];
    uint32_t chunkY = areaY;
    CMapArea* area = NULL;
    CMapChunk* chunk = NULL;

    C3Vector delta = seg.t - seg.b;

    if (delta.y * delta.y + delta.x * delta.x + delta.z * delta.z < 0.000099999997f) {
        return false;
    }

    C3Ray local;
    local.origin = seg.b;
    local.dir = delta;

    if (!CWorldMap::s_rasterSpanCount) {
        return false;
    }

    while (true) {
        uint32_t x = span[0];
        uint32_t y = span[1];
        CWorldMap::s_rasterSpanCount -= 2;
        span += 2;

        if ((x & 0x1F80) != areaX || (y & 0x1F80) != areaY) {
            int32_t ax = (x >> 7) & 0x3F;
            int32_t ay = (y >> 7) & 0x3F;
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(ax, ay);

                if (!entry || !(entry->m_flags & 0x1)) {
                    return false;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                return false;
            }

            CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.Get(ax, ay);

            if (!areaEntry || !(areaEntry->m_flags & 0x2)) {
                return false;
            }

            area = areaEntry->m_area;

            if (!area) {
                return false;
            }

            areaX = x & 0x1F80;
            areaY = y & 0x1F80;
        }

        if ((x & 0x78) != chunkX || (y & 0x78) != chunkY) {
            chunk = area->m_chunks[((x >> 3) & 0xF) + 16 * ((y >> 3) & 0xF)];

            if (!chunk) {
                return false;
            }

            chunkX = x & 0x78;
            local.origin = seg.b - chunk->m_position;
            chunkY = y & 0x78;
        }

        if (chunk->Intersect(x & 0x7, y & 0x7, local, plane, t)) {
            return true;
        }

        if (!CWorldMap::s_rasterSpanCount) {
            return false;
        }
    }
}

// OFFSET: 0x00732110/0x14040D3A0
int32_t CWorldMap::QueryAreaId(CMapEntity* entity, uint32_t& areaId) {
    CWorldMap* worldMap = entity->m_worldMap;

    if (worldMap->m_isGlobalMapObj) {
        int32_t id = g_mapDB.GetRecord(worldMap->m_mapId)->m_areaTableID;

        if (!id) {
            return 0;
        }

        areaId = id;
        return 1;
    }

    CMapBaseObjLink* link = entity->m_parentLinkList.Head();

    if (link) {
        if (link->m_ref->m_type & 0x4) {
            float x = entity->m_position.x;

            for (; link; link = entity->m_parentLinkList.Next(link)) {
                CMapChunk* chunk = static_cast<CMapChunk*>(link->m_ref);

                if (x >= chunk->m_bbox.b.x && chunk->m_bbox.t.x >= x) {
                    float y = entity->m_position.y;

                    if (y >= chunk->m_bbox.b.y && chunk->m_bbox.t.y >= y) {
                        areaId = chunk->m_header.m_areaId;
                        return 1;
                    }
                }
            }
        } else {
            for (; link; link = entity->m_parentLinkList.Next(link)) {
                CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(link->m_ref);
                CMapObjDef* mapObjDef = defGroup->m_mapObjDef;

                if (mapObjDef->m_flags & 0x200) {
                    continue;
                }

                CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(defGroup->m_groupNum, 0);

                if (group) {
                    const WMOAreaTableRec* rec = SDBWMOAreaTableLookup(mapObjDef->m_mapObj->m_header->m_wmoId, mapObjDef->m_nameSet, group->m_wmoGroupId);

                    if (rec && rec->m_areaTableID) {
                        areaId = rec->m_areaTableID;
                        return 1;
                    }
                }

                break;
            }
        }
    }

    CMap* map = worldMap->GetMap(entity->m_position);
    areaId = map ? map->QueryAreaIdTerrain(entity->m_position) : 0;
    return 1;
}

// OFFSET: 0x00732270/0x14040D530
int32_t CWorldMap::QueryAreaIds(CMapEntity* entity, uint32_t* areaIds) {
    CWorldMap* worldMap = entity->m_worldMap;
    areaIds[0] = 0;
    areaIds[1] = 0;
    areaIds[2] = 0;

    if (worldMap->m_isGlobalMapObj) {
        int32_t id = g_mapDB.GetRecord(worldMap->m_mapId)->m_areaTableID;

        if (!id) {
            return 0;
        }

        areaIds[1] = id;
        return 1;
    }

    CMapBaseObjLink* link = entity->m_parentLinkList.Head();

    if (link) {
        int32_t found = 0;

        for (; link; link = entity->m_parentLinkList.Next(link)) {
            CMapBaseObj* ref = link->m_ref;

            if (ref->m_type & 0x4) {
                CMapChunk* chunk = static_cast<CMapChunk*>(ref);
                float x = entity->m_position.x;

                if (x >= chunk->m_bbox.b.x && chunk->m_bbox.t.x >= x) {
                    float y = entity->m_position.y;

                    if (y >= chunk->m_bbox.b.y && chunk->m_bbox.t.y >= y) {
                        areaIds[2] = chunk->m_header.m_areaId;
                        found = 1;
                    }
                }
            } else if (ref->m_type & MapBaseObj_Type_MapObjDefGroup) {
                CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(ref);
                CMapObjDef* mapObjDef = defGroup->m_mapObjDef;
                CMapObjGroup* group = mapObjDef->m_mapObj->GetGroup(defGroup->m_groupNum, 0);

                if (!group) {
                    continue;
                }

                const WMOAreaTableRec* rec = SDBWMOAreaTableLookup(mapObjDef->m_mapObj->m_header->m_wmoId, mapObjDef->m_nameSet, group->m_wmoGroupId);

                if (!rec || !rec->m_areaTableID) {
                    continue;
                }

                if (mapObjDef->m_flags & 0x200) {
                    if (!areaIds[0]) {
                        areaIds[0] = rec->m_areaTableID;
                    }
                } else if (!areaIds[1]) {
                    areaIds[1] = rec->m_areaTableID;
                }
            }
        }

        if (found) {
            return 1;
        }
    }

    CMap* map = worldMap->GetMap(entity->m_position);

    if (!map) {
        areaIds[2] = 0;
        return 1;
    }

    areaIds[2] = map->QueryAreaIdTerrain(entity->m_position);
    return 1;
}

// OFFSET: 0x00732410/0x14040D6E0
int32_t CWorldMap::QueryOutdoors(CMapEntity* entity) {
    CMapObjDef* mapObjDef;
    CMapObj* mapObj;
    CMapObjDefGroup* defGroup;
    CMapObjGroup* group = NULL;

    if (!entity->GetMapObjAndGroup(mapObjDef, mapObj, defGroup, group, 0)) {
        CMap* map = entity->m_worldMap->GetMap(entity->m_position);
        uint32_t areaId = map ? map->QueryAreaIdTerrain(entity->m_position) : 0;
        AreaTableRec* rec = g_areaTableDB.GetRecord(areaId);

        if (!rec || (rec->m_flags & 0x4000000)) {
            return 1;
        }

        return (rec->m_flags & 0x2000000) == 0;
    }

    int32_t outdoors = (group->m_groupFlags >> 3) & 1;
    const WMOAreaTableRec* rec = SDBWMOAreaTableLookup(mapObj->m_header->m_wmoId, mapObjDef->m_nameSet, group->m_wmoGroupId);

    if (rec) {
        uint32_t flags = rec->m_flags;

        if (flags & 0x4) {
            return 1;
        }

        if (flags & 0x2) {
            return 0;
        }
    }

    return outdoors;
}

// OFFSET: 0x007324F0
int32_t CWorldMap::QueryIntersectingModels(CM2Model** models, uint32_t maxModels, const CAaBox& box, uint32_t flags) {
    if (!maxModels) {
        return 0;
    }

    CWorldMap::s_queryTag++;
    int32_t count = 0;

    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);
        uint32_t defFlags = mapObjDef->m_flags;

        if ((defFlags & 0x40) && !(defFlags & 0x90)
            && mapObjDef->m_bbox.t.x >= box.b.x && mapObjDef->m_bbox.t.y >= box.b.y && mapObjDef->m_bbox.t.z >= box.b.z
            && box.t.x >= mapObjDef->m_bbox.b.x && box.t.y >= mapObjDef->m_bbox.b.y && box.t.z >= mapObjDef->m_bbox.b.z) {
            count += QueryMapObjDefModels(mapObjDef, models, maxModels, box, flags);
        }
    }

    int32_t minX = static_cast<int32_t>(-(box.t.y - 17066.666f) * 0.029999999f) >> 4;
    int32_t maxX = static_cast<int32_t>(-(box.b.y - 17066.666f) * 0.029999999f) >> 4;
    int32_t maxY = static_cast<int32_t>(-(box.b.x - 17066.666f) * 0.029999999f) >> 4;
    int32_t minY = static_cast<int32_t>(-(box.t.x - 17066.666f) * 0.029999999f) >> 4;

    for (int32_t y = minY; y <= maxY; y++) {
        for (int32_t x = minX; x <= maxX; x++) {
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (!entry || !(entry->m_flags & 0x1)) {
                    continue;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.Get(x, y);

            if (!areaEntry || !(areaEntry->m_flags & 0x2) || !areaEntry->m_area) {
                continue;
            }

            CMapArea* area = areaEntry->m_area;

            for (CMapBaseObjLink* link = area->m_linkList.Head(), * nextLink; link; link = nextLink) {
                nextLink = area->m_linkList.Next(link);
                CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);

                if (mapObjDef->unk_004C == CWorldMap::s_queryTag) {
                    continue;
                }

                uint32_t defFlags = mapObjDef->m_flags;
                mapObjDef->unk_004C = CWorldMap::s_queryTag;

                if ((defFlags & 0x40)
                    && mapObjDef->m_bbox.t.x >= box.b.x && mapObjDef->m_bbox.t.y >= box.b.y && mapObjDef->m_bbox.t.z >= box.b.z
                    && box.t.x >= mapObjDef->m_bbox.b.x && box.t.y >= mapObjDef->m_bbox.b.y && box.t.z >= mapObjDef->m_bbox.b.z) {
                    count += QueryMapObjDefModels(mapObjDef, models, maxModels, box, flags);
                }
            }
        }
    }

    return count;
}

// OFFSET: 0x00732830
bool CWorldMap::QueryFacets(const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags) {
    CWorldMap::s_queryTag++;
    facets.facets.SetCount(0);
    facets.facetIds.m_count = 0;
    facets.facetFlags.m_count = 0;
    facets.m_status = 0;

    if (!this->m_facetQueryValid || !this->QueryMapObjDefFacets(box, box2, facets, flags)) {
        return false;
    }

    if (this->m_isGlobalMapObj) {
        return true;
    }

    CiRect subChunkRect;
    memset(&subChunkRect, 0, sizeof(subChunkRect));
    CiRect chunkRect(0, 0, 0, 0);
    CiRect areaRect(0, 0, 0, 0);
    CWorldMap::GetQueryRects(box2, subChunkRect, chunkRect, areaRect);
    CiRect rect(0, 0, 0, 0);

    for (int32_t y = areaRect.minY; y <= areaRect.maxY; y++) {
        rect.minY = 16 * y <= chunkRect.minY ? chunkRect.minY : 16 * y;
        rect.maxY = 16 * y + 15 >= chunkRect.maxY ? chunkRect.maxY : 16 * y + 15;

        for (int32_t x = areaRect.minX; x <= areaRect.maxX; x++) {
            CMap* map = NULL;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (entry && (entry->m_flags & 0x1)) {
                    map = this->m_maps[entry->m_mapIndex];
                }
            }

            if (!map) {
                CAaBox blocker;
                blocker.t.y = -(static_cast<float>(x) * 533.33331f) + 17066.666f;
                blocker.b.y = blocker.t.y - 533.33331f;
                blocker.t.x = -(static_cast<float>(y) * 533.33331f) + 17066.666f;
                blocker.b.x = blocker.t.x - 533.33331f;
                blocker.b.z = box2.b.z - 100.0f;
                blocker.t.z = box2.t.z + 100.0f;
                World::AddAaBoxFacets(blocker, 0, facets, 1);
                continue;
            }

            rect.minX = 16 * x <= chunkRect.minX ? chunkRect.minX : 16 * x;
            rect.maxX = 16 * x + 15 >= chunkRect.maxX ? chunkRect.maxX : 16 * x + 15;

            if (!map->GetFacets(box, box2, rect, subChunkRect, facets, flags)) {
                return false;
            }

            if ((flags & 0x600) && !map->CreateFlightBoundsFacets(x, y, box2, facets, flags)) {
                return false;
            }
        }
    }

    return true;
}

// OFFSET: 0x00732AE0
bool CWorldMap::QueryFacets(const CAaBox& box, World::FacetData& facets, uint32_t flags) {
    return this->QueryFacets(box, box, facets, flags);
}

// OFFSET: 0x00732B00
bool CWorldMap::QueryFacets(const CFrustum& frustum, World::FacetData& facets, uint32_t flags) {
    CWorldMap::s_queryTag++;
    facets.facets.SetCount(0);
    facets.facetIds.m_count = 0;
    facets.facetFlags.m_count = 0;
    facets.m_status = 0;
    this->QueryMapObjDefFacets(frustum, facets, flags);

    if (!this->m_isGlobalMapObj) {
        CAaBox bounds = CAaBox::Bounding(frustum.corners, 8);
        CiRect subChunkRect;
        memset(&subChunkRect, 0, sizeof(subChunkRect));
        CiRect chunkRect(0, 0, 0, 0);
        CiRect areaRect(0, 0, 0, 0);
        CWorldMap::GetQueryRects(bounds, subChunkRect, chunkRect, areaRect);
        CiRect rect(0, 0, 0, 0);

        for (int32_t y = areaRect.minY; y <= areaRect.maxY; y++) {
            rect.minY = 16 * y <= chunkRect.minY ? chunkRect.minY : 16 * y;
            rect.maxY = 16 * y + 15 >= chunkRect.maxY ? chunkRect.maxY : 16 * y + 15;

            for (int32_t x = areaRect.minX; x <= areaRect.maxX; x++) {
                CMap* map;

                if (this->m_isGlobalMapObj) {
                    map = this->m_maps[0];
                } else {
                    CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                    if (!entry || !(entry->m_flags & 0x1)) {
                        continue;
                    }

                    map = this->m_maps[entry->m_mapIndex];
                }

                if (!map) {
                    continue;
                }

                rect.minX = 16 * x <= chunkRect.minX ? chunkRect.minX : 16 * x;
                rect.maxX = 16 * x + 15 >= chunkRect.maxX ? chunkRect.maxX : 16 * x + 15;
                map->GetFacets(frustum, bounds, rect, subChunkRect, facets, flags);
            }
        }
    }

    return facets.facets.Count() != 0;
}

// OFFSET: 0x00732CA0/0x14040E330
bool CWorldMap::QueryHeight(const C3Vector& pos, float range, float& height) {
    if (range < 0.0099999998f) {
        return false;
    }

    CWorldScene::s_m2Scene->BeginHitTest();
    C3Vector start(pos.x, pos.y, pos.z + range);
    C3Vector end(pos.x, pos.y, pos.z - range);
    float t = 1.0f;

    if (!this->QueryHeight(start, end, t)) {
        CWorldScene::s_m2Scene->ClearHitTestList();
        return false;
    }

    CWorldScene::s_m2Scene->EndHitTestCollisionWorld(start, end, t);

    if (t > 1.0f) {
        height = end.z;
        return true;
    }

    float z = start.z;

    if (t >= 0.0f) {
        z = start.z - (start.z - end.z) * t;
    }

    height = z;
    return true;
}

// OFFSET: 0x00732DD0/0x14040E460
bool CWorldMap::IntersectTerrain(const C3Segment& seg, float& t, C4Plane& plane, uint32_t flags) {
    float y1 = -(seg.t.y - 17066.666f);
    float y0 = -(seg.b.y - 17066.666f);
    float x0 = -(seg.b.x - 17066.666f);
    float x1 = -(seg.t.x - 17066.666f);
    float dy = y1 - y0;
    float dx = x1 - x0;

    int32_t coords[4];
    coords[0] = static_cast<int32_t>(x0 * 0.23999999f);
    coords[1] = static_cast<int32_t>(y0 * 0.23999999f);
    coords[2] = static_cast<int32_t>(x1 * 0.23999999f);
    coords[3] = static_cast<int32_t>(y1 * 0.23999999f);

    CWorldMap::s_rasterSpanCount = 0;
    CWorldMap::s_rasterSpans.SetCount(4096);

    dy = fabs(dy);

    if (dy < 0.00000023841858f || coords[1] == coords[3]) {
        this->RasterizeRow(coords);
    } else {
        dx = fabs(dx);

        if (dx < 0.00000023841858f || coords[0] == coords[2]) {
            this->RasterizeColumn(coords);
        } else {
            C3Vector start(y0, x0, 0.0f);
            C3Vector end(y1, x1, 0.0f);

            if (dy <= dx) {
                this->RasterizeShallow(start, end, coords);
            } else {
                this->RasterizeSteep(start, end, coords);
            }
        }
    }

    float scale = t;
    C3Segment clipped;
    clipped.b = seg.b;
    clipped.t.x = seg.b.x + (seg.t.x - seg.b.x) * scale;
    clipped.t.y = seg.b.y + (seg.t.y - seg.b.y) * scale;
    clipped.t.z = seg.b.z + (seg.t.z - seg.b.z) * scale;

    float localT = 1.0f;
    bool result = this->IntersectTerrainSpans(clipped, localT, plane, flags);

    if (result) {
        t = t * localT;
    }

    return result;
}

// OFFSET: 0x00733050
bool CWorldMap::Intersect(const C3Segment& seg, float& t, C4Plane& plane, uint32_t flags) {
    float dz = seg.t.z - seg.b.z;
    float dy = seg.t.y - seg.b.y;
    bool hit = false;

    if ((seg.t.x - seg.b.x) * (seg.t.x - seg.b.x) + dz * dz + dy * dy < 0.001f) {
        return false;
    }

    if (flags & 0xF000FF) {
        hit = this->IntersectMapObjDefs(seg, t, plane, flags);
    }

    if (flags & 0xF0010F) {
        return this->IntersectTerrain(seg, t, plane, flags) | hit;
    }

    return hit;
}

// OFFSET: 0x00762F70/0x1404481B0
static int32_t SortMapObjDefIntersects(const void* a, const void* b) {
    float distA = static_cast<const SMapObjDefVectorIntersect*>(a)->m_distSq;
    float distB = static_cast<const SMapObjDefVectorIntersect*>(b)->m_distSq;

    if (distB <= distA) {
        return distA > distB;
    }

    return -1;
}

// OFFSET: 0x00763050
void CWorldMap::RasterizeColumn(const int32_t* coords) {
    int32_t y = coords[1];

    if (y <= coords[3]) {
        do {
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = y;
            y++;
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[0];
        } while (y <= coords[3]);
    } else {
        do {
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = y;
            y--;
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[0];
        } while (y >= coords[3]);
    }
}

// OFFSET: 0x007630E0
void CWorldMap::RasterizeRow(const int32_t* coords) {
    int32_t x = coords[0];

    if (x <= coords[2]) {
        do {
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[1];
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = x++;
        } while (x <= coords[2]);
    } else {
        do {
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[1];
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = x--;
        } while (x >= coords[2]);
    }
}

// OFFSET: 0x00763170
void CWorldMap::RasterizeSteep(const C3Vector& start, const C3Vector& end, const int32_t* coords) {
    int32_t i = coords[1];
    int32_t j = coords[0];
    float slope = (end.y - start.y) / (end.x - start.x);
    float intercept = start.y - start.x * slope;
    float first;
    int32_t step;

    if (coords[3] <= i) {
        first = static_cast<float>(i);
        step = -1;
    } else {
        first = static_cast<float>(i + 1);
        step = 1;
    }

    CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = i;
    CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = j;
    float pos = first * 4.1666665f;

    if (i != step + coords[3]) {
        while (true) {
            if (CWorldMap::s_rasterSpanCount >= 4092) {
                return;
            }

            int32_t k = static_cast<int32_t>((pos * slope + intercept) * 0.23999999f);

            if (k != j) {
                CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = i;
                CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = k;
            }

            i += step;
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = i;
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = k;
            pos = static_cast<float>(step) * 4.1666665f + pos;
            j = k;

            if (i == step + coords[3]) {
                break;
            }
        }
    }

    if (j != coords[2]) {
        CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[3];
        CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[2];
    }
}

// OFFSET: 0x00763300
void CWorldMap::RasterizeShallow(const C3Vector& start, const C3Vector& end, const int32_t* coords) {
    float slope = (end.y - start.y) / (end.x - start.x);
    int32_t j = coords[1];
    int32_t i = coords[0];
    float intercept = start.y - start.x * slope;
    float invSlope = 1.0f / slope;
    float first;
    int32_t step;

    if (coords[2] <= i) {
        first = static_cast<float>(i);
        step = -1;
    } else {
        first = static_cast<float>(i + 1);
        step = 1;
    }

    CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[1];
    CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[0];
    float pos = first * 4.1666665f;

    if (i != step + coords[2]) {
        while (true) {
            if (CWorldMap::s_rasterSpanCount >= 4092) {
                return;
            }

            int32_t k = static_cast<int32_t>(((pos - intercept) * invSlope) * 0.23999999f);

            if (k != j) {
                CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = k;
                CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = i;
            }

            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = k;
            i += step;
            CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = i;
            pos = static_cast<float>(step) * 4.1666665f + pos;
            j = k;

            if (i == step + coords[2]) {
                break;
            }
        }
    }

    if (j != coords[3]) {
        CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[3];
        CWorldMap::s_rasterSpans.m_data[CWorldMap::s_rasterSpanCount++] = coords[2];
    }
}

// OFFSET: 0x00763550/0x1404488D0
void CWorldMap::VectorIntersectMapObjDef(CMapObjDef* mapObjDef, const C3Vector& start, const C3Vector& end) {
    if (!(mapObjDef->m_mapObj->m_flags & 0x40)) {
        return;
    }

    for (uint32_t i = 0; ; i++) {
        uint32_t count = mapObjDef->m_defGroups.m_count;

        if (count == 0xFFFFFFFF) {
            count = mapObjDef->m_defGroups.Dynamic().m_count;
        }

        if (i >= count) {
            break;
        }

        CMapObjDefGroup* group = mapObjDef->m_defGroups[i];

        if (!CWorldMath::VectorIntersectAABox2(group->m_bbox, start, end)) {
            continue;
        }

        SMapObjDefVectorIntersect intersect;
        intersect.m_mapObjDef = mapObjDef;
        intersect.m_group = group;
        const CAaBox& defBox = mapObjDef->m_bbox;

        if (start.x < defBox.b.x || start.y < defBox.b.y || start.z < defBox.b.z || defBox.t.x < start.x || defBox.t.y < start.y || defBox.t.z < start.z) {
            const CAaBox& box = group->m_bbox;
            float x = box.b.x <= start.x ? (start.x <= box.t.x ? start.x : box.t.x) : box.b.x;
            float y = box.b.y <= start.y ? (start.y <= box.t.y ? start.y : box.t.y) : box.b.y;
            float z = box.b.z <= start.z ? (start.z <= box.t.z ? start.z : box.t.z) : box.b.z;
            intersect.m_distSq = (start.x - x) * (start.x - x) + (start.z - z) * (start.z - z) + (start.y - y) * (start.y - y);
        } else {
            intersect.m_distSq = 0.0f;
        }

        CWorldMap::s_mapObjDefIntersects.Add(1, &intersect);
    }
}

// OFFSET: 0x007636C0/0x140448A80
void CWorldMap::VectorIntersectDoodadDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, uint32_t flags) {
    uint32_t mode;

    if (flags & 0x1) {
        mode = 3;
    } else if (flags & 0x8) {
        mode = 2;
    } else {
        if (!(flags & 0x6)) {
            return;
        }

        mode = (flags >> 24) & 0x1;
    }

    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        next = links.Next(link);
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->m_owner);

        if (doodadDef->unk_0020 == CWorldMap::s_queryTag) {
            continue;
        }

        doodadDef->unk_0020 = CWorldMap::s_queryTag;

        if (!(doodadDef->m_flags & 0x40)) {
            continue;
        }

        CM2Model* model = doodadDef->m_model;

        if (!(model->m_fFlags & 0x1)) {
            continue;
        }

        if (!model->m_hitTestPrev) {
            CM2Model** head = &model->m_scene->m_hitTestList;
            model->m_hitTestPrev = head;
            model->m_hitTestNext = *head;
            *head = model;

            if (model->m_hitTestNext) {
                model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
            }
        }

        model->m_hitTestMode = mode;
        model->m_hitTestGroup = 0;
        model->m_hitTestOwner = doodadDef;
    }
}

// OFFSET: 0x00763790/0x140448B70
void CWorldMap::VectorIntersectEntitys(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, uint32_t flags) {
    if (!World::s_entityRenderCallback) {
        return;
    }

    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        CMapEntity* entity = static_cast<CMapEntity*>(link->m_owner);
        next = links.Next(link);

        if ((flags & 0x1000000) && (entity->m_entityFlags & 0x60000) != 0x20000) {
            continue;
        }

        if ((flags & 0x800000) && (entity->m_entityFlags & 0x800)) {
            continue;
        }

        if (entity->m_flags & 0x80) {
            return;
        }

        if ((entity->m_flags & 0x10000) || entity->unk_0020 == CWorldMap::s_queryTag) {
            continue;
        }

        entity->unk_0020 = CWorldMap::s_queryTag;
        World::ObjectCollisionData data;

        if (!reinterpret_cast<World::OBJECT_COLLISION_CALLBACK>(World::s_entityRenderCallback)(entity->m_param64, entity->m_param32, &data, flags)) {
            continue;
        }

        if (!data.m_model) {
            if (!entity->m_model) {
                continue;
            }

            data.m_model = entity->m_model;
        }

        if (!data.m_model->IsLoaded()) {
            continue;
        }

        CM2Model* model = data.m_model;

        if (!(model->m_fFlags & 0x1)) {
            continue;
        }

        if (!model->m_hitTestPrev) {
            model->m_hitTestPrev = &model->m_scene->m_hitTestList;
            model->m_hitTestNext = model->m_scene->m_hitTestList;
            model->m_scene->m_hitTestList = model;

            if (model->m_hitTestNext) {
                model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
            }
        }

        model->m_hitTestMode = (flags & 0x100000) ? 3 : 0;
        model->m_hitTestGroup = 0;
        model->m_hitTestOwner = entity;
    }
}

// OFFSET: 0x00763990/0x140448DE0
void CWorldMap::VectorIntersectMapObjDefs(TSExplicitList<CMapObjDef, offsetof(CMapObjDef, m_link30)>& list, const C3Vector& start, const C3Vector& end, uint32_t flags) {
    for (CMapObjDef* mapObjDef = list.Head(), * next; mapObjDef; mapObjDef = next) {
        next = list.Next(mapObjDef);

        if ((mapObjDef->m_flags & 0x90) || mapObjDef->unk_004C == CWorldMap::s_queryTag) {
            continue;
        }

        mapObjDef->unk_004C = CWorldMap::s_queryTag;

        if (CWorldMath::VectorIntersectAABox2(mapObjDef->m_bbox, start, end)) {
            CWorldMap::VectorIntersectMapObjDef(mapObjDef, start, end);
        }
    }
}

// OFFSET: 0x00763A10
void CWorldMap::VectorIntersectMapObjDefs(TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links, const C3Vector& start, const C3Vector& end, uint32_t flags) {
    for (CMapBaseObjLink* link = links.Head(), * next; link; link = next) {
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);
        next = links.Next(link);

        if ((mapObjDef->m_flags & 0x90) || mapObjDef->unk_004C == CWorldMap::s_queryTag) {
            continue;
        }

        mapObjDef->unk_004C = CWorldMap::s_queryTag;

        if (CWorldMath::VectorIntersectAABox2(mapObjDef->m_bbox, start, end)) {
            CWorldMap::VectorIntersectMapObjDef(mapObjDef, start, end);
        }
    }
}

// OFFSET: 0x00763A90
bool CWorldMap::VectorIntersectSubChunkList(const C3Vector& start, const C3Vector& end, float& t, uint32_t flags, CMapChunk*& hitChunk, Liquid::CInstance*& hitLiquid) {
    const uint32_t* span = CWorldMap::s_rasterSpans.m_data;
    uint32_t chunkX = ~span[0];
    uint32_t areaX = chunkX;
    uint32_t chunkY = ~span[1];
    uint32_t areaY = chunkY;
    C3Vector dir(end.x - start.x, end.y - start.y, end.z - start.z);
    CMapArea* area = NULL;
    CMapChunk* chunk = NULL;
    CMapChunk* bestChunk = NULL;
    Liquid::CInstance* bestLiquid = NULL;
    float invLength = 1.0f / sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);

    C3Ray ray;
    ray.origin = start;
    ray.dir.x = dir.x * invLength;
    ray.dir.y = dir.y * invLength;
    ray.dir.z = dir.z * invLength;
    float best = t;
    int32_t testTerrain = 1;
    int32_t testLiquid = 1;
    int32_t filterLiquid = (flags & 0x10000) && !(flags & 0x20000);

    if (!CWorldMap::s_rasterSpanCount) {
        return false;
    }

    do {
        uint32_t x = span[0];
        uint32_t y = span[1];
        CWorldMap::s_rasterSpanCount -= 2;
        span += 2;

        if ((x & 0x1F80) != areaX || (y & 0x1F80) != areaY) {
            int32_t ax = (x >> 7) & 0x3F;
            int32_t ay = (y >> 7) & 0x3F;
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(ax, ay);

                if (!entry || !(entry->m_flags & 0x1)) {
                    break;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                break;
            }

            CMap::CMapAreaTableEntry* areaEntry = map->m_areaTable.Get(ax, ay);

            if (!areaEntry || !(areaEntry->m_flags & 0x2)) {
                break;
            }

            area = areaEntry->m_area;

            if (!area) {
                break;
            }

            areaX = x & 0x1F80;
            areaY = y & 0x1F80;
        }

        if ((x & 0x78) != chunkX || (y & 0x78) != chunkY) {
            chunk = area->m_chunks[((x >> 3) & 0xF) + 16 * ((y >> 3) & 0xF)];

            if (!chunk) {
                break;
            }

            ray.origin = start - chunk->m_position;
            chunkX = x & 0x78;
            chunkY = y & 0x78;
            testTerrain = 0;
            testLiquid = 0;

            if (CWorldMath::VectorIntersectAABox2(chunk->m_bbox, start, end)) {
                if (flags & 0x4000000F) {
                    CWorldMap::VectorIntersectDoodadDefs(chunk->m_doodadDefLinkList, flags);
                }

                testTerrain = 1;
                testLiquid = 1;
            }

            if (flags & 0x40F300FF) {
                CWorldMap::VectorIntersectMapObjDefs(chunk->m_mapObjDefLinkList, start, end, flags);
            }

            if (flags & 0x40F00000) {
                CWorldMap::VectorIntersectEntitys(chunk->m_entityLinkList, flags);
            }
        }

        uint32_t subY = y & 0x7;
        uint32_t subX = x & 0x7;

        if (testTerrain && (flags & 0x100)) {
            float hitT = FLT_MAX;

            if (chunk->Intersect(subX, subY, ray, hitT)) {
                if (hitT < FLT_MAX) {
                    hitT = hitT * invLength;
                }

                if (best > hitT && hitT >= 0.0f) {
                    best = hitT;
                    bestChunk = chunk;
                }
            }
        }

        if (testLiquid && (flags & 0x30000)) {
            for (CChunkLiquid* liquid = chunk->m_liquidList.Head(), * next; liquid; liquid = next) {
                next = chunk->m_liquidList.Next(liquid);

                if (filterLiquid && !(g_liquidTypeDB.GetRecord(liquid->m_liquidType)->m_flags & 0x4)) {
                    continue;
                }

                if (!liquid->TileExists(subX, subY)) {
                    continue;
                }

                float hitT = FLT_MAX;

                if (!liquid->Intersect(subX, subY, ray, hitT)) {
                    continue;
                }

                if (hitT < FLT_MAX) {
                    hitT = hitT * invLength;
                }

                if (best > hitT && hitT >= 0.0f) {
                    best = hitT;
                    bestChunk = chunk;
                    bestLiquid = liquid->m_instance;
                }
            }
        }
    } while (CWorldMap::s_rasterSpanCount);

    if (!bestChunk) {
        return false;
    }

    hitChunk = bestChunk;
    t = best;
    hitLiquid = bestLiquid;
    return true;
}

// OFFSET: 0x00763FC0/0x1404495A0
bool CWorldMap::VectorIntersectTerrain(const C3Vector& start, const C3Vector& end, float& t, uint32_t flags, CMapChunk*& hitChunk, Liquid::CInstance*& hitLiquid) {
    float y1 = -(end.y - 17066.666f);
    float x1 = -(end.x - 17066.666f);
    float y0 = -(start.y - 17066.666f);
    float x0 = -(start.x - 17066.666f);
    float dy = y1 - y0;
    float dx = x1 - x0;

    int32_t coords[4];
    coords[0] = static_cast<int32_t>(x0 * 0.23999999f);
    coords[3] = static_cast<int32_t>(y1 * 0.23999999f);
    coords[1] = static_cast<int32_t>(y0 * 0.23999999f);
    coords[2] = static_cast<int32_t>(x1 * 0.23999999f);

    CWorldMap::s_rasterSpanCount = 0;
    CWorldMap::s_rasterSpans.SetCount(4096);

    dy = fabs(dy);

    if (dy < 0.00000023841858f || coords[1] == coords[3]) {
        this->RasterizeRow(coords);
    } else {
        dx = fabs(dx);

        if (dx < 0.00000023841858f || coords[0] == coords[2]) {
            this->RasterizeColumn(coords);
        } else {
            C3Vector startRaster(y0, x0, 0.0f);
            C3Vector endRaster(y1, x1, 0.0f);

            if (dy <= dx) {
                this->RasterizeShallow(startRaster, endRaster, coords);
            } else {
                this->RasterizeSteep(startRaster, endRaster, coords);
            }
        }
    }

    return this->VectorIntersectSubChunkList(start, end, t, flags, hitChunk, hitLiquid);
}

// OFFSET: 0x007641B0/0x1404498E0
bool CWorldMap::VectorIntersect(const C3Vector& start, const C3Vector& end, C3Vector* hitPoint, float* dist, uint32_t flags, World::HitTestDebug* debug) {
    CWorldMap::s_queryTag++;
    CWorldMap::s_mapObjDefIntersects.m_count = 0;
    CMapBaseObj* hitObject = NULL;
    float best = *dist;
    bool hit = false;
    CWorldScene::s_m2Scene->m_lastHit.m_model = NULL;
    World::ClearLastCollisionGUID();
    uint32_t testModels = flags & 0x40F0000F;

    if (testModels) {
        CWorldScene::s_m2Scene->BeginHitTest();
    }

    if (flags & 0x40F300FF) {
        CWorldMap::VectorIntersectMapObjDefs(this->m_mapObjDefList, start, end, flags);
    }

    CMapChunk* chunk = NULL;
    Liquid::CInstance* liquid = NULL;
    float terrainT = best;

    if (flags & 0x40F301FF) {
        this->VectorIntersectTerrain(start, end, terrainT, flags, chunk, liquid);

        if (chunk) {
            best = terrainT;
            hitObject = chunk;
            hit = true;
        }
    }

    CMapObjDef* hitDef = NULL;
    CMapObjDefGroup* hitGroup = NULL;
    float wmoT = best;

    if (CWorldMap::s_mapObjDefIntersects.m_count) {
        qsort(CWorldMap::s_mapObjDefIntersects.m_data, CWorldMap::s_mapObjDefIntersects.m_count, sizeof(SMapObjDefVectorIntersect), SortMapObjDefIntersects);
        uint32_t ignoreFlags = CMapObj::CreateWmoIgnoreFlags(flags);
        float dz = end.z - start.z;
        float dy = end.y - start.y;
        float dx = end.x - start.x;
        float invLength = 1.0f / sqrt(dz * dz + dy * dy + dx * dx);

        for (uint32_t i = 0; i < CWorldMap::s_mapObjDefIntersects.m_count; i++) {
            SMapObjDefVectorIntersect& intersect = CWorldMap::s_mapObjDefIntersects.m_data[i];

            if (sqrt(intersect.m_distSq) * invLength > wmoT) {
                break;
            }

            CMapObjDef* mapObjDef = intersect.m_mapObjDef;
            CMapObjDefGroup* group = intersect.m_group;
            CMapObj* mapObj = mapObjDef->m_mapObj;
            C3Vector localStart = start * mapObjDef->m_invMat;
            C3Vector localEnd = end * mapObjDef->m_invMat;

            if (mapObj->TestGroupBounds(localStart, localEnd, group->m_groupNum)) {
                uint32_t faceIndex = 0xFFFFFFFF;

                if (mapObj->Intersect(localStart, localEnd, wmoT, flags, ignoreFlags, group->m_groupNum, faceIndex)) {
                    hitDef = mapObjDef;
                    hitGroup = group;
                }
            }

            if (testModels) {
                CWorldMap::VectorIntersectDoodadDefs(group->m_doodadLinks, flags);
            }

            if (flags & 0x40F00000) {
                CWorldMap::VectorIntersectEntitys(group->m_entityLinks, flags);
            }
        }

        if (hitDef) {
            if (hitDef->m_param64) {
                World::SetLastCollisionGUID(hitDef->m_param64);
            }

            best = wmoT;
            hitObject = hitGroup;
            hit = true;
        }
    }

    float modelT = best;

    if (testModels) {
        if (flags & 0x16000AE) {
            C3Vector collisionStart = start * World::s_camTargetView;
            C3Vector collisionEnd = end * World::s_camTargetView;
            CWorldScene::s_m2Scene->EndHitTest(collisionStart, collisionEnd, modelT, 0);
        } else {
            CWorldScene::s_m2Scene->EndHitTestCollisionWorld(start, end, modelT);
        }

        if (best > modelT && CWorldScene::s_m2Scene->m_lastHit.m_model) {
            CMapBaseObj* owner = static_cast<CMapBaseObj*>(CWorldScene::s_m2Scene->m_lastHitOwner);
            best = modelT;
            hitObject = owner;
            hit = true;

            if (owner->m_type & MapBaseObj_Type_Entity) {
                uint64_t guid = static_cast<CMapEntity*>(owner)->m_param64;

                if (guid) {
                    World::SetLastCollisionGUID(guid);
                }
            }
        } else if (!hit) {
            return false;
        }
    } else if (!hit) {
        return false;
    }

    if (debug) {
        CWorldMap::SetHitTestDebug(debug, hitObject);
    }

    if (liquid && debug) {
        CWorldMap::SetHitTestDebug(debug, liquid);
    }

    if (hitPoint) {
        hitPoint->x = start.x + (end.x - start.x) * best;
        hitPoint->y = start.y + (end.y - start.y) * best;
        hitPoint->z = start.z + (end.z - start.z) * best;
    }

    *dist = best;
    return hit;
}

// OFFSET: 0x00764920
void CWaterRipple::Init(const C3Vector& pos, float facing, float radius, float alpha, float duration, float growth, int32_t large) {
    this->m_large = large != 0;
    this->m_position = pos;
    this->m_facing = facing;
    float endRadius = duration * growth + radius;
    this->m_growRate = (endRadius - radius) / duration;
    this->m_alpha = 0.0f;
    this->m_radius = radius;
    this->m_maxAlpha = alpha;
    this->m_riseRate = alpha / (s_rippleRiseFraction[this->m_large] * duration);
    this->m_fallRate = -(alpha / ((1.0f - s_rippleRiseFraction[this->m_large]) * duration));
    this->m_endTime = FrameTime::GetCurTimeSec() + duration;

    CAaBox box;
    box.b.x = pos.x - endRadius;
    box.b.y = pos.y - endRadius;
    box.b.z = pos.z - 1.0f;
    box.t.x = pos.x + endRadius;
    box.t.y = pos.y + endRadius;
    box.t.z = pos.z + 1.0f;

    this->m_verts.SetCount(0);
    World::TriData::s_statusFlags = 0;
    World::TriData::s_nBatches = 0;
    World::TriData::s_faceIndexCursor = 0;
    World::TriData::s_indexCursor = 0;
    World::TriData::s_nMatrices = 0;
    World::TriData tris;
    World::GetTris(box, tris, 0x20000);

    for (uint32_t b = 0; b < World::TriData::s_nBatches; b++) {
        const World::TriData::Batch& batch = World::TriData::s_batches[b];
        const uint16_t* index = batch.m_indices;
        uint32_t base = this->m_verts.m_count;
        this->m_verts.SetCount(batch.m_faceCount + base + 2 * batch.m_faceCount);
        C3Vector* out = &this->m_verts.m_data[base];

        for (uint32_t i = 0; i < batch.m_faceCount; i++) {
            out[0] = batch.m_vertexList[index[0]] * *batch.m_matrix;
            out[1] = batch.m_vertexList[index[1]] * *batch.m_matrix;
            out[2] = batch.m_vertexList[index[2]] * *batch.m_matrix;
            index += 3;
            out += 3;
        }
    }
}

// PROVEN: ab810557 IDENTICAL/NONE
// OFFSET: 0x00764D50
CWaterRipple* CWaterRipple::Create(const C3Vector& pos, float facing, float radius, float duration, float alpha, float growth, int32_t large, int32_t player) {
    uint32_t slot;

    if (player) {
        slot = CWaterRipple::s_nextPlayerRipple++;

        if (CWaterRipple::s_nextPlayerRipple >= 32) {
            CWaterRipple::s_nextPlayerRipple = 0;
        }
    } else {
        slot = CWaterRipple::s_nextRipple++;

        if (CWaterRipple::s_nextRipple >= 128) {
            CWaterRipple::s_nextRipple = 32;
        }
    }

    CWaterRipple* ripple = &CWaterRipple::s_ripples.m_data[slot];
    ripple->Init(pos, -facing, radius, alpha, duration, growth, large);
    return ripple;
}

// OFFSET: 0x00764DF0
void CWorldMap::CreateWaterRipple(const C3Vector& pos, float facing, float radius, float duration, float alpha, float growth, int32_t large, int32_t player) {
    float scale = 1.0f;

    if (alpha < 0.16666667f) {
        scale = alpha * 6.0f;
    }

    CWaterRipple* ripple = CWaterRipple::Create(pos, facing, radius, duration, scale, growth, large, player);
    this->m_rippleList.LinkToTail(ripple);
}

// OFFSET: 0x00764E70/0x14044A720
void CWorldMap::RenderWaterRipples(CWorldView* view) {
    if (!this->m_rippleList.Head()) {
        return;
    }

    uint32_t counts[2] = { 0, 0 };
    TSExplicitList<CWaterRipple, offsetof(CWaterRipple, m_renderLink)> lists[2];

    for (CWaterRipple* ripple = this->m_rippleList.Head(), * next; ripple; ripple = next) {
        next = this->m_rippleList.Next(ripple);

        if (FrameTime::GetCurTimeSec() < ripple->m_endTime) {
            ripple->m_radius = FrameTime::GetTickTimeSec() * ripple->m_growRate + ripple->m_radius;
            float dt = FrameTime::GetTickTimeSec();

            if (ripple->m_riseRate != 0.0f) {
                ripple->m_alpha = ripple->m_riseRate * dt + ripple->m_alpha;

                if (ripple->m_alpha > ripple->m_maxAlpha) {
                    dt = dt - (ripple->m_alpha - ripple->m_maxAlpha) / ripple->m_riseRate;
                    ripple->m_alpha = ripple->m_maxAlpha;
                    ripple->m_riseRate = 0.0f;
                }
            }

            if (ripple->m_riseRate == 0.0f) {
                ripple->m_alpha = ripple->m_fallRate * dt + ripple->m_alpha;
            }

            if (ripple->m_alpha > 0.0f) {
                counts[ripple->m_large] += ripple->m_verts.m_count;
                lists[ripple->m_large].LinkToTail(ripple);
                continue;
            }
        }

        ripple->m_mapLink.Unlink();
    }

    if (!counts[0] && !counts[1]) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_BlendingMode, GxBlend_Add);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_PolygonOffset, static_cast<int32_t>(GxPolygonOffset_Default));

    C44Matrix world;
    world.d0 = -view->m_camPos.x;
    world.d1 = -view->m_camPos.y;
    world.d2 = -view->m_camPos.z;

    if (CWaterRipple::s_vertexShader) {
        GxRsSet(GxRs_VertexShader, CWaterRipple::s_vertexShader, 0);
        GxRsSet(GxRs_PixelShader, CWaterRipple::s_pixelShader, 0);
        C44Matrix viewMatrix;
        GxXform(GxXform_View, viewMatrix);
        C44Matrix projection = g_theGxDevicePtr->m_projNative;

        if (g_theGxDevicePtr->DeviceApi() == GxApi_OpenGl) {
            projection.c0 = projection.c0 * -1.0f;
            projection.c1 = projection.c1 * -1.0f;
            projection.c2 = projection.c2 * -1.0f;
            projection.c3 = projection.c3 * -1.0f;
        }

        C44Matrix transform = world * viewMatrix * projection;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
    } else {
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    }

    C44Matrix texMatrix;
    C44Matrix fadeMatrix;

    for (uint32_t i = 0; i < 2; i++) {
        if (!counts[i]) {
            continue;
        }

        CGxTex* gxTex = TextureGetGxTex(CWaterRipple::s_textures[i]);

        if (!gxTex) {
            continue;
        }

        CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT), counts[i]);
        CGxVertexPCT* vertex = reinterpret_cast<CGxVertexPCT*>(g_theGxDevicePtr->BufLock(vertexBuf));
        CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), counts[i]);
        uint16_t* index = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
        uint16_t count = 0;

        for (CWaterRipple* ripple = lists[i].Head(), * next; ripple; ripple = next) {
            next = lists[i].Next(ripple);

            if (!ripple->m_verts.m_count) {
                continue;
            }

            C44Matrix rotation = C44Matrix::RotationAroundZ(ripple->m_facing);
            CAaBox box;
            box.b.x = ripple->m_position.x - ripple->m_radius;
            box.b.y = ripple->m_position.y - ripple->m_radius;
            box.b.z = ripple->m_position.z - 1.0f;
            box.t.x = ripple->m_position.x + ripple->m_radius;
            box.t.y = ripple->m_position.y + ripple->m_radius;
            box.t.z = ripple->m_position.z + 1.0f;
            ProjectTex2dMakeMatrices(texMatrix, fadeMatrix, box, NULL, &rotation, 0.5f, 1, 0);

            CImVector color;
            color.b = 0xFF;
            color.g = 0xFF;
            color.r = 0xFF;
            color.a = static_cast<uint8_t>(static_cast<int32_t>(ripple->m_alpha * 255.0f));
            const C3Vector* source = ripple->m_verts.m_data;

            for (uint32_t j = 0; j < ripple->m_verts.m_count; j++) {
                vertex->p = *source;
                vertex->c = color;
                vertex->tc = C2Vector(*source * texMatrix);
                *index++ = count++;
                vertex++;
                source++;
            }
        }

        g_theGxDevicePtr->BufUnlock(indexBuf, 0);
        indexBuf->unk_001C = 1;
        g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
        vertexBuf->unk_001C = 1;

        if (count) {
            g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PCT);
            g_theGxDevicePtr->PrimIndexPtr(indexBuf);
            g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);

            CGxBatch batch;
            batch.m_primType = GxPrim_Triangles;
            batch.m_start = 0;
            batch.m_count = count;
            batch.m_minIndex = 0;
            batch.m_maxIndex = count - 1;
            g_theGxDevicePtr->Draw(batch);
        }
    }

    g_theGxDevicePtr->RsPop();

    if (!CWaterRipple::s_vertexShader) {
        GxXformPop(GxXform_World);
    }
}

// OFFSET: 0x00765D20/0x14044B880
void CWaterRipple::Initialize() {
    CWaterRipple::s_nextPlayerRipple = 0;
    CWaterRipple::s_nextRipple = 32;
    CStatus status;
    CGxTexFlags flags;
    flags.m_filter = 1;

    for (uint32_t i = 0; i < 2; i++) {
        CWaterRipple::s_textures[i] = TextureCreate(s_rippleTextureNames[i], flags, &status, 1, NULL, 2);
    }

    CWaterRipple::s_ripples.SetCount(128);
    CWaterRipple::s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "WaterRipples");
    CWaterRipple::s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "WaterRipples");
}

// OFFSET: 0x00765E00
void CWaterRipple::Destroy() {
    for (uint32_t i = 0; i < 2; i++) {
        if (CWaterRipple::s_textures[i]) {
            TextureRelease(CWaterRipple::s_textures[i]);
        }
    }

    if (CWaterRipple::s_ripples.m_count) {
        CWaterRipple::s_ripples.Clear();
    }

    if (CWaterRipple::s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(CWaterRipple::s_vertexShader);
        CWaterRipple::s_vertexShader = NULL;
    }

    if (CWaterRipple::s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(CWaterRipple::s_pixelShader);
        CWaterRipple::s_pixelShader = NULL;
    }
}
