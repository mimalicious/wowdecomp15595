#ifndef WOW_SOURCE_WORLDCLIENT_MAPENTITY_H
#define WOW_SOURCE_WORLDCLIENT_MAPENTITY_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/WorldClient/MapStaticEntity.h"
#include "WoW/Source/WorldClient/World.h"

class C44Matrix;
class CAaBox;
class CMapEntity;
struct SMOFog;

class CMapEntity : public CMapStaticEntity {
    public:
    /* 0090 */ TSLink<CMapEntity> m_viewLink;
    /* 0098 */ TSLink<CMapEntity> m_sortLink;
    /* 00A0 */ WORLD_LOAD_CALLBACK m_updateCallback;
    /* 00A4 */ void* m_updateParam;
    /* 00A8 */ uint32_t m_cullStamp;
    /* 00AC */ uint32_t unk_00AC;
    /* 00B0 */ uint64_t m_param64;
    /* 00B8 */ uint32_t m_param32;
    /* 00BC */ int32_t unk_00BC;
    /* 00C0 */ uint32_t unk_00C0;
    /* 00C4 */ uint32_t unk_00C4;
    /* 00C8 */ uint32_t unk_00C8;
    /* 00CC */ int32_t m_groundType;
    /* 00D0 */ uint16_t m_liquidType;
    /* 00D4 */ uint32_t unk_00D4;
    /* 00D8 */ uint32_t unk_00D8;
    /* 00DC */ float unk_00DC;
    /* 00E0 */ float unk_00E0;
    /* 00E4 */ uint32_t unk_00E4;

    static int32_t QueryCameraFog(SMOFog* fog, int32_t* found, float* distance);

    CMapEntity();
    void EnablePortals(int32_t enable);
    int32_t QueryInterior(int32_t a2);
    void Update();
    int32_t QueryMapObjZoneName(const char*& name);
    int32_t QueryMapObjSubzoneName(const char*& name, uint32_t& areaId);
    int32_t QueryMapObjAreaTable(const WMOAreaTableRec*& groupRec, const WMOAreaTableRec*& rootRec, uint32_t& wmoGroupId, int32_t a5);
    bool QueryMapObjIDs(uint32_t& wmoId, uint32_t& nameSet, uint32_t& groupId);
    int32_t QueryMapObjFileName(const char*& fileName);
    bool QueryMapObjMinimap(const CAaBox& box, TSStackArray<World::MinimapQuad>& quads, uint32_t maxQuads, bool a5, bool a6);
    bool QueryMapObjMatrix(C44Matrix* matrix, C44Matrix* invMatrix);
    void QueryLiquidSounds(TSGrowableArray<World::LiquidQueryResult>& results, uint32_t* distance);
};

#endif
