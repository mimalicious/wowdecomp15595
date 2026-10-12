#include "CGWorldStateInfo.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <ctime>
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/QuestTextParser.h"

// STATIC: 0x00F61038/0x140E56140
uint32_t CGWorldStateInfo::m_nextUITimeUpdate;

// STATIC: 0x00F6103C/0x140E56148
uint32_t CGWorldStateInfo::m_UITimeOffset;

// STATIC: 0x00F61040/0x140E56144
uint32_t CGWorldStateInfo::m_nextUpdateTime;

// STATIC: 0x00F61044/0x140E56150
int32_t CGWorldStateInfo::m_subAreaID;

// STATIC: 0x00F61048/0x140E56154
int32_t CGWorldStateInfo::m_areaID;

// STATIC: 0x00F6104C/0x140E56158
int32_t CGWorldStateInfo::m_mapID;

// STATIC: 0x00F61050/0x140BD0260
TSHashTable<WORLDSTATE, HASHKEY_NONE> CGWorldStateInfo::m_worldStates;

// STATIC: 0x00F6107C/0x140E56164
TSGrowableArray<CGWorldStateInfo::WorldStateUIData> CGWorldStateInfo::m_worldStateUI;

// PROVEN: bf14cfc0 IDENTICAL/DIFFERENT
// OFFSET: 0x009CD470/0x14075DB50
void CGWorldStateInfo::InitializeGame() {
    CGWorldStateInfo::m_mapID = -1;
    CGWorldStateInfo::m_areaID = 0;
    CGWorldStateInfo::m_nextUpdateTime = FrameTime::GetCurTimeMs();
    CGWorldStateInfo::m_UITimeOffset = 0;
    CGWorldStateInfo::m_nextUITimeUpdate = 0;
}

// PROVEN: cab1ad26 IDENTICAL/DIFFERENT
// OFFSET: 0x009CD5D0/0x14075DD30
int32_t CGWorldStateInfo::GetUITime() {
    return CGWorldStateInfo::m_UITimeOffset + static_cast<int32_t>(time(NULL));
}

// OFFSET: 0x009CD850/0x14075E010
int32_t GetWorldState(int32_t id) {
    WORLDSTATE* state = CGWorldStateInfo::m_worldStates.Ptr(id, HASHKEY_NONE());

    if (state) {
        return state->m_value;
    }

    return 0;
}

// OFFSET: 0x009CD630/0x14075DD80
void CGWorldStateInfo::RequestUITime() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_UI_TIME_REQUEST));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009CD7B0/0x14075DF60
void CGWorldStateInfo::TimerUpdate() {
    uint32_t now = FrameTime::GetCurTimeMs();

    if (static_cast<int32_t>(now - CGWorldStateInfo::m_nextUITimeUpdate) >= 0) {
        CGWorldStateInfo::m_nextUITimeUpdate = now + 20000;
        CGWorldStateInfo::RequestUITime();
    }

    if (static_cast<int32_t>(now - CGWorldStateInfo::m_nextUpdateTime) < 0) {
        return;
    }

    CGWorldStateInfo::m_nextUpdateTime = now + 1000;

    for (uint32_t i = 0; i < CGWorldStateInfo::m_worldStateUI.m_count; i++) {
        if (CGWorldStateInfo::m_worldStateUI.m_data[i].m_state == 3) {
            FrameScript_SignalEvent(SCRIPT_EVENT_WORLD_STATE_UI_TIMER_UPDATE, NULL);
            return;
        }
    }
}

// PROVEN: 45783951 IDENTICAL/IDENTICAL
// OFFSET: 0x009CD820/0x14075DFE0
bool CGWorldStateInfo::InitWorldStates(uint32_t mapId, uint32_t zoneId, uint32_t areaId) {
    CGWorldStateInfo::m_mapID = mapId;
    CGWorldStateInfo::m_areaID = zoneId;
    CGWorldStateInfo::m_subAreaID = areaId;
    CGWorldStateInfo::ResetWorldStateUI();
    return true;
}

// OFFSET: 0x009CD880/0x14075E0D0
bool CGWorldStateInfo::GetWorldStateKnown(int32_t id) {
    return CGWorldStateInfo::m_worldStates.Ptr(id, HASHKEY_NONE()) != NULL;
}

