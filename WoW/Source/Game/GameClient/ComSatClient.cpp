#include "ComSatClient.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/ComSat/ComSatInterface.h"
#include <cstring>
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include <new>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"
#include "WoW/Source/Game/GameClient/ComSatThread.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"

// STATIC: 0x00DC0D50
COMSATDATA s_comsatData[32];

// STATIC: 0x00DC6F50
int32_t s_voiceActivation;

// STATIC: 0x00CA0F98
static ComSatClientAllocator s_comsatAllocator;

// STATIC: 0x00CA0F9C
static ComSatClientCallback s_comsatCallback;

// STATIC: 0x00DC6F54
static float s_voiceFadeStartTime;

// STATIC: 0x00DC6F58
static float s_voiceFadeDelayRate;

// STATIC: 0x00DC6F5C
static float s_voiceFadeRate;

// STATIC: 0x00DC6F60
static int32_t s_talkingPlayerCount;

// STATIC: 0x00DC6F64
static uint8_t s_voiceFadeActive;

// STATIC: 0x00DC6F68
static CVar* s_cvarSoundSFXVolume;

// STATIC: 0x00DC6F6C
static CVar* s_cvarSoundMusicVolume;

// STATIC: 0x00DC6F70
static CVar* s_cvarSoundAmbienceVolume;

// STATIC: 0x00DC6F74
static CVar* s_cvarChatSoundVolume;

// STATIC: 0x00DC6F78
static CVar* s_cvarChatMusicVolume;

// STATIC: 0x00DC6F7C
static CVar* s_cvarChatAmbienceVolume;

// STATIC: 0x00DC6F80
static float s_sfxVolumeBase;

// STATIC: 0x00DC6F84
static float s_musicVolumeBase;

// STATIC: 0x00DC6F88
static float s_ambienceVolumeBase;

// STATIC: 0x00DC6F8C
static float s_sfxVolumeDelta;

// STATIC: 0x00DC6F90
static float s_musicVolumeDelta;

// STATIC: 0x00DC6F94
static float s_ambienceVolumeDelta;

// STATIC: 0x00CA0F14
uint32_t s_comsatSleepTime = 10;

// STATIC: 0x00CA0F18
int32_t s_pushToTalkKey = -1;

// STATIC: 0x00DC6F98
static CVar* s_cvarStartTalkingDelay;

// STATIC: 0x00DC6F9C
static CVar* s_cvarStartTalkingTime;

// STATIC: 0x00DC6FA0
static CVar* s_cvarStopTalkingDelay;

// STATIC: 0x00DC6FA4
static CVar* s_cvarStopTalkingTime;

// STATIC: 0x00DC6FB8
static uint32_t s_comsatIdleTimeout;

// STATIC: 0x00DC6FBC
static CVar* s_cvarComSatSleepTime;

// STATIC: 0x00DC0D44
static int32_t s_lastPushToTalkDown;

// STATIC: 0x00DC0D48
static int32_t s_localTalking;

// STATIC: 0x00DC0D38
uint32_t s_pushToTalkModifiers;

// STATIC: 0x00DC6FB0
CVar* s_cvarPushToTalkButton;

// STATIC: 0x00DC6FB4
int32_t s_voiceChatWasEnabled;

// STATIC: 0x00DC6FCC
uint32_t s_pushToTalkMouseButton;

// STATIC: 0x00DC6FE0
static TSGrowableArray<ComSatCallback> s_comsatCallbacks;

// STATIC: 0x00DC7000
static SMutex s_comsatClientMutex;

// STATIC: 0x00DC7034
TSHashTable<TALKINGPLAYER, CHashKeyGUID> s_talkingPlayers;

// STATIC: 0x00DC7060
static TSHashTable<MUTEDPLAYER, CHashKeyGUID> s_mutedPlayers;

// STATIC: 0x00DC6FF0
static TSGrowableArray<ComSatEvent> s_comSatEvents;

// STATIC: 0x00DC6FC0
static WOWGUID s_localPlayer;

// STATIC: 0x00DC6FC8
int32_t s_comsatDisabledByClient;

// STATIC: 0x00DC7008
SCritSect s_comsatDeviceLock;

// STATIC: 0x00DC708C
static int32_t s_lastFoundSession;

// STATIC: 0x00CA0F10
int32_t s_currentSession = -1;

// STATIC: 0x00D1C958
const char* s_sessionTypeNames[3] = { "Battleground", "Party", "Raid" };

// PROVEN: 9aaca926 IDENTICAL/NONE
// OFFSET: 0x004DB600/0x1401199E0
static int32_t ParseKeyNameToKeyCode(const char* name) {
    if (!name || !*name) {
        return -1;
    }

    if (!name[1] && name[0] > ' ') {
        return name[0];
    }

    if (!SStrCmpI(name, "LSHIFT", STORM_MAX_STR)) {
        return KEY_LSHIFT;
    }

    if (!SStrCmpI(name, "RSHIFT", STORM_MAX_STR)) {
        return KEY_RSHIFT;
    }

    if (!SStrCmpI(name, "LCTRL", STORM_MAX_STR)) {
        return KEY_LCONTROL;
    }

    if (!SStrCmpI(name, "RCTRL", STORM_MAX_STR)) {
        return KEY_RCONTROL;
    }

    if (!SStrCmpI(name, "LALT", STORM_MAX_STR)) {
        return KEY_LALT;
    }

    if (!SStrCmpI(name, "RALT", STORM_MAX_STR)) {
        return KEY_RALT;
    }

    if (!SStrCmpI(name, "SPACE", STORM_MAX_STR)) {
        return KEY_SPACE;
    }

    if (!SStrCmpI(name, "NUMPADPLUS", STORM_MAX_STR)) {
        return KEY_NUMPAD_PLUS;
    }

    if (!SStrCmpI(name, "NUMPADMINUS", STORM_MAX_STR)) {
        return KEY_NUMPAD_MINUS;
    }

    if (!SStrCmpI(name, "NUMPADMULTIPLY", STORM_MAX_STR)) {
        return KEY_NUMPAD_MULTIPLY;
    }

    if (!SStrCmpI(name, "NUMPADDIVIDE", STORM_MAX_STR)) {
        return KEY_NUMPAD_DIVIDE;
    }

    if (!SStrCmpI(name, "NUMPADDECIMAL", STORM_MAX_STR)) {
        return KEY_NUMPAD_DECIMAL;
    }

    if (!SStrCmpI(name, "NUMPADEQUALS", STORM_MAX_STR)) {
        return KEY_NUMPAD_EQUALS;
    }

    if (SStrStrI(name, "NUMPAD")) {
        if (name[6] >= 0 && name[6] <= '9') {
            return name[6] - '0' + KEY_NUMPAD0;
        }

        return -1;
    }

    if (!SStrCmpI(name, "ESCAPE", STORM_MAX_STR)) {
        return KEY_ESCAPE;
    }

    if (!SStrCmpI(name, "ENTER", STORM_MAX_STR)) {
        return KEY_ENTER;
    }

    if (!SStrCmpI(name, "BACKSPACE", STORM_MAX_STR)) {
        return KEY_BACKSPACE;
    }

    if (!SStrCmpI(name, "TAB", STORM_MAX_STR)) {
        return KEY_TAB;
    }

    if (!SStrCmpI(name, "LEFT", STORM_MAX_STR)) {
        return KEY_LEFT;
    }

    if (!SStrCmpI(name, "UP", STORM_MAX_STR)) {
        return KEY_UP;
    }

    if (!SStrCmpI(name, "RIGHT", STORM_MAX_STR)) {
        return KEY_RIGHT;
    }

    if (!SStrCmpI(name, "DOWN", STORM_MAX_STR)) {
        return KEY_DOWN;
    }

    if (!SStrCmpI(name, "INSERT", STORM_MAX_STR)) {
        return KEY_INSERT;
    }

    if (!SStrCmpI(name, "DELETE", STORM_MAX_STR)) {
        return KEY_DELETE;
    }

    if (!SStrCmpI(name, "HOME", STORM_MAX_STR)) {
        return KEY_HOME;
    }

    if (!SStrCmpI(name, "END", STORM_MAX_STR)) {
        return KEY_END;
    }

    if (!SStrCmpI(name, "PAGEUP", STORM_MAX_STR)) {
        return KEY_PAGEUP;
    }

    if (!SStrCmpI(name, "PAGEDOWN", STORM_MAX_STR)) {
        return KEY_PAGEDOWN;
    }

    if (!SStrCmpI(name, "NUMLOCK", STORM_MAX_STR)) {
        return KEY_NUMLOCK;
    }

    if (!SStrCmpI(name, "CAPSLOCK", STORM_MAX_STR)) {
        return KEY_CAPSLOCK;
    }

    if (!SStrCmpI(name, "PRINTSCREEN", STORM_MAX_STR)) {
        return KEY_PRINTSCREEN;
    }

    if (*name == 'F') {
        return SStrToInt(name + 1) + KEY_F1 - 1;
    }

    return -1;
}

