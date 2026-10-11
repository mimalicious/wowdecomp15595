#include "WorldMap.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaPOIRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/DungeonMapChunkRec.h"
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/ResearchSiteRec.h"
#include "WoW/Source/DB/Rec/TaxiNodesRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/DB/Rec/WorldMapContinentRec.h"
#include "WoW/Source/DB/Rec/WorldMapOverlayRec.h"
#include "WoW/Source/DB/Rec/WorldMapTransformsRec.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Ui/Types.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00F33E48
uint32_t CGWorldMap::m_numLandmarks;

// STATIC: 0x00F33E4C
uint32_t CGWorldMap::m_numOverlays;

// STATIC: 0x00F33E50
CSimpleModel* CGWorldMap::m_playerArrowFrame;

// STATIC: 0x00F33E54
CSimpleModel* CGWorldMap::m_playerMiniArrowFrame;

// STATIC: 0x00F33E58
CSimpleModel* CGWorldMap::m_playerArrowEffectFrame;

// STATIC: 0x00F33E5C
CSimpleModel* CGWorldMap::m_playerMiniArrowEffectFrame;

// STATIC: 0x00D112B8
int32_t CGWorldMap::m_currentContinent = -1;

// STATIC: 0x00D112BC
int32_t CGWorldMap::m_currentZone = -1;

// STATIC: 0x00D112C0
int32_t CGWorldMap::m_currentDungeonMap = -1;

// STATIC: 0x00D112C4
int32_t CGWorldMap::m_currentOrphanZone = -1;

// STATIC: 0x00F33E70
TSFixedArray<WorldMapContinentInfo> CGWorldMap::m_continents;

// STATIC: 0x00F33E7C
TSGrowableArray<WorldMapLandmarkInfo> CGWorldMap::m_landmarks;

// STATIC: 0x00F33E8C
TSGrowableArray<int32_t> CGWorldMap::m_overlays;

// STATIC: 0x00F33E9C
TSFixedArray<int32_t> CGWorldMap::m_orphanZones;

// STATIC: 0x00F33E60
static int32_t s_chunkCacheMapID;

// STATIC: 0x00F33E64
static int32_t s_chunkCacheIndex;

// STATIC: 0x00D114C4
static int32_t s_preloadedAreaID = -1;

// STATIC: 0x00D114C8
static uint32_t s_taxiNodeZoneRadius = 1;

// OFFSET: 0x00919950
int32_t CGWorldMap::IsContinent(const WorldMapAreaRec* area) {
    if (!area) {
        return 0;
    }

    if (area->m_flags & 0x8) {
        return 1;
    }

    return area->m_areaID == 0;
}

// OFFSET: 0x00919980/0x14067A7F0
static void CenterArrowModel(CSimpleModel* model) {
    C3Vector position(model->GetWidth() * 0.5f, model->GetHeight() * 0.5f, 0.0f);
    model->m_position = position;
}

// OFFSET: 0x009199E0/0x14067A870
void CGWorldMap::CreateArrowFrame(CSimpleFrame* parent) {
    if (!parent) {
        return;
    }

    if (!CGWorldMap::m_playerArrowFrame) {
        void* data = CSimpleModel::s_allocator.GetData(0, typeid(CSimpleModel).raw_name(), SLOG_OBJECT);
        CGWorldMap::m_playerArrowFrame = data ? new (data) CSimpleModel(parent) : NULL;
        CGWorldMap::m_playerArrowFrame->SetModel("Interface\\Minimap\\MinimapArrow.mdx");
        CGWorldMap::m_playerArrowFrame->m_modelScale = NDCToDDCHeight(1.0f) * 1.6666666f;
        CGWorldMap::m_playerArrowFrame->SetName("PlayerArrowFrame");

        if (CGWorldMap::m_playerArrowFrame->m_model && CGWorldMap::m_playerArrowFrame->m_model->IsLoaded()) {
            CenterArrowModel(CGWorldMap::m_playerArrowFrame);
        }
    }

    if (!CGWorldMap::m_playerArrowEffectFrame) {
        void* data = CSimpleModel::s_allocator.GetData(0, typeid(CSimpleModel).raw_name(), SLOG_OBJECT);
        CGWorldMap::m_playerArrowEffectFrame = data ? new (data) CSimpleModel(parent) : NULL;
        CGWorldMap::m_playerArrowEffectFrame->SetModel("Interface\\Minimap\\MinimapArrow.mdx");
        CGWorldMap::m_playerArrowEffectFrame->m_modelScale = NDCToDDCHeight(1.0f) * 1.6666666f;
        CGWorldMap::m_playerArrowEffectFrame->SetName("PlayerArrowEffectFrame");

        if (CGWorldMap::m_playerArrowEffectFrame->m_model && CGWorldMap::m_playerArrowEffectFrame->m_model->IsLoaded()) {
            CenterArrowModel(CGWorldMap::m_playerArrowEffectFrame);
        }
    }
}

// OFFSET: 0x00919B40/0x14067AA30
void CGWorldMap::CreateMiniArrowFrame(CSimpleFrame* parent) {
    if (!parent) {
        return;
    }

    if (!CGWorldMap::m_playerMiniArrowFrame) {
        void* data = CSimpleModel::s_allocator.GetData(0, typeid(CSimpleModel).raw_name(), SLOG_OBJECT);
        CGWorldMap::m_playerMiniArrowFrame = data ? new (data) CSimpleModel(parent) : NULL;
        CGWorldMap::m_playerMiniArrowFrame->SetModel("Interface\\Minimap\\MinimapArrow.mdx");
        CGWorldMap::m_playerMiniArrowFrame->m_modelScale = NDCToDDCHeight(1.0f) * 1.1111112f;
        CGWorldMap::m_playerMiniArrowFrame->SetName("PlayerMiniArrowFrame");

        if (CGWorldMap::m_playerMiniArrowFrame->m_model && CGWorldMap::m_playerMiniArrowFrame->m_model->IsLoaded()) {
            CenterArrowModel(CGWorldMap::m_playerMiniArrowFrame);
        }
    }

    if (!CGWorldMap::m_playerMiniArrowEffectFrame) {
        void* data = CSimpleModel::s_allocator.GetData(0, typeid(CSimpleModel).raw_name(), SLOG_OBJECT);
        CGWorldMap::m_playerMiniArrowEffectFrame = data ? new (data) CSimpleModel(parent) : NULL;
        CGWorldMap::m_playerMiniArrowEffectFrame->SetModel("Interface\\Minimap\\MinimapArrow.mdx");
        CGWorldMap::m_playerMiniArrowEffectFrame->m_modelScale = NDCToDDCHeight(1.0f) * 1.1111112f;
        CGWorldMap::m_playerMiniArrowEffectFrame->SetName("PlayerMiniArrowEffectFrame");

        if (CGWorldMap::m_playerMiniArrowEffectFrame->m_model && CGWorldMap::m_playerMiniArrowEffectFrame->m_model->IsLoaded()) {
            CenterArrowModel(CGWorldMap::m_playerMiniArrowEffectFrame);
        }
    }
}

// PROVEN: 543cc20c IDENTICAL/NONE
// OFFSET: 0x00919CA0/0x14067ABF0
void CGWorldMap::ClearArrowFrames() {
    if (CGWorldMap::m_playerArrowFrame) {
        delete CGWorldMap::m_playerArrowFrame;
        CGWorldMap::m_playerArrowFrame = NULL;
    }

    if (CGWorldMap::m_playerArrowEffectFrame) {
        delete CGWorldMap::m_playerArrowEffectFrame;
        CGWorldMap::m_playerArrowEffectFrame = NULL;
    }

    if (CGWorldMap::m_playerMiniArrowFrame) {
        delete CGWorldMap::m_playerMiniArrowFrame;
        CGWorldMap::m_playerMiniArrowFrame = NULL;
    }

    if (CGWorldMap::m_playerMiniArrowEffectFrame) {
        delete CGWorldMap::m_playerMiniArrowEffectFrame;
        CGWorldMap::m_playerMiniArrowEffectFrame = NULL;
    }
}

// PROVEN: 8af60fb7 IDENTICAL/NONE
// OFFSET: 0x00919930
int32_t CGWorldMap::GetCurrentDungeonMap() {
    if (CGWorldMap::m_currentOrphanZone != -1) {
        return CGWorldMap::m_currentDungeonMap;
    }

    return -1;
}

// OFFSET: 0x0091A410/0x14067B510
uint32_t CGWorldMap::GetNumZones(uint32_t continent) {
    if (continent < CGWorldMap::m_continents.m_count) {
        return CGWorldMap::m_continents.m_data[continent].m_zones.m_count;
    }

    return 0;
}

// OFFSET: 0x0091A440/0x14067B530
int32_t CGWorldMap::GetCurrentAreaID() {
    if (CGWorldMap::m_currentContinent >= 0) {
        if (CGWorldMap::m_currentZone >= 0) {
            return CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_zones.m_data[CGWorldMap::m_currentZone];
        }

        return CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_areaID;
    }

    return CGWorldMap::m_currentOrphanZone;
}

// OFFSET: 0x0091A480/0x14067B570
static int32_t QSortZones(const void* a, const void* b) {
    WorldMapAreaRec* zoneA = g_worldMapAreaDB.GetRecord(*static_cast<const int32_t*>(a));
    WorldMapAreaRec* zoneB = g_worldMapAreaDB.GetRecord(*static_cast<const int32_t*>(b));

    if (zoneA && zoneB) {
        AreaTableRec* areaA = g_areaTableDB.GetRecord(zoneA->m_areaID);
        AreaTableRec* areaB = g_areaTableDB.GetRecord(zoneB->m_areaID);

        if (areaA && areaB) {
            return SStrSortUTF8I(areaA->m_areaName, areaB->m_areaName, 0x7FFFFFFF);
        }
    }

    return 0;
}

