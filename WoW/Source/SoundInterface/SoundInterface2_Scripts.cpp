#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/ChatFrame.h"

// OFFSET: #0x009858B0
static int32_t Script_PlaySound(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        SI2::PlayUISound(static_cast<int32_t>(lua_tonumber(L, 1)));
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        lua_pushfstring(L, "Usage: PlaySound(\"sound\")");
        lua_error(L);
    }

    char name[256];
    SStrCopy(name, lua_tolstring(L, 1, NULL), sizeof(name));
    SI2::PlayUISound(SI2::LookupSoundKitId(name));
    return 0;
}

// OFFSET: #0x00985950
static int32_t Script_PlayMusic(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        lua_pushfstring(L, "Usage: PlayMusic(\"musicfile\")");
        lua_error(L);
    }

    SI2::PlaySoundFile(lua_tolstring(L, 1, NULL), 5);
    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: #0x009859B0
static int32_t Script_PlaySoundFile(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        lua_pushfstring(L, "Usage: PlaySoundFile(\"soundfile\")");
        lua_error(L);
    }

    SI2::PlaySoundFile(lua_tolstring(L, 1, NULL), 4);
    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: #0x00985A10
static int32_t Script_StopMusic(lua_State* L) {
    SI2::StopLuaMusic();
    return 0;
}

// OFFSET: #0x00985A20
static int32_t Script_Sound_ChatSystem_GetNumInputDrivers(lua_State* L) {
    lua_pushnumber(L, SI2::GetNumInputDrivers_Cached(1));
    return 1;
}

// OFFSET: #0x00985A50
static int32_t Script_Sound_ChatSystem_GetInputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_ChatSystem_GetInputDriverNameByIndex(InputDriverIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1));
    char name[2048];
    memset(name, 0, sizeof(name));
    SI2::GetInputDriverName_Cached(index, name, sizeof(name), 1);
    lua_pushstring(L, name);
    return 1;
}

// OFFSET: #0x00985AE0
static int32_t Script_Sound_ChatSystem_GetNumOutputDrivers(lua_State* L) {
    lua_pushnumber(L, SI2::GetNumOutputDrivers_Cached(1));
    return 1;
}

// OFFSET: #0x00985B10
static int32_t Script_Sound_ChatSystem_GetOutputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_ChatSystem_GetOutputDriverNameByIndex(OutputDriverIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1));
    char name[2048];
    memset(name, 0, sizeof(name));

    if (index >= 0 && index <= SI2::GetNumOutputDrivers_Cached(1)) {
        SI2::GetOutputDriverName_Cached(index, name, sizeof(name), 1);
    }

    lua_pushstring(L, name);
    return 1;
}

// OFFSET: #0x00985BB0
static int32_t Script_Sound_GameSystem_GetNumInputDrivers(lua_State* L) {
    lua_pushnumber(L, SI2::GetNumInputDrivers(0));
    return 1;
}

// OFFSET: #0x00985BE0
static int32_t Script_Sound_GameSystem_GetInputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_GameSystem_GetInputDriverNameByIndex(InputDriverIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1));
    char name[2048];
    memset(name, 0, sizeof(name));
    SI2::GetInputDriverName(index, name, sizeof(name), 0);
    lua_pushstring(L, name);
    return 1;
}

// OFFSET: #0x00985C70
static int32_t Script_Sound_GameSystem_GetNumOutputDrivers(lua_State* L) {
    lua_pushnumber(L, SI2::GetNumOutputDrivers_Cached(0));
    return 1;
}

// OFFSET: #0x00985CA0
static int32_t Script_Sound_GameSystem_GetOutputDriverNameByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Sound_GetOutputDriverNameByIndex(OutputDriverIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1));
    char name[2048];
    memset(name, 0, sizeof(name));
    SI2::GetOutputDriverName_Cached(index, name, sizeof(name), 0);
    lua_pushstring(L, name);
    return 1;
}

// OFFSET: #0x00985D30
static int32_t Script_Sound_GameSystem_RestartSoundSystem(lua_State* L) {
    SI2::ShutDown(1);
    OsSleep(200);
    SI2::Init(1);
    return 0;
}

// OFFSET: %0x00A67AE0
static int32_t Script_VoiceChat_StopCapture(lua_State* L) {
    SI2::VoiceChat_StopCapture();
    return 0;
}