// OFFSET: 0x004DC460/0x14011A940
void AddComSatEvent(uint32_t type, float value, uint64_t guid) {
    ComSatEvent* event = s_comSatEvents.New();

    if (event) {
        event->m_floatValue = value;
        event->m_type = type;
        event->m_guid = guid;
    }
}

// OFFSET: 0x004DC4E0/0x14011AA20
void AddComSatEvent(uint32_t type, uint32_t value, uint64_t guid) {
    ComSatEvent* event = s_comSatEvents.New();

    if (event) {
        event->m_type = type;
        event->m_guid = guid;
        event->m_uintValue = value;
    }
}

// OFFSET: 0x004DBF70/0x14011A3F0
static void ResetVoiceVolumeState() {
    s_voiceFadeStartTime = 0.0f;
    s_voiceFadeDelayRate = 0.0f;
    s_talkingPlayerCount = 0;
    s_voiceFadeRate = 0.0f;
    s_sfxVolumeDelta = 0.0f;
    s_musicVolumeDelta = 0.0f;
    s_ambienceVolumeDelta = 0.0f;

    if (s_cvarSoundSFXVolume) {
        s_sfxVolumeBase = s_cvarSoundSFXVolume->m_floatValue;
        SE3::sm_aSoundBus[SE3BUS_SFX].SetVolume(s_sfxVolumeBase);
    } else {
        s_sfxVolumeBase = 0.0f;
    }

    if (s_cvarSoundMusicVolume) {
        s_musicVolumeBase = s_cvarSoundMusicVolume->m_floatValue;
        SE3::sm_aSoundBus[SE3BUS_MUSIC].SetVolume(s_musicVolumeBase);
    } else {
        s_musicVolumeBase = 0.0f;
    }

    if (s_cvarSoundAmbienceVolume) {
        s_ambienceVolumeBase = s_cvarSoundAmbienceVolume->m_floatValue;
        SE3::sm_aSoundBus[SE3BUS_AMBIENCE].SetVolume(s_ambienceVolumeBase);
    } else {
        s_ambienceVolumeBase = 0.0f;
    }
}

