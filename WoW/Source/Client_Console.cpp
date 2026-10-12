#include <cstdint>
#include "WoW/Source/JamClientAutoCode/JamPlayerMove.h"
#include "Client.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/GameClientCommands.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x00401730/0x140001480
int32_t CCommand_ReloadUI(const char* command, const char* arguments) {
    CGlueMgr::m_reload = 1;
    CGGameUI::Reload();
    return 1;
}

// OFFSET: 0x009FCE20/0x14090FD50
int32_t CCommand_Perf(const char* command, const char* arguments) {
    return 1;
}

// OFFSET: 0x00402660/0x140005500
int32_t CCommand_TimingInfo(const char* command, const char* arguments) {
    int32_t desired = CVar::LookupRegistered("timingMethod")->m_intValue;
    OsTimeManager::TimingMethod selected = OsTimeGetTimingMethod();
    int32_t testError = CVar::LookupRegistered("timingTestError")->m_intValue;

    ConsolePrintf("Timing method desired: %d - %s", desired, GetTimingMethodName(static_cast<OsTimeManager::TimingMethod>(desired)));
    ConsolePrintf("Timing method selected: %d - %s", selected, GetTimingMethodName(selected));
    ConsolePrintf("Timing test error: %d", testError);

    return 1;
}

// OFFSET: 0x00408550/0x14000A880
int32_t CCommand_TogglePlayer(const char* command, const char* arguments) {
    if (CGWorldFrame::s_currentWorldFrame) {
        ConsoleWrite(Player_C_TogglePlayerRender() ? "player visible" : "player hidden", DEFAULT_COLOR);
    }

    return 1;
}

// OFFSET: 0x0041EE40
CDataStore& operator<<(CDataStore& msg, const C3Vector& vec) {
    msg.Put(vec.x);
    msg.Put(vec.y);
    msg.Put(vec.z);
    return msg;
}

// OFFSET: 0x00408710/0x14000AA50
static int32_t TeleportCheat(const char* arguments, int32_t charm) {
    C3Vector position;
    const char whitespace[] = "\t\r\n\" ,";
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 1;
    }

    if (*arguments && !IsNumeric(*arguments) && *arguments != '-') {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_TELEPORT_TO_UNIT));
        msg.PutString(arguments);
        msg.Finalize();
        ClientServices::SendPacket(&msg);
        return 1;
    }

    char token[64];
    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

    if (!token[0]) {
        ConsoleWrite("Usage: port [x y z] [facing]", WARNING_COLOR);
        return 0;
    }

    position.x = SStrToFloat(token);
    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

    if (!token[0]) {
        return 0;
    }

    position.y = SStrToFloat(token);
    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);
    position.z = token[0] ? SStrToFloat(token) : 0.0f;

    if (!World::IsValidPosition(position, 0.0f)) {
        ConsoleWrite("Coordinates out of range\n", DEFAULT_COLOR);
        return 1;
    }

    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);
    float facing;

    if (token[0]) {
        facing = CMath::normalizeangle0to2pi_(SStrToFloat(token) * 0.017453292f);
    } else {
        facing = player->GetFacing();
    }

    if (charm) {
        PlayerMoveCharmPortCheat msg(position, facing);
        ClientServices::Send(&msg);
        return 1;
    }

    PlayerMoveTeleportCheat msg(position, facing);
    ClientServices::Send(&msg);
    return 1;
}

// OFFSET: 0x00408950/0x14000AD90
int32_t CCommand_Teleport(const char* command, const char* arguments) {
    return TeleportCheat(arguments, 0);
}

// OFFSET: 0x00408970/0x14000ADA0
int32_t CCommand_CharmTeleport(const char* command, const char* arguments) {
    return TeleportCheat(arguments, 1);
}

