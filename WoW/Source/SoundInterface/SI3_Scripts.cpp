#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x0100A6C8/0x140EE21D4
static uint32_t s_testPillEmitter;

// OFFSET: 0x00B34860/0x1408FF870
static SE3BUS _PickScriptSoundBus(const char* name) {
    if (!name) {
        return SE3BUS_6;
    }

    if (!_stricmp("Master", name)) {
        return SE3BUS_5;
    }

    if (!_stricmp("Music", name)) {
        return SE3BUS_7;
    }

    if (!_stricmp("Ambience", name)) {
        return SE3BUS_8;
    }

    return SE3BUS_6;
}

// OFFSET: 0x00B348D0/0x1408FF900
static int32_t Script_PlaySoundKitID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        lua_pushfstring(L, "Usage: PlaySoundKitID(ID, optional[\"SFX\",\"Music\",\"Ambience\" or \"Master\"], optional[forceNoDuplicates])");
        lua_error(L);
    }

    int32_t soundKitID = static_cast<int32_t>(lua_tointeger(L, 1));
    bool forceNoDuplicates = true;
    SE3BUS bus = SE3BUS_6;

    if (lua_isstring(L, 2)) {
        bus = _PickScriptSoundBus(lua_tolstring(L, 2, NULL));
    }

    if (lua_type(L, 3) == LUA_TBOOLEAN) {
        forceNoDuplicates = lua_toboolean(L, 3) != 0;
    }

    SI3::PlayUISound(soundKitID, &bus, forceNoDuplicates);
    return 0;
}

// OFFSET: 0x00B34980/0x1408FF9D0
static int32_t Script_PlayMusic(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        lua_pushfstring(L, "Usage: PlayMusic(\"musicfile\")");
        lua_error(L);
    }

    SI3::PlayLuaMusic(lua_tolstring(L, 1, NULL));
    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00B349E0/0x1408FFA40
static int32_t Script_PlaySoundFile(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        lua_pushfstring(L, "Usage: PlaySoundFile(\"soundfile\", optional[\"SFX\",\"Music\",\"Ambience\" or \"Master\"])");
        lua_error(L);
    }

    SE3BUS bus = SE3BUS_6;

    if (lua_isstring(L, 2)) {
        bus = _PickScriptSoundBus(lua_tolstring(L, 2, NULL));
    }

    SE3::PlaySoundFile(lua_tolstring(L, 1, NULL), NULL, bus, static_cast<SE3USAGE>(2));
    lua_pushnumber(L, 1.0);
    return 1;
}

// PROVEN: 4ff77aac IDENTICAL/IDENTICAL
// OFFSET: 0x00B34A70/0x1408FFAF0
static int32_t Script_StopMusic(lua_State* L) {
    SI3::StopLuaMusic();
    return 0;
}

// PROVEN: 03306fce IDENTICAL/IDENTICAL
// OFFSET: 0x00B34A80/0x1408FFB00
static int32_t Script_Sound_ChatSystem_GetNumInputDrivers(lua_State* L) {
    lua_pushnumber(L, SE3::GetNumInputDrivers(SE3::sm_pChatSystem));
    return 1;
}

// PROVEN: 89571e78 IDENTICAL/DIFFERENT
// OFFSET: 0x00B34AB0/0x1408FFB30
static int32_t Script_Sound_ChatSystem_GetInputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_ChatSystem_GetInputDriverNameByIndex(InputDriverIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1));
    char name[2048] = "None";
    SE3::GetInputDriverName_Cached(index, name, sizeof(name), SE3::sm_pChatSystem);
    lua_pushstring(L, name);
    return 1;
}

// PROVEN: 941f9e71 IDENTICAL/IDENTICAL
// OFFSET: 0x00B34B50/0x1408FFBE0
static int32_t Script_Sound_ChatSystem_GetNumOutputDrivers(lua_State* L) {
    lua_pushnumber(L, SE3::GetNumOutputDrivers_Cached(SE3::sm_pChatSystem));
    return 1;
}