// OFFSET: 0x004DC0B0
void* ComSatClientAllocator::Alloc(uint32_t size) {
    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// OFFSET: 0x004DC0D0
void ComSatClientAllocator::Free(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// OFFSET: 0x004DF240
static void ProcessComSatCallbacks() {
    for (uint32_t i = 0; i < s_comsatCallbacks.Count(); i++) {
        ComSatCallback& callback = s_comsatCallbacks[i];

        switch (callback.m_type) {
            case 0:
                Comsat::VoiceStart();
                break;

            case 1:
                Comsat::VoiceStop();
                break;

            case 2:
                Comsat::VoicePlateStart(callback.unk_0008, callback.unk_0010);
                break;

            case 3:
                Comsat::VoicePlateStop(callback.unk_0008, callback.unk_0010);
                break;

            default:
                break;
        }
    }

    s_comsatCallbacks.Clear();
}

// OFFSET: 0x004DF3E0/0x14011E560
void ComSatClient_Shutdown() {
    if (ComSatThread::s_threadRunning) {
        ComSatThread::s_threadRunning = 0;
        ComSatThread::s_threadShutdownEvent.Set();
        ComSatThread::s_comsatThread.Wait(0xFFFFFFFF);
    }

    ProcessComSatCallbacks();
    EventUnregister(EVENT_ID_IDLE, VoiceChat_TickWrapper);
    ComSat::Interface::Destroy(ComSatThread::s_comsat, s_comsatAllocator);
    ComSatThread::s_comsat = NULL;
    s_mutedPlayers.InternalClear(0);
    s_talkingPlayers.InternalClear(0);

    if (s_comsatClientMutex.Valid()) {
        s_comsatClientMutex.Close();
        s_comsatClientMutex.Release();
    }

    ResetVoiceVolumeState();
    SE3::DestroyChatSystem();
}

// OFFSET: 0x004DC390/0x14011A860
void UpdatePushToTalkKey() {
    s_pushToTalkKey = -1;
    s_pushToTalkMouseButton = 0;
    s_pushToTalkModifiers = 0;

    if (!s_cvarPushToTalkButton) {
        return;
    }

    const char* binding = s_cvarPushToTalkButton->m_stringValue.m_str;

    if (!binding || !*binding) {
        return;
    }

    s_pushToTalkModifiers = 0;
    CGUIBindings::AddModifiers(s_pushToTalkModifiers, binding);
    s_pushToTalkMouseButton = StringToMouseButton(binding);

    if (!s_pushToTalkMouseButton) {
        s_pushToTalkKey = ParseKeyNameToKeyCode(binding);
    }
}

// OFFSET: 0x004DAF20/0x140119410
int32_t ComSatClient_IsTalkerSquelched(WOWGUID guid, uint32_t session) {
    if (!guid || session > 31) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    COMSATDATA& data = s_comsatData[session];

    for (int32_t i = 0; i < static_cast<int32_t>(data.m_numMembers); i++) {
        if (data.m_members[i].m_guid == guid) {
            int32_t result = data.m_members[i].m_flags & 0x8;
            s_comsatDeviceLock.Leave();
            return result;
        }
    }

    s_comsatDeviceLock.Leave();
    return 0;
}

// OFFSET: 0x004DAFD0/0x1401194D0
int32_t ComSatClient_IsTalkerSquelchedInChannel(WOWGUID guid, int32_t type, const char* name) {
    int32_t session = ComSatClient_FindSessionByTypeAndName(type, name);
    return session >= 0 ? ComSatClient_IsTalkerSquelched(guid, session) : 0;
}

// OFFSET: 0x004DB000/0x140119500
int32_t ComSatClient_IsMemberVoiceActive(WOWGUID guid, uint32_t session) {
    if (!guid || session > 31) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    COMSATDATA& data = s_comsatData[session];

    for (int32_t i = 0; i < static_cast<int32_t>(data.m_numMembers); i++) {
        if (data.m_members[i].m_guid == guid) {
            int32_t result = data.m_members[i].m_flags & 0x40;
            s_comsatDeviceLock.Leave();
            return result;
        }
    }

    s_comsatDeviceLock.Leave();
    return 0;
}

// OFFSET: 0x004DB090/0x1401195B0
int32_t ComSatClient_IsMemberInVoiceSession(WOWGUID guid, int32_t type, const char* name) {
    int32_t session = ComSatClient_FindSessionByTypeAndName(type, name);
    return session >= 0 ? ComSatClient_IsMemberVoiceActive(guid, session) : 0;
}

// OFFSET: 0x004DB0C0/0x1401195E0
int32_t ComSatClient_GetMemberVoiceStatus(WOWGUID guid, uint32_t session) {
    if (!guid || session > 31) {
        return 0;
    }

    s_comsatDeviceLock.Enter();
    COMSATDATA& data = s_comsatData[session];

    for (int32_t i = 0; i < static_cast<int32_t>(data.m_numMembers); i++) {
        if (data.m_members[i].m_guid == guid) {
            int32_t result = data.m_members[i].m_flags & 0x4;
            s_comsatDeviceLock.Leave();
            return result;
        }
    }

    s_comsatDeviceLock.Leave();
    return 0;
}

// OFFSET: 0x004DB150/0x140119690
int32_t ComSatClient_GetChannelMemberVoiceStatus(WOWGUID guid, int32_t type, const char* name) {
    int32_t session = ComSatClient_FindSessionByTypeAndName(type, name);
    return session >= 0 ? ComSatClient_GetMemberVoiceStatus(guid, session) : 0;
}

// PROVEN: 17e8f8de IDENTICAL/NONE
// OFFSET: 0x004DB3D0/0x1401199D0
int32_t ComSatClient_IsComsatDisabledByClient() {
    return s_comsatDisabledByClient;
}

// OFFSET: 0x004DAEE0/0x1401193C0
int32_t ComSatClient_GetCurrentSessionType() {
    if (s_currentSession >= 0) {
        return s_comsatData[s_currentSession].m_type;
    }

    return 4;
}

// OFFSET: 0x004DAF00/0x1401193F0
const char* ComSatClient_GetCurrentSessionName() {
    if (s_currentSession >= 0) {
        return s_comsatData[s_currentSession].m_name;
    }

    return NULL;
}

// PROVEN: 8193692e IDENTICAL/NONE
// OFFSET: 0x004DAE80/0x140119370
int32_t ComSatClient_HasSession(int32_t type, const char* name) {
    return ComSatClient_FindSessionByTypeAndName(type, name) >= 0;
}

// OFFSET: 0x004DAD10/0x140119190
int32_t ComSatClient_FindSessionByTypeAndName(int32_t type, const char* name) {
    int32_t cached = s_lastFoundSession;

    if (s_comsatData[cached].m_type == type) {
        if (type) {
            return cached;
        }

        if (!SStrCmpUTF8I(s_comsatData[cached].m_name, name, 0x7FFFFFFF)) {
            return s_lastFoundSession;
        }
    }

    if (!type && !name) {
        return -1;
    }

    for (int32_t i = 0; i < 32; i++) {
        COMSATDATA* data = &s_comsatData[i];

        if (data->m_sessionID && data->m_type == type && (type || !SStrCmpUTF8I(data->m_name, name, 0x7FFFFFFF))) {
            s_lastFoundSession = i;
            return i;
        }
    }

    return -1;
}

// PROVEN: 1f91c4fe IDENTICAL/NONE
// OFFSET: 0x004DADC0/0x140119290
static void NameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        ComSatClient_SendEvent(static_cast<int32_t>(reinterpret_cast<intptr_t>(param)), guid);
    }
}

// OFFSET: 0x004DADF0/0x1401192B0
void ComSatClient_SendEvent(int32_t event, WOWGUID guid) {
    if (!CGGameUI::m_initialized) {
        return;
    }

    const char* name = reinterpret_cast<const char*>(g_nameDBCache.GetRecord(guid.m_guid, guid, NameQueryCallback, reinterpret_cast<void*>(static_cast<intptr_t>(event)), false));

    if (!name) {
        name = FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);
    }

    const char* token = Script_GetTokenFromGUID(&guid);

    if (!name || !*name) {
        return;
    }

    if (token) {
        FrameScript_SignalEvent(event, "%s%s", name, token);
    } else {
        FrameScript_SignalEvent(event, "%s", name);
    }
}

// OFFSET: 0x004DDD20/0x14011C5D0
bool ComSatClient_IsPlayerMuted(WOWGUID guid) {
    return s_mutedPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid)) != NULL;
}

// OFFSET: 0x004DDFB0/0x14011C8C0
static void ComSatClient_SetCurrentSessionI(int32_t index) {
    int32_t type;
    const char* name;

    if (index == -1) {
        AddComSatEvent(5, 0u, 0ull);
        s_currentSession = -1;
        type = 4;
        name = "";
        WOWGUID player = ClntObjMgrGetActivePlayer();

        if (s_talkingPlayers.Ptr(static_cast<uint32_t>(player.m_guid), CHashKeyGUID(player.m_guid))) {
            Script_SendUnitSignal(player, SCRIPT_EVENT_VOICE_STOP);
            ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_STOP, player);
        }
    } else {
        AddComSatEvent(5, 0u, s_comsatData[index].m_sessionID);
        s_currentSession = index;
        type = s_comsatData[index].m_type;
        name = s_comsatData[index].m_name;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_ACTIVE_VOICE_CHANNEL));
    msg.Put(static_cast<uint32_t>(type));
    msg.PutString(name);
    msg.Finalize();
    ClientServices::Send2(&msg);

    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_SESSIONS_UPDATE, NULL);
    }
}

// PROVEN: 6a7dbe58 IDENTICAL/NONE
// OFFSET: 0x004DE0F0/0x14011CA60
int32_t ComSatClient_SetCurrentSession(int32_t type, const char* name) {
    int32_t index = ComSatClient_FindSessionByTypeAndName(type, name);

    if (index < 0) {
        return 0;
    }

    ComSatClient_SetCurrentSessionI(index);
    return 1;
}

// PROVEN: 7189708f IDENTICAL/NONE
// OFFSET: 0x004DE120/0x14011CA90
void ComSatClient_DisableCurrentSession() {
    ComSatClient_SetCurrentSessionI(-1);
}

// OFFSET: 0x004DAFB0/0x1401194C0
int32_t ComSatClient_IsTalkerSquelchedInCurrent(WOWGUID guid) {
    return ComSatClient_IsTalkerSquelched(guid, s_currentSession);
}

// OFFSET: 0x004DB180/0x1401196C0
int32_t ComSatClient_DetermineSessionTypeFromName(const char* name) {
    if (!name || !*name) {
        return 4;
    }

    if (!SStrCmpI(name, s_sessionTypeNames[1], STORM_MAX_STR)) {
        return 2;
    }

    if (!SStrCmpI(name, s_sessionTypeNames[2], STORM_MAX_STR)) {
        return 3;
    }

    return !SStrCmpI(name, s_sessionTypeNames[0], STORM_MAX_STR);
}