// OFFSET: 0x00408990/0x14000ADB0
int32_t CCommand_WorldTeleport(const char* command, const char* arguments) {
    const char whitespace[] = "\t\r\n\" ";
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 1;
    }

    uint32_t continent;
    C3Vector position;
    float facing;

    if (IsNumeric(*arguments)) {
        char token[64];
        SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

        if (!token[0]) {
            ConsoleWrite("Usage: worldport <continentID> [x y z] [facing]", WARNING_COLOR);
            return 0;
        }

        continent = SStrToUnsigned(token);
        position = player->GetPosition();

        SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

        if (token[0]) {
            position.x = SStrToFloat(token);
        }

        SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

        if (token[0]) {
            position.y = SStrToFloat(token);
        }

        SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

        if (token[0]) {
            position.z = SStrToFloat(token);
        }

        SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);
        facing = token[0] ? SStrToFloat(token) * 0.017453292f : player->GetFacing();
    } else {
        WorldSafeLocsRec* loc = NULL;
        int32_t i;

        for (i = 0; i < g_worldSafeLocsDB.m_numRecords; i++) {
            loc = g_worldSafeLocsDB.GetRecordByIndex(i);

            if (SStrStrI(loc->m_areaName, arguments)) {
                break;
            }
        }

        if (i >= g_worldSafeLocsDB.m_numRecords) {
            ConsolePrintf("Could not find location: %s", arguments);
            return 1;
        }

        continent = loc->m_continent;
        position.x = loc->m_loc[0];
        position.y = loc->m_loc[1];
        position.z = loc->m_loc[2];
        ConsolePrintf("Porting to %s", static_cast<const char*>(loc->m_areaName));
        facing = player->GetFacing();
    }

    if (!g_mapDB.GetRecord(continent)) {
        ConsoleWriteA("Bad world number: %i\n", ERROR_COLOR, continent);
        return 0;
    }

    uint32_t time = OsGetAsyncTimeMs();

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_WORLD_TELEPORT));
    msg.Put(time);
    msg.Put(continent);
    msg.Put(static_cast<uint64_t>(0));
    msg.Put(position.x);
    msg.Put(position.y);
    msg.Put(position.z);
    msg.Put(facing);
    msg.Finalize();
    ClientServices::SendPacket(&msg);
    return 1;
}

// OFFSET: 0x00408C50/0x14000B150
int32_t CCommand_SetRawPos(const char* command, const char* arguments) {
    C3Vector rawPosition;
    const char whitespace[] = "\t\r\n\" ,";
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 1;
    }

    if (!*arguments || (!IsNumeric(*arguments) && *arguments != '-')) {
        ConsoleWrite("Usage: setrawpos [x y z] [facing]", WARNING_COLOR);
        return 0;
    }

    char token[64];
    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

    if (!token[0]) {
        return 0;
    }

    rawPosition.x = SStrToFloat(token);
    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);

    if (!token[0]) {
        return 0;
    }

    rawPosition.y = SStrToFloat(token);
    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);
    rawPosition.z = token[0] ? SStrToFloat(token) : 0.0f;

    C3Vector position = player->m_movement->GetPosition(rawPosition);

    if (!World::IsValidPosition(position, 0.0f)) {
        ConsoleWrite("Coordinates out of range\n", DEFAULT_COLOR);
        return 1;
    }

    SStrTokenize(&arguments, token, sizeof(token), whitespace, NULL);
    float facing = token[0] ? SStrToFloat(token) * 0.017453292f : player->GetRawFacing();

    PlayerMoveSetRawPosition msg(rawPosition, facing);
    ClientServices::SendPacket(&msg);
    return 1;
}

// OFFSET: 0x00408E30/0x14000B3D0
int32_t CCommand_ToggleHelm(const char* command, const char* arguments) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SHOWING_HELM));
        msg.Put(static_cast<uint8_t>((player->m_player->m_playerFlags >> 10) & 1));
        msg.Finalize();
        StaticSingleton<CommandManager>::Instance()->InternalSend(&msg);
    }

    return 1;
}

// OFFSET: 0x00408F10/0x14000B4F0
int32_t CCommand_ToggleCloak(const char* command, const char* arguments) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SHOWING_CLOAK));
        msg.Put(static_cast<uint8_t>((player->m_player->m_playerFlags >> 11) & 1));
        msg.Finalize();
        StaticSingleton<CommandManager>::Instance()->InternalSend(&msg);
    }

    return 1;
}
