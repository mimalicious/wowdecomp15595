#include "Flyout.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/Rec/SpellFlyoutItemRec.h"
#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"

// OFFSET: 0x009A4300/0x140729E00
static int32_t Script_GetNumFlyouts(lua_State* L) {
    int32_t count = 0;

    for (CFlyout* flyout = s_flyouts.Head(); flyout; flyout = s_flyouts.Next(flyout)) {
        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x009A4360/0x140729E70
static int32_t Script_GetFlyoutID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: flyoutID = GetFlyoutID(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tointeger(L, 1)) - 1;
    CFlyout* flyout = s_flyouts.Head();
    int32_t count = 0;

    if (flyout) {
        while (index != count++) {
            flyout = s_flyouts.Next(flyout);

            if (!flyout) {
                break;
            }
        }
    }

    if (index < 0 || index >= count) {
        luaL_error(L, "Error: No flyout found at index = %i, index should be in the range [1, %i]", index, count);
        return 0;
    }

    lua_pushnumber(L, flyout->m_rec ? flyout->m_rec->m_id : 0);
    return 1;
}

// OFFSET: 0x009A4430/0x140729F40
static int32_t Script_GetFlyoutInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: name, description, numSlots, isKnown = GetFlyoutInfo(flyoutID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tointeger(L, 1));
    CFlyout* flyout = s_flyouts.Ptr(id, HASHKEY_NONE());

    if (!flyout) {
        luaL_error(L, "Error: No flyout found for ID=%i", id);
        return 0;
    }

    lua_pushstring(L, flyout->m_rec->m_name);
    lua_pushstring(L, flyout->m_rec->m_description);
    lua_pushnumber(L, flyout->m_slots.m_count);
    lua_pushboolean(L, flyout->m_numKnownSpells == 1);
    return 4;
}

// OFFSET: 0x009A44F0/0x14072A090
static int32_t Script_GetFlyoutSlotInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: spellID, isKnown = GetFlyoutSlotInfo(flyoutID, slot)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tointeger(L, 1));
    int32_t slot = static_cast<int32_t>(lua_tointeger(L, 2)) - 1;
    CFlyout* flyout = s_flyouts.Ptr(id, HASHKEY_NONE());

    if (!flyout) {
        luaL_error(L, "Error: No flyout found for ID=%i", id);
        return 0;
    }

    if (slot < 0 || slot >= static_cast<int32_t>(flyout->m_slots.m_count)) {
        luaL_error(L, "Error: No slot %i found for ID=%i, slot should be in the range [1, %i]", slot, id, flyout->m_slots.m_count);
        return 0;
    }

    lua_pushnumber(L, flyout->m_slots.m_data[slot].m_rec->m_spellID);
    lua_pushboolean(L, flyout->m_slots.m_data[slot].m_known == 1);
    return 2;
}

// STATIC: 0x00D15090/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumFlyouts", &Script_GetNumFlyouts },
    { "GetFlyoutID", &Script_GetFlyoutID },
    { "GetFlyoutInfo", &Script_GetFlyoutInfo },
    { "GetFlyoutSlotInfo", &Script_GetFlyoutSlotInfo },
};

// PROVEN: 799e8602 IDENTICAL/UNPROVEN
// OFFSET: 0x009A3D00/0x140729390
void CFlyout::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 7a9a4f9f IDENTICAL/UNPROVEN
// OFFSET: 0x009A3D30/0x1407293E0
void CFlyout::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