// OFFSET: 0x004DD970/0x14011C1C0
static void ComSatClient_LeaveSessionNotifyAll(int32_t index) {
    for (int32_t i = 0; i < static_cast<int32_t>(s_comsatData[index].m_numMembers); i++) {
        WOWGUID guid = s_comsatData[index].m_members[i].m_guid;

        if (!guid || guid == ClntObjMgrGetActivePlayer()) {
            continue;
        }

        Script_SendUnitSignal(guid, SCRIPT_EVENT_VOICE_STOP);
        ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_STOP, guid);
        TALKINGPLAYER* talker = s_talkingPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (talker) {
            s_talkingPlayers.Delete(talker);
        }

        if (--s_talkingPlayerCount < 0) {
            s_talkingPlayerCount = 0;
        }
    }
}

// OFFSET: 0x004DDBD0/0x14011C430
void ComSatClient_LeaveAllSessions(WOWGUID guid) {
    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (comsat) {
        int32_t index = 0;

        while (!static_cast<uint32_t>(s_comsatData[index].m_sessionID)) {
            if (++index >= 32) {
                break;
            }
        }

        if (index < 32) {
            comsat->SessionRemove(s_comsatData[index].m_sessionID, guid.m_guid);
            comsat->SessionDestroy(s_comsatData[index].m_sessionID);
            ComSatClient_LeaveSessionNotifyAll(index);
            s_comsatData[index].m_sessionID = 0;
            s_comsatData[index].m_name[0] = 0;
            s_comsatData[index].m_type = 4;
            s_comsatData[index].m_numMembers = 0;

            if (s_currentSession == index) {
                s_currentSession = -1;
                WOWGUID player = ClntObjMgrGetActivePlayer();

                if (s_talkingPlayers.Ptr(static_cast<uint32_t>(player.m_guid), CHashKeyGUID(player.m_guid))) {
                    Script_SendUnitSignal(player, SCRIPT_EVENT_VOICE_STOP);
                    ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_STOP, player);
                }
            }
        }

        if (CGGameUI::m_initialized) {
            FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_LEFT_SESSION, NULL);

            if (CGGameUI::m_initialized) {
                FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_SESSIONS_UPDATE, NULL);
            }
        }
    }

    s_comsatDeviceLock.Leave();
}

// PROVEN: 0e62d12b IDENTICAL/NONE
// OFFSET: 0x004DAEA0
int32_t ComSatClient_IsCurrentSession(uint64_t session) {
    return s_currentSession >= 0 && s_comsatData[s_currentSession].m_sessionID == session;
}

// OFFSET: 0x004DC1F0/0x14011A690
static bool FillComsatData(uint64_t session, int32_t type, const char* name) {
    int32_t index = -1;
    int32_t i;

    for (i = 0; i < 32; i++) {
        if (s_comsatData[i].m_sessionID == session) {
            break;
        }

        if (index == -1 && !static_cast<uint32_t>(s_comsatData[i].m_sessionID)) {
            index = i;
        }
    }

    if (i < 32) {
        index = i;
        s_comsatData[index].m_sessionID = 0;
    }

    if (index == -1) {
        return false;
    }

    s_comsatData[index].m_sessionID = session;
    s_comsatData[index].m_type = type;
    s_comsatData[index].m_numMembers = 0;

    if (type) {
        s_comsatData[index].m_name[0] = 0;
    } else {
        SStrCopy(s_comsatData[index].m_name, name, sizeof(s_comsatData[index].m_name));
    }

    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_SESSIONS_UPDATE, NULL);
    }

    CGChat::NewVoiceSession(name, type);
    return true;
}

// OFFSET: 0x004DCAF0
static bool ComSatClient_IsPlayerTalking(WOWGUID guid) {
    return s_talkingPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid)) != NULL;
}

// OFFSET: 0x004DD2A0/0x14011BA90
void ComSatClient_PopulateSessionDataWithoutJoin(uint64_t session, int32_t type, const char* name, uint64_t self) {
    if (!name) {
        return;
    }

    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (!comsat) {
        s_comsatDeviceLock.Leave();
        return;
    }

    if (comsat->SessionCreate(session)) {
        comsat->SessionAddLocal(session, 0, self);

        if (!FillComsatData(session, type, name)) {
            s_comsatDeviceLock.Leave();
            return;
        }

        if (type == 2 || type == 3 || type == 1) {
            Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_STATUS_UPDATE);
        }

        WOWGUID player = ClntObjMgrGetActivePlayer();

        if (ComSatClient_IsPlayerTalking(player)) {
            Script_SendUnitSignal(player, SCRIPT_EVENT_VOICE_START);
            ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_START, player);
        }

        if ((type == 2 || type == 3 || type == 1) && (CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers)) {
            CGChat::JoiningParty(type == 3 || type == 1, type == 1);
        }
    }

    CGChat::VoiceSessionUpdate(type ? "" : name, type);
    s_comsatDeviceLock.Leave();
}