// OFFSET: 0x0091A530/0x14067B670
static int32_t QSortOrphanZones(const void* a, const void* b) {
    WorldMapAreaRec* zoneA = g_worldMapAreaDB.GetRecord(*static_cast<const int32_t*>(a));
    WorldMapAreaRec* zoneB = g_worldMapAreaDB.GetRecord(*static_cast<const int32_t*>(b));

    if (!zoneA || !zoneB) {
        return 0;
    }

    if (zoneA->m_mapID < zoneB->m_mapID) {
        return -1;
    }

    return zoneA->m_mapID > zoneB->m_mapID;
}

// OFFSET: 0x0091A5A0/0x14067B700
int32_t CGWorldMap::InitializeChunkZones(const WorldMapAreaRec* area, int32_t (&zoneMap)[128][128]) {
    if (area->m_flags & 0x8) {
        memset(zoneMap, 0, sizeof(zoneMap));
        return 1;
    }

    uint32_t numAreas = g_worldMapAreaDB.m_numRecords;
    char fileName[260];
    SStrPrintf(fileName, sizeof(fileName), "Interface\\WorldMap\\%s.zmp", static_cast<const char*>(area->m_areaName));
    SFileImpl* file;

    if (!SFileOpenFile(fileName, &file)) {
        return 0;
    }

    SFile::Read(file, zoneMap, sizeof(zoneMap), NULL);
    SFileCloseFile(file);

    for (uint32_t y = 0; y < 128; y++) {
        for (uint32_t x = 0; x < 128; x++) {
            int32_t areaID = zoneMap[y][x];
            zoneMap[y][x] = 0;

            if (!areaID) {
                continue;
            }

            AreaTableRec* areaRec = g_areaTableDB.GetRecord(areaID);

            if (!areaRec) {
                continue;
            }

            int32_t zoneID = 0;
            int32_t parentZoneID = 0;

            for (uint32_t i = 0; i < numAreas; i++) {
                WorldMapAreaRec* zone = g_worldMapAreaDB.GetRecordByIndex(i);

                if (zone->m_flags & 0x2) {
                    continue;
                }

                int32_t mapID = zone->m_displayMapID;

                if (mapID < 0) {
                    mapID = zone->m_mapID;
                }

                if (mapID != area->m_mapID) {
                    continue;
                }

                if (zone->m_areaID == areaID) {
                    zoneID = zone->m_id;
                    break;
                }

                if (areaRec->m_parentAreaID && zone->m_areaID == areaRec->m_parentAreaID) {
                    parentZoneID = zone->m_id;
                }
            }

            if (!zoneID) {
                zoneID = parentZoneID;
            }

            zoneMap[y][x] = zoneID;
        }
    }

    return 1;
}