// PROVEN: cd79dbc5 IDENTICAL/DIFFERENT
// OFFSET: 0x00B34B80/0x1408FFC10
static int32_t Script_Sound_ChatSystem_GetOutputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_ChatSystem_GetOutputDriverNameByIndex(OutputDriverIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1));
    char name[2048] = "None";

    if (index >= 0 && index <= SE3::GetNumOutputDrivers_Cached(SE3::sm_pChatSystem)) {
        SE3::GetOutputDriverName_Cached(index, name, sizeof(name), SE3::sm_pChatSystem);
    }

    lua_pushstring(L, name);
    return 1;
}

// PROVEN: 8b781dde IDENTICAL/IDENTICAL
// OFFSET: 0x00B34C40/0x1408FFCE0
static int32_t Script_VoiceChat_StopCapture(lua_State* L) {
    SI3::VoiceChat_StopCapture();
    return 0;
}

// PROVEN: 4d84899d DIFFERENT/IDENTICAL
// OFFSET: 0x00B34C50/0x1408FFCF0
static int32_t Script_VoiceChat_RecordLoopbackSound(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: VoiceChat_RecordLoopbackSound(MaxRecordTimeInSeconds)");
        return 0;
    }

    SI3::VoiceChat_RecordLoopbackSound(static_cast<int32_t>(lua_tonumber(L, 1)));
    return 0;
}

// PROVEN: 4140f27b IDENTICAL/IDENTICAL
// OFFSET: 0x00B34CC0/0x1408FFD40
static int32_t Script_VoiceChat_StopRecordingLoopbackSound(lua_State* L) {
    SI3::VoiceChat_StopRecordingLoopbackSound();
    return 0;
}

// PROVEN: 6c79221b IDENTICAL/IDENTICAL
// OFFSET: 0x00B34CD0/0x1408FFD50
static int32_t Script_VoiceChat_PlayLoopbackSound(lua_State* L) {
    SI3::VoiceChat_PlayLoopbackSound();
    return 0;
}

// PROVEN: 6453f5bc IDENTICAL/IDENTICAL
// OFFSET: 0x00B34CE0/0x1408FFD60
static int32_t Script_VoiceChat_StopPlayingLoopbackSound(lua_State* L) {
    SI3::VoiceChat_StopPlayingLoopbackSound();
    return 0;
}

// PROVEN: 59647ae8 IDENTICAL/DIFFERENT
// OFFSET: 0x00B34CF0/0x1408FFD70
static int32_t Script_VoiceChat_IsRecordingLoopbackSound(lua_State* L) {
    lua_pushnumber(L, SI3::VoiceChat_IsRecordingLoopbackSound());
    return 1;
}

// OFFSET: 0x00B34D20/0x1408FFDA0
static int32_t Script_VoiceChat_IsPlayingLoopbackSound(lua_State* L) {
    lua_pushnumber(L, SI3::VoiceChat_IsPlayingLoopbackSound());
    return 1;
}

// PROVEN: 5ce4c1a7 IDENTICAL/IDENTICAL
// OFFSET: 0x00B34D50/0x1408FFDD0
static int32_t Script_VoiceChat_GetCurrentMicrophoneSignalLevel(lua_State* L) {
    lua_pushnumber(L, SI3::VoiceChat_GetCurrentMicrophoneSignalLevel());
    return 1;
}

// PROVEN: 58546ecd IDENTICAL/IDENTICAL
// OFFSET: 0x00B34D80/0x1408FFE00
static int32_t Script_VoiceChat_ActivatePrimaryCaptureCallback(lua_State* L) {
    SI3::VoiceChat_ActivatePrimaryCaptureCallback();
    return 0;
}