// OFFSET: 0x004DD400/0x14011BBE0
void VoiceSessionRosterUpdate(uint64_t session, uint16_t networkId, int32_t type, const char* name, const uint8_t* key, uint64_t self, uint8_t selfNetworkId, uint8_t selfFlags, int32_t count, VoiceRosterMember* members) {
    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (!comsat || !name) {
        s_comsatDeviceLock.Leave();
        return;
    }

    if (comsat->SessionCreate(session)) {
        comsat->SessionAddLocal(session, 0, self);

        if (!FillComsatData(session, type, name)) {
            s_comsatDeviceLock.Leave();
            return;
        }

        WOWGUID player = ClntObjMgrGetActivePlayer();

        if (ComSatClient_IsPlayerTalking(player)) {
            Script_SendUnitSignal(player, SCRIPT_EVENT_VOICE_START);
            ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_START, player);
        }

        if ((type == 2 || type == 3 || type == 1) && (CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers)) {
            CGChat::JoiningParty(type == 3 || type == 1, type == 1);
        }
    }

    comsat->SessionSetNetworkId(session, networkId);
    comsat->SessionSetTalkerNetworkIdLocal(session, self, selfNetworkId);
    int32_t index;

    for (index = 0; index < 32; index++) {
        if (s_comsatData[index].m_sessionID == session) {
            break;
        }
    }

    if (index < 32 && s_comsatData[index].m_type != type) {
        s_comsatData[index].m_type = type;

        if (type) {
            s_comsatData[index].m_name[0] = 0;
        } else {
            SStrCopy(s_comsatData[index].m_name, name, sizeof(s_comsatData[index].m_name));
        }
    }

    int32_t talkers = comsat->SessionNumMembers(session);

    for (int32_t i = 0; i < talkers; i++) {
        uint64_t talker;
        uint8_t flags;

        if (!comsat->SessionGetTalkerByIndex(session, i, talker, flags) || flags != 0xFF) {
            continue;
        }

        int32_t j = 0;

        if (count >= 0) {
            for (; j <= count; j++) {
                if (members[j].m_guid.m_guid == talker) {
                    break;
                }
            }
        }

        if (j != count) {
            continue;
        }

        comsat->SessionRemove(session, talker);
        i--;
        talkers--;

        if (s_talkingPlayers.Ptr(static_cast<uint32_t>(talker), CHashKeyGUID(talker))) {
            WOWGUID guid;
            guid.m_guid = talker;
            Script_SendUnitSignal(guid, SCRIPT_EVENT_VOICE_STOP);
            ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_STOP, guid);
            TALKINGPLAYER* talking = s_talkingPlayers.Ptr(static_cast<uint32_t>(talker), CHashKeyGUID(talker));

            if (talking) {
                s_talkingPlayers.Delete(talking);
            }
        }
    }

    uint8_t sessionKey[16];
    memcpy(sessionKey, key, sizeof(sessionKey));

    for (int32_t i = 0; i < count; i++) {
        comsat->SessionAddRemote(session, members[i].m_guid.m_guid, members[i].m_priority, sessionKey, members[i].m_address);
        comsat->SessionSetTalkerPriority(session, members[i].m_guid.m_guid, members[i].m_talkerPriority);
    }

    COMSATDATA* data = &s_comsatData[index];
    data->m_members[0].m_guid.m_guid = self;
    data->m_members[0].m_flags = selfFlags;
    data->m_numMembers = count + 1;

    for (uint32_t k = 1; k < data->m_numMembers; k++) {
        data->m_members[k].m_guid = members[k - 1].m_guid;
        data->m_members[k].m_flags = members[k - 1].m_flags;
    }

    for (int32_t i = 0; i < count; i++) {
        uint32_t j;

        for (j = 0; j < FriendList::s_friendList->GetNumIgnores(); j++) {
            if (members[i].m_guid.m_guid == FriendList::s_friendList->GetIgnore(j)) {
                break;
            }
        }

        if (j < FriendList::s_friendList->GetNumIgnores()) {
            comsat->MuteTalker(0, members[i].m_guid.m_guid, static_cast<ComSat::MuteSetting>(0));
            continue;
        }

        for (uint32_t k = 0; k < FriendList::s_friendList->GetNumMutes(); k++) {
            if (members[i].m_guid.m_guid == FriendList::s_friendList->GetIgnore(k)) {
                comsat->MuteTalker(0, members[i].m_guid.m_guid, static_cast<ComSat::MuteSetting>(0));
            }
        }
    }

    if (s_voiceChatWasEnabled) {
        switch (type) {
            case 2:
                CGChat::JoiningParty(false, 0);
                s_voiceChatWasEnabled = 0;
                break;
            case 3:
                CGChat::JoiningParty(true, 0);
                s_voiceChatWasEnabled = 0;
                break;
            case 1:
                CGChat::JoiningParty(true, 1);
                s_voiceChatWasEnabled = 0;
                break;
            default:
                break;
        }
    }

    CGChat::VoiceSessionUpdate(type ? "" : name, type);

    if (type == 2 || type == 3 || type == 1) {
        Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_STATUS_UPDATE);
    } else {
        Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_CHANNEL_STATUS_UPDATE);
    }

    s_comsatDeviceLock.Leave();
}

// OFFSET: 0x004DDA60/0x14011C280
void VoiceSessionLeave(uint64_t talker, uint64_t session, int32_t type) {
    s_comsatDeviceLock.Enter();
    ComSat::Interface* comsat = ComSatThread::s_comsat;

    if (!comsat) {
        s_comsatDeviceLock.Leave();
        return;
    }

    comsat->SessionRemove(session, talker);
    comsat->SessionDestroy(session);
    int32_t index;

    for (index = 0; index < 32; index++) {
        if (s_comsatData[index].m_sessionID == session || (type == s_comsatData[index].m_type && (type == 2 || type == 3))) {
            break;
        }
    }

    if (index < 32) {
        if (s_currentSession == index) {
            ComSatClient_LeaveSessionNotifyAll(index);

            if (CGGameUI::m_initialized) {
                FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_LEFT_SESSION, NULL);
            }

            s_currentSession = -1;
            WOWGUID player = ClntObjMgrGetActivePlayer();

            if (s_talkingPlayers.Ptr(static_cast<uint32_t>(player.m_guid), CHashKeyGUID(player.m_guid))) {
                Script_SendUnitSignal(player, SCRIPT_EVENT_VOICE_STOP);
                ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_STOP, player);
            }
        }

        s_comsatData[index].m_sessionID = 0;
        s_comsatData[index].m_name[0] = 0;
        s_comsatData[index].m_type = 4;
        s_comsatData[index].m_numMembers = 0;
    }

    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_SESSIONS_UPDATE, NULL);
    }

    s_comsatDeviceLock.Leave();
}

// OFFSET: 0x004DCAA0
static void RemoveTalkingPlayer(WOWGUID guid) {
    TALKINGPLAYER* talker = s_talkingPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (talker) {
        s_talkingPlayers.Delete(talker);
    }
}

// OFFSET: 0x004DD110
void Comsat::VoiceStop() {
    if (!s_localPlayer && !(s_localPlayer = ClntObjMgrGetActivePlayer())) {
        return;
    }

    Script_SendUnitSignal(s_localPlayer, SCRIPT_EVENT_VOICE_STOP);
    ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_STOP, s_localPlayer);
    RemoveTalkingPlayer(s_localPlayer);
}

// OFFSET: 0x004DD170
void Comsat::VoicePlateStop(uint64_t sessionId, uint64_t guid) {
    WOWGUID none = { 0 };
    NameCache* name = g_nameDBCache.GetRecord(guid, none, NULL, NULL, false);

    if (!name) {
        return;
    }

    int32_t index;

    for (index = 0; index < 32; index++) {
        if (s_comsatData[index].m_sessionID == sessionId) {
            break;
        }
    }

    if (index >= 32) {
        return;
    }

    Script_SendUnitSignal(name->m_guid, SCRIPT_EVENT_VOICE_STOP);
    ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_STOP, name->m_guid);
    WOWGUID player = name->m_guid;
    TALKINGPLAYER* talker = s_talkingPlayers.Ptr(static_cast<uint32_t>(player.m_guid), CHashKeyGUID(player.m_guid));

    if (talker) {
        s_talkingPlayers.Delete(talker);
    }

    if (--s_talkingPlayerCount < 0) {
        s_talkingPlayerCount = 0;
    }
}

// OFFSET: 0x004DEEF0
static void AddTalkingPlayer(WOWGUID guid) {
    if (!s_talkingPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid))) {
        s_talkingPlayers.New(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid), 0, 0);
    }
}

// OFFSET: 0x004DF060
void Comsat::VoiceStart() {
    if ((s_localPlayer || (s_localPlayer = ClntObjMgrGetActivePlayer())) && s_currentSession >= 0) {
        Script_SendUnitSignal(s_localPlayer, SCRIPT_EVENT_VOICE_START);
        ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_START, s_localPlayer);
        AddTalkingPlayer(s_localPlayer);
    }
}

// OFFSET: 0x004DF0D0
void Comsat::VoicePlateStart(uint64_t sessionId, uint64_t guid) {
    WOWGUID none = { 0 };
    NameCache* name = g_nameDBCache.GetRecord(guid, none, NULL, NULL, false);

    if (!name) {
        return;
    }

    int32_t index;

    for (index = 0; index < 32; index++) {
        if (s_comsatData[index].m_sessionID == sessionId) {
            break;
        }
    }

    if (index >= 32) {
        return;
    }

    Script_SendUnitSignal(name->m_guid, SCRIPT_EVENT_VOICE_START);
    ComSatClient_SendEvent(SCRIPT_EVENT_VOICE_PLATE_START, name->m_guid);
    AddTalkingPlayer(name->m_guid);
    s_talkingPlayerCount++;
}

