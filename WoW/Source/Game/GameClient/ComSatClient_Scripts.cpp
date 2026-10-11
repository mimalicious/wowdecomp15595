#include "ComSatClient.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Game/GameClient/ComSatThread.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include <cstring>

// STATIC: 0x00DC0D3C
int32_t s_currentOutputDevice;

// STATIC: 0x00DC0D40
int32_t s_currentCaptureDevice;

// OFFSET: 0x004DC2E0/0x14011A7A0
static int32_t Script_VoiceIsDisabledByClient(lua_State* L) {
    if (s_comsatDisabledByClient) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004DA900/0x140118DE0
static int32_t Script_VoiceGetCurrentCaptureDevice(lua_State* L) {
    if (ComSatThread::s_comsat) {
        lua_pushnumber(L, s_currentCaptureDevice);
    } else {
        lua_pushnumber(L, -1.0);
    }

    return 1;
}

// OFFSET: 0x004DA570/0x1401189F0
static int32_t Script_VoiceEnumerateOutputDevices(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (!comsat) {
        lua_pushnil(L);
        s_comsatDeviceLock.Leave();
        return 1;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (comsat->EnumerateOutputDevices(devices, 20) <= index) {
        lua_pushnil(L);
    } else {
        lua_pushstring(L, devices[index].m_name);
    }

    s_comsatDeviceLock.Leave();
    return 1;
}

// OFFSET: 0x004DA630/0x140118AD0
static int32_t Script_VoiceEnumerateCaptureDevices(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (!comsat) {
        s_comsatDeviceLock.Leave();
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (comsat->EnumerateCaptureDevices(devices, 20) <= index) {
        lua_pushnil(L);
    } else {
        lua_pushstring(L, devices[index].m_name);
    }

    s_comsatDeviceLock.Leave();
    return 1;
}

// OFFSET: 0x004DA6E0/0x140118BC0
static int32_t Script_VoiceSelectOutputDevice(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isstring(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (comsat) {
        const char* name = lua_tolstring(L, 1, NULL);
        int32_t count = comsat->EnumerateOutputDevices(devices, 20);

        for (int32_t i = 0; i < count; i++) {
            if (!strcmp(name, devices[i].m_name)) {
                comsat->LocalTalkerChangeOutputDevice(0, i);
                break;
            }
        }
    }

    s_comsatDeviceLock.Leave();
    return 0;
}

// OFFSET: 0x004DA7D0/0x140118CB0
static int32_t Script_VoiceSelectCaptureDevice(lua_State* L) {
    ComSat::DeviceInfo devices[20];

    if (!lua_isstring(L, 1)) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (comsat) {
        const char* name = lua_tolstring(L, 1, NULL);
        int32_t count = comsat->EnumerateCaptureDevices(devices, 20);

        for (int32_t i = 0; i < count; i++) {
            if (!strcmp(name, devices[i].m_name)) {
                comsat->LocalTalkerChangeCaptureDevice(0, i);
                break;
            }
        }
    }

    s_comsatDeviceLock.Leave();
    return 0;
}

// OFFSET: 0x004DA950/0x140118E20
static int32_t Script_GetNumVoiceSessions(lua_State* L) {
    int32_t count = 0;

    for (COMSATDATA* data = s_comsatData; data < s_comsatData + sizeof(s_comsatData) / sizeof(s_comsatData[0]); data++) {
        if (data->m_sessionID) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x004DAB00/0x140118FC0
static int32_t Script_GetVoiceCurrentSessionID(lua_State* L) {
    if (s_currentSession < 0) {
        lua_pushnil(L);
        return 1;
    }

    int32_t id = 0;

    for (int32_t i = 0; i < 32; i++) {
        if (static_cast<uint32_t>(s_comsatData[i].m_sessionID)) {
            id++;

            if (i == s_currentSession) {
                lua_pushnumber(L, id);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004DA9D0/0x140118E60
static int32_t Script_GetVoiceSessionInfo(lua_State* L) {
    uint32_t id;

    if (!lua_isnumber(L, 1) || (id = static_cast<uint32_t>(lua_tonumber(L, 1))) > 31) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t count = 0;

    for (int32_t i = 0; i < 32; i++) {
        if (!static_cast<uint32_t>(s_comsatData[i].m_sessionID) || ++count != id) {
            continue;
        }

        switch (s_comsatData[i].m_type) {
            case 2:
                lua_pushstring(L, FrameScript_GetText("CHAT_MSG_PARTY", -1, GENDER_NOT_APPLICABLE));
                break;

            case 3:
                lua_pushstring(L, FrameScript_GetText("CHAT_MSG_RAID", -1, GENDER_NOT_APPLICABLE));
                break;

            case 1:
                lua_pushstring(L, FrameScript_GetText("CHAT_MSG_BATTLEGROUND", -1, GENDER_NOT_APPLICABLE));
                break;

            default:
                lua_pushstring(L, s_comsatData[i].m_name);
                break;
        }

        if (i == s_currentSession) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004DAC80/0x1401190F0
static int32_t Script_GetNumVoiceSessionMembersBySessionID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    uint32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (id > 31) {
        return 0;
    }

    uint32_t count = 0;

    for (int32_t i = 0; i < 32; i++) {
        if (static_cast<uint32_t>(s_comsatData[i].m_sessionID) && ++count == id) {
            lua_pushnumber(L, s_comsatData[i].m_numMembers);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004DDF10/0x14011C830
static int32_t Script_UnitIsTalking(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    WOWGUID guid = { 0 };
    CGChat::ChatTokenToGUID(lua_tolstring(L, 1, NULL), guid);

    if (guid && s_talkingPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004DB1F0/0x140119740
static int32_t Script_GetVoiceStatus(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    WOWGUID guid = WOWGUID();
    CGChat::ChatTokenToGUID(lua_tolstring(L, 1, NULL), guid);

    if (!guid) {
        return 0;
    }

    int32_t status;

    if (lua_isstring(L, 2)) {
        int32_t type = ComSatClient_DetermineSessionTypeFromName(lua_tolstring(L, 2, NULL));
        status = ComSatClient_IsMemberInVoiceSession(guid, type, lua_tolstring(L, 2, NULL));
    } else {
        if (s_currentSession < 0) {
            lua_pushnil(L);
            return 1;
        }

        status = ComSatClient_GetMemberVoiceStatus(guid, s_currentSession);
    }

    if (status) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004DA8B0/0x140118DA0
static int32_t Script_VoiceGetCurrentOutputDevice(lua_State* L) {
    if (ComSatThread::s_comsat) {
        lua_pushnumber(L, s_currentOutputDevice);
    } else {
        lua_pushnumber(L, -1.0);
    }

    return 1;
}

// PROVEN: 645851d3 IDENTICAL/NONE
// OFFSET: 0x004DAB90
static int32_t Script_SetActiveVoiceChannelBySessionID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        lua_pushnil(L);
        return 1;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));

    if (id <= 0) {
        CGChat::SetActiveVoiceSession(NULL, 4, -1);
        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (id >= 32) {
        lua_pushnil(L);
        return 1;
    }

    int32_t count = 0;

    for (int32_t i = 0; i < 32; i++) {
        if (s_comsatData[i].m_sessionID && ++count == id) {
            CGChat::SetActiveVoiceSession(s_comsatData[i].m_name, s_comsatData[i].m_type, -1);
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// STATIC: 0x00CA0F20
static FrameScript_Method s_ScriptFunctions[] = {
    { "VoiceEnumerateOutputDevices", &Script_VoiceEnumerateOutputDevices },
    { "VoiceEnumerateCaptureDevices", &Script_VoiceEnumerateCaptureDevices },
    { "VoiceSelectOutputDevice", &Script_VoiceSelectOutputDevice },
    { "VoiceSelectCaptureDevice", &Script_VoiceSelectCaptureDevice },
    { "VoiceGetCurrentOutputDevice", &Script_VoiceGetCurrentOutputDevice },
    { "VoiceGetCurrentCaptureDevice", &Script_VoiceGetCurrentCaptureDevice },
    { "GetVoiceStatus", &Script_GetVoiceStatus },
    { "GetNumVoiceSessions", &Script_GetNumVoiceSessions },
    { "GetVoiceSessionInfo", &Script_GetVoiceSessionInfo },
    { "GetVoiceCurrentSessionID", &Script_GetVoiceCurrentSessionID },
    { "SetActiveVoiceChannelBySessionID", &Script_SetActiveVoiceChannelBySessionID },
    { "GetNumVoiceSessionMembersBySessionID", &Script_GetNumVoiceSessionMembersBySessionID },
    { "GetVoiceSessionMemberInfoBySessionID", &Script_GetVoiceSessionMemberInfoBySessionID },
    { "VoiceIsDisabledByClient", &Script_VoiceIsDisabledByClient },
    { "UnitIsTalking", &Script_UnitIsTalking },
};

// PROVEN: 23a6a688 IDENTICAL/NONE
// OFFSET: 0x004DB2D0/0x140119880
void VoiceRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: d2acb2c7 IDENTICAL/NONE
// OFFSET: 0x004DB300/0x1401198D0
void VoiceUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}

// OFFSET: 0x004DDD50/0x14011C630
int32_t Script_GetVoiceSessionMemberInfoBySessionID(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        return 0;
    }

    uint32_t sessionNumber = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (sessionNumber > 31) {
        return 0;
    }

    uint32_t memberIndex = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (memberIndex > 39) {
        return 0;
    }

    uint32_t found = 0;
    int32_t index;

    for (index = 0; index < 32; index++) {
        if (s_comsatData[index].m_sessionID && ++found == sessionNumber) {
            break;
        }
    }

    if (index >= 32) {
        return 0;
    }

    COMSATMEMBER& member = s_comsatData[index].m_members[memberIndex];
    WOWGUID none = { 0 };
    const char* name = reinterpret_cast<const char*>(g_nameDBCache.GetRecord(member.m_guid.m_guid, none, NULL, NULL, false));

    if (!name) {
        name = FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);
    }

    lua_pushstring(L, name);

    if (member.m_flags & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (member.m_flags & 0x40) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (ComSatClient_IsMutedPlayer(member.m_guid)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (member.m_flags & 0x8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 5;
}