// PROVEN: d3549769 IDENTICAL/IDENTICAL
// OFFSET: 0x00B34D90/0x1408FFE10
static int32_t Script_Sound_GameSystem_RestartSoundSystem(lua_State* L) {
    SI3::SignalRestart();
    return 0;
}

// PROVEN: 51bebe2c IDENTICAL/IDENTICAL
// OFFSET: [0x0080C320/0x1405229B0]
static int32_t Script_Sound_GameSystem_GetNumInputDrivers(lua_State* L) {
    lua_pushnumber(L, 0.0);
    return 1;
}

// PROVEN: 4b3d49a8 IDENTICAL/DIFFERENT
// OFFSET: 0x00B34DA0/0x1408FFE20
static int32_t Script_Sound_GameSystem_GetInputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_GameSystem_GetInputDriverNameByIndex(InputDriverIndex)");
        return 0;
    }

    char name[2048] = "None";
    lua_pushstring(L, name);
    return 1;
}

// PROVEN: beb2f5ed IDENTICAL/IDENTICAL
// OFFSET: 0x00B34E20/0x1408FFEA0
static int32_t Script_Sound_GameSystem_GetNumOutputDrivers(lua_State* L) {
    lua_pushnumber(L, SE3::GetNumOutputDrivers_Cached(SE3::sm_pGameSystem));
    return 1;
}

// PROVEN: a018f2ac IDENTICAL/DIFFERENT
// OFFSET: 0x00B34E50/0x1408FFED0
static int32_t Script_Sound_GameSystem_GetOutputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_GetOutputDriverNameByIndex(OutputDriverIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1));
    char name[2048] = "None";
    SE3::GetOutputDriverName(index, name, sizeof(name), SE3::sm_pGameSystem);
    lua_pushstring(L, name);
    return 1;
}

// OFFSET: 0x00B34F40/0x140900010
static int32_t Script_PlaySound(lua_State* L) {
    if (LoadingScreenDrawing() || ((!CGlueMgr::m_initialized || CGlueMgr::m_suspended) && !CGGameUI::s_inWorld)) {
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        lua_pushfstring(L, "Usage: PlaySound(\"sound\", optional[\"SFX\",\"Music\",\"Ambience\" or \"Master\"])");
        lua_error(L);
    }

    char name[256] = "None";
    SStrCopy(name, lua_tolstring(L, 1, NULL), sizeof(name));
    int32_t soundKitID = SE3::GetSoundKitID(name);
    SE3BUS bus = SE3BUS_6;

    if (lua_isstring(L, 2)) {
        bus = _PickScriptSoundBus(lua_tolstring(L, 2, NULL));
    }

    SI3::PlayUISound(soundKitID, &bus, true);
    return 0;
}

// PROVEN: e9ac96ba IDENTICAL/IDENTICAL
// OFFSET: 0x00B35060/0x140900160
static int32_t Script_VoiceChat_StartCapture(lua_State* L) {
    static CVar* s_enableMicrophone = CVar::Lookup("EnableMicrophone");
    static CVar* s_enableVoiceChat = CVar::Lookup("EnableVoiceChat");

    if (s_enableVoiceChat && s_enableVoiceChat->m_intValue && s_enableMicrophone && s_enableMicrophone->m_intValue && CGChat::IsVoiceChatAllowed()) {
        SI3::VoiceChat_StartCapture();
    }

    return 0;
}

