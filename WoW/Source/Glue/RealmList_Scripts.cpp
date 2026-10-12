#include "RealmList.h"
#include <cstdint>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// PROVEN: d9d3fd74 IDENTICAL/IDENTICAL
// OFFSET: 0x0080A960/0x140520C80
static int32_t Script_RequestRealmList(lua_State* L) {
    CGlueMgr::GetRealmList(StringToBOOL(L, 1, 0));
    return 0;
}

// PROVEN: ee014148 IDENTICAL/IDENTICAL
// OFFSET: 0x0080A980/0x140520CA0
static int32_t Script_RealmListUpdateRate(lua_State* L) {
    if (ClientServices::LoginConnection() && ClientServices::LoginConnection()->GetLoginServerType() == 1) {
        lua_pushnumber(L, 4.0);
        return 1;
    }

    lua_pushnumber(L, 5.0);
    return 1;
}

// PROVEN: d33a4253 IDENTICAL/IDENTICAL
// OFFSET: 0x0080A9E0/0x140520D00
static int32_t Script_CancelRealmListQuery(lua_State* L) {
    CGlueMgr::CancelRealmListQuery();
    return 0;
}

// PROVEN: 5753a85b IDENTICAL/IDENTICAL
// OFFSET: 0x0080A9F0/0x140520D10
static int32_t Script_RealmListDialogCancelled(lua_State* L) {
    CGlueMgr::RealmListDialogCancelled();
    return 0;
}

