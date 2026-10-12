#include <cstring>
#include <malloc.h>
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Game/GameClient/Minimap.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00CA1388/0x140B3DD08
static int32_t s_syncMapLoad = 1;

// STATIC: 0x00CA138C/0x140B3DD10
static CPhaseShift s_phaseShift(8);

// STATIC: 0x00CA13BC
static CPhaseShift s_defaultPhaseShift(8);

// STATIC: 0x00DC716C
static int32_t s_phaseGroupIndexInitialized;

// STATIC: 0x00DC7170/0x140CACDB8
static TSFixedArray<CPhaseShift> s_areaTriggerPhases;

// STATIC: 0x00DC717C/0x140CACDC8
static TSFixedArray<CPhaseShift> s_worldStateUIPhases;

// STATIC: 0x00DC7188/0x140CACDD8
static TSFixedArray<uint16_t> s_phasedWorldMaps;

// STATIC: 0x00DC7194/0x140CACDE8
TSGrowableArray<CPhaseMapTracker> CPhaseMapTracker::s_loadedMaps;

// STATIC: 0x00DC71A4/0x140CACE00
static TSFixedArray<PhaseGroupLookup> s_phaseGroupLookup;

// INLINED
CPhaseMapTracker::CPhaseMapTracker() {
    this->m_mapID = -1;
    this->m_active = 0;
    this->m_used = 0;
    this->m_firstAreaTrigger = -1;
    this->m_lastAreaTrigger = -1;
}

// PROVEN: f5d58798 IDENTICAL/IDENTICAL
// OFFSET: 0x004E3F70/0x140125320
void PhaseShift_C_ShutdownGame() {
    ClientPhaseShiftChange::s_handler = NULL;
}

// PROVEN: 2565ab92 IDENTICAL/IDENTICAL
// OFFSET: 0x004E3F80/0x140125330
void PhaseShift_C_EnterWorld(int32_t reload) {
    if (!reload) {
        s_syncMapLoad = 0;
    }
}

// PROVEN: 08c0de35 IDENTICAL/UNPROVEN
// OFFSET: 0x004E3FA0/0x140125350
CPhaseShift* PhaseShift_C_GetPhase() {
    return &s_phaseShift;
}

// OFFSET: 0x004E4200
static bool GetPhaseGroupLookup(uint32_t phaseGroupID, int32_t* first, int32_t* count) {
    if (phaseGroupID >= s_phaseGroupLookup.Count()) {
        *first = -1;
        return false;
    }

    *first = s_phaseGroupLookup[phaseGroupID].m_first;
    *count = s_phaseGroupLookup[phaseGroupID].m_count;
    return *first != -1;
}

// OFFSET: 0x004E4250
void CPhaseMapTracker::SetMap(int32_t mapID) {
    this->m_lastAreaTrigger = -1;
    this->m_firstAreaTrigger = -1;
    this->m_mapID = mapID;
    this->m_active = 0;
    this->m_used = 1;

    for (int32_t i = 0; i < g_areaTriggerDB.m_numRecords; i++) {
        AreaTriggerRec* rec = g_areaTriggerDB.GetRecordByIndex(i);

        if (rec->m_continentID < mapID) {
            continue;
        }

        if (rec->m_continentID > mapID) {
            return;
        }

        if (this->m_firstAreaTrigger == -1) {
            this->m_firstAreaTrigger = i;
        }

        this->m_lastAreaTrigger = i;
    }
}

// PROVEN: 77ed9063 IDENTICAL/NONE
// OFFSET: 0x004E42C0
void CPhaseMapTracker::RemoveUnusedMaps() {
    for (uint32_t i = 0; i < CPhaseMapTracker::s_loadedMaps.Count(); i++) {
        CPhaseMapTracker& tracker = CPhaseMapTracker::s_loadedMaps[i];

        if (tracker.m_mapID && !tracker.m_used) {
            World::SetMapActive(tracker.m_mapID, 0);
            World::RemoveMapById(tracker.m_mapID);
            tracker.m_mapID = 0;
            tracker.m_active = 0;
        }

        tracker.m_used = 0;
    }
}