// OFFSET: 0x0091A720/0x14067B910
const char* CGWorldMap::GetContinentName(uint32_t continent) {
    if (continent >= CGWorldMap::m_continents.m_count) {
        return NULL;
    }

    WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[continent];
    MapRec* map = g_mapDB.GetRecord(entry.m_mapID);

    if (map) {
        return map->m_mapName;
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(entry.m_areaID);
    AreaTableRec* areaRec;

    if (area && (areaRec = g_areaTableDB.GetRecord(area->m_areaID))) {
        return areaRec->m_areaName;
    }

    return NULL;
}

// OFFSET: 0x0091A7C0
int32_t CGWorldMap::GetZoneID(uint32_t continent, uint32_t zone) {
    if (continent >= CGWorldMap::m_continents.m_count) {
        return -1;
    }

    WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[continent];

    if (zone >= entry.m_zones.m_count) {
        return -1;
    }

    return entry.m_zones.m_data[zone];
}

// OFFSET: 0x0091A800/0x14067B9D0
const char* CGWorldMap::GetZoneName(uint32_t continent, uint32_t zone) {
    if (continent < CGWorldMap::m_continents.m_count) {
        WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[continent];

        if (zone >= entry.m_zones.m_count) {
            return NULL;
        }

        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(entry.m_zones.m_data[zone]);
        AreaTableRec* areaRec = area ? g_areaTableDB.GetRecord(area->m_areaID) : NULL;

        if (!areaRec) {
            return NULL;
        }

        return areaRec->m_areaName;
    }

    return NULL;
}

// OFFSET: 0x0091A880/0x14067BA70
bool CGWorldMap::DetermineAlternateMapPosition(int32_t mapID, int32_t areaID, float x, float y, int32_t& newMapID, int32_t& newAreaID, float& newX, float& newY, int32_t& dungeonMapID, bool& forceDungeonMap) {
    newX = x;
    newY = y;
    newMapID = mapID;
    newAreaID = areaID;
    dungeonMapID = 0;
    forceDungeonMap = false;

    for (int32_t i = 0; i < g_worldMapTransformsDB.m_numRecords; i++) {
        WorldMapTransformsRec* transform = g_worldMapTransformsDB.GetRecordByIndex(i);

        if (transform && mapID == transform->m_mapID && x >= transform->m_regionMin[0] && transform->m_regionMax[0] >= x && y >= transform->m_regionMin[1] && transform->m_regionMax[1] >= y) {
            newMapID = transform->m_newMapID;

            if (transform->unk_002C) {
                newAreaID = transform->unk_002C;
            }

            newX = transform->m_regionOffset[0] + newX;
            newY = transform->m_regionOffset[1] + newY;
            dungeonMapID = transform->m_newDungeonMapID;
            forceDungeonMap = transform->unk_0028 & 0x1;
            return true;
        }

        continue;
    }

    return false;
}

// OFFSET: 0x0091A960/0x14067BB70
int32_t CGWorldMap::GetDungeonMapFromChunk(int32_t mapID, int32_t wmoGroupID, float z, int32_t worldMapAreaID) {
    int32_t start = 0;

    if (mapID <= 0 || wmoGroupID <= 0) {
        return -1;
    }

    bool foundMap = false;
    int32_t dungeonMap = -1;
    bool foundGroup = false;

    if (s_chunkCacheMapID == mapID) {
        start = s_chunkCacheIndex;
    } else {
        s_chunkCacheMapID = mapID;
        s_chunkCacheIndex = 0;
    }

    for (uint32_t i = start; i < static_cast<uint32_t>(g_dungeonMapChunkDB.m_numRecords); i++) {
        DungeonMapChunkRec* rec = g_dungeonMapChunkDB.GetRecordByIndex(i);

        if (rec->m_mapID != mapID) {
            if (foundMap) {
                break;
            }
        } else {
            if (!foundMap) {
                foundMap = true;
                s_chunkCacheIndex = i;
            }

            if (rec->m_wmogroupID != wmoGroupID) {
                if (foundGroup) {
                    break;
                }
            } else {
                foundGroup = true;

                if (z >= rec->m_minZ) {
                    dungeonMap = rec->m_dungeonMapID;
                }
            }
        }
    }

    if (foundMap && !foundGroup && worldMapAreaID >= 0) {
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(worldMapAreaID);

        if (area && !(area->m_flags & 0x100)) {
            int32_t floor = area->m_defaultDungeonFloor;

            for (uint32_t i = 0; i < static_cast<uint32_t>(g_dungeonMapDB.m_numRecords); i++) {
                DungeonMapRec* rec = g_dungeonMapDB.GetRecordByIndex(i);

                if (rec->m_mapID == mapID) {
                    if (floor > 0) {
                        if (floor == rec->m_id) {
                            return rec->m_id;
                        }
                    } else if (rec->m_floorIndex == 1) {
                        return rec->m_id;
                    }
                }
            }
        }
    }

    return dungeonMap;
}

// OFFSET: 0x0091AC70/0x14067BE70
int32_t CGWorldMap::GetWorldMapAreaByZone(int32_t continent, uint32_t y, uint32_t x) {
    for (uint32_t i = 0; i < CGWorldMap::m_continents.m_data[continent].m_phasedChunks.m_count; i++) {
        if (CGWorldMap::m_continents.m_data[continent].m_phasedChunks.m_data[i].m_areaID && PhaseShift_C_IsPhasedWorldMapActive(CGWorldMap::m_continents.m_data[continent].m_phasedChunks.m_data[i].m_areaID)) {
            int32_t area = CGWorldMap::m_continents.m_data[continent].m_phasedChunks.m_data[i].m_zoneMap[y][x];

            if (area) {
                return area;
            }
        }
    }

    return CGWorldMap::m_continents.m_data[continent].m_zoneMap[y][x];
}

// OFFSET: 0x0091AD20/0x14067BF50
static int32_t FindFirstDungeonMap(int32_t mapID) {
    int32_t low = 0;
    int32_t high = g_dungeonMapDB.m_numRecords - 1;
    int32_t mid;

    for (;;) {
        if (low > high) {
            return -1;
        }

        mid = (high + low) >> 1;
        DungeonMapRec* rec = g_dungeonMapDB.GetRecordByIndex(mid);

        if (mapID == rec->m_mapID) {
            break;
        }

        if (mapID > rec->m_mapID) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    DungeonMapRec* rec = g_dungeonMapDB.GetRecordByIndex(mid);

    while (mid >= 0 && mapID == rec->m_mapID) {
        mid--;
        rec = g_dungeonMapDB.GetRecordByIndex(mid);
    }

    return mid + 1;
}

// OFFSET: 0x0091ADD0/0x14067C000
static int32_t FindFirstOrphanZone(int32_t mapID) {
    int32_t low = 0;
    int32_t high = CGWorldMap::m_orphanZones.m_count - 1;
    int32_t mid;

    for (;;) {
        if (low > high) {
            return -1;
        }

        mid = (high + low) >> 1;
        WorldMapAreaRec* rec = g_worldMapAreaDB.GetRecord(CGWorldMap::m_orphanZones.m_data[mid]);

        if (mapID == rec->m_mapID) {
            break;
        }

        if (mapID > rec->m_mapID) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    WorldMapAreaRec* rec = g_worldMapAreaDB.GetRecord(CGWorldMap::m_orphanZones.m_data[mid]);

    while (mid >= 0 && rec->m_mapID == mapID) {
        mid--;
        rec = g_worldMapAreaDB.GetRecordByIndex(mid);
    }

    return mid + 1;
}

// OFFSET: 0x0091AEB0/0x14067C0F0
const char* CGWorldMap::GetMapName(int32_t areaID) {
    const WorldMapAreaRec* area;

    if (areaID != -1 && (area = PhaseShift_C_GetPhasedWorldMap(g_worldMapAreaDB.GetRecord(areaID)))) {
        return area->m_areaName;
    }

    return NULL;
}

// OFFSET: 0x0091AF00/0x14067C160
int32_t CGWorldMap::GetMapHeight() {
    int32_t areaID = CGWorldMap::GetCurrentAreaID();

    if (areaID == -1) {
        return 0;
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(areaID);

    if (!area) {
        return 0;
    }

    DungeonMapRec* dungeonRec = g_dungeonMapDB.GetRecord(CGWorldMap::m_currentOrphanZone == -1 ? -1 : CGWorldMap::m_currentDungeonMap);
    float width;
    float height;

    if (dungeonRec) {
        width = dungeonRec->m_maxX - dungeonRec->m_minX;
        height = dungeonRec->m_maxY - dungeonRec->m_minY;
    } else if (CGWorldMap::m_currentZone == -1) {
        WorldMapContinentRec* continentRec = NULL;

        for (uint32_t i = 0; i < static_cast<uint32_t>(g_worldMapContinentDB.m_numRecords); i++) {
            WorldMapContinentRec* rec = g_worldMapContinentDB.GetRecordByIndex(i);

            if (rec->m_mapID == area->m_mapID) {
                continentRec = rec;
                break;
            }
        }

        if (!continentRec) {
            return 0;
        }

        width = static_cast<float>(continentRec->m_rightBoundary - continentRec->m_leftBoundary + 1);
        height = static_cast<float>(continentRec->m_bottomBoundary - continentRec->m_topBoundary + 1);
    } else {
        width = area->m_locLeft - area->m_locRight;
        height = area->m_locTop - area->m_locBottom;
    }

    if (width == 0.0f || height == 0.0f) {
        return 0;
    }

    return static_cast<uint32_t>(height * 1002.0 / width);
}

// OFFSET: 0x0091B070
WorldMapContinentRec* CGWorldMap::GetContinentRec(int32_t mapID) {
    if (mapID == -1) {
        return NULL;
    }

    for (uint32_t i = g_worldMapContinentDB.m_numRecords; i; ) {
        i--;
        WorldMapContinentRec* rec = g_worldMapContinentDB.GetRecordByIndex(i);

        if (rec->m_mapID == mapID) {
            return rec;
        }
    }

    return NULL;
}

// OFFSET: 0x0091B0C0
bool CGWorldMap::GetContinentPosition(float x, float y, int32_t mapID, float* mapX, float* mapY) {
    WorldMapContinentRec* rec = CGWorldMap::GetContinentRec(mapID);

    if (!rec) {
        return false;
    }

    float top = rec->m_continentOffset[1] * 0.023952097f + 0.5f - x * 0.000044910183f * rec->m_scale;
    *mapX = rec->m_continentOffset[0] * 0.015968064f + 0.5f - y * 0.00002994012f * rec->m_scale;
    *mapY = 1.0f - top;
    return true;
}

// OFFSET: 0x0091B150/0x14067C380
bool CGWorldMap::IsMapOnContinent(int32_t mapID, uint32_t continent) {
    WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[continent];

    if (entry.m_mapID == -1) {
        for (uint32_t i = 0; i < entry.m_zones.m_count; i++) {
            WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(entry.m_zones.m_data[i]);

            if (area && area->m_mapID == mapID) {
                return true;
            }
        }
    } else {
        if (continent < CGWorldMap::m_continents.m_count && entry.m_mapID == mapID) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x0091B1E0/0x14067C430
int32_t CGWorldMap::GetWorldMapChildDepth(int32_t parent, int32_t child, int32_t* childArea) {
    if (parent == child) {
        return 0;
    }

    int32_t parentAreaID = 0;

    if (parent >= 0) {
        WorldMapAreaRec* rec = g_worldMapAreaDB.GetRecord(parent);

        if (!rec) {
            return -1;
        }

        parentAreaID = rec->m_areaID;
    }

    WorldMapAreaRec* childRec = g_worldMapAreaDB.GetRecord(child);
    AreaTableRec* area = childRec && childRec->m_areaID ? g_areaTableDB.GetRecord(childRec->m_areaID) : NULL;
    int32_t depth = 1;

    for (;;) {
        if (!area) {
            return -1;
        }

        if (area->m_parentAreaID == parentAreaID) {
            break;
        }

        area = g_areaTableDB.GetRecord(area->m_parentAreaID);
        depth++;
    }

    if (childArea) {
        for (int32_t i = 0; i < g_worldMapAreaDB.m_numRecords; i++) {
            WorldMapAreaRec* rec = g_worldMapAreaDB.GetRecordByIndex(i);

            if (rec && rec->m_areaID == area->m_id) {
                *childArea = rec->m_id;
                break;
            }
        }
    }

    return depth;
}

// OFFSET: 0x0091B300/0x14067C590
void CGWorldMap::UpdateArrowFrames() {
    if (CGWorldMap::m_playerArrowFrame && CGWorldMap::m_playerArrowFrame->m_model && CGWorldMap::m_playerArrowFrame->m_model->IsLoaded()) {
        CenterArrowModel(CGWorldMap::m_playerArrowFrame);
    }

    if (CGWorldMap::m_playerArrowEffectFrame && CGWorldMap::m_playerArrowEffectFrame->m_model && CGWorldMap::m_playerArrowEffectFrame->m_model->IsLoaded()) {
        CenterArrowModel(CGWorldMap::m_playerArrowEffectFrame);
    }

    if (CGWorldMap::m_playerMiniArrowFrame && CGWorldMap::m_playerMiniArrowFrame->m_model && CGWorldMap::m_playerMiniArrowFrame->m_model->IsLoaded()) {
        CenterArrowModel(CGWorldMap::m_playerMiniArrowFrame);
    }

    if (CGWorldMap::m_playerMiniArrowEffectFrame && CGWorldMap::m_playerMiniArrowEffectFrame->m_model && CGWorldMap::m_playerMiniArrowEffectFrame->m_model->IsLoaded()) {
        CenterArrowModel(CGWorldMap::m_playerMiniArrowEffectFrame);
    }

    CGObject_C* target = ClntObjMgrObjectPtr(CGWorldFrame::GetActiveCamera()->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!target) {
        return;
    }

    float facing = target->GetFacing();

    if (CGWorldMap::m_playerArrowFrame) {
        CGWorldMap::m_playerArrowFrame->m_facing = facing;
    }

    if (CGWorldMap::m_playerArrowEffectFrame) {
        CGWorldMap::m_playerArrowEffectFrame->m_facing = facing;
    }

    if (CGWorldMap::m_playerMiniArrowFrame) {
        CGWorldMap::m_playerMiniArrowFrame->m_facing = facing;
    }

    if (CGWorldMap::m_playerMiniArrowEffectFrame) {
        CGWorldMap::m_playerMiniArrowEffectFrame->m_facing = facing;
    }
}

// OFFSET: 0x0091B420/0x14067C6D0
int32_t CGWorldMap::GetCurrentDungeonFloor(DungeonMapRec** dungeonMap) {
    DungeonMapRec* rec = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());
    int32_t floor = rec ? rec->m_floorIndex : 0;
    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::m_currentOrphanZone);

    if (area && ((area->m_flags & 0x4) || area->m_defaultDungeonFloor == -1)) {
        floor++;
    }

    if (dungeonMap) {
        *dungeonMap = rec;
    }

    return floor;
}

// OFFSET: 0x0091B4A0/0x14067C770
WorldMapOverlayRec* GetMapOverlayTexture(uint32_t index, char* texture, uint32_t size) {
    int32_t overlayID = index >= CGWorldMap::m_numOverlays ? 0 : CGWorldMap::m_overlays.m_data[index];
    WorldMapOverlayRec* overlay = g_worldMapOverlayDB.GetRecord(overlayID);

    if (!overlay) {
        return overlay;
    }

    const char* name = overlay->m_textureName;

    if (!name || !*name) {
        *texture = '\0';
        return overlay;
    }

    const WorldMapAreaRec* area = PhaseShift_C_GetPhasedWorldMap(g_worldMapAreaDB.GetRecord(overlay->m_mapAreaID));

    if (area) {
        SStrPrintf(texture, size, "Interface\\WorldMap\\%s\\%s", static_cast<const char*>(area->m_areaName), name);
    } else {
        SStrPrintf(texture, size, "Interface\\WorldMap\\World\\%s", name);
    }

    return overlay;
}

// OFFSET: 0x0091B560/0x14067C8E0
void CGWorldMap::PreloadMapTileTextures() {
    int32_t areaID = CGWorldMap::GetCurrentAreaID();

    if (areaID < 0 || areaID == s_preloadedAreaID) {
        return;
    }

    s_preloadedAreaID = areaID;
    const char* name = CGWorldMap::GetMapName(areaID);

    if (!name) {
        return;
    }

    char fileName[260];
    SStrCopy(fileName, name, sizeof(fileName));
    int32_t floor = CGWorldMap::GetCurrentDungeonFloor(NULL);

    for (uint32_t i = 1; i <= 12; i++) {
        if (floor > 0) {
            SStrPrintf(fileName, sizeof(fileName), "Interface\\WorldMap\\%s\\%s%d_%d.blp", name, name, floor, i);
        } else {
            SStrPrintf(fileName, sizeof(fileName), "Interface\\WorldMap\\%s\\%s%d.blp", name, name, i);
        }

        AsyncFileReadCreateObject(fileName, 5, 0, 0, NULL, NULL, NULL, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, 0, 0);
    }
}

// OFFSET: 0x0091B680/0x14067CAA0
void CGWorldMap::PreloadMapOverlayTextures() {
    for (uint32_t i = 0; i < CGWorldMap::m_numOverlays; i++) {
        char texture[260];
        WorldMapOverlayRec* overlay = GetMapOverlayTexture(i, texture, sizeof(texture));

        if (!overlay || !texture[0]) {
            continue;
        }

        uint32_t columns = (overlay->m_textureWidth + 255) / 256;
        uint32_t rows = (overlay->m_textureHeight + 255) / 256;
        int32_t index = 1;

        for (int32_t row = 0; row < rows; row++) {
            for (int32_t column = 0; column < columns; column++) {
                char fileName[260];
                SStrPrintf(fileName, sizeof(fileName), "%s%d.blp", texture, index + column);
                AsyncFileReadCreateObject(fileName, 5, 0, 0, NULL, NULL, NULL, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, 0, 0);
            }

            index += columns;
        }
    }
}

// OFFSET: 0x0091C950/0x14067E1B0
bool CGWorldMap::DetermineAlternateMapPosition(int32_t mapID, float x, float y, int32_t& newMapID, float& newX, float& newY, int32_t& dungeonMapID, bool& forceDungeonMap) {
    int32_t newAreaID;
    return CGWorldMap::DetermineAlternateMapPosition(mapID, 0, x, y, newMapID, newAreaID, newX, newY, dungeonMapID, forceDungeonMap);
}

// OFFSET: 0x0091C990/0x14067E210
void CGWorldMap::GetMapForPosition(int32_t mapID, int32_t areaID, int32_t wmoGroupID, const C3Vector& position, int32_t& continent, int32_t& zone, int32_t& dungeonMap, int32_t debug) {
    continent = -1;
    zone = -1;
    dungeonMap = -1;
    float y = position.y;
    float x = position.x;
    int32_t newMapID;
    int32_t newAreaID;
    float newX;
    float newY;
    int32_t newDungeonMap;
    bool forceDungeonMap;
    bool transformed = CGWorldMap::DetermineAlternateMapPosition(mapID, areaID, x, y, newMapID, newAreaID, newX, newY, newDungeonMap, forceDungeonMap);
    uint32_t current = -1;

    for (uint32_t i = 0; i < CGWorldMap::m_continents.m_count; i++) {
        if (CGWorldMap::IsMapOnContinent(newMapID, i)) {
            current = i;
            break;
        }
    }

    char text[256];

    if (debug) {
        if (transformed && !newDungeonMap && !(mapID == newMapID && x == newX && y == newY)) {
            SStrPrintf(text, sizeof(text) - 1, "Player at (%03d, %5.2f, %5.2f, %5.2f), alternate position is (%03d, %5.2f, %5.2f, %5.2f)", mapID, x, y, position.z, newMapID, newX, newY, position.z);
        } else {
            SStrPrintf(text, sizeof(text) - 1, "Player at (%03d, %5.2f, %5.2f, %5.2f)", mapID, x, y, position.z);
        }
    }

    if (current == static_cast<uint32_t>(-1)) {
        float bestSize = FLT_MAX;
        int32_t bestArea = 0;
        int32_t firstArea = 0;

        for (uint32_t i = 0; i < CGWorldMap::m_orphanZones.m_count; i++) {
            WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::m_orphanZones.m_data[i]);

            if (!area) {
                continue;
            }

            if (area->m_mapID != newMapID) {
                continue;
            }

            if (!firstArea) {
                firstArea = area->m_id;
            }

            int32_t parentArea = newAreaID;

            if (!parentArea) {
                continue;
            }

            bool match = true;

            while (parentArea != area->m_areaID) {
                AreaTableRec* areaRec = g_areaTableDB.GetRecord(parentArea);

                if (!areaRec || !(parentArea = areaRec->m_parentAreaID)) {
                    match = false;
                    break;
                }
            }

            if (match && area->m_locTop >= newX && newX >= area->m_locBottom && area->m_locLeft >= newY && newY >= area->m_locRight) {
                float size = (area->m_locLeft - area->m_locRight) * (area->m_locTop - area->m_locBottom);

                if (bestSize > size) {
                    bestSize = size;
                    bestArea = area->m_id;
                }
            }
        }

        if (firstArea) {
            WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(bestArea ? bestArea : firstArea);
            int32_t areaDungeonMap = CGWorldMap::GetDungeonMapFromChunk(area->m_mapID, wmoGroupID, position.z, firstArea);

            if (forceDungeonMap && newDungeonMap) {
                areaDungeonMap = newDungeonMap;

                if (debug) {
                    SStrPrintf(text, sizeof(text) - 1, "%s, dungeon map found, but forcing override dungeon map", text);
                }
            } else if (areaDungeonMap > 0) {
                if (debug) {
                    SStrPrintf(text, sizeof(text) - 1, "%s, found", text);
                }
            } else if (newDungeonMap) {
                areaDungeonMap = newDungeonMap;

                if (debug) {
                    SStrPrintf(text, sizeof(text) - 1, "%s, no dungeon found, using override dungeon map", text);
                }
            } else if (debug) {
                SStrPrintf(text, sizeof(text) - 1, "%s, NO MAP FOUND (will use terrain)", text);
            }

            if (debug) {
                SStrPrintf(text, sizeof(text) - 1, "%s, Dungeon Map Info:  WMOGroupID = %d  DMapID = %d  WorldMapAreaRecID = %d  MapID = %d", text, wmoGroupID, areaDungeonMap, area->m_id, area->m_mapID);
                CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
                ConsolePrintf("%s\n", text);
            }

            continent = -2;
            zone = area->m_id;
            dungeonMap = areaDungeonMap;
            return;
        }

        MapRec* map = g_mapDB.GetRecord(newMapID);

        if (map && (fabs(map->m_corpse[0]) >= 0.00000023841858f || fabs(map->m_corpse[1]) >= 0.00000023841858f)) {
            int32_t corpseAreaID;
            CGWorldMap::DetermineAlternateMapPosition(map->m_corpseMapID, 0, map->m_corpse[0], map->m_corpse[1], newMapID, corpseAreaID, newX, newY, newDungeonMap, forceDungeonMap);

            for (uint32_t i = 0; i < CGWorldMap::m_continents.m_count; i++) {
                if (CGWorldMap::IsMapOnContinent(newMapID, i)) {
                    current = i;
                    break;
                }
            }
        }

        if (current == static_cast<uint32_t>(-1)) {
            if (debug) {
                SStrPrintf(text, sizeof(text) - 1, "%s, True Orphan:  Continent = -1", text);
                CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
                ConsolePrintf("%s\n", text);
            }

            continent = -2;
            zone = -1;
            dungeonMap = -1;
            return;
        }

        float gridX = 0.5f - newX * 0.000029296876f;
        float gridY = 0.5f - newY * 0.000029296876f;

        if (gridX == (gridX >= 1.0f ? 1.0f : gridX >= 0.0f ? gridX : 0.0f) && gridY == (gridY >= 1.0f ? 1.0f : gridY >= 0.0f ? gridY : 0.0f)) {
            int32_t zoneArea = CGWorldMap::GetWorldMapAreaByZone(current, static_cast<uint32_t>(128.0 * gridX), static_cast<uint32_t>(gridY * 128.0));
            uint32_t numZones = CGWorldMap::GetNumZones(current);

            for (uint32_t i = 0; i < numZones; i++) {
                if (CGWorldMap::m_continents.m_data[current].m_zones.m_data[i] != zoneArea) {
                    continue;
                }

                if (debug) {
                    SStrPrintf(text, sizeof(text) - 1, "%s Inside Instance:  Continent = %02d  MapID = %03d", text, current, i);
                    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
                    ConsolePrintf("%s\n", text);
                }

                continent = current;
                zone = i;
                dungeonMap = -1;
                return;
            }
        }
    }

    WorldMapContinentInfo* entry = &CGWorldMap::m_continents.m_data[current];
    int32_t currentZone = -1;

    for (uint32_t i = 0; i < entry->m_zones.m_count; i++) {
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(entry->m_zones.m_data[i]);

        if (!area || area->m_areaID != newAreaID) {
            continue;
        }

        bool usesTerrain;

        if ((area->m_flags & 0x4) || area->m_defaultDungeonFloor == -1) {
            usesTerrain = true;
        } else {
            usesTerrain = false;

            if (fabs(area->m_locLeft) >= 0.00000023841858f || fabs(area->m_locRight) >= 0.00000023841858f || fabs(area->m_locTop) >= 0.00000023841858f || fabs(area->m_locBottom) >= 0.00000023841858f) {
                currentZone = i;
                break;
            }
        }

        int32_t areaDungeonMap = CGWorldMap::GetDungeonMapFromChunk(area->m_mapID, wmoGroupID, position.z, -1);

        if (areaDungeonMap >= 0) {
            if (debug) {
                SStrPrintf(text, sizeof(text) - 1, "%s, Dungeon Map Info (continent):  WMOID = %d  DMapID = %d  DMapRec = %d  z = %5.2f", text, wmoGroupID, areaDungeonMap, area->m_id, position.z);
                CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
                ConsolePrintf("%s\n", text);
            }

            continent = -2;
            zone = area->m_id;
            dungeonMap = areaDungeonMap;
            return;
        }

        if (usesTerrain || !area->m_parentWorldMapID) {
            currentZone = i;
            break;
        }

        for (uint32_t j = 0; j < entry->m_zones.m_count; j++) {
            if (entry->m_zones.m_data[j] == area->m_parentWorldMapID) {
                currentZone = j;
                break;
            }
        }
    }

    if (currentZone == -1) {
        AreaTableRec* areaRec = g_areaTableDB.GetRecord(areaID);

        if (areaRec) {
            int32_t worldMapAreaID = 0;
            int32_t areaMapID = areaRec->m_continentID;
            int32_t index = FindFirstDungeonMap(areaMapID);
            bool found = false;

            if (index != -1) {
                for (; index < g_dungeonMapDB.m_numRecords; index++) {
                    DungeonMapRec* rec = g_dungeonMapDB.GetRecordByIndex(index);

                    if (rec->m_mapID != areaMapID) {
                        break;
                    }

                    WorldMapAreaRec* parent;

                    if (rec->m_parentWorldMapID && (parent = g_worldMapAreaDB.GetRecord(rec->m_parentWorldMapID)) && parent->m_mapID == newMapID) {
                        worldMapAreaID = parent->m_id;

                        if (worldMapAreaID) {
                            found = true;
                        }

                        break;
                    }
                }
            }

            if (!found) {
                index = FindFirstOrphanZone(areaMapID);

                if (index != -1) {
                    for (; index < static_cast<int32_t>(CGWorldMap::m_orphanZones.m_count); index++) {
                        WorldMapAreaRec* orphan = g_worldMapAreaDB.GetRecord(CGWorldMap::m_orphanZones.m_data[index]);

                        if (orphan->m_mapID != areaMapID) {
                            break;
                        }

                        if (orphan->m_areaID != areaRec->m_id && orphan->m_areaID != areaRec->m_parentAreaID) {
                            continue;
                        }

                        WorldMapAreaRec* parent = g_worldMapAreaDB.GetRecord(orphan->m_parentWorldMapID);

                        if (parent && parent->m_mapID == newMapID) {
                            worldMapAreaID = parent->m_id;
                            break;
                        }
                    }
                }
            }

            WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(worldMapAreaID);

            if (area) {
                for (uint32_t i = 0; i < CGWorldMap::m_continents.m_count; i++) {
                    if (CGWorldMap::IsMapOnContinent(area->m_mapID, i)) {
                        current = i;
                        break;
                    }
                }

                if (current != static_cast<uint32_t>(-1)) {
                    WorldMapContinentInfo* areaEntry = &CGWorldMap::m_continents.m_data[current];

                    for (uint32_t i = 0; i < areaEntry->m_zones.m_count; i++) {
                        if (areaEntry->m_zones.m_data[i] == area->m_id) {
                            continent = current;
                            zone = i;
                            dungeonMap = -1;
                            return;
                        }
                    }

                    current = -1;
                }
            }
        }
    }

    if (debug) {
        SStrPrintf(text, sizeof(text) - 1, "%s, Normal Map:  Continent = %02d  MapID = %03d", text, current, currentZone);
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
        ConsolePrintf("%s\n", text);
    }

    continent = current;
    zone = currentZone;
    dungeonMap = -1;
}

// OFFSET: 0x0091D440/0x14067EE50
int32_t CGWorldMap::GetNumDungeonLevels() {
    int32_t count = 0;
    uint32_t floors = 0;

    if (CGWorldMap::m_currentOrphanZone >= 0) {
        if (CGWorldMap::m_currentContinent >= 0) {
            WorldMapContinentInfo& continent = CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent];
            int32_t zone = CGWorldMap::m_currentZone >= 0 ? continent.m_zones.m_data[CGWorldMap::m_currentZone] : continent.m_areaID;

            if (zone != CGWorldMap::m_currentOrphanZone) {
                return 0;
            }
        }

        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::m_currentOrphanZone);

        if (!area) {
            return 0;
        }

        for (uint32_t i = 0; i < static_cast<uint32_t>(g_dungeonMapDB.m_numRecords); i++) {
            DungeonMapRec* rec = g_dungeonMapDB.GetRecordByIndex(i);

            if (rec->m_mapID != area->m_mapID) {
                continue;
            }

            uint32_t bit = 1 << rec->m_floorIndex;

            if (!(bit & floors)) {
                floors |= bit;
                count++;
            }
        }

        if (area && ((area->m_flags & 0x4) || area->m_defaultDungeonFloor == -1)) {
            count++;
        }

        return count;
    }

    return 0;
}

// OFFSET: 0x0091D540/0x14067EF80
int32_t CGWorldMap::GetChunkFromPos(float x, float y, uint32_t* chunkX, uint32_t* chunkY) {
    int32_t areaID = CGWorldMap::GetCurrentAreaID();

    if (areaID == -1 || CGWorldMap::m_currentContinent == -2) {
        return 0;
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(areaID);

    if (!area) {
        return 0;
    }

    int32_t mapID = area->m_mapID;
    int32_t newMapID;
    int32_t newAreaID;
    int32_t dungeonMapID;
    bool forceDungeonMap;
    float topX;
    float topY;
    float bottomX;
    float bottomY;
    CGWorldMap::DetermineAlternateMapPosition(mapID, 0, area->m_locTop, area->m_locLeft, newMapID, newAreaID, topX, topY, dungeonMapID, forceDungeonMap);
    CGWorldMap::DetermineAlternateMapPosition(mapID, 0, area->m_locBottom, area->m_locRight, newMapID, newAreaID, bottomX, bottomY, dungeonMapID, forceDungeonMap);
    float clampedX = x >= 1.0f ? 1.0f : x < 0.0f ? 0.0f : x;
    float clampedY = y >= 1.0f ? 1.0f : y < 0.0f ? 0.0f : y;
    float rangeX = topX - bottomX;
    float rangeY = topY - bottomY;

    if (rangeY == 0.0f || rangeX == 0.0f) {
        return 0;
    }

    float gridX = 0.5f - ((1.0f - clampedX) * rangeY + bottomY) * 0.000029296876f;
    float gridY = 0.5f - ((1.0f - clampedY) * rangeX + bottomX) * 0.000029296876f;

    if (gridX < 0.0f || gridX > 1.0f || gridY < 0.0f || gridY > 1.0f) {
        return 0;
    }

    *chunkX = static_cast<uint32_t>(gridX * 128.0);
    *chunkY = static_cast<uint32_t>(128.0 * gridY);
    return 1;
}

// OFFSET: 0x0091D760
bool CGWorldMap::GetWorldPosition(const C3Vector& position, int32_t mapID, int32_t wmoGroupID, float* x, float* y, bool* outland, bool noClamp, int32_t dungeonMapID) {
    *x = 0.0f;
    *y = 0.0f;
    C2Vector transformed(0.0f, 0.0f);

    if (outland) {
        *outland = false;
    }

    int32_t newMapID;
    int32_t newAreaID;
    int32_t transformedDungeonMap;
    bool forceDungeonMap;
    CGWorldMap::DetermineAlternateMapPosition(mapID, 0, position.x, position.y, newMapID, newAreaID, transformed.x, transformed.y, transformedDungeonMap, forceDungeonMap);
    WorldMapContinentRec* continentRec = CGWorldMap::GetContinentRec(newMapID);

    if (continentRec && CGWorldMap::m_currentContinent == -1) {
        if (outland && newMapID == 530) {
            *outland = true;
        }

        *x = continentRec->m_continentOffset[0] * 0.015968064f + 0.5f - transformed.y * 0.00002994012f * continentRec->m_scale;
        *y = continentRec->m_continentOffset[1] * 0.023952097f + 0.5f - transformed.x * 0.000044910183f * continentRec->m_scale;
        return false;
    }

    int32_t areaID;

    if (CGWorldMap::m_currentContinent < 0) {
        areaID = CGWorldMap::m_currentOrphanZone;
    } else {
        if (static_cast<uint32_t>(CGWorldMap::m_currentContinent) >= CGWorldMap::m_continents.m_count || !CGWorldMap::IsMapOnContinent(newMapID, CGWorldMap::m_currentContinent)) {
            return false;
        }

        WorldMapContinentInfo& continent = CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent];
        areaID = CGWorldMap::m_currentZone < 0 ? continent.m_areaID : continent.m_zones.m_data[CGWorldMap::m_currentZone];
    }

    if (areaID == -1) {
        return false;
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(areaID);

    if (!area || (area->m_mapID != mapID && area->m_mapID != newMapID)) {
        return false;
    }

    C2Vector pos = CGWorldMap::m_currentZone >= 0 && area->m_mapID == mapID ? C2Vector(position) : transformed;
    int32_t dungeonMap = dungeonMapID;

    if (dungeonMap <= 0) {
        dungeonMap = CGWorldMap::GetDungeonMapFromChunk(mapID, wmoGroupID, position.z, -1);
    }

    if (!dungeonMap && transformedDungeonMap > 0) {
        dungeonMap = transformedDungeonMap;
    }

    DungeonMapRec* dungeonRec = NULL;

    if (dungeonMap == (CGWorldMap::m_currentOrphanZone == -1 ? -1 : CGWorldMap::m_currentDungeonMap)) {
        dungeonRec = g_dungeonMapDB.GetRecord(dungeonMap);
    } else if (transformedDungeonMap == (CGWorldMap::m_currentOrphanZone == -1 ? -1 : CGWorldMap::m_currentDungeonMap)) {
        dungeonRec = g_dungeonMapDB.GetRecord(transformedDungeonMap);
    } else if (CGWorldMap::GetCurrentDungeonMap() > 0) {
        return false;
    }

    float maxX = dungeonRec ? dungeonRec->m_maxX : area->m_locLeft;
    float minX = dungeonRec ? dungeonRec->m_minX : area->m_locRight;
    float rangeX = maxX - minX;
    float offsetX = pos.y - minX;
    *x = offsetX == 0.0f || rangeX == 0.0f ? 0.0f : 1.0f - offsetX / rangeX;

    float maxY = dungeonRec ? dungeonRec->m_maxY : area->m_locTop;
    float minY = dungeonRec ? dungeonRec->m_minY : area->m_locBottom;
    float rangeY = maxY - minY;
    float offsetY = pos.x - minY;
    *y = offsetY == 0.0f || rangeY == 0.0f ? 0.0f : 1.0f - offsetY / rangeY;

    if (!noClamp && (*x < 0.0f || *x > 1.0f || *y < 0.0f || *y > 1.0f)) {
        *x = 0.0f;
        *y = 0.0f;
    }

    return true;
}

// OFFSET: 0x0091DA70/0x14067F640
void CGWorldMap::GetPlayerPosition(WOWGUID guid, float* x, float* y) {
    *x = 0.0f;
    *y = 0.0f;
    bool outland = false;
    C3Vector position(0.0f, 0.0f, 0.0f);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        uint32_t wmoGroupID;

        if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            wmoGroupID = CGGameUI::m_wmoGroupID;
        } else {
            const WMOAreaTableRec* groupRec = NULL;
            const WMOAreaTableRec* rootRec = NULL;
            wmoGroupID = 0;
            World::QueryMapObjAreaTable(player->m_worldObject, groupRec, rootRec, wmoGroupID, 1);
        }

        position = player->GetPosition();
        CGWorldMap::GetWorldPosition(position, CGPlayer_C::GetNewContinentID(), wmoGroupID, x, y, &outland, false, 0);
    } else {
        PartyMemberState* state = CGGameUI::GetPartyMemberState(guid);

        if (!state || (state->m_flags & 0x20)) {
            return;
        }

        AreaTableRec* area = g_areaTableDB.GetRecord(state->m_zoneID);

        if (!area) {
            return;
        }

        position.x = static_cast<float>(state->m_posX);
        position.y = static_cast<float>(state->m_posY);
        position.z = static_cast<float>(state->m_posZ);
        CGWorldMap::GetWorldPosition(position, area->m_continentID, state->unk_0022, x, y, &outland, false, 0);
    }

    if (outland) {
        *x = 0.0f;
        *y = 0.0f;
    }
}