// PROVEN: 1a355b85 DIFFERENT/IDENTICAL
// OFFSET: 0x00B350E0/0x140900200
static int32_t Script_TestCreatePill(lua_State* L) {
    C3Vector listener;
    SE3::GetListenerPosition(&listener);

    SE3SoundKitProperties properties;
    properties.m_bit0 = 1;
    properties.m_fadeInTime = 3000;
    properties.m_maxAudibleBehavior = 1;
    properties.m_priority = 128;

    SE3::EmitterMangerSendMessage_DestroyEmitter(s_testPillEmitter);
    SE3SoundChunk* chunk = SE3::EmitterManagerGetOrCreateSoundChunk(SE3::EmitterManagerGetSoundChunkHandle(listener));

    TSGrowableArray<C3Vector> points;
    int32_t count = rand() % 8 + 1;
    points.SetCount(count + 1);
    points[0] = listener;

    for (int32_t i = 0; i < count; i++) {
        C3Vector point = points[i];
        point.x = static_cast<float>(rand() % 40) + point.x;
        point.y = static_cast<float>(rand() % 40 - 20) + point.y;
        point.z = static_cast<float>(rand() % 40 - 20) + point.z;
        points[i + 1] = point;
    }

    SE3::EmitterManagerSendMessage_CreateEmitter(&s_testPillEmitter, chunk, 22457, listener, &properties, &points, -12345);
    return 0;
}

// STATIC: 0x00D28038/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "TestCreatePill", &Script_TestCreatePill },
    { "PlaySoundKitID", &Script_PlaySoundKitID },
    { "PlaySound", &Script_PlaySound },
    { "PlayMusic", &Script_PlayMusic },
    { "PlaySoundFile", &Script_PlaySoundFile },
    { "StopMusic", &Script_StopMusic },
    { "Sound_GameSystem_GetNumInputDrivers", &Script_Sound_GameSystem_GetNumInputDrivers },
    { "Sound_GameSystem_GetInputDriverNameByIndex", &Script_Sound_GameSystem_GetInputDriverNameByIndex },
    { "Sound_GameSystem_GetNumOutputDrivers", &Script_Sound_GameSystem_GetNumOutputDrivers },
    { "Sound_GameSystem_GetOutputDriverNameByIndex", &Script_Sound_GameSystem_GetOutputDriverNameByIndex },
    { "Sound_ChatSystem_GetNumInputDrivers", &Script_Sound_ChatSystem_GetNumInputDrivers },
    { "Sound_ChatSystem_GetInputDriverNameByIndex", &Script_Sound_ChatSystem_GetInputDriverNameByIndex },
    { "Sound_ChatSystem_GetNumOutputDrivers", &Script_Sound_ChatSystem_GetNumOutputDrivers },
    { "Sound_ChatSystem_GetOutputDriverNameByIndex", &Script_Sound_ChatSystem_GetOutputDriverNameByIndex },
    { "Sound_GameSystem_RestartSoundSystem", &Script_Sound_GameSystem_RestartSoundSystem },
    { "VoiceChat_StartCapture", &Script_VoiceChat_StartCapture },
    { "VoiceChat_StopCapture", &Script_VoiceChat_StopCapture },
    { "VoiceChat_RecordLoopbackSound", &Script_VoiceChat_RecordLoopbackSound },
    { "VoiceChat_StopRecordingLoopbackSound", &Script_VoiceChat_StopRecordingLoopbackSound },
    { "VoiceChat_PlayLoopbackSound", &Script_VoiceChat_PlayLoopbackSound },
    { "VoiceChat_StopPlayingLoopbackSound", &Script_VoiceChat_StopPlayingLoopbackSound },
    { "VoiceChat_IsRecordingLoopbackSound", &Script_VoiceChat_IsRecordingLoopbackSound },
    { "VoiceChat_IsPlayingLoopbackSound", &Script_VoiceChat_IsPlayingLoopbackSound },
    { "VoiceChat_GetCurrentMicrophoneSignalLevel", &Script_VoiceChat_GetCurrentMicrophoneSignalLevel },
    { "VoiceChat_ActivatePrimaryCaptureCallback", &Script_VoiceChat_ActivatePrimaryCaptureCallback },
};

// PROVEN: 196bba3d IDENTICAL/IDENTICAL
// OFFSET: 0x00B34EF0/0x1408FFF80
void SI3::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 310678ea IDENTICAL/IDENTICAL
// OFFSET: 0x00B34F20/0x1408FFFD0
void SI3::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
