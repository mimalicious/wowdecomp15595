#include "PetitionVendor.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00C0E954
static const uint32_t s_arenaTeamSizes[3] = { 2, 3, 5 };

// OFFSET: 0x00A09D90/0x1407AAD20
static int32_t Script_CreateArenaTeam(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: CreateArenaTeam(teamSize, teamName)");
        return 0;
    }

    uint32_t teamSize = static_cast<uint32_t>(lua_tonumber(L, 1));
    uint32_t slot = 0;

    while (teamSize != s_arenaTeamSizes[slot]) {
        if (++slot >= 3) {
            return 0;
        }
    }

    const char* name = lua_tolstring(L, 2, NULL);
    uint32_t background = 0xFFFFFFFF;
    uint32_t emblemColor = 0xFFFFFFFF;
    uint32_t borderColor = 0xFFFFFFFF;
    int32_t emblemStyle = -1;
    int32_t borderStyle = -1;
    int32_t hasEmblem = 1;

    for (uint32_t i = 3; i <= 13; i++) {
        if (!lua_isnumber(L, i)) {
            hasEmblem = 0;
            break;
        }
    }

    int32_t error = ValidateGuildName(s_locale, name);

    if (error) {
        CGGameUI::DisplayNameError(error);
        return 0;
    }

    if (hasEmblem) {
        CImVector color = CImVector(0x00000000);
        FrameScript_GetColorNoAlpha(L, 3, color);
        background = *reinterpret_cast<uint32_t*>(&color);
        emblemStyle = static_cast<int32_t>(lua_tonumber(L, 6));
        FrameScript_GetColorNoAlpha(L, 7, color);
        emblemColor = *reinterpret_cast<uint32_t*>(&color);
        borderStyle = static_cast<int32_t>(lua_tonumber(L, 10));
        FrameScript_GetColorNoAlpha(L, 11, color);
        borderColor = *reinterpret_cast<uint32_t*>(&color);

        if (static_cast<uint32_t>(emblemStyle - 1) > 101 || borderStyle <= 0 || borderStyle > 6) {
            return 0;
        }
    }

    UserClientArenaTeamCreate msg;
    msg.unk_0010 = slot;
    SStrCopy(msg.unk_0024, name, sizeof(msg.unk_0024));
    msg.unk_001C = background;
    msg.unk_0020 = emblemStyle;
    msg.unk_0084 = emblemColor;
    msg.unk_0018 = borderStyle;
    msg.unk_0014 = borderColor;
    ClientServices::Send(&msg);
    return 0;
}

// STATIC: 0x00D179F4
static FrameScript_Method s_ScriptFunctions[] = {
    { "CreateArenaTeam", &Script_CreateArenaTeam },
};

// OFFSET: 0x00A09D60/0x1407AACF0
void PetitionVendorRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00A09D80/0x1407AAD10
void PetitionVendorUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
