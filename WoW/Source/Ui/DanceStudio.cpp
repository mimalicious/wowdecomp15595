#include "DanceStudio.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/DanceMovesRec.h"
#include "WoW/Source/Object/DanceCache.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"

// STATIC: 0x00F4D578/0x140E41F40
uint64_t CGDanceStudio::m_learnedDanceMoves;

// STATIC: 0x00F4D580/0x140BCF320
static TSHashTable<KNOWNDANCES, HASHKEY_STRI> s_knownDances;

// OFFSET: 0x009B8C40/0x140743780
int32_t Packet_SMSG_NOTIFY_DANCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t flags;
    msg->Get(flags);

    uint32_t error = 0;
    uint32_t danceId = 0;
    uint32_t unk = 0;
    char name[128];

    if (flags & 8) {
        msg->Get(error);
    } else {
        msg->Get(danceId);
        msg->GetString(name, 0x7FFFFFFF);
        msg->Get(unk);
    }

    if (flags & 1) {
        KNOWNDANCES* dance = s_knownDances.Ptr(name);

        if (!dance) {
            dance = s_knownDances.New(name, 0, 0);
        }

        dance->m_id = danceId;
        dance->unk_001C = unk;
    }

    if (flags & 2) {
        KNOWNDANCES* dance = s_knownDances.m_fulllist.Head();

        while (dance) {
            KNOWNDANCES* next = s_knownDances.Next(dance);

            if (dance->m_id == danceId) {
                s_knownDances.Delete(dance);
            }

            dance = next;
        }

        dance = s_knownDances.New(name, 0, 0);
        dance->m_id = danceId;
        dance->unk_001C = unk;
        g_danceCache.Invalidate(danceId);
    }

    if (flags & 4) {
        KNOWNDANCES* dance = s_knownDances.m_fulllist.Head();

        while (dance) {
            KNOWNDANCES* next = s_knownDances.Next(dance);

            if (dance->m_id == danceId) {
                s_knownDances.Delete(dance);
            }

            dance = next;
        }

        g_danceCache.Invalidate(danceId);
    }

    if (flags & 8) {
        if (error == 0) {
            CGGameUI::DisplayError(ERR_DANCE_CREATE_DUPLICATE);
        } else if (error == 1) {
            CGGameUI::DisplayError(ERR_DANCE_SAVE_FAILED);
        } else if (error == 2) {
            CGGameUI::DisplayError(ERR_DANCE_DELETE_FAILED);
        }
    }

    return 1;
}

// OFFSET: 0x009B8FD0/0x140743DF0
void CGDanceStudio::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_PLAY_DANCE, Packet_SMSG_PLAY_DANCE, NULL);
    ClientServices::SetMessageHandler(SMSG_STOP_DANCE, Packet_SMSG_STOP_DANCE, NULL);
    ClientServices::SetMessageHandler(SMSG_NOTIFY_DANCE, Packet_SMSG_NOTIFY_DANCE, NULL);
    ClientServices::SetMessageHandler(SMSG_LEARNED_DANCE_MOVES, Packet_SMSG_LEARNED_DANCE_MOVES, NULL);
}

// PROVEN: e5087bb4 IDENTICAL/IDENTICAL
// OFFSET: 0x009B7D70/0x1407422E0
int32_t Packet_SMSG_LEARNED_DANCE_MOVES(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(CGDanceStudio::m_learnedDanceMoves);
    return 1;
}

// PROVEN: 9ded33b6 IDENTICAL/NONE
// OFFSET: 0x009B8540
void CGDanceStudio::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_PLAY_DANCE);
    ClientServices::ClearMessageHandler(SMSG_STOP_DANCE);
    ClientServices::ClearMessageHandler(SMSG_NOTIFY_DANCE);
    ClientServices::ClearMessageHandler(SMSG_LEARNED_DANCE_MOVES);
    s_knownDances.InternalClear(0);
}

// OFFSET: 0x009B7C90
static bool IsDanceMoveAvailable(const DanceMovesRec* move, uint8_t race) {
    if (!move) {
        return false;
    }

    if (move->m_racemask && !((1 << (race - 1)) & move->m_racemask)) {
        return false;
    }

    return !move->m_lockID || (CGDanceStudio::m_learnedDanceMoves & (1ull << (move->m_lockID - 1)));
}

// PROVEN: 2a9da619 IDENTICAL/DIFFERENT
// OFFSET: 0x009B7D20/0x140742290
int32_t Packet_SMSG_STOP_DANCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->StopDance();
    }

    return 1;
}

// OFFSET: 0x009B7D90/0x140742300
void CGDanceStudio::PlayDance(DanceCache* record, DANCE_REQUEST* request) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(request->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (player->m_dance) {
        ConsoleWrite("WARNING! Cannot start a new dance while one is in progress", ERROR_COLOR);
        return;
    }

    PlayerDance* dance = new (SMemAlloc(sizeof(PlayerDance), __FILE__, __LINE__, 0)) PlayerDance;
    dance->m_count = record->m_dataCount;
    dance->m_moves = new (__FILE__, __LINE__) int16_t[dance->m_count];
    int32_t count = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(dance->m_count); i++) {
        if (i < record->m_dataCount && record->m_data[i].m_chance && GetRandomValue(100, &request->m_seed) >= record->m_data[i].m_chance) {
            continue;
        }

        if (i < record->m_dataCount && record->m_data[i].m_type == 1) {
            DanceMovesRec* move = g_danceMovesDB.GetRecord(record->m_data[i].m_moveId);

            if (move) {
                dance->m_moves[count] = move->m_fallback;
            }
        } else {
            dance->m_moves[count] = i < record->m_dataCount ? record->m_data[i].m_moveId : 0;
        }

        count++;
    }

    dance->m_count = count;
    dance->m_start = request->m_start;
    player->StartDance(dance);
}

