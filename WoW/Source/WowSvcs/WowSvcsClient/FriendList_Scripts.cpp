#include "FriendList.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include <cstdlib>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/Types.h"

// PROVEN: a4bcdc11 IDENTICAL/NONE
// OFFSET: 0x004D2580/0x14010F800
static int32_t Script_SetWhoToUI(lua_State* L) {
    FriendList::s_whoToUI = StringToBOOL(L, 1, 0);
    return 0;
}

// PROVEN: bc58d40f IDENTICAL/NONE
// OFFSET: 0x004D2520/0x14010F7C0
static int32_t Script_GetNumWhoResults(lua_State* L) {
    lua_pushnumber(L, FriendList::s_numWhoResults);
    lua_pushnumber(L, FriendList::s_totalWhoResults);
    return 2;
}

// OFFSET: 0x004D3610/0x140110590
static int32_t Script_ShowFriends(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CONTACT_LIST));
    msg.Put(static_cast<uint32_t>(1));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x004D3180/0x140110060
static int32_t Script_GetNumFriends(lua_State* L) {
    FriendList* list = FriendList::s_friendList;

    if (!list) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 2;
    }

    lua_pushnumber(L, list->GetNumFriends());
    uint32_t online = 0;
    uint32_t count = list->GetNumFriends();

    for (uint32_t i = 0; i < count; i++) {
        if (i <= 100 && list->m_friends[i].m_connected) {
            online++;
        }
    }

    lua_pushnumber(L, online);
    return 2;
}

// PROVEN: 56067868 IDENTICAL/NONE
// OFFSET: 0x004D3690/0x140110620
static int32_t Script_GetNumIgnores(lua_State* L) {
    if (FriendList::s_friendList) {
        lua_pushnumber(L, FriendList::s_friendList->GetNumIgnores());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004D3B10/0x140110B40
static int32_t Script_GetWhoInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetWhoInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index > FriendList::s_numWhoResults) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 7;
    }

    WhoResult& result = FriendList::s_whoResults[index];
    lua_pushstring(L, result.m_name);
    lua_pushstring(L, result.m_guild);
    lua_pushnumber(L, result.m_level);
    const char* raceName = CGUnit_C::GetDisplayRaceNameFromRecord(g_chrRacesDB.GetRecord(result.m_race), result.m_gender, NULL);

    if (!raceName) {
        raceName = FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);
    }

    lua_pushstring(L, raceName);
    ChrClassesRec* chrClass = g_chrClassesDB.GetRecord(result.m_class);
    const char* className = CGUnit_C::GetDisplayClassNameFromRecord(chrClass, result.m_gender, NULL);

    if (!className) {
        className = FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);
    }

    lua_pushstring(L, className);
    AreaTableRec* area = g_areaTableDB.GetRecord(result.m_zone);
    lua_pushstring(L, area ? area->m_areaName : FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));

    if (chrClass) {
        lua_pushstring(L, chrClass->m_filename);
    } else {
        lua_pushnil(L);
    }

    return 7;
}

// OFFSET: 0x004D59C0/0x1401136D0
static int32_t Script_AddFriend(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);
    const char* note = lua_isstring(L, 2) ? lua_tolstring(L, 2, NULL) : NULL;

    if (name) {
        FriendList::AddFriend(name, note ? note : "");
    }

    return 0;
}

// PROVEN: cb5416e7 IDENTICAL/NONE
// OFFSET: 0x004D5A20/0x140113740
static int32_t Script_SendWho(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SendWho(\"filter\")");
        return 0;
    }

    if (FriendList::s_friendList) {
        FriendList::s_friendList->SendWho(lua_tolstring(L, 1, NULL));
    }

    return 0;
}