// OFFSET: 0x0091DD50
int32_t CGWorldMap::GetMapAreaFromPos(float x, float y) {
    uint32_t chunkX = 0;
    uint32_t chunkY = 0;

    if (!CGWorldMap::GetChunkFromPos(x, y, &chunkX, &chunkY)) {
        return 0;
    }

    return CGWorldMap::GetWorldMapAreaByZone(CGWorldMap::m_currentContinent, chunkY, chunkX);
}

// OFFSET: 0x0091DDB0/0x14067F9F0
bool CGWorldMap::GetWorldPosition(const C3Vector& position, int32_t mapID, float* x, float* y, bool* outland, bool noClamp, int32_t dungeonMapID) {
    return CGWorldMap::GetWorldPosition(position, mapID, CGGameUI::m_wmoGroupID, x, y, outland, noClamp, dungeonMapID);
}

// OFFSET: 0x0091DDE0/0x14067FA30
int32_t CGWorldMap::GetMapHighlight(float x, float y) {
    if (CGWorldMap::m_currentContinent == -2) {
        return 0;
    }

    if (CGWorldMap::m_currentContinent == -1) {
        for (uint32_t i = CGWorldMap::m_continents.m_count; i; ) {
            i--;
            WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[i];

            if (entry.m_worldMapID && x >= entry.m_left && entry.m_right >= x && y >= entry.m_top && entry.m_bottom >= y) {
                return entry.m_areaID;
            }
        }

        return 0;
    }

    int32_t area = CGWorldMap::GetMapAreaFromPos(x, y);
    int32_t childArea = area;
    int32_t current = CGWorldMap::m_currentZone >= 0 ? CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_zones.m_data[CGWorldMap::m_currentZone] : -1;
    return CGWorldMap::GetWorldMapChildDepth(current, area, &childArea) ? childArea : 0;
}

