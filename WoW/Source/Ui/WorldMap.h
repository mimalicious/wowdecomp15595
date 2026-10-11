#ifndef WOW_SOURCE_UI_WORLDMAP_H
#define WOW_SOURCE_UI_WORLDMAP_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Storm/H/stpl.h"

struct lua_State;
class AreaPOIRec;
class C3Vector;
class CSimpleFrame;
class CSimpleModel;
class DungeonMapRec;
class WorldMapAreaRec;
class WorldMapContinentRec;
class WorldMapOverlayRec;

struct DebugElement {
    /* 0000 */ int32_t unk_0000;
    /* 0004 */ int32_t unk_0004;
    /* 0008 */ int32_t m_array1 [6];
    /* 0020 */ int32_t m_array2 [4];
    /* 0030 */ int32_t m_computed48;
    /* 0034 */ int32_t m_computed52;
    /* 0038 */ int32_t m_computed56;
    /* 003C */ int32_t m_extra60;
    /* 0040 */ int32_t m_extra64;
    /* 0044 */ int32_t m_extra68;
    /* 0048 */ float m_extra76;
    /* 004C */ void* unk_004C;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ int32_t unk_0064;
    /* 0068 */ int32_t unk_0068;
};

struct WorldMapPhasedChunkInfo {
    /* 00000 */ int32_t m_areaID;
    /* 00004 */ int32_t m_zoneMap[128][128];
};

struct WorldMapContinentInfo {
    /* 00000 */ int32_t m_mapID;
    /* 00004 */ int32_t m_areaID;
    /* 00008 */ TSFixedArray<int32_t> m_zones;
    /* 00014 */ int32_t m_zoneMap[128][128];
    /* 10014 */ TSFixedArray<WorldMapPhasedChunkInfo> m_phasedChunks;
    /* 10020 */ float m_top;
    /* 10024 */ float m_left;
    /* 10028 */ float m_bottom;
    /* 1002C */ float m_right;
    /* 10030 */ int32_t m_worldMapID;

    WorldMapContinentInfo();
};

struct WorldMapLandmarkInfo {
    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_x;
    /* 0008 */ float m_y;
    /* 000C */ int32_t m_type;
};


class CGWorldMap {
    public:
    static uint32_t m_numLandmarks;
    static uint32_t m_numOverlays;
    static int32_t m_currentContinent;
    static int32_t m_currentZone;
    static int32_t m_currentDungeonMap;
    static int32_t m_currentOrphanZone;
    static TSFixedArray<WorldMapContinentInfo> m_continents;
    static TSGrowableArray<WorldMapLandmarkInfo> m_landmarks;
    static TSGrowableArray<int32_t> m_overlays;
    static TSFixedArray<int32_t> m_orphanZones;
    static CSimpleModel* m_playerArrowFrame;
    static CSimpleModel* m_playerMiniArrowFrame;
    static CSimpleModel* m_playerArrowEffectFrame;
    static CSimpleModel* m_playerMiniArrowEffectFrame;
    static void CreateArrowFrame(CSimpleFrame* parent);
    static void CreateMiniArrowFrame(CSimpleFrame* parent);
    static void UpdateArrowFrames();
    static void ProcessClick(float x, float y);
    static void SetDungeonLevel(int32_t level);
    static void EnterWorld();
    static void LeaveWorld();
    static bool GetContinentPosition(float x, float y, int32_t mapID, float* mapX, float* mapY);
    static void InitializeGame();
    static void ShutdownGame();
    static void ClearArrowFrames();
    static void SetMapToCurrentZone(int32_t debug);
    static void RefreshCurrentMap();
    static void SetMap(uint32_t continent, uint32_t zone, int32_t dungeonMap);
    static int32_t GetZoneID(uint32_t continent, uint32_t zone);
    static uint32_t GetNumZones(uint32_t continent);
    static int32_t GetCurrentDungeonMap();
    static int32_t IsContinent(const WorldMapAreaRec* area);
    static int32_t InitializeChunkZones(const WorldMapAreaRec* area, int32_t (&zoneMap)[128][128]);
    static const char* GetContinentName(uint32_t continent);
    static bool DetermineAlternateMapPosition(int32_t mapID, int32_t areaID, float x, float y, int32_t& newMapID, int32_t& newAreaID, float& newX, float& newY, int32_t& dungeonMapID, bool& forceDungeonMap);
    static bool DetermineAlternateMapPosition(int32_t mapID, float x, float y, int32_t& newMapID, float& newX, float& newY, int32_t& dungeonMapID, bool& forceDungeonMap);
    static int32_t GetDungeonMapFromChunk(int32_t mapID, int32_t wmoGroupID, float z, int32_t worldMapAreaID);
    static int32_t GetWorldMapAreaByZone(int32_t continent, uint32_t y, uint32_t x);
    static WorldMapContinentRec* GetContinentRec(int32_t mapID);
    static bool IsMapOnContinent(int32_t mapID, uint32_t continent);
    static int32_t GetWorldMapChildDepth(int32_t parent, int32_t child, int32_t* childArea);
    static int32_t GetCurrentDungeonFloor(DungeonMapRec** dungeonMap);
    static void PreloadMapTileTextures();
    static void PreloadMapOverlayTextures();
    static void GetMapForPosition(int32_t mapID, int32_t areaID, int32_t wmoGroupID, const C3Vector& position, int32_t& continent, int32_t& zone, int32_t& dungeonMap, int32_t debug);
    static int32_t GetChunkFromPos(float x, float y, uint32_t* chunkX, uint32_t* chunkY);
    static bool GetWorldPosition(const C3Vector& position, int32_t mapID, int32_t wmoGroupID, float* x, float* y, bool* outland, bool noClamp, int32_t dungeonMapID);
    static bool GetWorldPosition(const C3Vector& position, int32_t mapID, float* x, float* y, bool* outland, bool noClamp, int32_t dungeonMapID);
    static void GetPOIPosition(const AreaPOIRec* poi, float* x, float* y);
    static int32_t GetNumDungeonLevels();
    static const char* GetZoneName(uint32_t continent, uint32_t zone);
    static int32_t GetMapAreaFromPos(float x, float y);
    static int32_t GetMapHighlight(float x, float y);
    static int32_t GetMapHeight();
    static void GetPlayerPosition(WOWGUID guid, float* x, float* y);
    static int32_t GetCurrentAreaID();
    static const char* GetMapName(int32_t areaID);
};

void WorldMapRegisterScriptFunctions();
void WorldMapUnregisterScriptFunctions();

WorldMapOverlayRec* GetMapOverlayTexture(uint32_t index, char* texture, uint32_t size);
int32_t PlayerExploredZonesUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);

#include "WoW/Source/Ui/WorldMap.inl"

#endif
