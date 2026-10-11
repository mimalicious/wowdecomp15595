#include "SkillInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x009E21B0/0x1407792C0
static int32_t Script_AbandonSkill(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: AbandonSkill(index)");
        return 0;
    }

    uint32_t skillLineID = static_cast<uint32_t>(lua_tonumber(L, 1));
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_UNLEARN_SKILL));
    msg.Put(skillLineID);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// STATIC: 0x00D16860
static FrameScript_Method s_ScriptFunctions[] = {
    { "AbandonSkill", &Script_AbandonSkill },
};

// OFFSET: 0x009E2150/0x140779290
void SkillInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009E2170/0x1407792B0
void SkillInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
