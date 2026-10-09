#ifndef WOW_SOURCE_GAME_GAMECLIENT_PHASESHIFT_C_H
#define WOW_SOURCE_GAME_GAMECLIENT_PHASESHIFT_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/PhaseShiftShared.h"

class AreaTriggerRec;
class WorldMapAreaRec;
class WorldStateUIRec;

class CPhaseMapTracker {
    public:
    static TSGrowableArray<CPhaseMapTracker> s_loadedMaps;

    static void LeaveWorld();
    static CPhaseMapTracker* LoadMap(int32_t mapID, int32_t active);
    static void RemoveUnusedMaps();
    static void UpdateMaps(uint32_t activeCount, const uint16_t* activeMaps, uint32_t inactiveCount, const uint16_t* inactiveMaps);

    /* 0000 */ int32_t m_mapID;
    /* 0004 */ uint8_t m_active;
    /* 0005 */ uint8_t m_used;
    /* 0008 */ int32_t m_firstAreaTrigger;
    /* 000C */ int32_t m_lastAreaTrigger;

    CPhaseMapTracker();
    void SetMap(int32_t mapID);
};

struct PhaseGroupLookup {
    /* 0000 */ int32_t m_first;
    /* 0004 */ int32_t m_count;
};

void PhaseShiftShared_InitPhaseXPhaseGroupIndex();
void PhaseShift_C_CleanupDB();
void PhaseShift_C_EnterWorld(int32_t reload);
int32_t PhaseShift_C_GetNumActivePhaseMaps();
int32_t PhaseShift_C_GetNumPhaseMaps();
CPhaseShift* PhaseShift_C_GetPhase();
const WorldMapAreaRec* PhaseShift_C_GetPhasedWorldMap(const WorldMapAreaRec* rec);
int32_t PhaseShift_C_GetPhaseMapAreaTriggers(int32_t index, int32_t* first, int32_t* last);
int32_t PhaseShift_C_HasSharedPhase(const AreaTriggerRec* rec);
int32_t PhaseShift_C_HasSharedPhase(const CPhaseShift* phaseShift);
int32_t PhaseShift_C_HasSharedPhase(const WorldStateUIRec* rec);
void PhaseShift_C_InitDB();
void PhaseShift_C_InitializeGame();
int32_t PhaseShift_C_IsPhasedWorldMapActive(int32_t worldMapAreaID);
void PhaseShift_C_LeaveWorld(int32_t reload);

#endif
