#include "WoW/Source/WorldClient/WorldMap.h"
#include <cstring>
#include <new>
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapObj.h"
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

// STATIC: 0x00E71E8C
uint32_t CWorldMap::s_activeWorldMapIndex;

// STATIC: 0x00E71E98
CM2Light CWorldMap::s_sunLight;

// STATIC: 0x00E71E90
CWorldMap* CWorldMap::s_worldMaps[2];

// STATIC: 0x00D00C38
float CWorldMap::s_oceanDepthScale = 0.58333331f;

// OFFSET: 0x0072C7D0
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

// OFFSET: 0x0072C9F0
void CWorldMap::Initialize() {
    CMap::Initialize();
    CWorldMap::InitializeLiquid();
    CWorldMap::FootprintInitialize();
    CWorldMap::s_worldMaps[0] = NULL;
    CWorldMap::s_worldMaps[1] = NULL;
    CWorldMap::s_activeWorldMapIndex = 0;
    CWorldMap::s_queryTag = 0;
}

// OFFSET: 0x0072CA20
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

// OFFSET: 0x0072CAA0
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

// OFFSET: 0x0072CAE0
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

// OFFSET: 0x0072CB60
void CWorldMap::RemoveOtherMaps() {
    for (uint32_t i = 0; i < 12; i++) {
        CMap* map = this->m_maps[i];

        if (map && map->m_mapId != this->m_mapId) {
            map->m_mapFlags = (map->m_mapFlags & ~(CMap::Flag_Current | CMap::Flag_Remove)) | CMap::Flag_Remove;
        }
    }
}

// OFFSET: 0x0072CC20
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

// OFFSET: 0x0072CCE0
void CWorldMap::RemoveMap(int32_t mapId) {
    for (uint32_t i = 1; i < 12; i++) {
        if (this->m_maps[i] && this->m_maps[i]->m_mapId == mapId) {
            CMap* map = this->m_maps[i];
            map->m_mapFlags = (map->m_mapFlags & ~(CMap::Flag_Current | CMap::Flag_Remove)) | CMap::Flag_Remove;
            return;
        }
    }
}

// OFFSET: 0x0072CD20
void CWorldMap::SetCurrent(int32_t mapId, int32_t current) {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i] && this->m_maps[i]->m_mapId == mapId) {
            CMap* map = this->m_maps[i];
            map->m_mapFlags ^= (map->m_mapFlags ^ (4 * (current != 0))) & CMap::Flag_Current;
            return;
        }
    }
}

// OFFSET: 0x0072D000
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

// OFFSET: 0x0072D1C0
void CWorldMap::UpdateEntityLinks(const CAaBox& box) {
    for (CMapEntity* entity = this->m_entityList.Head(); entity; entity = this->m_entityList.Next(entity)) {
        if (box.t.x >= entity->m_bbox.b.x && box.t.y >= entity->m_bbox.b.y && box.t.z >= entity->m_bbox.b.z && entity->m_bbox.t.x >= box.b.x && entity->m_bbox.t.y >= box.b.y && entity->m_bbox.t.z >= box.b.z) {
            entity->m_entityFlags |= 0x4000;
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

// OFFSET: 0x0072D9C0
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

    this->m_list00EC.UnlinkAll();
    CMapObj::ClearCache(1);
    CWorldScene::s_m2Scene->PurgeModelCache();
    TextureGxCacheFlush();
    DayNight::UnloadMap();
    CWorldMap::ClearAllFootprints();
    CWorldScene::OnMapUnload();
    this->DestroyLiquidPerturbators();
}

// OFFSET: 0x0072DC20
void CWorldMap::PurgeMaps() {
    for (uint32_t i = 0; i < 12; i++) {
        if (this->m_maps[i]) {
            this->m_maps[i]->Purge();
        }
    }

    this->m_list00EC.UnlinkAll();
    CMapObj::ClearCache(1);
    TextureGxCacheFlush();
}

// OFFSET: 0x0072EBF0
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

// OFFSET: 0x0072F350
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

// OFFSET: 0x00730200
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
    mapObjDef->unk_0154[0] = 0;
    mapObjDef->unk_0154[1] = 0;
    mapObjDef->unk_0154[2] = 0;

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
