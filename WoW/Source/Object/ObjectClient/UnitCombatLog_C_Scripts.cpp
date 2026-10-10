#include "UnitCombatLog_C.h"
#include <cstdint>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"

extern TSList<CombatLogFilter, TSGetLink<CombatLogFilter> > s_filters;

// OFFSET: 0x005F6240/0x140279B80
static int32_t Script_CombatLogResetFilter(lua_State* L) {
    s_filters.Clear();
    return 0;
}

// OFFSET: 0x005F5000/0x140278560
static int32_t Script_CombatLogGetRetentionTime(lua_State* L) {
    lua_pushinteger(L, g_combatLogRetentionTimeCVar->m_intValue);
    return 1;
}

extern TSList<CombatLogEntry, TSGetLink<CombatLogEntry> > s_entries;

// OFFSET: 0x005F7760/0x14027B900
static int32_t Script_CombatLogClearEntries(lua_State* L) {
    CombatLogEntry* entry = s_entries.Head();

    while (entry) {
        CombatLogEntry* next = s_entries.Next(entry);
        RecycleCombatLogEntry(entry);
        entry = next;
    }

    return 0;
}

// OFFSET: 0x005F3A70/0x140276730
static int32_t Script_CombatLogSetRetentionTime(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CombatLogSetRetentionTime(seconds)");
        return 0;
    }

    g_combatLogRetentionTimeCVar->Set(static_cast<int32_t>(lua_tointeger(L, 1)), true, false, false, true);
    return 0;
}

// OFFSET: 0x005F3AC0/0x140276790
static int32_t Script_CombatLog_Object_IsA(lua_State* L) {
    uint32_t flags = static_cast<uint32_t>(lua_tonumber(L, 1));
    flags &= static_cast<uint32_t>(lua_tonumber(L, 2));

    if ((flags & 0xFFFF0000) || ((flags & 0xF) && (flags & 0xF0) && (flags & 0x300) && (flags & 0xFC00))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x005F6570/0x14027A010
static int32_t Script_CombatLogAddFilter(lua_State* L) {
    WOWGUID sourceGuid = WOWGUID();
    WOWGUID destGuid = WOWGUID();
    uint32_t sourceFlags = 0;
    uint32_t destFlags = 0;
    int32_t spellId = 0;
    const char* spellName = NULL;
    const char* events = lua_tolstring(L, 1, NULL);

    switch (lua_type(L, 2)) {
        case LUA_TNUMBER:
            sourceFlags = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));

            if (!(sourceFlags & 0xFFFF0000) && (!(sourceFlags & 0xF) || !(sourceFlags & 0xF0) || !(sourceFlags & 0x300) || !(sourceFlags & 0xFC00))) {
                luaL_error(L, "CombatLogAddFilter(): incomplete filter for srcMask");
                return 0;
            }

            break;

        case LUA_TSTRING:
            sourceGuid = StringToGUID(lua_tolstring(L, 2, NULL));
            break;

        default:
            sourceFlags = 0xFFFFFFFF;
            break;
    }

    switch (lua_type(L, 3)) {
        case LUA_TNUMBER:
            destFlags = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 3)));

            if (!(destFlags & 0xFFFF0000) && (!(destFlags & 0xF) || !(destFlags & 0xF0) || !(destFlags & 0x300) || !(destFlags & 0xFC00))) {
                luaL_error(L, "CombatLogAddFilter(): incomplete filter for dstMask");
                return 0;
            }

            break;

        case LUA_TSTRING:
            destGuid = StringToGUID(lua_tolstring(L, 3, NULL));
            break;

        default:
            destFlags = 0xFFFFFFFF;
            break;
    }

    switch (lua_type(L, 4)) {
        case LUA_TNUMBER:
            spellId = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 4)));
            break;

        case LUA_TSTRING:
            spellName = lua_tolstring(L, 4, NULL);
            break;
    }

    CombatLogFilter* filter = s_filters.NewNode(2, 0, 0);
    filter->SetFilterData(events, sourceGuid, sourceFlags, 0, destGuid, destFlags, 0, spellId, spellName);
    return 0;
}