// OFFSET: 0x004D3250/0x140110140
static int32_t Script_GetFriendInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetFriendInfo(index or name)");
        return 0;
    }

    FriendInfo* info;

    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        info = index <= 100 ? &FriendList::s_friendList->m_friends[index] : NULL;
    } else {
        info = FriendList::s_friendList->GetFriendInfoByName(lua_tolstring(L, 1, NULL));
    }

    const char* unknown = FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE);

    if (!info) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, unknown);
        lua_pushstring(L, unknown);
        lua_pushnil(L);
        lua_pushstring(L, "");
        lua_pushnil(L);
        lua_pushnil(L);
        return 8;
    }

    lua_pushstring(L, info->m_name);
    lua_pushnumber(L, info->m_level);
    ChrClassesRec* chrClass = g_chrClassesDB.GetRecord(info->m_class);

    if (chrClass) {
        WOWGUID none = WOWGUID();
        const char* className = CGUnit_C::GetDisplayClassName(NULL, g_nameDBCache.GetRecord(info->m_guid.m_guid, none, NULL, NULL, false));
        lua_pushstring(L, className ? className : chrClass->m_name);
    } else {
        lua_pushstring(L, unknown);
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(info->m_area);
    const char* areaName;

    if (area) {
        AreaTableRec* parent;

        if (area->m_parentAreaID && (parent = g_areaTableDB.GetRecord(area->m_parentAreaID))) {
            area = parent;
        }

        areaName = area->m_areaName;
    } else {
        areaName = unknown;
    }

    lua_pushstring(L, areaName);

    if (info->m_connected) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (info->m_connected && (info->m_flags & 0x4)) {
        lua_pushstring(L, FrameScript_GetText("CHAT_FLAG_DND", -1, GENDER_NOT_APPLICABLE));
    } else if (info->m_connected && (info->m_flags & 0x2)) {
        lua_pushstring(L, FrameScript_GetText("CHAT_FLAG_AFK", -1, GENDER_NOT_APPLICABLE));
    } else {
        lua_pushstring(L, "");
    }

    if (info->m_notes[0]) {
        lua_pushstring(L, info->m_notes);
    } else {
        lua_pushnil(L);
    }

    if (info->m_flags & 0x8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 8;
}

// OFFSET: 0x004D3520/0x1401104A0
static int32_t Script_SetSelectedFriend(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedFriend(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    FriendList* list = FriendList::s_friendList;

    if (index <= 100) {
        list->m_selectedFriend = list->m_friends[index].m_guid;
    } else {
        list->m_selectedFriend = WOWGUID();
    }

    return 0;
}

// PROVEN: 843d200e IDENTICAL/NONE
// OFFSET: 0x004D35D0/0x140110530
static int32_t Script_GetSelectedFriend(lua_State* L) {
    lua_pushnumber(L, FriendList::s_friendList->GetFriendSelectionIndex() + 1);
    return 1;
}

// PROVEN: df75fd91 IDENTICAL/NONE
// OFFSET: 0x004D36F0/0x140110680
static int32_t Script_GetNumMutes(lua_State* L) {
    if (FriendList::s_friendList) {
        lua_pushnumber(L, FriendList::s_friendList->GetNumMutes());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x004D3750/0x1401106E0
static int32_t Script_GetIgnoreName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetIgnoreName(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    NameCache* record;

    if (index <= 50 && FriendList::s_friendList->m_ignores[index].m_guid) {
        WOWGUID none = WOWGUID();

        if ((record = g_nameDBCache.GetRecord(FriendList::s_friendList->m_ignores[index].m_guid.m_guid, none, NULL, NULL, false))) {
            char name[308];

            if (record->m_realmName[0]) {
                SStrPrintf(name, 305, "%s-%s", static_cast<const char*>(record->m_name), record->m_realmName);
            } else {
                SStrCopy(name, record->m_name, 305);
            }

            lua_pushstring(L, name);
            return 1;
        }
    }

    lua_pushstring(L, FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
    return 1;
}

// OFFSET: 0x004D3840/0x140110820
static int32_t Script_GetMuteName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMuteName(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    NameCache* record;

    if (index <= 50 && FriendList::s_friendList->m_mutes[index].m_guid) {
        WOWGUID none = WOWGUID();

        if ((record = g_nameDBCache.GetRecord(FriendList::s_friendList->m_mutes[index].m_guid.m_guid, none, NULL, NULL, false))) {
            char name[308];

            if (record->m_realmName[0]) {
                SStrPrintf(name, 305, "%s-%s", static_cast<const char*>(record->m_name), record->m_realmName);
            } else {
                SStrCopy(name, record->m_name, 305);
            }

            lua_pushstring(L, name);
            return 1;
        }
    }

    lua_pushstring(L, FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
    return 1;
}

// OFFSET: 0x004D3930/0x140110960
static int32_t Script_SetSelectedIgnore(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedIgnore(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    FriendList* list = FriendList::s_friendList;

    if (index <= 50) {
        list->m_selectedIgnore = list->m_ignores[index].m_guid;
    } else {
        list->m_selectedIgnore = WOWGUID();
    }

    return 0;
}

// OFFSET: 0x004D39E0/0x1401109F0
static int32_t Script_SetSelectedMute(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedMute(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    FriendList* list = FriendList::s_friendList;

    if (index <= 50) {
        list->m_selectedMute = list->m_mutes[index].m_guid;
    } else {
        list->m_selectedMute = WOWGUID();
    }

    return 0;
}

// PROVEN: e8796e95 IDENTICAL/NONE
// OFFSET: 0x004D3A90/0x140110A80
static int32_t Script_GetSelectedIgnore(lua_State* L) {
    lua_pushnumber(L, FriendList::s_friendList->GetIgnoreSelectionIndex() + 1);
    return 1;
}

// PROVEN: c0a099a5 IDENTICAL/NONE
// OFFSET: 0x004D3AD0/0x140110AE0
static int32_t Script_GetSelectedMute(lua_State* L) {
    lua_pushnumber(L, FriendList::s_friendList->GetMuteSelectionIndex() + 1);
    return 1;
}

// OFFSET: 0x004D3F40/0x1401110D0
static int32_t Script_SortWho(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SortWho(\"type\")");
        return 0;
    }

    int32_t field = 4;
    const char* type = lua_tolstring(L, 1, NULL);

    if (!SStrCmpI(type, "name", STORM_MAX_STR)) {
        field = 4;
    } else if (!SStrCmpI(type, "level", STORM_MAX_STR)) {
        field = 1;
    } else if (!SStrCmpI(type, "class", STORM_MAX_STR)) {
        field = 2;
    } else if (!SStrCmpI(type, "group", STORM_MAX_STR)) {
        field = 3;
    } else if (!SStrCmpI(type, "race", STORM_MAX_STR)) {
        field = 5;
    } else if (!SStrCmpI(type, "zone", STORM_MAX_STR)) {
        field = 0;
    } else if (!SStrCmpI(type, "guild", STORM_MAX_STR)) {
        field = 6;
    }

    uint32_t i;

    for (i = 0; i < 7; i++) {
        if (FriendList::s_whoSort[i].m_field == field) {
            break;
        }
    }

    if (i < 7) {
        int32_t reverse = FriendList::s_whoSort[i].unk_0004;

        if (!i) {
            reverse = !reverse;
        }

        for (uint32_t j = i; j > 0; j--) {
            FriendList::s_whoSort[j] = FriendList::s_whoSort[j - 1];
        }

        FriendList::s_whoSort[0].m_field = field;
        FriendList::s_whoSort[0].unk_0004 = reverse;
    }

    qsort(FriendList::s_whoResults, FriendList::s_numWhoResults, sizeof(WhoResult), CompareWhoResults);
    FrameScript_SignalEvent(SCRIPT_EVENT_WHO_LIST_UPDATE, NULL);
    return 0;
}

// PROVEN: 1afc437f IDENTICAL/NONE
// OFFSET: 0x004D5A70/0x1401137A0
static int32_t Script_AddOrDelIgnore(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (name && FriendList::s_friendList) {
        FriendList::s_friendList->AddOrDelIgnore(name);
    }

    return 0;
}

// PROVEN: 807e06a5 IDENTICAL/NONE
// OFFSET: 0x004D5AA0/0x1401137D0
static int32_t Script_AddOrDelMute(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (name && FriendList::s_friendList) {
        FriendList::s_friendList->AddOrDelVoiceIgnore(name);
    }

    return 0;
}

// PROVEN: d908bd54 IDENTICAL/NONE
// OFFSET: 0x004D5AD0/0x140113800
static int32_t Script_AddIgnore(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (name && FriendList::s_friendList) {
        FriendList::s_friendList->AddIgnore(name);
    }

    return 0;
}

// PROVEN: ae257bf2 IDENTICAL/NONE
// OFFSET: 0x004D5B00/0x140113830
static int32_t Script_AddMute(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (name && FriendList::s_friendList) {
        FriendList::s_friendList->AddVoiceIgnore(name);
    }

    return 0;
}

// OFFSET: 0x004D5B30/0x140113860
static int32_t Script_IsIgnored(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);

    if (token) {
        WOWGUID guid = WOWGUID();
        CGChat::ChatTokenToGUID(token, guid);

        if (guid && FriendList::s_friendList && FriendList::s_friendList->IsIgnored(guid, true)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004D5BC0/0x140113920
static int32_t Script_IsMuted(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);

    if (token) {
        WOWGUID guid = WOWGUID();
        CGChat::ChatTokenToGUID(token, guid);

        if (guid && FriendList::s_friendList && FriendList::s_friendList->IsMuted(guid)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004D5C50/0x1401139A0
static int32_t Script_IsIgnoredOrMuted(lua_State* L) {
    const char* token = lua_tolstring(L, 1, NULL);

    if (token) {
        WOWGUID guid = WOWGUID();
        CGChat::ChatTokenToGUID(token, guid);

        if (guid && FriendList::s_friendList && FriendList::s_friendList->IsIgnoredOrMuted(guid)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// PROVEN: e014aa9d IDENTICAL/NONE
// OFFSET: 0x004D6910/0x140114B60
static int32_t Script_AddOrRemoveFriend(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);
    const char* notes = lua_isstring(L, 2) ? lua_tolstring(L, 2, NULL) : NULL;

    if (name) {
        FriendList::s_friendList->AddOrRemoveFriend(name, notes ? notes : "");
    }

    return 0;
}

// OFFSET: 0x004D6970/0x140114BD0
static int32_t Script_RemoveFriend(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

        if (index < 100) {
            WOWGUID guid = FriendList::s_friendList->m_friends[index].m_guid;

            if (guid) {
                FriendList::RemoveFriend(guid);
            }
        }

        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RemoveFriend([\"name\"] or [index])");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);

    if (name) {
        FriendList::s_friendList->RemoveFriend(name);
    }

    return 0;
}

// OFFSET: 0x004D6A30/0x140114C70
static int32_t Script_SetFriendNotes(lua_State* L) {
    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetFriendNotes([\"name\"] or [index], [\"notes\"])");
    }

    const char* notes = lua_tolstring(L, 2, NULL);

    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

        if (index < 100) {
            WOWGUID guid = FriendList::s_friendList->m_friends[index].m_guid;

            if (guid) {
                FriendList::s_friendList->SetNotes(guid, notes);
            }
        }

        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetFriendNotes([\"name\"] or [index], [\"notes\"])");
    }

    const char* name = lua_tolstring(L, 1, NULL);

    if (name) {
        FriendList::s_friendList->SetFriendNotes(name, notes);
    }

    return 0;
}

// OFFSET: 0x004D6B10/0x140114D60
static int32_t Script_DelIgnore(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (name && FriendList::s_friendList) {
        FriendList::s_friendList->DelIgnore(name);
    }

    return 0;
}

// PROVEN: 1a347170 IDENTICAL/NONE
// OFFSET: 0x004D6B40/0x140114D90
static int32_t Script_DelMute(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (name && FriendList::s_friendList) {
        FriendList::s_friendList->DelMute(name);
    }

    return 0;
}

// STATIC: 0x00CA0B60
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumFriends", &Script_GetNumFriends },
    { "GetFriendInfo", &Script_GetFriendInfo },
    { "SetSelectedFriend", &Script_SetSelectedFriend },
    { "GetSelectedFriend", &Script_GetSelectedFriend },
    { "AddOrRemoveFriend", &Script_AddOrRemoveFriend },
    { "AddFriend", &Script_AddFriend },
    { "RemoveFriend", &Script_RemoveFriend },
    { "ShowFriends", &Script_ShowFriends },
    { "SetFriendNotes", &Script_SetFriendNotes },
    { "GetNumIgnores", &Script_GetNumIgnores },
    { "GetIgnoreName", &Script_GetIgnoreName },
    { "SetSelectedIgnore", &Script_SetSelectedIgnore },
    { "GetSelectedIgnore", &Script_GetSelectedIgnore },
    { "AddOrDelIgnore", &Script_AddOrDelIgnore },
    { "AddIgnore", &Script_AddIgnore },
    { "DelIgnore", &Script_DelIgnore },
    { "GetNumMutes", &Script_GetNumMutes },
    { "GetMuteName", &Script_GetMuteName },
    { "SetSelectedMute", &Script_SetSelectedMute },
    { "GetSelectedMute", &Script_GetSelectedMute },
    { "AddOrDelMute", &Script_AddOrDelMute },
    { "AddMute", &Script_AddMute },
    { "DelMute", &Script_DelMute },
    { "IsIgnored", &Script_IsIgnored },
    { "IsMuted", &Script_IsMuted },
    { "IsIgnoredOrMuted", &Script_IsIgnoredOrMuted },
    { "SendWho", &Script_SendWho },
    { "GetNumWhoResults", &Script_GetNumWhoResults },
    { "GetWhoInfo", &Script_GetWhoInfo },
    { "SetWhoToUI", &Script_SetWhoToUI },
    { "SortWho", &Script_SortWho },
};

// PROVEN: 0753ba7f IDENTICAL/NONE
// OFFSET: 0x004D25A0/0x14010F820
void FriendList::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 6fc81244 IDENTICAL/NONE
// OFFSET: 0x004D25D0/0x14010F870
void FriendList::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