// OFFSET: 0x004DB3E0
static int32_t AcquireSingleInstanceMutex() {
    s_comsatClientMutex.Create(1, "WOWCOMSATCLIENT15595");

    if (s_comsatClientMutex.Valid()) {
        if (!s_comsatClientMutex.Wait(0)) {
            return 1;
        }

        s_comsatClientMutex.Close();
        s_comsatClientMutex.Release();
    }

    return 0;
}

// OFFSET: 0x004DBC70
static void InitSoundGroupFadeRates() {
    if (s_voiceFadeActive) {
        return;
    }

    s_voiceFadeActive = 1;
    float delay = s_cvarStartTalkingDelay ? s_cvarStartTalkingDelay->m_floatValue : 0.0f;
    s_voiceFadeDelayRate = 0.0f == delay ? 0.0f : 1.0f / delay;
    float time = s_cvarStartTalkingTime ? s_cvarStartTalkingTime->m_floatValue : 0.2f;
    s_voiceFadeRate = 0.0f != time ? 1.0f / time : 0.0f;
    s_sfxVolumeBase = SE3::sm_aSoundBus[SE3BUS_SFX].GetVolume();
    s_musicVolumeBase = SE3::sm_aSoundBus[SE3BUS_MUSIC].GetVolume();
    s_ambienceVolumeBase = SE3::sm_aSoundBus[SE3BUS_AMBIENCE].GetVolume();

    if (s_cvarSoundSFXVolume && s_cvarChatSoundVolume) {
        s_sfxVolumeDelta = s_cvarChatSoundVolume->m_floatValue * s_cvarSoundSFXVolume->m_floatValue - s_sfxVolumeBase;
    }

    if (s_cvarSoundMusicVolume && s_cvarChatMusicVolume) {
        s_musicVolumeDelta = s_cvarChatMusicVolume->m_floatValue * s_cvarSoundMusicVolume->m_floatValue - s_musicVolumeBase;
    }

    if (s_cvarSoundAmbienceVolume && s_cvarChatAmbienceVolume) {
        s_ambienceVolumeDelta = s_cvarChatAmbienceVolume->m_floatValue * s_cvarSoundAmbienceVolume->m_floatValue - s_ambienceVolumeBase;
    }
}

// OFFSET: 0x004DBDF0
static void UpdateSoundGroupFadeRates() {
    if (!s_voiceFadeActive) {
        return;
    }

    s_voiceFadeActive = 0;
    s_voiceFadeStartTime = static_cast<float>(OsGetAsyncTimeMs() * 0.001);
    float delay = s_cvarStopTalkingDelay ? s_cvarStopTalkingDelay->m_floatValue : 3.0f;
    s_voiceFadeDelayRate = 0.0f == delay ? 0.0f : 1.0f / delay;
    float time = s_cvarStopTalkingTime ? s_cvarStopTalkingTime->m_floatValue : 3.0f;
    s_voiceFadeRate = 0.0f != time ? 1.0f / time : 0.0f;
    s_sfxVolumeBase = SE3::sm_aSoundBus[SE3BUS_SFX].GetVolume();
    s_musicVolumeBase = SE3::sm_aSoundBus[SE3BUS_MUSIC].GetVolume();
    s_ambienceVolumeBase = SE3::sm_aSoundBus[SE3BUS_AMBIENCE].GetVolume();

    if (s_cvarSoundSFXVolume) {
        s_sfxVolumeDelta = s_cvarSoundSFXVolume->m_floatValue - s_sfxVolumeBase;
    }

    if (s_cvarSoundMusicVolume) {
        s_musicVolumeDelta = s_cvarSoundMusicVolume->m_floatValue - s_musicVolumeBase;
    }

    if (s_cvarSoundAmbienceVolume) {
        s_ambienceVolumeDelta = s_cvarSoundAmbienceVolume->m_floatValue - s_ambienceVolumeBase;
    }
}

// OFFSET: 0x004DA4C0/0x140118930
static void UpdateConnectionState() {
    if (s_pushToTalkDown != s_lastPushToTalkDown) {
        if (s_pushToTalkDown) {
            Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_PUSH_TO_TALK_START);
        } else {
            Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_PUSH_TO_TALK_STOP);
        }
    }

    if (ComSatThread::s_comsat) {
        s_currentOutputDevice = ComSatThread::s_comsat->LocalTalkerGetCurrentOutputDeviceIndex(0);
        s_currentCaptureDevice = ComSatThread::s_comsat->LocalTalkerGetCurrentCaptureDeviceIndex(0);
        s_lastPushToTalkDown = s_pushToTalkDown;
        s_localTalking = s_localTalkerActive;
    } else {
        s_currentOutputDevice = -1;
        s_currentCaptureDevice = -1;
        s_lastPushToTalkDown = 0;
        s_localTalking = 0;
    }

    if (s_cvarComSatSleepTime) {
        s_comsatSleepTime = s_cvarComSatSleepTime->m_intValue;
    } else {
        s_comsatSleepTime = 10;
    }
}

// OFFSET: 0x004DC410/0x14011A8E0
static void DoComSatWork() {
    if (ComSatThread::s_threadRunning) {
        return;
    }

    ComSatThread::s_threadRunning = 1;
    ComSatThread::s_threadShutdownEvent.Reset();
    SThread::Create(ComSatThread::Work, NULL, ComSatThread::s_comsatThread, const_cast<char*>("ComSatThread"), 0);
    ComSatThread::s_comsatThread.SetPriority(4);
}

// OFFSET: 0x004DC5F0/0x14011ABE0
static void DispatchComSatEvents() {
    if (ComSatThread::s_comsat) {
        for (uint32_t i = 0; i < s_comSatEvents.Count(); i++) {
            ComSatEvent& event = s_comSatEvents[i];

            switch (event.m_type) {
                case 1:
                    ComSatThread::s_comsat->LocalTalkerSetSessionVolume(0, event.m_guid, event.m_floatValue);
                    break;

                case 2:
                    s_pushToTalkDown = 0;
                    ComSatThread::s_comsat->LocalTalkerEnable(0, event.m_uintValue != 0);
                    break;

                case 3:
                    ComSatThread::s_comsat->LocalTalkerThreshold(0, event.m_floatValue);
                    break;

                case 4:
                    ComSatThread::s_comsat->MuteTalker(0, event.m_guid, static_cast<ComSat::MuteSetting>(event.m_uintValue));
                    break;

                case 5:
                    ComSatThread::s_comsat->LocalTalkerSelectSession(0, event.m_guid);
                    break;

                case 6:
                    ComSatThread::s_comsat->LocalTalkerSetSessionPriority(0, event.m_guid, static_cast<uint8_t>(event.m_uintValue));
                    break;

                case 7:
                    ComSatThread::s_comsat->MuteSession(0, event.m_guid, static_cast<ComSat::MuteSetting>(event.m_uintValue));
                    break;

                case 8:
                    ComSatThread::s_comsat->DebugSetSequenceDelay(event.m_uintValue);
                    break;

                case 9:
                    ComSatThread::s_comsat->DebugSetSoundOutputDelay(0, event.m_uintValue);
                    break;

                case 10:
                    UpdatePushToTalkKey();
                    break;
            }
        }
    }

    s_comSatEvents.Clear();
}