// OFFSET: 0x005F5B20/0x1402792E0
static int32_t Script_CombatLogGetNumEntries(lua_State* L) {
    int32_t all = StringToBOOL(L, 1, 0);
    int32_t count = 0;

    for (CombatLogEntry* entry = s_entries.Head(); entry && !entry->m_pendingMask; entry = s_entries.Next(entry)) {
        if (all || entry->MatchesFilter()) {
            count++;
        }
    }

    lua_pushinteger(L, count);
    return 1;
}

// PROVEN: b7848753 IDENTICAL/NONE
// OFFSET: 0x005F3A40/0x140276700
static int32_t Script_CombatTextSetActiveUnit(lua_State* L) {
    s_combatTextActiveUnit = GetGuidByKeyword(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x005F5B90/0x140279370
static int32_t Script_CombatLogSetCurrentEntry(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CombatLogSetCurrentEntry(index [,ignoreFilter])");
        return 0;
    }

    s_currentEntry = NULL;
    int32_t index = lua_tointeger(L, 1);
    int32_t all = StringToBOOL(L, 2, 0);

    if (index <= 0) {
        CombatLogEntry* entry = s_pendingEntries ? s_entries.Prev(s_pendingEntries) : s_entries.Tail();

        for (; entry; entry = s_entries.Prev(entry)) {
            if ((all || entry->MatchesFilter()) && index++ == 0) {
                s_currentEntry = entry;
                break;
            }
        }
    } else {
        for (CombatLogEntry* entry = s_entries.Head(); entry && entry != s_pendingEntries; entry = s_entries.Next(entry)) {
            if ((all || entry->MatchesFilter()) && --index == 0) {
                s_currentEntry = entry;
                break;
            }
        }
    }

    if (s_currentEntry) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x005F5020/0x140278580
static int32_t Script_CombatLogGetCurrentEntry(lua_State* L) {
    if (!s_currentEntry) {
        return 0;
    }

    return s_currentEntry->PushEvent(L);
}

// OFFSET: 0x005F5CD0/0x140279520
static int32_t Script_CombatLogAdvanceEntry(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CombatLogAdvanceEntry(count [,ignoreFilter])");
        return 0;
    }

    int32_t count = lua_tointeger(L, 1);
    int32_t ignoreFilter = StringToBOOL(L, 2, 0);

    if (count) {
        CombatLogEntry* entry = s_currentEntry;
        CombatLogEntry* result = NULL;

        if (count <= 0) {
            while (entry) {
                if ((ignoreFilter || entry->MatchesFilter()) && !count++) {
                    result = entry;
                    break;
                }

                entry = entry->m_link.Prev();
            }
        } else if (entry) {
            while (entry != s_pendingEntries) {
                if ((ignoreFilter || entry->MatchesFilter()) && !count--) {
                    result = entry;
                    break;
                }

                entry = s_entries.Next(entry);

                if (!entry) {
                    break;
                }
            }
        }

        s_currentEntry = result;
    }

    if (s_currentEntry) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00CA79A0
static FrameScript_Method s_ScriptFunctions[] = {
    { "CombatLogResetFilter", &Script_CombatLogResetFilter },
    { "CombatLogAddFilter", &Script_CombatLogAddFilter },
    { "CombatLogSetRetentionTime", &Script_CombatLogSetRetentionTime },
    { "CombatLogGetRetentionTime", &Script_CombatLogGetRetentionTime },
    { "CombatLogGetNumEntries", &Script_CombatLogGetNumEntries },
    { "CombatLogSetCurrentEntry", &Script_CombatLogSetCurrentEntry },
    { "CombatLogGetCurrentEntry", &Script_CombatLogGetCurrentEntry },
    { "CombatLogAdvanceEntry", &Script_CombatLogAdvanceEntry },
    { "CombatLogClearEntries", &Script_CombatLogClearEntries },
    { "CombatLog_Object_IsA", &Script_CombatLog_Object_IsA },
    { "CombatTextSetActiveUnit", &Script_CombatTextSetActiveUnit },
};

// OFFSET: 0x005F3B70
void UnitCombatLogRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x005F3BA0
void UnitCombatLogUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