// OFFSET: 0x0080B060/0x140521490
static int32_t Script_GetNumRealms(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = GetCategoryIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
        RealmCategory* category = index < CRealmList::m_categories.Count() ? CRealmList::m_categories[index] : NULL;
        lua_pushnumber(L, category ? category->m_numRealms : 0);
        return 1;
    }

    uint32_t count = 0;

    for (uint32_t i = 0; i < CRealmList::m_categories.Count(); i++) {
        RealmCategory* category = CRealmList::m_categories[i];

        if (category) {
            count += category->m_numRealms;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x0080B130/0x140521560
static int32_t Script_GetRealmInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetRealmInfo(category, index)");
        return 0;
    }

    REALM_INFO* realm = NULL;

    if (lua_isnumber(L, 2)) {
        uint32_t categoryIndex = GetCategoryIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
        RealmCategory* category = categoryIndex < CRealmList::m_categories.Count() ? CRealmList::m_categories[categoryIndex] : NULL;
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

        if (category && category->m_numRealms > index) {
            realm = ClientServices::GetRealmInfoByIndex(category->m_realms[index]);
        }
    } else {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        for (uint32_t i = 0; i < CRealmList::m_categories.Count(); i++) {
            RealmCategory* category = CRealmList::m_categories[i];

            if (category) {
                if (category->m_numRealms > index) {
                    realm = ClientServices::GetRealmInfoByIndex(category->m_realms[index]);
                } else {
                    index -= category->m_numRealms;
                }
            }
        }
    }

    if (!realm) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 14;
    }

    lua_pushstring(L, realm->m_realmName);
    lua_pushnumber(L, realm->m_numCharacters);

    if (SFile::IsTrial() || !(realm->m_flags & 0x1)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    if (realm->m_flags & 0x2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (SStrCmpUTF8I(realm->m_realmName, ClientServices::GetSelectedRealmName(), STORM_MAX_STR)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    bool roleplaying = false;
    bool pvp = false;

    for (int32_t i = 0; i < g_cfg_ConfigsDB.m_numRecords; i++) {
        Cfg_ConfigsRec* config = g_cfg_ConfigsDB.GetRecordByIndex(i);

        if (config->m_realmType == realm->m_type) {
            roleplaying = config->m_roleplaying != 0;
            pvp = config->m_playerKillingAllowed != 0;
            break;
        }
    }

    if (pvp) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (roleplaying) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, GetRealmPopulation(realm));

    if (realm->m_otherFlags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (realm->m_flags & 0x4) {
        lua_pushnumber(L, realm->m_versionMajor);
        lua_pushnumber(L, realm->m_versionMinor);
        lua_pushnumber(L, realm->m_versionRevision);
        lua_pushnumber(L, realm->m_versionBuild);
        lua_pushnumber(L, realm->m_type);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 14;
}

// OFFSET: 0x0080B580/0x1405219A0
static int32_t Script_ChangeRealm(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ChangeRealm(category, index)");
        return 0;
    }

    REALM_INFO* realm = NULL;

    if (lua_isnumber(L, 2)) {
        uint32_t categoryIndex = GetCategoryIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
        CRealmList::m_selectedCategory = categoryIndex;
        RealmCategory* category = categoryIndex < CRealmList::m_categories.Count() ? CRealmList::m_categories[categoryIndex] : NULL;
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

        if (!category || category->m_numRealms <= index) {
            return 0;
        }

        realm = ClientServices::GetRealmInfoByIndex(category->m_realms[index]);
    } else {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        if (!CRealmList::m_categories.Count()) {
            return 0;
        }

        for (uint32_t i = 0; i < CRealmList::m_categories.Count(); i++) {
            RealmCategory* category = CRealmList::m_categories[i];

            if (category) {
                if (category->m_numRealms > index) {
                    realm = ClientServices::GetRealmInfoByIndex(category->m_realms[index]);
                } else {
                    index -= category->m_numRealms;
                }
            }
        }
    }

    if (realm) {
        if (CRealmList::m_preferredCategory == -1) {
            CRealmList::m_preferredCategory = 0;
        }

        CGlueMgr::ChangeRealm(realm);
    }

    return 0;
}

// OFFSET: 0x0080B6E0/0x140521B20
static int32_t Script_GetRealmCategories(lua_State* L) {
    uint32_t count = CRealmList::m_categories.Count();
    lua_checkstack(L, count);

    int32_t pushed = 0;

    for (uint32_t i = 0; i < count; i++) {
        RealmCategory* category = i < CRealmList::m_categories.Count() ? CRealmList::m_categories[i] : NULL;

        if (category && category->m_numRealms) {
            lua_pushstring(L, category->m_category ? category->m_category->m_name : "UNKNOWN");
            pushed++;
        }
    }

    if (pushed) {
        return pushed;
    }

    RealmCategory* category = CRealmList::m_categories.Count() ? CRealmList::m_categories[0] : NULL;
    lua_pushstring(L, category->m_category ? category->m_category->m_name : "UNKNOWN");

    return 1;
}

// OFFSET: 0x0080B790/0x140521C30
static int32_t Script_IsInvalidTournamentRealmCategory(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsInvalidTournamentRealmCategory(category)");
        return 0;
    }

    uint32_t index = GetCategoryIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (index >= CRealmList::m_categories.Count()) {
        return 0;
    }

    RealmCategory* category = CRealmList::m_categories[index];

    if (!category || !category->m_category || !(category->m_category->m_flags & 0x1)) {
        return 0;
    }

    if (ClientServices::LoginConnection()->IsProPassAccount() || ClientServices::LoginConnection()->IsGMAccount()) {
        return 0;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x0080B840/0x140521CE0
static int32_t Script_IsTournamentRealmCategory(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsTournamentRealmCategory(category)");
        return 0;
    }

    uint32_t index = GetCategoryIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (index >= CRealmList::m_categories.Count()) {
        return 0;
    }

    RealmCategory* category = CRealmList::m_categories[index];

    if (!category || !category->m_category || !(category->m_category->m_flags & 0x1)) {
        return 0;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x0080B8C0/0x140521D70
static int32_t Script_IsInvalidLocale(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_IsInvalidLocale(category)");
        return 0;
    }

    uint32_t index = GetCategoryIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (index >= CRealmList::m_categories.Count()) {
        return 0;
    }

    RealmCategory* category = CRealmList::m_categories[index];

    if (!category || !category->m_category || !category->m_category->m_localeMask) {
        return 0;
    }

    if ((1 << s_locale) & category->m_category->m_localeMask) {
        return 0;
    }

    if (ClientServices::LoginConnection()->IsGMAccount()) {
        return 0;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// PROVEN: fb9ae15c IDENTICAL/DIFFERENT
// OFFSET: 0x0080B970/0x140521E20
static int32_t Script_SetPreferredInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetPreferredInfo(index, pvp, rp)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1));
    int32_t pvp = StringToBOOL(L, 2, 0);
    int32_t roleplaying = StringToBOOL(L, 3, 0);
    CRealmList::SetPreferredInfo(index, pvp, roleplaying);

    return 0;
}

// PROVEN: 0d677a2a IDENTICAL/IDENTICAL
// OFFSET: 0x0080B9F0/0x140521EB0
static int32_t Script_SortRealms(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usgae: SortRealms(\"type\")");
        return 0;
    }

    int32_t criterion = 2;
    const char* type = lua_tolstring(L, 1, NULL);

    if (SStrCmpI(type, "name", STORM_MAX_STR)) {
        if (!SStrCmpI(type, "load", STORM_MAX_STR)) {
            criterion = 1;
        } else if (!SStrCmpI(type, "characters", STORM_MAX_STR)) {
            criterion = 0;
        } else if (!SStrCmpI(type, "mode", STORM_MAX_STR)) {
            criterion = 3;
        }
    }

    CRealmList::SortRealms(criterion);

    return 0;
}

// PROVEN: dd35d129 IDENTICAL/UNPROVEN
// OFFSET: 0x0080BAB0/0x140521F70
static int32_t Script_GetSelectedCategory(lua_State* L) {
    int32_t visible = -1;
    int32_t selected = CRealmList::m_selectedCategory;

    for (int32_t i = 0; i < static_cast<int32_t>(CRealmList::m_categories.Count()); i++) {
        RealmCategory* category = static_cast<uint32_t>(i) < CRealmList::m_categories.Count() ? CRealmList::m_categories[i] : NULL;

        if (category && category->m_numRealms) {
            visible++;

            if (i >= selected) {
                lua_pushnumber(L, visible + 1);
                return 1;
            }
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// STATIC: 0x00D07A08/0x140BD1C10
static FrameScript_Method s_scriptFunctions[] = {
    { "RequestRealmList", &Script_RequestRealmList },
    { "RealmListUpdateRate", &Script_RealmListUpdateRate },
    { "CancelRealmListQuery", &Script_CancelRealmListQuery },
    { "GetNumRealms", &Script_GetNumRealms },
    { "GetRealmInfo", &Script_GetRealmInfo },
    { "ChangeRealm", &Script_ChangeRealm },
    { "GetRealmCategories", &Script_GetRealmCategories },
    { "SetPreferredInfo", &Script_SetPreferredInfo },
    { "SortRealms", &Script_SortRealms },
    { "GetSelectedCategory", &Script_GetSelectedCategory },
    { "RealmListDialogCancelled", &Script_RealmListDialogCancelled },
    { "IsInvalidTournamentRealmCategory", &Script_IsInvalidTournamentRealmCategory },
    { "IsTournamentRealmCategory", &Script_IsTournamentRealmCategory },
    { "IsInvalidLocale", &Script_IsInvalidLocale },
};

// PROVEN: 745fcfb5 IDENTICAL/UNPROVEN
// OFFSET: 0x0080AA00/0x140520D20
void RealmListRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// PROVEN: 9616674b IDENTICAL/UNPROVEN
// OFFSET: 0x0080AA30/0x140520D70
void RealmListUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