// OFFSET: 0x0091DED0/0x14067FB50
void CGWorldMap::GetPOIPosition(const AreaPOIRec* poi, float* x, float* y) {
    *x = 0.0f;
    *y = 0.0f;

    if (!poi) {
        return;
    }

    if (((CGWorldMap::m_currentContinent >= 0 && CGWorldMap::m_currentZone >= 0) || CGWorldMap::m_currentContinent == -2) && !(poi->m_flags & 0x4)) {
        return;
    }

    if (CGWorldMap::m_currentContinent == -1 && !(poi->m_flags & 0x10)) {
        return;
    }

    if (CGWorldMap::m_currentZone < 0 && CGWorldMap::m_currentOrphanZone < 0 && !(poi->m_flags & 0x8)) {
        return;
    }

    if (poi->m_flags & 0x400) {
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetZoneID(CGWorldMap::m_currentContinent, CGWorldMap::m_currentZone));

        if (!area || area->m_areaID != poi->m_areaID || area->m_mapID != poi->m_continentID) {
            return;
        }
    }

    C3Vector position(poi->m_pos[0], poi->m_pos[1], 0.0f);
    bool outland;
    CGWorldMap::GetWorldPosition(position, poi->m_continentID, x, y, &outland, false, 0);

    if (outland) {
        *x = 0.0f;
        *y = 0.0f;
    }
}