// OFFSET: 0x004DCB20/0x14011B290
bool ComSatClient_IsMutedPlayer(WOWGUID guid) {
    return s_mutedPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid)) != NULL;
}

// OFFSET: 0x004DCE30
static bool IsVoiceActive() {
    if (s_talkingPlayerCount > 0) {
        return true;
    }

    WOWGUID player = s_localPlayer;

    if (!s_talkingPlayers.Ptr(static_cast<uint32_t>(player.m_guid), CHashKeyGUID(player.m_guid)) && !SI3::VoiceChat_IsRecordingLoopbackSound() && !SI3::VoiceChat_IsPlayingLoopbackSound()) {
        return false;
    }

    return true;
}

// OFFSET: 0x004DCE90
static void ApplySoundGroupFade(uint32_t time) {
    double now = time * 0.001;

    if (s_voiceFadeDelayRate == 0.0f) {
        if (s_voiceFadeRate == 0.0f) {
            if (IsVoiceActive()) {
                if (s_cvarSoundSFXVolume && s_cvarChatSoundVolume) {
                    float volume = s_cvarChatSoundVolume->m_floatValue * s_cvarSoundSFXVolume->m_floatValue;

                    if (volume != SE3::sm_aSoundBus[SE3BUS_SFX].GetVolume()) {
                        SE3::sm_aSoundBus[SE3BUS_SFX].SetVolume(s_cvarChatSoundVolume->m_floatValue * s_cvarSoundSFXVolume->m_floatValue);
                    }
                }

                if (s_cvarSoundMusicVolume && s_cvarChatMusicVolume) {
                    float volume = s_cvarChatMusicVolume->m_floatValue * s_cvarSoundMusicVolume->m_floatValue;

                    if (volume != SE3::sm_aSoundBus[SE3BUS_MUSIC].GetVolume()) {
                        SE3::sm_aSoundBus[SE3BUS_MUSIC].SetVolume(s_cvarChatMusicVolume->m_floatValue * s_cvarSoundMusicVolume->m_floatValue);
                    }
                }

                if (s_cvarSoundAmbienceVolume && s_cvarChatAmbienceVolume) {
                    float volume = s_cvarChatAmbienceVolume->m_floatValue * s_cvarSoundAmbienceVolume->m_floatValue;

                    if (volume != SE3::sm_aSoundBus[SE3BUS_AMBIENCE].GetVolume()) {
                        SE3::sm_aSoundBus[SE3BUS_AMBIENCE].SetVolume(s_cvarChatAmbienceVolume->m_floatValue * s_cvarSoundAmbienceVolume->m_floatValue);
                    }
                }
            }
        } else {
            float fade = static_cast<float>(s_voiceFadeRate * (now - s_voiceFadeStartTime));

            if (fade >= 1.0f) {
                fade = 1.0f;
                s_voiceFadeRate = 0.0f;
                s_voiceFadeStartTime = 0.0f;
            }

            SE3::sm_aSoundBus[SE3BUS_SFX].SetVolume(fade * s_sfxVolumeDelta + s_sfxVolumeBase);
            SE3::sm_aSoundBus[SE3BUS_MUSIC].SetVolume(s_musicVolumeDelta * fade + s_musicVolumeBase);
            SE3::sm_aSoundBus[SE3BUS_AMBIENCE].SetVolume(s_ambienceVolumeDelta * fade + s_ambienceVolumeBase);
        }
    } else if (s_voiceFadeDelayRate * (now - s_voiceFadeStartTime) >= 1.0) {
        s_voiceFadeDelayRate = 0.0f;
        s_voiceFadeStartTime = static_cast<float>(now);
    }

    if (!s_voiceFadeActive) {
        if (IsVoiceActive()) {
            InitSoundGroupFadeRates();
            return;
        }

        if (!s_voiceFadeActive) {
            return;
        }
    }

    if (!IsVoiceActive()) {
        UpdateSoundGroupFadeRates();
    }
}

// OFFSET: 0x004DEF90
static void MarkPlayerMuted(WOWGUID guid, int32_t muted) {
    MUTEDPLAYER* entry = s_mutedPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (entry) {
        if (!muted) {
            s_mutedPlayers.Delete(entry);
        }
    } else if (muted) {
        s_mutedPlayers.New(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid), 0, 0);
    }
}

// OFFSET: 0x004DF1C0
void VoiceSessionSetTalkerMuted(WOWGUID guid, int32_t muted) {
    AddComSatEvent(4, static_cast<uint32_t>(muted), guid.m_guid);

    if (muted == 2) {
        MarkPlayerMuted(guid, s_mutedPlayers.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid)) == NULL);
    } else {
        MarkPlayerMuted(guid, muted == 0);
    }

    if (CGGameUI::m_initialized) {
        CGChat::UpdateRosterMember(guid);
        Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_STATUS_UPDATE);
    }
}

// OFFSET: 0x004DF310/0x14011E470
static void VoiceChat_Tick() {
    s_comsatDeviceLock.Enter();
    ProcessComSatCallbacks();
    UpdateConnectionState();
    DispatchComSatEvents();
    int32_t active = s_localTalking | (CGChat::IsVoiceChatEnabled() && s_currentSession != -1);
    s_comsatDeviceLock.Leave();

    if (active) {
        DoComSatWork();
        s_comsatIdleTimeout = OsGetAsyncTimeMs() + 3000;
    } else if (!s_comsatIdleTimeout || OsGetAsyncTimeMs() > s_comsatIdleTimeout) {
        if (ComSatThread::s_threadRunning) {
            ComSatThread::s_threadRunning = 0;
            ComSatThread::s_threadShutdownEvent.Set();
            ComSatThread::s_comsatThread.Wait(0xFFFFFFFF);
        }

        SE3::CaptureWork();
        s_comsatIdleTimeout = 0;
    }
}

// PROVEN: a03a80c0 IDENTICAL/NONE
// OFFSET: 0x004DF3C0/0x14011E530
int32_t VoiceChat_TickWrapper(const void* data, void* param) {
    if (ComSatThread::s_comsat) {
        VoiceChat_Tick();
    }

    ApplySoundGroupFade(OsGetAsyncTimeMs());
    return 1;
}

// OFFSET: 0x004DC560/0x14011AB00
void AddComSatCallback(uint32_t type, uint64_t a2, uint64_t a3) {
    ComSatCallback* callback = s_comsatCallbacks.New();

    if (callback) {
        callback->m_type = type;
        callback->unk_0008 = a2;
        callback->unk_0010 = a3;
    }
}

// OFFSET: 0x004DC7A0/0x14011AE10
void ComSatClientCallback::NotifyLocalTalkerStart(uint8_t localTalker) {
    if (localTalker == 0 && s_currentSession >= 0) {
        DebugLog("COMSAT_START: NotifyLocalTalkerStart\r\n");
        s_localTalkerActive = 1;
        AddComSatCallback(0, 0, 0);
    }
}

// OFFSET: 0x004DC7E0/0x14011AE50
void ComSatClientCallback::NotifyLocalTalkerStop(uint8_t localTalker) {
    DebugLog("COMSAT_STOP: NotifyLocalTalkerStop\r\n");

    if (localTalker == 0) {
        s_localTalkerActive = 0;
        AddComSatCallback(1, 0, 0);
    }
}