// OFFSET: 0x009B7F00/0x1407424E0
static void DanceSequenceCallBack(int32_t id, const WOWGUID& guid, void* param, bool success) {
    DANCE_REQUEST* request = static_cast<DANCE_REQUEST*>(param);

    if (success) {
        WOWGUID none = WOWGUID();
        DanceCache* record = g_danceCache.GetRecord(id, none, NULL, NULL, false);

        if (ClntObjMgrObjectPtr(request->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
            CGDanceStudio::PlayDance(record, request);
        }
    }

    delete request;
}

// OFFSET: 0x009B7F80/0x140742570
int32_t Packet_SMSG_PLAY_DANCE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    int32_t danceId;
    uint32_t start;
    uint32_t seed;
    uint32_t version;
    msg->Get(guid.m_guid);
    msg->Get(reinterpret_cast<uint32_t&>(danceId));
    msg->Get(start);
    msg->Get(seed);
    msg->Get(version);

    WOWGUID none = WOWGUID();
    DanceCache* record = g_danceCache.GetRecord(danceId, none, NULL, NULL, false);

    if (record && record->unk_0098 == version) {
        DANCE_REQUEST request;
        request.m_danceId = danceId;
        InitializeRandomSeed(&request.m_seed, seed);
        request.m_start = start;
        request.m_guid = guid;
        CGDanceStudio::PlayDance(record, &request);
        return 1;
    }

    DANCE_REQUEST* request = new (SMemAlloc(sizeof(DANCE_REQUEST), __FILE__, __LINE__, 0)) DANCE_REQUEST;
    request->m_danceId = danceId;
    InitializeRandomSeed(&request->m_seed, seed);
    request->m_start = start;
    request->m_guid = guid;
    g_danceCache.GetRecord(danceId, none, DanceSequenceCallBack, request, false);
    return 1;
}

// OFFSET: 0x009B80B0/0x1407426D0
void CGDanceStudio::DisplayDance(DanceCache* record) {
    ConsoleWrite("Dance Loaded:", DEFAULT_COLOR);
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t i = 0; i < record->m_dataCount; i++) {
        int32_t id = *reinterpret_cast<const int32_t*>(&record->m_data[i]);
        DanceMovesRec* move = g_danceMovesDB.GetRecord(id);

        if (move) {
            char text[64];
            SStrPrintf(text, sizeof(text), "%d) Move: %s Chance: %d%%", i + 1, static_cast<const char*>(move->m_internalName), i < record->m_dataCount ? record->m_data[i].m_chance : 0);
            COLOR_T color = DEFAULT_COLOR;

            if (!IsDanceMoveAvailable(move, player->m_unit->m_unitFieldBytes0[0])) {
                color = ERROR_COLOR;
            }

            ConsoleWrite(text, color);
        } else {
            char text[128];
            SStrPrintf(text, sizeof(text), "Unknown dance move (ID: %u)!", id);
            ConsoleWrite(text, DEFAULT_COLOR);
        }
    }
}

// OFFSET: 0x009B81E0/0x1407428A0
static void DanceSequenceLoadCallBack(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        ConsoleWrite("Dance no longer exists", ERROR_COLOR);
        return;
    }

    WOWGUID none = WOWGUID();
    DanceCache* record = g_danceCache.GetRecord(id, none, NULL, NULL, false);

    if (record) {
        CGDanceStudio::DisplayDance(record);
    }
}

// OFFSET: 0x009B8240/0x140742900
void CGDanceStudio::LoadDance(int32_t danceId) {
    g_danceCache.Invalidate(danceId);
    WOWGUID none = WOWGUID::Make(0);
    DanceCache* record = g_danceCache.GetRecord(danceId, none, DanceSequenceLoadCallBack, NULL, false);

    if (record) {
        CGDanceStudio::DisplayDance(record);
    }
}

// OFFSET: 0x009B8290/0x140742960
void PlayDance(const char* name) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (player->m_dance) {
        ConsoleWrite("WARNING! Cannot start a new dance while one is in progress", ERROR_COLOR);
        return;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PLAY_DANCE));
    KNOWNDANCES* dance = s_knownDances.Ptr(name);

    if (dance) {
        msg.Put(dance->m_id);
        msg.Put(dance->unk_001C);
        msg.Finalize();
        ClientServices::SendPacket(&msg);
    } else {
        ConsoleWrite("You haven't created a dance with that name", DEFAULT_COLOR);
    }
}

// PROVEN: ff4cf9df IDENTICAL/DIFFERENT
// OFFSET: 0x009B8360/0x140742B10
bool CGDanceStudio::HasDance(const char* name) {
    return s_knownDances.Ptr(name) != NULL;
}

// PROVEN: 1b286450 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGDanceStudio::LeaveWorld() {
}