// OFFSET: 0x0091E950/0x140680930
void CGWorldMap::SetMap(uint32_t continent, uint32_t zone, int32_t dungeonMap) {
    if (continent < CGWorldMap::m_continents.m_count) {
        CGWorldMap::m_currentContinent = continent;
        CGWorldMap::m_currentZone = continent < CGWorldMap::m_continents.m_count && zone < CGWorldMap::m_continents.m_data[continent].m_zones.m_count ? zone : -1;
        CGWorldMap::m_currentOrphanZone = -1;
        CGWorldMap::m_currentDungeonMap = -1;
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetZoneID(continent, zone));

        if (area && (area->m_defaultDungeonFloor > 0 || (area->m_flags & 0x4) || area->m_defaultDungeonFloor == -1)) {
            CGWorldMap::SetMap(-2, area->m_id, area->m_defaultDungeonFloor);
            return;
        }
    } else {
        CGWorldMap::m_currentContinent = continent;
        CGWorldMap::m_currentZone = -1;
        CGWorldMap::m_currentOrphanZone = continent == static_cast<uint32_t>(-2) ? zone : -1;
        CGWorldMap::m_currentDungeonMap = dungeonMap;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGWorldMap::m_numLandmarks = 0;
    uint32_t numPOIs = g_areaPOIDB.m_numRecords;

    for (uint32_t i = 0; i < numPOIs; i++) {
        AreaPOIRec* poi = g_areaPOIDB.GetRecordByIndex(i);
        float x = 0.0f;
        float y = 0.0f;
        CGWorldMap::GetPOIPosition(poi, &x, &y);

        if (fabs(x) < 0.00000023841858f && fabs(y) < 0.00000023841858f) {
            continue;
        }

        if (poi->m_areaID > 0 && !(poi->m_flags & 0x1000)) {
            AreaTableRec* area = g_areaTableDB.GetRecord(poi->m_areaID);

            if (!area || !player || (area->m_explorationLevel >= 0 && !player->HasZoneExplored(area->m_areaBit))) {
                continue;
            }
        }

        if (poi->m_worldStateID && !GetWorldState(poi->m_worldStateID)) {
            continue;
        }

        if (CGWorldMap::m_landmarks.m_count <= CGWorldMap::m_numLandmarks) {
            CGWorldMap::m_landmarks.SetCount(2 * CGWorldMap::m_landmarks.m_count);
        }

        CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_id = poi->m_id;
        CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_x = x;
        CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_y = y;
        CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks++].m_type = 0;
    }

    AreaPOIRec* gossip = MinimapGetGossipPoi();

    if (gossip && (fabs(gossip->m_pos[0]) >= 0.00000023841858f || fabs(gossip->m_pos[1]) >= 0.00000023841858f)) {
        float x = 0.0f;
        float y = 0.0f;
        C3Vector position(gossip->m_pos[0], gossip->m_pos[1], 0.0f);
        bool outland = false;
        CGWorldMap::GetWorldPosition(position, gossip->m_continentID, &x, &y, &outland, false, 0);

        if (!outland && (fabs(x) >= 0.00000023841858f || fabs(y) >= 0.00000023841858f)) {
            if (CGWorldMap::m_landmarks.m_count <= CGWorldMap::m_numLandmarks) {
                CGWorldMap::m_landmarks.SetCount(2 * CGWorldMap::m_landmarks.m_count);
            }

            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_x = x;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_y = y;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks++].m_type = 2;
        }
    }

    CVar* digSites;

    if (zone == static_cast<uint32_t>(-1) && continent != static_cast<uint32_t>(-1) && (digSites = CVar::Lookup("digSites")) && digSites->m_intValue > 0 && player) {
        for (uint32_t i = 0; i < 16; i++) {
            uint16_t site = player->m_player->m_playerFieldReserachSite1[i];
            ResearchSiteRec* siteRec;
            CGResearchFrame::BLOBPOIDATA* data;

            if (!site || !(siteRec = g_researchSiteDB.GetRecord(site)) || !(data = CGResearchFrame::GetPOIData(siteRec->m_questPOIBlobID)) || !data->m_blobs.m_count) {
                continue;
            }

            CGResearchFrame::BLOBPOI* blob = data->m_blobs.m_data;
            C3Vector position(static_cast<float>(blob->m_center.x), static_cast<float>(blob->m_center.y), 0.0f);
            float x = 0.0f;
            float y = 0.0f;
            bool outland = false;
            CGWorldMap::GetWorldPosition(position, blob->m_mapID, &x, &y, &outland, false, 0);

            if (outland || (fequal(x, 0.0f) && fequal(y, 0.0f))) {
                continue;
            }

            if (CGWorldMap::m_landmarks.m_count <= CGWorldMap::m_numLandmarks) {
                CGWorldMap::m_landmarks.SetCount(2 * CGWorldMap::m_landmarks.m_count);
            }

            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_id = site;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_x = x;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_y = y;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks++].m_type = 3;
        }
    }

    int32_t currentArea = CGWorldMap::GetCurrentAreaID();
    WorldMapAreaRec* currentRec = g_worldMapAreaDB.GetRecord(currentArea);

    if (currentRec && !(currentRec->m_flags & 0x80) && player && continent != static_cast<uint32_t>(-1) && zone != static_cast<uint32_t>(-1)) {
        int32_t faction = player->GetPVPFactionIndex();

        for (int32_t i = 0; i < g_taxiNodesDB.m_numRecords; i++) {
            TaxiNodesRec* node = g_taxiNodesDB.GetRecordByIndex(i);

            if (faction == 0) {
                if (!(node->unk_0020 & 0x2)) {
                    continue;
                }
            } else if (faction == 1) {
                if (!(node->unk_0020 & 0x1)) {
                    continue;
                }
            }

            float y = 0.0f;
            float x = 0.0f;
            bool outland = false;
            CGWorldMap::GetWorldPosition(*reinterpret_cast<const C3Vector*>(node->m_pos), node->m_continentID, &x, &y, &outland, false, 0);

            if ((x == 0.0f && y == 0.0f) || outland) {
                continue;
            }

            if (continent != static_cast<uint32_t>(-2)) {
                uint32_t chunkY = 0;
                uint32_t chunkX = 0;

                if (!CGWorldMap::GetChunkFromPos(x, y, &chunkX, &chunkY)) {
                    continue;
                }

                int32_t area = CGWorldMap::GetWorldMapAreaByZone(CGWorldMap::m_currentContinent, chunkY, chunkX);

                if (node->unk_0020 & 0x4) {
                    if (area == CGWorldMap::GetCurrentAreaID()) {
                        continue;
                    }

                    bool nearby = false;

                    for (uint32_t cy = chunkY >= s_taxiNodeZoneRadius ? chunkY - s_taxiNodeZoneRadius : 0; cy <= s_taxiNodeZoneRadius + chunkY && cy < 128; cy++) {
                        for (uint32_t cx = chunkX < s_taxiNodeZoneRadius ? 0 : chunkX - s_taxiNodeZoneRadius; cx <= chunkX + s_taxiNodeZoneRadius && cx < 128; cx++) {
                            if (CGWorldMap::GetWorldMapAreaByZone(CGWorldMap::m_currentContinent, cy, cx) == currentArea) {
                                nearby = true;
                                break;
                            }
                        }
                    }

                    if (!nearby) {
                        continue;
                    }
                } else if (area != currentArea) {
                    continue;
                }
            }

            if (CGWorldMap::m_landmarks.m_count <= CGWorldMap::m_numLandmarks) {
                CGWorldMap::m_landmarks.SetCount(2 * CGWorldMap::m_landmarks.m_count);
            }

            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_id = node->m_id;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_x = node->unk_0024 + x;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks].m_y = y - node->unk_0028;
            CGWorldMap::m_landmarks.m_data[CGWorldMap::m_numLandmarks++].m_type = 4;
        }
    }

    CGWorldMap::m_numOverlays = 0;
    int32_t mapAreaID = CGWorldMap::GetCurrentAreaID();
    const WorldMapAreaRec* mapArea = g_worldMapAreaDB.GetRecord(mapAreaID);
    const WorldMapAreaRec* phased = PhaseShift_C_GetPhasedWorldMap(mapArea);

    if (phased) {
        mapArea = phased;
        mapAreaID = phased->m_id;
    }

    if (player && ((continent != static_cast<uint32_t>(-2) && (CGWorldMap::m_currentZone >= 0 || CGWorldMap::m_currentOrphanZone > 0)) || (mapArea && (mapArea->m_flags & 0x40)))) {
        uint32_t numOverlays = g_worldMapOverlayDB.m_numRecords;

        for (uint32_t i = 0; i < numOverlays; i++) {
            WorldMapOverlayRec* overlay = g_worldMapOverlayDB.GetRecordByIndex(i);

            if (mapAreaID != overlay->m_mapAreaID) {
                continue;
            }

            for (uint32_t j = 0; j < 4; j++) {
                AreaTableRec* area = overlay->m_areaID[j] ? g_areaTableDB.GetRecord(overlay->m_areaID[j]) : NULL;

                if (!area) {
                    continue;
                }

                if (area->m_explorationLevel >= 0 && !player->HasZoneExplored(area->m_areaBit)) {
                    bool explored = false;

                    if (!(area->m_flags & 0x40000000)) {
                        for (int32_t k = 0; k < g_areaTableDB.m_numRecords; k++) {
                            AreaTableRec* subArea = g_areaTableDB.GetRecordByIndex(k);

                            if (subArea && subArea->m_parentAreaID == area->m_id && player->HasZoneExplored(subArea->m_areaBit)) {
                                explored = true;
                                break;
                            }
                        }
                    }

                    if (!explored) {
                        continue;
                    }
                }

                if (CGWorldMap::m_overlays.m_count <= CGWorldMap::m_numOverlays) {
                    CGWorldMap::m_overlays.SetCount(2 * CGWorldMap::m_overlays.m_count);
                }

                CGWorldMap::m_overlays.m_data[CGWorldMap::m_numOverlays++] = overlay->m_id;
                break;
            }
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_WORLD_MAP_UPDATE, NULL);
}