// OFFSET: 0x004DC820/0x14011AE90
void ComSatClientCallback::NotifyTalkerStart(uint8_t localTalker, uint64_t session, uint64_t talker) {
    DebugLog("COMSAT_START: NotifyTalkerStart\r\n");

    if (localTalker == 0) {
        AddComSatCallback(2, session, talker);
    }
}

// OFFSET: 0x004DC860/0x14011AEE0
void ComSatClientCallback::NotifyTalkerStop(uint8_t localTalker, uint64_t session, uint64_t talker) {
    DebugLog("COMSAT_STOP: NotifyTalkerStop\r\n");

    if (localTalker == 0) {
        AddComSatCallback(3, session, talker);
    }
}

// PROVEN: 05ae74ae IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void ComSatClientCallback::NotifyLocalTalkerVoiceLevel(uint8_t localTalker, float level, bool talking) {
}

// PROVEN: 39561cba IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void ComSatClientCallback::NotifyDeviceError(uint8_t localTalker) {
}

// OFFSET: 0x004DF480/0x14011E610
void ComSatThread::Init() {
    s_pushToTalkKey = -1;
    s_pushToTalkMouseButton = 0;

    ComSat::Initialization init = {};
    init.m_maxLocalPlayers = 1;
    init.unk_0004 = 50;
    init.m_time = OsGetAsyncTimeMs();
    init.m_window = g_theGxDevicePtr->DeviceWindow();
    init.m_port = 0;
    init.m_maxChannels = 4;

    ComSatThread::s_comsat = ComSat::Interface::Create(init, s_comsatAllocator);

    if (!ComSatThread::s_comsat) {
        return;
    }

    ComSatThread::s_comsat->CallbackSet(s_comsatCallback);
    ComSat::DeviceInfo devices[8];
    int32_t numDevices = ComSatThread::s_comsat->EnumerateOutputDevices(devices, 8);
    UpdateConnectionState();

    if (numDevices > 0) {
        WOWGUID player = ClntObjMgrGetActivePlayer();
        ComSatThread::s_comsat->LocalTalkerCreate(0, player.m_guid, 0.6f, 0, 0);
        EventRegister(EVENT_ID_IDLE, VoiceChat_TickWrapper);
    }

    CVar* voiceChatMode = CVar::Lookup("VoiceChatMode");

    if (voiceChatMode && voiceChatMode->m_intValue) {
        s_voiceActivation = 1;
    } else {
        s_voiceActivation = 0;
    }

    ComSatThread::s_comsat->LocalTalkerEnable(0, false);
    ComSatThread::s_comsat->DebugSetSequenceDelay(2);
    ComSatThread::s_comsat->DebugSetSoundOutputDelay(0, 150);
    s_voiceChatWasEnabled = 0;
}

// OFFSET: 0x004DF5E0/0x14011E790
void ComSatClient_Init() {
    s_comsatDisabledByClient = 0;
    s_cvarStartTalkingDelay = CVar::Lookup("StartTalkingDelay");
    s_cvarStartTalkingTime = CVar::Lookup("StartTalkingTime");
    s_cvarStopTalkingDelay = CVar::Lookup("StopTalkingDelay");
    s_cvarStopTalkingTime = CVar::Lookup("StopTalkingTime");
    ResetVoiceVolumeState();
    CVar* outboundVolume = CVar::Lookup("OutboundChatVolume");
    CVar* inboundVolume = CVar::Lookup("InboundChatVolume");
    CVar* voiceChatMode = CVar::Lookup("VoiceChatMode");
    CVar* sensitivity = CVar::Lookup("VoiceActivationSensitivity");
    s_cvarEnableMicrophone = CVar::Lookup("EnableMicrophone");
    s_cvarEnableVoiceChat = CVar::Lookup("EnableVoiceChat");
    CVar::Lookup("VoiceChatSelfMute");
    s_cvarPushToTalkButton = CVar::Lookup("PushToTalkButton");
    s_cvarComSatSleepTime = NULL;

    if (!AcquireSingleInstanceMutex()) {
        s_comsatDisabledByClient = 1;
        SE3::Log_Write(" -###########################################################################################");
        SE3::Log_Write(" -# ERROR! Voice Chat does not support multiple clients on one computer.");
        SE3::Log_Write(" -# Voice Chat DISABLED for this client.");
        SE3::Log_Write(" -###########################################################################################");
        return;
    }

    ComSatThread::Init();
    s_pushToTalkModifiers = 0;
    memset(s_comsatData, 0, sizeof(s_comsatData));
    s_cvarSoundSFXVolume = CVar::Lookup("Sound_SFXVolume");
    s_cvarSoundMusicVolume = CVar::Lookup("Sound_MusicVolume");
    s_cvarSoundAmbienceVolume = CVar::Lookup("Sound_AmbienceVolume");
    s_cvarChatSoundVolume = CVar::Lookup("ChatSoundVolume");
    s_cvarChatMusicVolume = CVar::Lookup("ChatMusicVolume");
    s_cvarChatAmbienceVolume = CVar::Lookup("ChatAmbienceVolume");

    SE3::Log_Write(" ");
    SE3::Log_Write("=> Voice Chat User Setting Report:");
    SE3::Log_Write(" - ===========================");
    SE3::Log_Write(" - Enable Voice Chat  [%d]", s_cvarEnableVoiceChat->m_intValue);
    SE3::Log_Write(" ");
    SE3::Log_Write(" - ========= TALKING =========");
    SE3::Log_Write(" - Enable Microphone  [%d]", s_cvarEnableMicrophone->m_intValue);
    SE3::Log_Write(" - Microphone Volume  [%.2f]", outboundVolume->m_floatValue);
    SE3::Log_Write(" - Chat Mode          [%s]", voiceChatMode->m_intValue ? "VOICE ACTIVATED" : "PUSH TO TALK");

    if (voiceChatMode->m_intValue) {
        SE3::Log_Write(" - Sensitivity        [%.2f]", 1.0f - sensitivity->m_floatValue);
    } else {
        SE3::Log_Write(" - Push To Talk Key   [%s]", s_cvarPushToTalkButton->m_stringValue.m_str);
    }

    SE3::Log_Write(" ");
    SE3::Log_Write(" - ======== LISTENING ========");
    SE3::Log_Write(" - Speaker Volume     [%.2f]", inboundVolume->m_floatValue);
    SE3::Log_Write(" - Sound ducking      [%.2f]", s_cvarChatSoundVolume->m_floatValue);
    SE3::Log_Write(" - Music ducking      [%.2f]", s_cvarChatMusicVolume->m_floatValue);
    SE3::Log_Write(" - Ambience ducking   [%.2f]", s_cvarChatAmbienceVolume->m_floatValue);
    SE3::Log_Write("=> End of Voice Chat User Setting Report");
    SE3::Log_Write(" ");

    s_mutedPlayers.InternalClear(0);
    s_talkingPlayers.InternalClear(0);

    int32_t voiceChatEnabled = s_cvarEnableVoiceChat->m_intValue;
    int32_t microphoneEnabled = s_cvarEnableMicrophone->m_intValue;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_VOICE_SESSION_ENABLE));
    msg.Put(static_cast<uint8_t>(voiceChatEnabled != 0));
    msg.Put(static_cast<uint8_t>(microphoneEnabled != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    s_localPlayer = ClntObjMgrGetActivePlayer();
}