// PROVEN: 8fd6b9f7 IDENTICAL/UNPROVEN
// OFFSET: 0x004E4320/0x140125620
void CPhaseMapTracker::LeaveWorld() {
    for (uint32_t i = 0; i < CPhaseMapTracker::s_loadedMaps.Count(); i++) {
        CPhaseMapTracker& tracker = CPhaseMapTracker::s_loadedMaps[i];

        if (tracker.m_active) {
            World::SetMapActive(tracker.m_mapID, 0);
            World::RemoveMapById(tracker.m_mapID);
        }

        tracker.m_active = 0;
        tracker.m_mapID = 0;
        tracker.m_used = 0;
    }
}

// PROVEN: 0975e468 IDENTICAL/UNPROVEN
// OFFSET: 0x004E4380/0x1401256A0
int32_t PhaseShift_C_GetNumPhaseMaps() {
    return CPhaseMapTracker::s_loadedMaps.Count();
}

// PROVEN: 19f4f87d IDENTICAL/UNPROVEN
// OFFSET: 0x004E4390/0x1401256B0
int32_t PhaseShift_C_GetNumActivePhaseMaps() {
    int32_t count = 0;

    for (uint32_t i = 0; i < CPhaseMapTracker::s_loadedMaps.Count(); i++) {
        if (CPhaseMapTracker::s_loadedMaps[i].m_active) {
            count++;
        }
    }

    return count;
}