// OFFSET: 0x004E82B0/0x14012A8D0
void CGWorldMap::RefreshCurrentMap() {
    if (CGWorldMap::m_currentOrphanZone == -1) {
        CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentZone, CGWorldMap::m_currentOrphanZone);
    } else {
        CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentOrphanZone, CGWorldMap::m_currentDungeonMap);
    }
}

// OFFSET: 0x0091F500/0x140681680
void CGWorldMap::ProcessClick(float x, float y) {
    uint32_t continent = CGWorldMap::m_currentContinent;

    if (continent == static_cast<uint32_t>(-2)) {
        return;
    }

    if (continent == static_cast<uint32_t>(-1)) {
        for (uint32_t i = CGWorldMap::m_continents.m_count; i; ) {
            i--;
            WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[i];

            if (entry.m_worldMapID && x >= entry.m_left && entry.m_right >= x && y >= entry.m_top && entry.m_bottom >= y) {
                CGWorldMap::SetMap(i, -1, -1);
                return;
            }
        }

        return;
    }

    int32_t area = CGWorldMap::GetMapAreaFromPos(x, y);
    int32_t childArea = area;
    int32_t current = CGWorldMap::m_currentZone >= 0 ? CGWorldMap::m_continents.m_data[continent].m_zones.m_data[CGWorldMap::m_currentZone] : -1;
    CGWorldMap::GetWorldMapChildDepth(current, area, &childArea);

    if (!childArea) {
        return;
    }

    for (uint32_t zone = 0; continent < CGWorldMap::m_continents.m_count; zone++) {
        WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[continent];

        if (zone >= entry.m_zones.m_count) {
            break;
        }

        if (entry.m_zones.m_data[zone] == childArea) {
            CGWorldMap::SetMap(continent, zone, -1);
            return;
        }
    }
}

// OFFSET: 0x0091FEF0/0x140682280
int32_t PlayerExploredZonesUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    if (CGWorldMap::m_currentOrphanZone == -1) {
        CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentZone, CGWorldMap::m_currentOrphanZone);
    } else {
        CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentOrphanZone, CGWorldMap::m_currentDungeonMap);
    }

    CGWorldMap::PreloadMapOverlayTextures();
    return 1;
}

// OFFSET: 0x0091FF40/0x1406822D0
void CGWorldMap::EnterWorld() {
    ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerExploredZones1), sizeof(static_cast<CGPlayerLocalData*>(NULL)->m_playerExploredZones1), PlayerExploredZonesUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x0091FF70/0x140682320
void CGWorldMap::LeaveWorld() {
    ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, offsetof(CGPlayerLocalData, m_playerExploredZones1), PlayerExploredZonesUpdateHandler, NULL);
}

// OFFSET: 0x0091FF90/0x140682360
void CGWorldMap::SetMapToCurrentZone(int32_t debug) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        CGWorldMap::SetMap(-1, -1, -1);
        return;
    }

    int32_t mapID = CGPlayer_C::GetNewContinentID();
    int32_t areaID = CGGameUI::m_areaID;
    int32_t wmoGroupID = CGGameUI::m_wmoGroupID;
    C3Vector position = player->GetPosition();
    int32_t continent = -1;
    int32_t zone = -1;
    int32_t dungeonMap = -1;
    CGWorldMap::GetMapForPosition(mapID, areaID, wmoGroupID, position, continent, zone, dungeonMap, debug);

    if (!debug) {
        CGWorldMap::SetMap(continent, zone, dungeonMap);
        CGWorldMap::PreloadMapTileTextures();
        CGWorldMap::PreloadMapOverlayTextures();
    }
}