// OFFSET: 0x009CE3B0/0x14075F030
bool CGWorldStateInfo::SetWorldState(uint32_t state, uint32_t value, bool hidden) {
    WORLDSTATE* worldState = CGWorldStateInfo::m_worldStates.Ptr(state, HASHKEY_NONE());

    if (!worldState) {
        worldState = CGWorldStateInfo::m_worldStates.New(state, HASHKEY_NONE(), 0, 0);
    }

    if (worldState->m_value == static_cast<int32_t>(value) && worldState->m_hidden == static_cast<uint8_t>(hidden)) {
        return false;
    }

    worldState->m_value = value;
    worldState->m_hidden = hidden;

    if (state == 3191) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ARENA_SEASON_WORLD_STATE, NULL);
    }

    RefreshWorldStatePOIs(state);
    CGGameUI::OnWorldStateChanged(state);
    return true;
}

// PROVEN: d4dff9de IDENTICAL/DIFFERENT
// OFFSET: 0x009CD5B0/0x14075DD10
void CGWorldStateInfo::OnUITime(uint32_t time) {
    CGWorldStateInfo::m_UITimeOffset = time - static_cast<uint32_t>(::time(NULL));
}

// OFFSET: 0x009CD6A0/0x14075DE00
void CGWorldStateInfo::ResetWorldStateUI() {
    CGWorldStateInfo::m_worldStateUI.m_count = 0;

    for (int32_t i = 0; i < g_worldStateUIDB.m_numRecords; i++) {
        WorldStateUIRec* rec = g_worldStateUIDB.GetRecordByIndex(i);

        if (rec->m_mapID != -1 && rec->m_mapID != CGWorldStateInfo::m_mapID) {
            continue;
        }

        if (rec->m_areaID && rec->m_areaID != CGWorldStateInfo::m_areaID && rec->m_areaID != CGWorldStateInfo::m_subAreaID) {
            continue;
        }

        if (!PhaseShift_C_HasSharedPhase(rec)) {
            continue;
        }

        if (rec->m_type && rec->m_type != 3 && rec->m_type != 1) {
            continue;
        }

        WorldStateUIData* entry = CGWorldStateInfo::m_worldStateUI.New();
        entry->m_id = rec->m_id;
        entry->m_state = rec->m_type;
    }
}

// PROVEN: 9bb6698c IDENTICAL/NONE
// OFFSET: 0x009CDC70
void CGWorldStateInfo::ShutdownGame() {
    CGWorldStateInfo::m_worldStates.InternalClear(0);
    CGWorldStateInfo::m_worldStateUI.Clear();
}

// PROVEN: ff93c3fb IDENTICAL/DIFFERENT
// OFFSET: 0x009CDCC0/0x14075E620
void CGWorldStateInfo::LeaveWorld(bool reload) {
    if (!reload) {
        CGWorldStateInfo::m_worldStates.InternalClear(0);
        CGWorldStateInfo::m_worldStateUI.Clear();
    }
}

// OFFSET: 0x00992F50/0x140712960
bool WorldStateParseText(const char* text, char* buffer, uint32_t size) {
    if (!text) {
        return false;
    }

    int32_t error = 0;
    *buffer = '\0';
    const char* start = text;

    for (const char* percent = SStrChr(text, '%'); percent && *percent; percent = SStrChr(start, '%')) {
        uint32_t length = percent - start;

        if (length) {
            uint32_t used = SStrLen(buffer);
            SStrPack(buffer, start, size);

            if (used + length < size) {
                buffer[used + length] = '\0';
            }
        }

        const char* cursor = percent + 1;
        const char* number = cursor;

        while (*cursor >= '0' && *cursor <= '9') {
            cursor++;
        }

        switch (*cursor) {
            case 'K':
            case 'k':
                ParseCountDownTimerText(buffer, size, GetWorldState(SStrToInt(number)));
                cursor++;
                break;

            case 'W':
            case 'w': {
                char value[16];
                SStrPrintf(value, sizeof(value), "%d", GetWorldState(SStrToInt(number)));
                SStrPack(buffer, value, size);
                cursor++;
                break;
            }

            default:
                error = 1;
                SStrPack(buffer, "%", size);
                break;
        }

        start = cursor;
    }

    SStrPack(buffer, start, size);
    return error == 0;
}