// OFFSET: #0x00985D60
static int32_t Script_VoiceChat_RecordLoopbackSound(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: VoiceChat_RecordLoopbackSound(MaxRecordTimeInSeconds)");
        return 0;
    }

    SI2::VoiceChat_RecordLoopbackSound(static_cast<int32_t>(lua_tonumber(L, 1)));
    return 0;
}

// OFFSET: %0x00A67B60
static int32_t Script_VoiceChat_StopRecordingLoopbackSound(lua_State* L) {
    SI2::VoiceChat_StopRecordingLoopbackSound();
    return 0;
}

// OFFSET: #0x00985DE0
static int32_t Script_VoiceChat_PlayLoopbackSound(lua_State* L) {
    SI2::VoiceChat_PlayLoopbackSound();
    return 0;
}

// OFFSET: #0x00985DF0
static int32_t Script_VoiceChat_StopPlayingLoopbackSound(lua_State* L) {
    SI2::VoiceChat_StopPlayingLoopbackSound();
    return 0;
}

// PROVEN: 6aa4d188 IDENTICAL/NONE
// OFFSET: 0x00B34CF0/0x1408FFD70
static int32_t Script_VoiceChat_IsRecordingLoopbackSound(lua_State* L) {
    lua_pushnumber(L, SI2::VoiceChat_IsRecordingLoopbackSound());
    return 1;
}

// OFFSET: #0x00985E30
static int32_t Script_VoiceChat_IsPlayingLoopbackSound(lua_State* L) {
    lua_pushnumber(L, SI2::VoiceChat_IsPlayingLoopbackSound());
    return 1;
}

// OFFSET: %0x00A67BF0
static int32_t Script_VoiceChat_GetCurrentMicrophoneSignalLevel(lua_State* L) {
    lua_pushnumber(L, SI2::VoiceChat_GetCurrentMicrophoneSignalLevel());
    return 1;
}

// OFFSET: %0x00A67C20
static int32_t Script_VoiceChat_ActivatePrimaryCaptureCallback(lua_State* L) {
    SI2::VoiceChat_ActivatePrimaryCaptureCallback();
    return 0;
}

// PROVEN: a0a8ceb4 IDENTICAL/NONE
// OFFSET: 0x00B35060/0x140900160
static int32_t Script_VoiceChat_StartCapture(lua_State* L) {
    static CVar* s_enableMicrophone = CVar::Lookup("EnableMicrophone");
    static CVar* s_enableVoiceChat = CVar::Lookup("EnableVoiceChat");

    if (s_enableVoiceChat && s_enableVoiceChat->m_intValue && s_enableMicrophone && s_enableMicrophone->m_intValue && CGChat::IsVoiceChatAllowed()) {
        SI2::VoiceChat_StartCapture();
    }

    return 0;
}

// STATIC: #0x00B2D928
static FrameScript_Method s_ScriptFunctions[] = {
    { "PlaySound", &Script_PlaySound },
    { "PlayMusic", &Script_PlayMusic },
    { "PlaySoundFile", &Script_PlaySoundFile },
    { "StopMusic", &Script_StopMusic },
    { "Sound_GameSystem_GetNumInputDrivers", &Script_Sound_GameSystem_GetNumInputDrivers },
    { "Sound_GameSystem_GetInputDriverNameByIndex", &Script_Sound_GameSystem_GetInputDriverNameByIndex },
    { "Sound_GameSystem_GetNumOutputDrivers", &Script_Sound_GameSystem_GetNumOutputDrivers },
    { "Sound_GameSystem_GetOutputDriverNameByIndex", &Script_Sound_GameSystem_GetOutputDriverNameByIndex },
    { "Sound_GameSystem_RestartSoundSystem", &Script_Sound_GameSystem_RestartSoundSystem },
    { "Sound_ChatSystem_GetNumInputDrivers", &Script_Sound_ChatSystem_GetNumInputDrivers },
    { "Sound_ChatSystem_GetInputDriverNameByIndex", &Script_Sound_ChatSystem_GetInputDriverNameByIndex },
    { "Sound_ChatSystem_GetNumOutputDrivers", &Script_Sound_ChatSystem_GetNumOutputDrivers },
    { "Sound_ChatSystem_GetOutputDriverNameByIndex", &Script_Sound_ChatSystem_GetOutputDriverNameByIndex },
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

// OFFSET: #0x00985EA0
void SI2::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: #0x00985ED0
void SI2::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