// OFFSET: 0x00920050
void CGWorldMap::SetDungeonLevel(int32_t level) {
    if (CGWorldMap::m_currentOrphanZone < 0) {
        return;
    }

    if (CGWorldMap::m_currentContinent >= 0) {
        WorldMapContinentInfo& entry = CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent];
        int32_t zoneArea = CGWorldMap::m_currentZone >= 0 ? entry.m_zones.m_data[CGWorldMap::m_currentZone] : entry.m_areaID;

        if (zoneArea != CGWorldMap::m_currentOrphanZone) {
            return;
        }
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::m_currentOrphanZone);

    if (!area) {
        return;
    }

    int32_t mapID = area->m_mapID;

    if (!level) {
        CGWorldMap::m_currentDungeonMap = -1;
        CGWorldMap::SetMap(-2, area->m_id, -1);
        return;
    }

    bool inMap = false;

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_dungeonMapDB.m_numRecords); i++) {
        DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecordByIndex(i);

        if (dungeonMap->m_mapID == mapID) {
            if (dungeonMap->m_floorIndex == level) {
                if (dungeonMap->m_id > 0) {
                    CGWorldMap::m_currentDungeonMap = dungeonMap->m_id;
                    CGWorldMap::RefreshCurrentMap();
                }

                return;
            }

            if (!inMap) {
                inMap = true;
            }
        } else if (inMap) {
            return;
        }
    }
}

// OFFSET: 0x00920370
void CGWorldMap::ShutdownGame() {
    CGWorldMap::ClearArrowFrames();

    for (uint32_t i = 0; i < CGWorldMap::m_continents.m_count; i++) {
        CGWorldMap::m_continents.m_data[i].m_zones.Clear();
    }

    CGWorldMap::m_continents.Clear();
    CGWorldMap::m_landmarks.Clear();
    CGWorldMap::m_overlays.Clear();
    CGWorldMap::m_orphanZones.Clear();
}

// OFFSET: 0x00920550/0x140682AE0
void CGWorldMap::InitializeGame() {
    uint32_t numAreas = g_worldMapAreaDB.m_numRecords;
    uint32_t numContinents = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_worldMapAreaDB.m_numRecords); i++) {
        if (CGWorldMap::IsContinent(g_worldMapAreaDB.GetRecordByIndex(i))) {
            numContinents++;
        }
    }

    CGWorldMap::m_continents.SetCount(numContinents);
    uint32_t continentIndex = 0;

    for (uint32_t i = 0; i < numAreas; i++) {
        WorldMapAreaRec* area = g_worldMapAreaDB.GetRecordByIndex(i);

        if (!CGWorldMap::IsContinent(area)) {
            continue;
        }

        CGWorldMap::m_continents.m_data[continentIndex].m_mapID = area->m_mapID;
        CGWorldMap::m_continents.m_data[continentIndex].m_areaID = area->m_id;
        uint32_t numZones = 0;

        for (uint32_t j = 0; j < numAreas; j++) {
            WorldMapAreaRec* zone = g_worldMapAreaDB.GetRecordByIndex(j);

            if (!zone || (zone->m_flags & 0xA)) {
                continue;
            }

            if (zone->m_flags & 0x10) {
                if (zone->m_parentWorldMapID == area->m_id) {
                    numZones++;
                }
            } else {
                int32_t mapID = zone->m_displayMapID;

                if (mapID < 0) {
                    mapID = zone->m_mapID;
                }

                if (zone->m_areaID && mapID == area->m_mapID) {
                    numZones++;
                }
            }
        }

        CGWorldMap::m_continents.m_data[continentIndex].m_zones.SetCount(numZones);
        uint32_t zoneIndex = 0;

        for (uint32_t j = 0; j < numAreas; j++) {
            WorldMapAreaRec* zone = g_worldMapAreaDB.GetRecordByIndex(j);

            if (!zone || (zone->m_flags & 0xA)) {
                continue;
            }

            if (zone->m_flags & 0x10) {
                if (zone->m_parentWorldMapID == area->m_id) {
                    CGWorldMap::m_continents.m_data[continentIndex].m_zones.m_data[zoneIndex++] = zone->m_id;
                }
            } else {
                int32_t mapID = zone->m_displayMapID;

                if (mapID < 0) {
                    mapID = zone->m_mapID;
                }

                if (zone->m_areaID && mapID == area->m_mapID) {
                    CGWorldMap::m_continents.m_data[continentIndex].m_zones.m_data[zoneIndex++] = zone->m_id;
                }
            }
        }

        qsort(CGWorldMap::m_continents.m_data[continentIndex].m_zones.m_data, zoneIndex, sizeof(int32_t), QSortZones);

        for (uint32_t j = 0; j < static_cast<uint32_t>(g_worldMapContinentDB.m_numRecords); j++) {
            WorldMapContinentRec* rec = g_worldMapContinentDB.GetRecordByIndex(j);

            if (rec->m_mapID == area->m_mapID || (rec->m_mapID == -1 && rec->m_worldMapID == area->m_id && (area->m_flags & 0x8))) {
                float offset = rec->m_scale * 32.0f;
                float y = 20.875f - offset + rec->m_continentOffset[1];
                float x = (31.3125f - offset + rec->m_continentOffset[0]) * 0.015968064f;
                CGWorldMap::m_continents.m_data[continentIndex].m_left = rec->m_leftBoundary * rec->m_scale * 0.015968064f + x;
                CGWorldMap::m_continents.m_data[continentIndex].m_right = (rec->m_rightBoundary + 1) * rec->m_scale * 0.015968064f + x;
                y *= 0.023952097f;
                CGWorldMap::m_continents.m_data[continentIndex].m_top = rec->m_topBoundary * rec->m_scale * 0.023952097f + y;
                CGWorldMap::m_continents.m_data[continentIndex].m_bottom = (rec->m_bottomBoundary + 1) * rec->m_scale * 0.023952097f + y;
                CGWorldMap::m_continents.m_data[continentIndex].m_worldMapID = rec->m_worldMapID;
                break;
            }
        }

        CGWorldMap::InitializeChunkZones(area, CGWorldMap::m_continents.m_data[continentIndex].m_zoneMap);
        uint32_t numPhased = 0;

        for (uint32_t j = 0; j < numAreas; j++) {
            WorldMapAreaRec* zone = g_worldMapAreaDB.GetRecordByIndex(j);

            if (zone && (zone->m_flags & 0x2) && zone->m_mapID == area->m_mapID) {
                numPhased++;
            }
        }

        CGWorldMap::m_continents.m_data[continentIndex].m_phasedChunks.SetCount(numPhased);
        uint32_t phasedIndex = 0;

        for (uint32_t j = 0; j < numAreas; j++) {
            WorldMapAreaRec* zone = g_worldMapAreaDB.GetRecordByIndex(j);

            if (!zone || !(zone->m_flags & 0x2) || zone->m_mapID != area->m_mapID) {
                continue;
            }

            if (CGWorldMap::InitializeChunkZones(zone, CGWorldMap::m_continents.m_data[continentIndex].m_phasedChunks.m_data[phasedIndex].m_zoneMap)) {
                CGWorldMap::m_continents.m_data[continentIndex].m_phasedChunks.m_data[phasedIndex].m_areaID = zone->m_id;
            } else {
                CGWorldMap::m_continents.m_data[continentIndex].m_phasedChunks.m_data[phasedIndex].m_areaID = 0;
            }

            phasedIndex++;
        }

        continentIndex++;
    }

    uint32_t numOrphans = 0;

    for (uint32_t i = 0; i < numAreas; i++) {
        WorldMapAreaRec* zone = g_worldMapAreaDB.GetRecordByIndex(i);

        if (!zone || !zone->m_areaID) {
            continue;
        }

        if (fabs(zone->m_locLeft) >= 0.00000023841858f || fabs(zone->m_locRight) >= 0.00000023841858f || fabs(zone->m_locTop) >= 0.00000023841858f || fabs(zone->m_locBottom) >= 0.00000023841858f) {
            uint32_t c = 0;

            for (; c < CGWorldMap::m_continents.m_count; c++) {
                if (CGWorldMap::IsMapOnContinent(zone->m_mapID, c)) {
                    break;
                }
            }

            if (c < CGWorldMap::m_continents.m_count) {
                continue;
            }
        }

        numOrphans++;
    }

    CGWorldMap::m_orphanZones.SetCount(numOrphans);

    if (numOrphans) {
        uint32_t orphanIndex = 0;

        for (uint32_t i = 0; i < numAreas; i++) {
            WorldMapAreaRec* zone = g_worldMapAreaDB.GetRecordByIndex(i);

            if (!zone || !zone->m_areaID) {
                continue;
            }

            if (fabs(zone->m_locLeft) >= 0.00000023841858f || fabs(zone->m_locRight) >= 0.00000023841858f || fabs(zone->m_locTop) >= 0.00000023841858f || fabs(zone->m_locBottom) >= 0.00000023841858f) {
                uint32_t c = 0;

                for (; c < CGWorldMap::m_continents.m_count; c++) {
                    if (CGWorldMap::IsMapOnContinent(zone->m_mapID, c)) {
                        break;
                    }
                }

                if (c < CGWorldMap::m_continents.m_count) {
                    continue;
                }
            }

            CGWorldMap::m_orphanZones.m_data[orphanIndex++] = zone->m_id;
        }
    }

    qsort(CGWorldMap::m_orphanZones.m_data, CGWorldMap::m_orphanZones.m_count, sizeof(int32_t), QSortOrphanZones);
    CGWorldMap::m_currentOrphanZone = -1;
    CGWorldMap::m_numLandmarks = 0;
    CGWorldMap::m_numOverlays = 0;
    CGWorldMap::m_landmarks.SetCount(16);
    CGWorldMap::m_overlays.SetCount(32);
}