// PROVEN: 7987744b IDENTICAL/UNPROVEN
// OFFSET: 0x004E43C0/0x1401256E0
int32_t PhaseShift_C_IsPhasedWorldMapActive(int32_t worldMapAreaID) {
    for (uint32_t i = 0; i < s_phasedWorldMaps.Count(); i++) {
        if (s_phasedWorldMaps[i] == worldMapAreaID) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004E4400/0x140125720
const WorldMapAreaRec* PhaseShift_C_GetPhasedWorldMap(const WorldMapAreaRec* rec) {
    if (!rec) {
        return rec;
    }

    for (uint32_t i = s_phasedWorldMaps.Count(); i;) {
        uint16_t worldMapAreaID = s_phasedWorldMaps[--i];
        WorldMapAreaRec* phased = worldMapAreaID ? g_worldMapAreaDB.GetRecord(worldMapAreaID) : NULL;

        if (phased && phased->m_areaID == rec->m_areaID && phased->m_mapID == rec->m_mapID) {
            return phased;
        }
    }

    return rec;
}

// OFFSET: 0x004E4470/0x1401257E0
int32_t PhaseShift_C_GetPhaseMapAreaTriggers(int32_t index, int32_t* first, int32_t* last) {
    CPhaseMapTracker& tracker = CPhaseMapTracker::s_loadedMaps[index];

    if (!tracker.m_active || tracker.m_firstAreaTrigger == -1) {
        return 0;
    }

    *first = tracker.m_firstAreaTrigger;
    *last = tracker.m_lastAreaTrigger;
    return 1;
}

// OFFSET: 0x004E4550/0x140125910
int32_t CPhaseShift::CalcNormalPhase() const {
    if (!this->m_phases.Count()) {
        return 1;
    }

    for (uint32_t i = 0; i < this->m_phases.Count(); i++) {
        PhaseRec* rec = g_phaseDB.GetRecord(this->m_phases.Ptr()[i]);

        if (rec && (rec->m_flags & 8)) {
            return 1;
        }
    }

    return 0;
}

// PROVEN: d5cf31d2 IDENTICAL/NONE
// OFFSET: 0x004E4640
int32_t CPhaseShift::HasSharedPhase(const CPhaseShift* other) const {
    return PhaseShift_CompareSortedPhaseArray(this->m_flags, this->m_phases.Ptr(), this->m_phases.Count(), other->m_flags, other->m_phases.Ptr(), other->m_phases.Count());
}

// OFFSET: 0x004E4720/0x140125BE0
void PhaseShiftShared_InitPhaseXPhaseGroupIndex() {
    s_phaseGroupIndexInitialized = 1;

    if (!g_phaseXPhaseGroupDB.m_numRecords) {
        return;
    }

    int32_t maxPhaseGroupID = 0;
    PhaseXPhaseGroupRec* last = g_phaseXPhaseGroupDB.GetRecordByIndex(g_phaseXPhaseGroupDB.m_numRecords - 1);

    if (last) {
        maxPhaseGroupID = last->m_phaseGroupID;
    }

    s_phaseGroupLookup.SetCount(maxPhaseGroupID + 1);

    for (int32_t i = 0; i <= maxPhaseGroupID; i++) {
        s_phaseGroupLookup[i].m_first = -1;
        s_phaseGroupLookup[i].m_count = 0;
    }

    for (int32_t i = 0; i < g_phaseXPhaseGroupDB.m_numRecords; i++) {
        PhaseXPhaseGroupRec* rec = g_phaseXPhaseGroupDB.GetRecordByIndex(i);

        if (s_phaseGroupLookup[rec->m_phaseGroupID].m_first == -1) {
            s_phaseGroupLookup[rec->m_phaseGroupID].m_first = i;
        }

        s_phaseGroupLookup[rec->m_phaseGroupID].m_count++;
    }
}

// PROVEN: 920e41ea IDENTICAL/UNPROVEN
// OFFSET: 0x004E4850/0x140125D40
int32_t PhaseShift_C_HasSharedPhase(const CPhaseShift* phaseShift) {
    return s_phaseShift.HasSharedPhase(phaseShift);
}

// OFFSET: 0x004E4870/0x140125DB0
CPhaseMapTracker* CPhaseMapTracker::LoadMap(int32_t mapID, int32_t active) {
    if (!mapID) {
        return NULL;
    }

    CPhaseMapTracker* tracker = NULL;

    for (uint32_t i = 0; i < CPhaseMapTracker::s_loadedMaps.Count(); i++) {
        if (CPhaseMapTracker::s_loadedMaps[i].m_mapID == mapID) {
            CPhaseMapTracker::s_loadedMaps[i].m_used = 1;
            tracker = &CPhaseMapTracker::s_loadedMaps[i];
        }
    }

    if (!tracker) {
        for (uint32_t i = 0; i < CPhaseMapTracker::s_loadedMaps.Count(); i++) {
            if (!CPhaseMapTracker::s_loadedMaps[i].m_mapID) {
                tracker = &CPhaseMapTracker::s_loadedMaps[i];
                break;
            }
        }

        if (!tracker) {
            tracker = CPhaseMapTracker::s_loadedMaps.New();

            if (!tracker) {
                return NULL;
            }
        }

        if (World::AddMapById(mapID, s_syncMapLoad)) {
            tracker->SetMap(mapID);
        }
    }

    if (active) {
        if (!tracker->m_active) {
            tracker->m_active = World::SetMapActive(tracker->m_mapID, 1);
        }
    } else if (tracker->m_active) {
        tracker->m_active = 0;
        World::SetMapActive(tracker->m_mapID, 0);
    }

    return tracker;
}

// PROVEN: ad6641b3 IDENTICAL/UNPROVEN
// OFFSET: 0x004E49F0/0x140125F40
void CPhaseMapTracker::UpdateMaps(uint32_t activeCount, const uint16_t* activeMaps, uint32_t inactiveCount, const uint16_t* inactiveMaps) {
    for (uint32_t i = 0; i < activeCount; i++) {
        CPhaseMapTracker::LoadMap(activeMaps[i], 1);
    }

    for (uint32_t i = 0; i < inactiveCount; i++) {
        uint32_t j;

        for (j = 0; j < CPhaseMapTracker::s_loadedMaps.Count(); j++) {
            if (CPhaseMapTracker::s_loadedMaps[j].m_mapID == inactiveMaps[i] && CPhaseMapTracker::s_loadedMaps[j].m_active) {
                break;
            }
        }

        if (j == CPhaseMapTracker::s_loadedMaps.Count()) {
            CPhaseMapTracker::LoadMap(inactiveMaps[i], 0);
        }
    }

    s_syncMapLoad = 0;
    CPhaseMapTracker::RemoveUnusedMaps();
    MinimapSetForceUpdate();
    SI3::ZoneSoundsPhaseChanged();
}

// OFFSET: 0x004E4A90/0x140126070
int32_t PhaseShift_C_HasSharedPhase(const AreaTriggerRec* rec) {
    if (static_cast<uint32_t>(rec->m_id) >= s_areaTriggerPhases.Count()) {
        return s_phaseShift.HasSharedPhase(&s_defaultPhaseShift);
    }

    return s_phaseShift.HasSharedPhase(&s_areaTriggerPhases[rec->m_id]);
}

// OFFSET: 0x004E4AD0/0x1401260F0
int32_t PhaseShift_C_HasSharedPhase(const WorldStateUIRec* rec) {
    if (static_cast<uint32_t>(rec->m_id) >= s_worldStateUIPhases.Count()) {
        return s_phaseShift.HasSharedPhase(&s_defaultPhaseShift);
    }

    return s_phaseShift.HasSharedPhase(&s_worldStateUIPhases[rec->m_id]);
}

// OFFSET: 0x004E4BD0/0x140126320
void CPhaseShift::SetPhase(uint32_t flags, uint16_t phase) {
    this->m_flags = flags;

    if (!phase || phase == 169) {
        if (flags & 2) {
            this->m_phases.SetCount(0);
            this->m_flags |= 4;
        } else {
            this->m_phases.SetCount(0);
            this->m_flags |= 8;
        }
    } else {
        this->m_phases.SetCount(1);
        this->m_phases.Ptr()[0] = phase;
    }

    uint32_t current = this->m_flags;

    if (current & 2) {
        if (!(current & 4) && this->CalcNormalPhase()) {
            this->m_flags = current | 4;
        }
    } else if (!(current & 8) && this->CalcNormalPhase()) {
        this->m_flags = current | 8;
    }
}

// OFFSET: 0x004E4C90/0x140126440
void CPhaseShift::SetPhasesFromDB(uint32_t flags, uint32_t phaseID, uint32_t phaseGroupID) {
    uint32_t phaseFlags = 0;

    if (flags & 1) {
        phaseFlags = 9;
    }

    if (flags & 2) {
        phaseFlags |= 2;
    }

    if (phaseID) {
        this->SetPhase(phaseFlags, phaseID);
        return;
    }

    if (phaseGroupID) {
        int32_t first;
        int32_t count;

        if (GetPhaseGroupLookup(phaseGroupID, &first, &count) && count) {
            uint16_t* phases = static_cast<uint16_t*>(alloca(sizeof(uint16_t) * count));

            for (int32_t i = 0; i < count; i++) {
                phases[i] = g_phaseXPhaseGroupDB.GetRecordByIndex(first + i)->m_phaseID;
            }

            this->m_flags = phaseFlags;
            this->m_phases.SetCount(count);
            memcpy(this->m_phases.Ptr(), phases, sizeof(uint16_t) * count);
        }
    } else {
        this->m_flags = phaseFlags;
    }

    uint32_t current = this->m_flags;

    if (current & 2) {
        if (!(current & 4) && this->CalcNormalPhase()) {
            this->m_flags = current | 4;
        }
    } else if (!(current & 8) && this->CalcNormalPhase()) {
        this->m_flags = current | 8;
    }
}

// PROVEN: 2dc02795 IDENTICAL/UNPROVEN
// OFFSET: 0x004E4DF0/0x1401265E0
void PhaseShift_C_LeaveWorld(int32_t reload) {
    if (reload) {
        return;
    }

    s_syncMapLoad = 1;
    CPhaseMapTracker::LeaveWorld();
    s_phaseShift.m_phases.SetCount(0);
    s_phaseShift.m_flags = 8;
}

// PROVEN: 0c9935da IDENTICAL/NONE
// OFFSET: 0x004E53B0
static void InitAreaTriggerPhases() {
    s_areaTriggerPhases.SetCount(g_areaTriggerDB.m_maxID + 1);

    for (int32_t i = 0; i < g_areaTriggerDB.m_numRecords; i++) {
        AreaTriggerRec* rec = g_areaTriggerDB.GetRecordByIndex(i);
        s_areaTriggerPhases[rec->m_id].SetPhasesFromDB(rec->m_phaseUseFlags, rec->m_phaseID, rec->m_phaseGroupID);
    }
}

// OFFSET: 0x004E5490/0x140126F10
void PhaseShift_C_InitDB() {
    PhaseShiftShared_InitPhaseXPhaseGroupIndex();
    InitAreaTriggerPhases();
    s_worldStateUIPhases.SetCount(g_worldStateUIDB.m_maxID + 1);

    for (int32_t i = 0; i < g_worldStateUIDB.m_numRecords; i++) {
        WorldStateUIRec* rec = g_worldStateUIDB.GetRecordByIndex(i);
        s_worldStateUIPhases[rec->m_id].SetPhasesFromDB(rec->m_phaseUseFlags, rec->m_phaseID, rec->m_phaseGroupID);
    }
}

// PROVEN: 9c415678 IDENTICAL/UNPROVEN
// OFFSET: 0x004E54A0/0x140126F30
void PhaseShift_C_CleanupDB() {
    s_phaseGroupLookup.Clear();
    s_areaTriggerPhases.Clear();
    s_worldStateUIPhases.Clear();
}

// OFFSET: 0x004E4E40/0x140126640
static void SetPhasedWorldMaps(uint32_t count, const uint16_t* maps) {
    uint32_t merged = 0;

    if (!count || !maps) {
        s_phasedWorldMaps.Clear();
        return;
    }

    uint16_t* result = static_cast<uint16_t*>(alloca(sizeof(uint16_t) * count));
    TSStackArray<uint16_t> incoming(static_cast<uint16_t*>(alloca(sizeof(uint16_t) * count)), count, 1);
    incoming.Set(count, maps);

    for (uint32_t i = 0; i < s_phasedWorldMaps.m_count; i++) {
        uint16_t map = s_phasedWorldMaps.m_data[i];

        for (uint32_t j = 0; j < count; j++) {
            if (map == incoming.m_data[j]) {
                result[merged++] = map;
                incoming.m_data[j] = 0;
                break;
            }
        }
    }

    uint32_t kept = merged;

    for (uint32_t i = 0; i < count; i++) {
        uint16_t map = incoming.m_data[i];

        if (!map) {
            continue;
        }

        uint32_t j = kept;

        while (j < merged && map != result[j]) {
            j++;
        }

        if (j >= merged) {
            result[merged++] = map;
        }
    }

    if (merged) {
        s_phasedWorldMaps.Set(merged, result);
    } else {
        s_phasedWorldMaps.Clear();
    }
}

// OFFSET: 0x004E4FD0/0x1401268A0
void CPhaseShift::SetPhasesFromMessage(ClientPhaseShiftChange* msg) {
    uint32_t count = msg->m_data.m_phaseIDs.m_size >> 1;
    void* data = NULL;

    if (count) {
        msg->m_data.m_phaseIDs.GetDataInSitu(data, count);
        msg->m_data.m_phaseIDs.m_read = 0;
    }

    this->m_flags = msg->m_data.m_flags;
    this->m_phases.SetCount(count);

    if (data) {
        memcpy(this->m_phases.Ptr(), data, sizeof(uint16_t) * count);
    }
}

// OFFSET: 0x004E5060/0x140126960
static bool OnPhaseShiftChange(ClientPhaseShiftChange* msg) {
    uint32_t activeCount = msg->m_data.m_visibleMapIDs.m_size >> 1;
    void* activeMaps = NULL;

    if (activeCount) {
        msg->m_data.m_visibleMapIDs.GetDataInSitu(activeMaps, 2 * activeCount);
    }

    uint32_t inactiveCount = msg->m_data.m_preloadMapIDs.m_size >> 1;
    void* inactiveMaps = NULL;

    if (inactiveCount) {
        msg->m_data.m_preloadMapIDs.GetDataInSitu(inactiveMaps, 2 * inactiveCount);
    }

    uint32_t worldMapCount = msg->m_data.m_uiWorldMapAreaIDSwaps.m_size >> 1;
    void* worldMaps = NULL;

    if (worldMapCount) {
        msg->m_data.m_uiWorldMapAreaIDSwaps.GetDataInSitu(worldMaps, 2 * worldMapCount);
    }

    SetPhasedWorldMaps(worldMapCount, static_cast<const uint16_t*>(worldMaps));
    CPhaseShift phaseShift;
    phaseShift.SetPhasesFromMessage(msg);

    if (!(s_phaseShift == phaseShift)) {
        s_phaseShift = phaseShift;
        CGWorldStateInfo::ResetWorldStateUI();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_WORLD_STATES, NULL);
    }

    CPhaseMapTracker::UpdateMaps(activeCount, static_cast<const uint16_t*>(activeMaps), inactiveCount, static_cast<const uint16_t*>(inactiveMaps));
    return true;
}

// OFFSET: 0x004E52B0/0x140126C40
void PhaseShift_C_InitializeGame() {
    ClientPhaseShiftChange::s_handler = JamHandlerEncode(OnPhaseShiftChange, g_jamHandlerKey_CFD350);
}
