#include "AchievementInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/DB/Rec/Achievement_CategoryRec.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"

// OFFSET: 0x009478D0/0x1406B4E90
static int32_t Script_CanShowAchievementUI(lua_State* L) {
    lua_pushboolean(L, CGAchievementInfo::m_showAchievementUI);
    return 1;
}

// OFFSET: 0x00948C20/0x1406B6530
static int32_t Script_QueryQuestsCompleted(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUERY_QUESTS_COMPLETED));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00948700/0x1406B5EF0
static int32_t Script_GetPreviousAchievement(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        AchievementRec* rec = g_achievementDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

        if (!rec || !rec->m_supercedes) {
            return 0;
        }

        lua_pushnumber(L, rec->m_supercedes);
        return 1;
    }

    luaL_error(L, "Usage: GetPreviousAchievement(achievementID)");
    return 0;
}

// OFFSET: 0x00948950/0x1406B6210
static int32_t Script_GetAchievementCategory(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAchievementCategory(achievementID)");
        return 0;
    }

    AchievementRec* rec = g_achievementDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (!rec || !rec->m_category) {
        return 0;
    }

    lua_pushnumber(L, rec->m_category);
    return 1;
}

// OFFSET: 0x009489E0/0x1406B62A0
static int32_t Script_GetAchievementNumRewards(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        AchievementRec* rec = g_achievementDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

        if (rec) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    luaL_error(L, "Usage: GetAchievementNumRewards(achievementID)");
    return 0;
}

// OFFSET: 0x00948A70/0x1406B6340
static int32_t Script_GetAchievementReward(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetAchievementNumRewards(achievementID, rewardIndex)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    lua_tonumber(L, 2);
    AchievementRec* rec = g_achievementDB.GetRecord(id);

    if (rec) {
        lua_pushnumber(L, rec->m_points);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00948B20/0x1406B6410
static int32_t Script_SetAchievementComparisonUnit(lua_State* L) {
    WOWGUID guid = WOWGUID();

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: AddAchievementComparisonUnit(unitToken)");
        return 0;
    }

    Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false);

    if (!guid) {
        return 0;
    }

    CGAchievementInfo::m_inspect = guid;
    lua_pushnumber(L, 1.0);
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUERY_INSPECT_ACHIEVEMENTS));
    SmartGUID packed;
    packed.m_guid = guid.m_guid;
    msg << packed;
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 1;
}

// OFFSET: 0x0094A360/0x1406B8360
static int32_t Script_GetTrackedAchievements(lua_State* L) {
    int32_t count = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);

        count++;
        lua_pushnumber(L, node->m_id);

        node = next;
    }

    return count;
}

// OFFSET: 0x00948FB0/0x1406B68E0
static int32_t Script_HasCompletedAnyAchievement(lua_State* L) {
    lua_pushboolean(L, s_completedAchievements.Head() != NULL);
    return 1;
}

// OFFSET: 0x009482D0/0x1406B5A00
static int32_t Script_GetCategoryList(lua_State* L) {
    int32_t count = 1;

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    for (int32_t i = 0; i < g_achievement_CategoryDB.m_numRecords; i++) {
        Achievement_CategoryRec* category = g_achievement_CategoryDB.GetRecordByIndex(i);

        if (!category || category->m_id == 1 || category->m_id == 15076) {
            continue;
        }

        Achievement_CategoryRec* parent = g_achievement_CategoryDB.GetRecord(category->m_parent);

        while (parent && parent->m_id != 1 && parent->m_id != 15076) {
            parent = g_achievement_CategoryDB.GetRecord(parent->m_parent);
        }

        if (parent) {
            continue;
        }

        lua_checkstack(L, 2);
        lua_pushnumber(L, count);
        lua_pushnumber(L, category->m_id);
        lua_settable(L, -3);
        count++;
    }

    return 1;
}

// OFFSET: 0x00948630/0x1406B5E10
static int32_t Script_GetCategoryInfo(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        Achievement_CategoryRec* category = g_achievement_CategoryDB.GetRecord(static_cast<uint32_t>(lua_tonumber(L, 1)));

        if (category) {
            lua_pushstring(L, category->m_name);
            if (category->m_parent == 1) {
                lua_pushnumber(L, -1.0);
            } else {
                lua_pushnumber(L, category->m_parent);
            }
            lua_pushnumber(L, 0.0);
            return 3;
        }
    } else {
        luaL_error(L, "Usage: GetCategoryInfo(categoryID)");
    }

    return 0;
}

// OFFSET: 0x00948E90/0x1406B67B0
static int32_t Script_GetTotalAchievementPoints(lua_State* L) {
    int32_t points = 0;
    bool guild = lua_toboolean(L, 1) != 0;

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); node; node = s_completedAchievements.Next(node)) {
        AchievementRec* achievement = g_achievementDB.GetRecord(node->m_id);

        if (achievement && guild == ((achievement->m_flags & 0x4000) != 0)) {
            points += achievement->m_points;
        }
    }

    lua_pushnumber(L, points);
    return 1;
}

// OFFSET: 0x00949440/0x1406B6F00
static int32_t Script_GetCategoryNumAchievements(lua_State* L) {
    int32_t total = 0;
    int32_t pending = 0;
    int32_t completed = 0;
    int32_t previous = 0;

    if (lua_isnumber(L, 1)) {
        int32_t categoryId = static_cast<int32_t>(lua_tonumber(L, 1));

        for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
            AchievementRec* achievement = g_achievementDB.GetRecordByIndex(i);
            bool anyCategory = false;
            bool includeAll = false;

            if (categoryId == -1 && achievement && !(achievement->m_flags & 0x1)) {
                anyCategory = true;
                includeAll = true;
            } else if (categoryId == -2 && achievement && (achievement->m_flags & 0x1)) {
                anyCategory = true;
                includeAll = true;
            } else if (lua_type(L, 2) == LUA_TBOOLEAN && lua_toboolean(L, 2) == 1) {
                includeAll = true;
            }

            if (!achievement || (achievement->m_category != categoryId && !anyCategory)) {
                continue;
            }

            int32_t id = achievement->m_id;
            CLIENT_ACHIEVEMENTS* done = CGAchievementInfo::GetAchievement(id);
            CLIENT_ACHIEVEMENTS* inspected = NULL;

            for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(node); ) {
                CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(node);

                if (node->m_id == id) {
                    inspected = node;
                    break;
                }

                node = next;
            }

            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player && achievement->m_faction != -1 && achievement->m_faction != player->GetPVPFactionIndex() && !inspected) {
                continue;
            }

            if ((achievement->m_category == 81 || achievement->m_category == 15093 || (achievement->m_flags & 0x800)) && !done && !inspected) {
                continue;
            }

            if (previous && previous == achievement->m_supercedes) {
                previous = achievement->m_id;

                if (includeAll) {
                    total++;
                } else if (inspected) {
                    pending++;
                }

                continue;
            }

            if (done) {
                if (!CGAchievementInfo::IsSupersededByCompleted(achievement->m_id) || includeAll) {
                    completed++;
                    total++;
                }

                continue;
            }

            if (!inspected) {
                previous = achievement->m_id;
                total++;
                continue;
            }

            pending++;
        }

        lua_pushnumber(L, categoryId == 81 || categoryId == 15093 ? pending + completed : pending + total);
        lua_pushnumber(L, completed);
        return 2;
    }

    luaL_error(L, "Usage: GetCategoryNumAchievements(categoryID)");
    return 0;
}

// OFFSET: 0x00949800/0x1406B73D0
static int32_t Script_GetNumCompletedAchievements(lua_State* L) {
    int32_t total = 0;
    int32_t completed = 0;
    bool guild = lua_toboolean(L, 1) != 0;

    for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
        AchievementRec* achievement = g_achievementDB.GetRecordByIndex(i);

        if (!achievement || !CGAchievementInfo::IsRealAchievement(achievement->m_id, guild)) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && achievement->m_faction != -1 && achievement->m_faction != player->GetPVPFactionIndex()) {
            continue;
        }

        CLIENT_ACHIEVEMENTS* done = CGAchievementInfo::GetAchievement(achievement->m_id);

        if (achievement->m_category == 81 || achievement->m_category == 15093) {
            continue;
        }

        if (done) {
            completed++;
            total++;
        } else if (!(achievement->m_flags & 0x800)) {
            total++;
        }
    }

    lua_pushnumber(L, total);
    lua_pushnumber(L, completed);
    return 2;
}

// OFFSET: 0x00948FE0
static CLIENT_ACHIEVEMENT_CRITERIA* GetCriteria(int32_t id) {
    CLIENT_ACHIEVEMENT_CRITERIA* result = NULL;
    CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head();

    while (!s_criteria.IsEnd(node)) {
        CLIENT_ACHIEVEMENT_CRITERIA* entry = node;
        node = s_criteria.RawNext(node);

        if (entry->m_id == id) {
            result = entry;
            break;
        }
    }

    return result;
}

// OFFSET: 0x00949010
static CLIENT_ACHIEVEMENTS* GetInspectAchievement(int32_t id) {
    CLIENT_ACHIEVEMENTS* result = NULL;
    CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head();

    while (!s_inspectAchievements.IsEnd(node)) {
        CLIENT_ACHIEVEMENTS* entry = node;
        node = s_inspectAchievements.RawNext(node);

        if (entry->m_id == id) {
            result = entry;
            break;
        }
    }

    return result;
}

// PROVEN: e8e0b2db IDENTICAL/NONE
// OFFSET: 0x00948520/0x1406B5CC0
static int32_t Script_GetStatisticsCategoryList(lua_State* L) {
    int32_t count = 1;

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    for (int32_t i = 0; i < g_achievement_CategoryDB.m_numRecords; i++) {
        Achievement_CategoryRec* category = g_achievement_CategoryDB.GetRecordByIndex(i);

        if (!category) {
            continue;
        }

        Achievement_CategoryRec* parent = g_achievement_CategoryDB.GetRecord(category->m_parent);

        while (parent && parent->m_id != 1) {
            parent = g_achievement_CategoryDB.GetRecord(parent->m_parent);
        }

        if (!parent) {
            continue;
        }

        lua_checkstack(L, 2);
        lua_pushnumber(L, count);
        lua_pushnumber(L, category->m_id);
        lua_settable(L, -3);
        count++;
    }

    return 1;
}

// OFFSET: 0x00948790/0x1406B5F80
static int32_t Script_GetAchievementInfoFromCriteria(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

        if (criteria) {
            AchievementRec* achievement = g_achievementDB.GetRecord(criteria->m_achievementID);

            if (!achievement) {
                return 0;
            }

            lua_pushnumber(L, criteria->m_achievementID);
            char text[3000];
            QuestParserParseText(achievement->m_title, text, sizeof(text), ClntObjMgrGetActivePlayer(), NULL, 0);
            lua_pushstring(L, text);
            lua_pushnumber(L, achievement->m_points);
            QuestParserParseText(achievement->m_description, text, sizeof(text), ClntObjMgrGetActivePlayer(), NULL, 0);
            lua_pushstring(L, text);
            lua_pushnumber(L, achievement->m_flags);
            SpellIconRec* icon = g_spellIconDB.GetRecord(achievement->m_iconID);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
            } else {
                lua_pushstring(L, "");
            }

            QuestParserParseText(achievement->m_reward, text, sizeof(text), ClntObjMgrGetActivePlayer(), NULL, 0);
            lua_pushstring(L, text);
            return 7;
        }
    } else {
        luaL_error(L, "Usage: GetAchievementInfoFromCriteria(criteriaID)");
    }

    return 0;
}

// OFFSET: 0x00948CD0/0x1406B65F0
static int32_t Script_GetLatestCompletedAchievements(lua_State* L) {
    int32_t count = 0;
    bool guild = lua_toboolean(L, 1) != 0;

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); node; node = s_completedAchievements.Next(node)) {
        if (count >= 5) {
            break;
        }

        int32_t id = node->m_id;

        if (CGAchievementInfo::IsRealAchievement(id, guild)) {
            lua_pushnumber(L, id);
            count++;
        }
    }

    return count;
}

// OFFSET: 0x00948D60/0x1406B6680
static int32_t Script_GetLatestUpdatedStats(lua_State* L) {
    int32_t count = 0;

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_recentCriteria.Head(); node; node = s_recentCriteria.Next(node)) {
        if (count >= 5) {
            break;
        }

        lua_pushnumber(L, node->m_id);
        count++;
    }

    return count;
}

// OFFSET: 0x00948DC0/0x1406B66E0
static int32_t Script_GetLatestCompletedComparisonAchievements(lua_State* L) {
    int32_t count = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); node; node = s_inspectAchievements.Next(node)) {
        if (count >= 5) {
            break;
        }

        int32_t id = node->m_id;

        if (CGAchievementInfo::IsRealAchievement(id, false)) {
            lua_pushnumber(L, id);
            count++;
        }
    }

    return count;
}

// OFFSET: 0x00948E30/0x1406B6750
static int32_t Script_GetLatestUpdatedComparisonStats(lua_State* L) {
    int32_t count = 0;

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectRecentCriteria.Head(); node; node = s_inspectRecentCriteria.Next(node)) {
        if (count >= 5) {
            break;
        }

        lua_pushnumber(L, node->m_id);
        count++;
    }

    return count;
}

// OFFSET: 0x00948F30/0x1406B6860
static int32_t Script_GetComparisonAchievementPoints(lua_State* L) {
    int32_t points = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); node; node = s_inspectAchievements.Next(node)) {
        AchievementRec* achievement = g_achievementDB.GetRecord(node->m_id);

        if (achievement) {
            points += achievement->m_points;
        }
    }

    lua_pushnumber(L, points);
    return 1;
}

// OFFSET: 0x0094A3B0/0x1406B83C0
static int32_t Script_IsTrackedAchievement(lua_State* L) {
    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));

    if (id) {
        int32_t tracked = 0;

        for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
            CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);

            if (node->m_id == id) {
                tracked = 1;
                break;
            }

            node = next;
        }

        lua_pushboolean(L, tracked);
        return 1;
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x0094A440/0x1406B8440
static int32_t Script_GetNumTrackedAchievements(lua_State* L) {
    int32_t count = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);

        count++;

        node = next;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x00949690/0x1406B7220
static int32_t Script_GetComparisonCategoryNumAchievements(lua_State* L) {
    int32_t count = 0;

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetComparisonCategoryNumAchievements(categoryID)");
        return 0;
    }

    int32_t categoryId = static_cast<int32_t>(lua_tonumber(L, 1));

    for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
        AchievementRec* achievement = g_achievementDB.GetRecordByIndex(i);
        bool includeAll = false;

        if (categoryId == -1 && achievement && !(achievement->m_flags & 0x1)) {
            includeAll = true;
        } else if (categoryId == -2 && achievement && (achievement->m_flags & 0x1)) {
            includeAll = true;
        }

        if (!achievement || (achievement->m_category != categoryId && !includeAll)) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGAchievementInfo::m_inspect, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && achievement->m_faction != -1 && achievement->m_faction != player->GetPVPFactionIndex()) {
            continue;
        }

        for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(node); ) {
            CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(node);

            if (node->m_id == achievement->m_id) {
                if (!CGAchievementInfo::IsSupersededByCompleted(achievement->m_id) || includeAll) {
                    count++;
                }

                break;
            }

            node = next;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x00949920/0x1406B7530
static int32_t Script_GetNumComparisonCompletedAchievements(lua_State* L) {
    int32_t total = 0;
    int32_t completed = 0;

    for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
        AchievementRec* achievement = g_achievementDB.GetRecordByIndex(i);

        if (!achievement || !CGAchievementInfo::IsRealAchievement(achievement->m_id, false)) {
            continue;
        }

        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGAchievementInfo::m_inspect, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && achievement->m_faction != -1 && achievement->m_faction != player->GetPVPFactionIndex()) {
            continue;
        }

        CLIENT_ACHIEVEMENTS* inspected = NULL;

        for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(node); ) {
            CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(node);

            if (node->m_id == achievement->m_id) {
                inspected = node;
                break;
            }

            node = next;
        }

        if (achievement->m_category == 81 || achievement->m_category == 15093) {
            continue;
        }

        if (inspected) {
            completed++;
            total++;
        } else if (!(achievement->m_flags & 0x800)) {
            total++;
        }
    }

    lua_pushnumber(L, total);
    lua_pushnumber(L, completed);
    return 2;
}

// OFFSET: 0x00949A50/0x1406B7680
static int32_t Script_GetNextAchievement(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNextAchievement(achievementID)");
    } else {
        int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));

        for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
            AchievementRec* achievement = g_achievementDB.GetRecordByIndex(i);

            if (achievement && achievement->m_supercedes == id) {
                lua_pushnumber(L, achievement->m_id);

                if (CGAchievementInfo::GetAchievement(achievement->m_id)) {
                    lua_pushboolean(L, 1);
                    return 2;
                }

                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x00949B00/0x1406B7780
static int32_t Script_GetAchievementInfo(lua_State* L) {
    int32_t previous = 0;
    int32_t id = -1;

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAchievementInfo(achievementID)");
        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        id = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t categoryId = static_cast<int32_t>(lua_tonumber(L, 1));
        int32_t index = static_cast<int32_t>(lua_tonumber(L, 2));

        for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); node && id == -1; node = s_completedAchievements.Next(node)) {
            CLIENT_ACHIEVEMENTS* inspected = NULL;

            for (CLIENT_ACHIEVEMENTS* other = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(other); ) {
                CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(other);

                if (other->m_id == node->m_id) {
                    inspected = other;
                    break;
                }

                other = next;
            }

            if (!inspected) {
                continue;
            }

            AchievementRec* achievement = g_achievementDB.GetRecord(node->m_id);

            if (categoryId == -1 && !(achievement->m_flags & 0x1)) {
                if (!(achievement->m_flags & 0x2)) {
                    index--;
                }

                if (!index) {
                    id = achievement->m_id;
                }
            } else if (categoryId == -2 && achievement && (achievement->m_flags & 0x1)) {
                if (!(achievement->m_flags & 0x2)) {
                    index--;
                }

                if (!index) {
                    id = achievement->m_id;
                }
            } else if (achievement && achievement->m_category == categoryId && !CGAchievementInfo::IsSupersededByCompleted(achievement->m_id) && !--index) {
                id = achievement->m_id;
            }
        }

        if (id == -1 && index > 0) {
            for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); node && id == -1; node = s_completedAchievements.Next(node)) {
                CLIENT_ACHIEVEMENTS* inspected = NULL;

                for (CLIENT_ACHIEVEMENTS* other = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(other); ) {
                    CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(other);

                    if (other->m_id == node->m_id) {
                        inspected = other;
                        break;
                    }

                    other = next;
                }

                if (inspected) {
                    continue;
                }

                AchievementRec* achievement = g_achievementDB.GetRecord(node->m_id);

                if (categoryId == -1 && !(achievement->m_flags & 0x1)) {
                    if (!(achievement->m_flags & 0x2)) {
                        index--;
                    }

                    if (!index) {
                        id = achievement->m_id;
                    }
                } else if (categoryId == -2 && achievement && (achievement->m_flags & 0x1)) {
                    if (!(achievement->m_flags & 0x2)) {
                        index--;
                    }

                    if (!index) {
                        id = achievement->m_id;
                    }
                } else if (achievement && achievement->m_category == categoryId && !CGAchievementInfo::IsSupersededByCompleted(achievement->m_id) && !--index) {
                    id = achievement->m_id;
                }
            }

            if (id == -1 && index > 0) {
                for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); node && id == -1; node = s_inspectAchievements.Next(node)) {
                    CLIENT_ACHIEVEMENTS* done = NULL;

                    for (CLIENT_ACHIEVEMENTS* other = s_completedAchievements.Head(); !s_completedAchievements.IsEnd(other); ) {
                        CLIENT_ACHIEVEMENTS* next = s_completedAchievements.RawNext(other);

                        if (other->m_id == node->m_id) {
                            done = other;
                            break;
                        }

                        other = next;
                    }

                    if (done) {
                        continue;
                    }

                    AchievementRec* achievement = g_achievementDB.GetRecord(node->m_id);

                    if (categoryId == -1 && !(achievement->m_flags & 0x1)) {
                        if (!(achievement->m_flags & 0x2)) {
                            index--;
                        }

                        if (!index) {
                            id = achievement->m_id;
                        }
                    } else if (categoryId == -2 && achievement && (achievement->m_flags & 0x1)) {
                        if (!(achievement->m_flags & 0x2)) {
                            index--;
                        }

                        if (!index) {
                            id = achievement->m_id;
                        }
                    } else if (achievement && achievement->m_category == categoryId && !CGAchievementInfo::IsSupersededByCompleted(achievement->m_id) && !--index) {
                        id = achievement->m_id;
                    }
                }
            }
        }

        if (id == -1) {
            if (categoryId == 81) {
                return 0;
            }

            for (int32_t i = 0; i < g_achievementDB.m_numRecords && id == -1; i++) {
                AchievementRec* achievement = g_achievementDB.GetRecordByIndex(i);
                CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

                if (player && achievement->m_faction != -1 && achievement->m_faction != player->GetPVPFactionIndex()) {
                    continue;
                }

                if (categoryId == -1) {
                    if (!(achievement->m_flags & 0x1) && achievement->m_category != 81) {
                        if (CGAchievementInfo::GetAchievement(achievement->m_id) || GetInspectAchievement(achievement->m_id)) {
                            continue;
                        }

                        if (!(achievement->m_flags & 0x800)) {
                            if (!(achievement->m_flags & 0x2)) {
                                index--;
                            }

                            if (!index) {
                                id = achievement->m_id;
                            }
                        }

                        continue;
                    }
                } else if (categoryId == -2) {
                    if (!achievement) {
                        continue;
                    }

                    if ((achievement->m_flags & 0x1) && achievement->m_category != 81) {
                        if (achievement->m_flags & 0x800) {
                            continue;
                        }

                        if (!(achievement->m_flags & 0x2)) {
                            index--;
                        }

                        if (!index) {
                            id = achievement->m_id;
                        }

                        continue;
                    }
                }

                if (!achievement || achievement->m_category != categoryId) {
                    continue;
                }

                CLIENT_ACHIEVEMENTS* done = CGAchievementInfo::GetAchievement(achievement->m_id);
                CLIENT_ACHIEVEMENTS* inspected = GetInspectAchievement(achievement->m_id);

                if (previous && previous == achievement->m_supercedes) {
                    previous = achievement->m_id;
                } else if (!done && !inspected && !(achievement->m_flags & 0x800)) {
                    previous = achievement->m_id;

                    if (index-- == 1) {
                        id = achievement->m_id;
                    }
                }
            }

            if (id == -1) {
                return 0;
            }
        }
    }

    if (id < 0) {
        return 0;
    }

    AchievementRec* achievement = g_achievementDB.GetRecord(id);

    if (!achievement || (achievement->m_flags & 0x2)) {
        return 0;
    }

    CLIENT_ACHIEVEMENTS* done = NULL;

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); !s_completedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_completedAchievements.RawNext(node);

        if (node->m_id == achievement->m_id) {
            done = node;
            break;
        }

        node = next;
    }

    if ((achievement->m_flags & 0x800) && !done) {
        return 0;
    }

    lua_pushnumber(L, id);
    char text[3000];
    WOWGUID guid = ClntObjMgrGetActivePlayer();
    QuestParserParseText(achievement->m_title, text, sizeof(text), guid, NULL, 0);
    lua_pushstring(L, text);
    lua_pushnumber(L, achievement->m_points);

    if (done) {
        lua_pushboolean(L, 1);
        lua_pushnumber(L, done->m_date.m_month + 1);
        lua_pushnumber(L, done->m_date.m_monthDay + 1);
        lua_pushnumber(L, done->m_date.m_year);
    } else {
        lua_pushboolean(L, 0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    guid = ClntObjMgrGetActivePlayer();
    QuestParserParseText(achievement->m_description, text, sizeof(text), guid, NULL, 0);
    lua_pushstring(L, text);
    lua_pushnumber(L, achievement->m_flags);
    SpellIconRec* icon = g_spellIconDB.GetRecord(achievement->m_iconID);
    lua_pushstring(L, icon ? icon->m_textureFilename : "");
    guid = ClntObjMgrGetActivePlayer();
    QuestParserParseText(achievement->m_reward, text, sizeof(text), guid, NULL, 0);
    lua_pushstring(L, text);
    return 11;
}

// OFFSET: 0x0094C930/0x1406BB7C0
static int32_t Script_ClearAchievementComparisonUnit(lua_State* L) {
    CGAchievementInfo::ReleaseInspect();
    return 0;
}

// OFFSET: 0x0094A260/0x1406B8210
static int32_t Script_GetAchievementComparisonInfo(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        int32_t id = static_cast<uint32_t>(lua_tonumber(L, 1));

        if (CGAchievementInfo::m_inspect) {
            CLIENT_ACHIEVEMENTS* inspected = GetInspectAchievement(id);
            AchievementRec* achievement = g_achievementDB.GetRecord(id);

            if (inspected && achievement) {
                lua_pushboolean(L, 1);
                lua_pushnumber(L, inspected->m_date.m_month + 1);
                lua_pushnumber(L, inspected->m_date.m_monthDay + 1);
                lua_pushnumber(L, inspected->m_date.m_year);
                lua_pushnumber(L, achievement->m_flags);
                return 5;
            }
        }
    } else {
        luaL_error(L, "Usage: GetAchievementComparisonInfo(achievementID)");
    }

    return 0;
}

// OFFSET: 0x0094D970/0x1406BCF20
static int32_t Script_AddTrackedAchievement(lua_State* L) {
    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));

    if (!id || !AddTrackedAchievement(id)) {
        luaL_error(L, "Usage: AddTrackedAchievement(achievementID)");
        return 0;
    }

    return 0;
}

// OFFSET: 0x0094D9B0/0x1406BCF60
static int32_t Script_RemoveTrackedAchievement(lua_State* L) {
    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));

    if (!id) {
        luaL_error(L, "Usage: RemoveTrackedAchievement(achievementID)");
        return 0;
    }

    CGAchievementInfo::RemoveTrackedAchievement(id);
    return 0;
}

// OFFSET: 0x0094A490/0x1406B8490
static int32_t Script_GetQuestsCompleted(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    lua_checkstack(L, 2);

    for (COMPLETED_QUESTS* node = s_completedQuests.Head(); !s_completedQuests.IsEnd(node); ) {
        COMPLETED_QUESTS* next = s_completedQuests.RawNext(node);

        lua_pushnumber(L, node->m_questId);
        lua_pushboolean(L, 1);
        lua_settable(L, -3);

        node = next;
    }

    return 1;
}

// OFFSET: 0x0094BE30/0x1406BA740
static int32_t Script_GetAchievementNumCriteria(lua_State* L) {
    int32_t count = 0;
    int32_t result = 0;

    if (lua_isnumber(L, 1)) {
        int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
        ACHIEVEMENT* achievement = s_achievements.Ptr(id, HASHKEY_NONE());

        if (achievement) {
            ACHIEVEMENT* source = achievement;
            AchievementRec* rec = achievement->m_rec;

            if (rec->m_sharesCriteria) {
                source = s_achievements.Ptr(rec->m_sharesCriteria, HASHKEY_NONE());
            }

            if (source) {
                if (rec->m_flags & 0xB0) {
                    count = 1;
                }

                for (SPECIFIC_CRITERIA* node = source->m_criteria.Head(); !source->m_criteria.IsEnd(node); ) {
                    SPECIFIC_CRITERIA* next = source->m_criteria.RawNext(node);

                    if (node->m_criteria && !(node->m_criteria->m_flags & 0x2)) {
                        count++;
                    }

                    node = next;
                }

                result = count;
            }

            lua_pushnumber(L, result);
            return 1;
        }
    } else {
        luaL_error(L, "Usage: GetAchievementNumCriteria(achievementID)");
    }

    return 0;
}

// OFFSET: 0x0094BFF0/0x1406BA9F0
static int32_t Script_GetStatistic(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetStatistic(achievementID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    uint64_t value = 0;
    uint64_t sum = 0;
    uint64_t highest = 0;
    uint64_t count = 0;
    int32_t highestId = -1;
    int32_t criteriaFlags = 0;
    bool tie = false;
    CLIENT_ACHIEVEMENT_CRITERIA* earliest = NULL;
    ACHIEVEMENT* achievement = s_achievements.Ptr(id, HASHKEY_NONE());

    if (!achievement) {
        return 0;
    }

    ACHIEVEMENT* source = achievement;

    if (achievement->m_rec->m_sharesCriteria) {
        source = s_achievements.Ptr(achievement->m_rec->m_sharesCriteria, HASHKEY_NONE());
    }

    Achievement_CriteriaRec* earliestRec = NULL;

    if (source) {
        for (SPECIFIC_CRITERIA* spec = source->m_criteria.Head(); !source->m_criteria.IsEnd(spec); spec = source->m_criteria.RawNext(spec)) {
            for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head(); !s_criteria.IsEnd(node); node = s_criteria.RawNext(node)) {
                if (node->m_id != spec->m_criteria->m_id) {
                    continue;
                }

                int32_t flags = achievement->m_rec->m_flags;

                if (flags & 0x8) {
                    sum += node->m_counter;
                }

                if (flags & 0x10) {
                    if (node->m_counter > highest) {
                        highest = node->m_counter;
                        highestId = spec->m_criteria->m_id;
                        tie = false;
                    } else if (node->m_counter == highest) {
                        tie = true;
                    }
                }

                if ((flags & 0x20) && node->m_counter >= spec->m_criteria->m_quantity) {
                    count++;
                }

                value = node->m_counter;
                Achievement_CriteriaRec* rec = g_achievement_CriteriaDB.GetRecord(node->m_id);

                if (rec) {
                    criteriaFlags = rec->m_flags;
                }

                if (!earliest || node->m_timer2 < earliest->m_timer2) {
                    earliest = node;
                }

                break;
            }
        }

        if (tie && (source->m_rec->m_flags & 0x400)) {
            highestId = 0;
        }

        if (earliest) {
            earliestRec = g_achievement_CriteriaDB.GetRecord(earliest->m_id);
        }
    }

    char text[3000];
    uint64_t required = 0;
    CGAchievementInfo::BuildOutputString(text, sizeof(text), sum, highest, highestId, count, value, earliest, achievement->m_rec->m_flags, criteriaFlags, false, achievement->m_rec, earliestRec, &required);
    lua_pushstring(L, text);
    return 1;
}

// OFFSET: 0x00947880
static void CriteriaPlayerNameCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (success && g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_RECEIVED_ACHIEVEMENT_MEMBER_LIST, "%d", param);
    }
}

// OFFSET: 0x00947840
static bool IsCriteriaEligible(const Achievement_CriteriaRec* criteria) {
    return !criteria->unk_0040 || !CGWorldStateInfo::m_worldStates.Ptr(criteria->unk_0040, HASHKEY_NONE()) || GetWorldState(criteria->unk_0040) == criteria->unk_0044;
}

// OFFSET: 0x0094C280/0x1406BAE10
static int32_t Script_GetAchievementCriteriaInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAchievementCrieriaInfo(achievementID, criteriaIndex)");
        return 0;
    }

    int32_t achievementId = -1;
    bool showAll = false;
    int32_t criteriaId;

    if (lua_isnumber(L, 2)) {
        achievementId = static_cast<int32_t>(lua_tonumber(L, 1));
        int32_t index = static_cast<int32_t>(lua_tonumber(L, 2));
        ACHIEVEMENT* achievement = s_achievements.Ptr(achievementId, HASHKEY_NONE());

        if (!achievement) {
            return 0;
        }

        ACHIEVEMENT* source = achievement;

        if (achievement->m_rec->m_sharesCriteria) {
            source = s_achievements.Ptr(achievement->m_rec->m_sharesCriteria, HASHKEY_NONE());
        }

        if (!source) {
            return 0;
        }

        SPECIFIC_CRITERIA* spec = source->m_criteria.Head();

        for (;; spec = source->m_criteria.Next(spec)) {
            if (!spec) {
                return 0;
            }

            if (!(spec->m_criteria->m_flags & 0x2) || (achievement->m_rec->m_flags & 0xB0)) {
                if (achievement->m_rec->m_flags & 0xB0) {
                    showAll = true;
                }

                if (!--index) {
                    break;
                }
            }
        }

        criteriaId = spec->m_criteria->m_id;
    } else {
        criteriaId = static_cast<int32_t>(lua_tonumber(L, 1));
    }

    if (criteriaId < 0) {
        return 0;
    }

    Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(criteriaId);

    if (!criteria) {
        return 0;
    }

    if (achievementId == -1) {
        achievementId = criteria->m_achievementID;
    }

    AchievementRec* achievementRec = g_achievementDB.GetRecord(achievementId);

    if (!achievementRec || (achievementRec->m_flags & 0x2)) {
        return 0;
    }

    CLIENT_ACHIEVEMENT_CRITERIA* progress = GetCriteria(criteriaId);
    CLIENT_ACHIEVEMENTS* done = CGAchievementInfo::GetAchievement(achievementId);
    uint64_t sum = 0;
    uint64_t highest = 0;
    uint64_t count = 0;
    int32_t highestId = -1;
    bool tie = false;
    ACHIEVEMENT* achievement = s_achievements.Ptr(achievementId, HASHKEY_NONE());

    if (!achievement) {
        return 0;
    }

    ACHIEVEMENT* source = achievement;

    if (achievement->m_rec->m_sharesCriteria) {
        source = s_achievements.Ptr(achievement->m_rec->m_sharesCriteria, HASHKEY_NONE());
    }

    if (source) {
        for (SPECIFIC_CRITERIA* spec = source->m_criteria.Head(); !source->m_criteria.IsEnd(spec); spec = source->m_criteria.RawNext(spec)) {
            CLIENT_ACHIEVEMENT_CRITERIA* node = GetCriteria(spec->m_criteria->m_id);

            if (!node) {
                continue;
            }

            int32_t flags = achievement->m_rec->m_flags;

            if (flags & 0x8) {
                sum += node->m_counter;
            }

            if (flags & 0x10) {
                if (node->m_counter > highest) {
                    highest = node->m_counter;
                    highestId = spec->m_criteria->m_id;
                    tie = false;
                } else if (node->m_counter == highest) {
                    tie = true;
                }
            }

            if ((flags & 0x20) && node->m_counter >= spec->m_criteria->m_quantity) {
                count++;
            }
        }

        if (tie && (achievement->m_rec->m_flags & 0x400)) {
            highestId = 0;
        }
    }

    AchievementRec* rec = achievement->m_rec;
    int32_t criteriaFlags = criteria->m_flags;
    int32_t flags = rec->m_flags;
    uint64_t required = 0;

    if (flags & 0x80) {
        criteriaFlags |= 0x1;

        if (done && (flags & 0x8)) {
            sum = criteria->m_quantity;
        }

        if (flags & 0x20) {
            required = rec->m_minimumCriteria;
        }
    }

    char text[3000];
    int64_t shown = CGAchievementInfo::BuildOutputString(text, sizeof(text), sum, highest, highestId, count, progress ? progress->m_counter : 0, progress, flags, criteriaFlags, done != NULL, rec, criteria, &required);

    if (!progress || showAll) {
        lua_pushstring(L, criteria->m_description);
        lua_pushnumber(L, criteria->m_type);

        if (!done || achievement->m_rec->m_minimumCriteria) {
            lua_pushboolean(L, 0);
            lua_pushnumber(L, static_cast<double>(shown));
            lua_pushnumber(L, static_cast<double>(required ? required : criteria->m_quantity));
        } else {
            lua_pushboolean(L, 1);
            lua_pushnumber(L, static_cast<double>(required ? required : criteria->m_quantity));
            lua_pushnumber(L, static_cast<double>(required ? required : criteria->m_quantity));
        }

        lua_pushnil(L);
    } else {
        lua_pushstring(L, criteria->m_description);
        lua_pushnumber(L, criteria->m_type);

        if ((!done || achievementRec->m_minimumCriteria) && progress->m_counter < criteria->m_quantity) {
            lua_pushboolean(L, 0);
        } else {
            lua_pushboolean(L, 1);
        }

        lua_pushnumber(L, static_cast<double>(shown));
        lua_pushnumber(L, static_cast<double>(required ? required : criteria->m_quantity));
        NameCache* name = NULL;

        if (progress->m_player) {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                name = g_nameDBCache.GetRecord2(progress->m_player.m_guid, player->m_obj->m_guid, CriteriaPlayerNameCallback, reinterpret_cast<void*>(achievementRec->m_id), true);
            } else {
                name = g_nameDBCache.GetRecord(progress->m_player.m_guid, WOWGUID(), NULL, NULL, false);
            }
        }

        if (name) {
            lua_pushstring(L, name->m_name);
        } else {
            lua_pushnil(L);
        }
    }

    lua_pushnumber(L, criteriaFlags);
    lua_pushnumber(L, criteria->m_assetID);
    lua_pushstring(L, text);
    lua_pushnumber(L, criteriaId);
    lua_pushboolean(L, IsCriteriaEligible(criteria));
    return 11;
}

// OFFSET: 0x0094C940/0x1406BB7D0
static int32_t Script_GetComparisonStatistic(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetComparisonStatistic(achievementID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    uint64_t value = 0;
    uint64_t sum = 0;
    uint64_t highest = 0;
    uint64_t count = 0;
    int32_t highestId = -1;
    int32_t criteriaFlags = 0;
    bool tie = false;
    ACHIEVEMENT* achievement = s_achievements.Ptr(id, HASHKEY_NONE());

    if (!achievement) {
        return 0;
    }

    ACHIEVEMENT* source = achievement;
    CLIENT_ACHIEVEMENT_CRITERIA* earliest = NULL;

    if (achievement->m_rec->m_sharesCriteria) {
        source = s_achievements.Ptr(achievement->m_rec->m_sharesCriteria, HASHKEY_NONE());
    }

    Achievement_CriteriaRec* earliestRec = NULL;

    if (source) {
        for (SPECIFIC_CRITERIA* spec = source->m_criteria.Head(); !source->m_criteria.IsEnd(spec); spec = source->m_criteria.RawNext(spec)) {
            int32_t criteriaId = spec->m_criteria->m_id;

            for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectCriteria.Head(); !s_inspectCriteria.IsEnd(node); node = s_inspectCriteria.RawNext(node)) {
                if (node->m_id != criteriaId) {
                    continue;
                }

                int32_t flags = achievement->m_rec->m_flags;

                if (flags & 0x8) {
                    sum += node->m_counter;
                }

                if (flags & 0x10) {
                    if (node->m_counter > highest) {
                        highest = node->m_counter;
                        highestId = spec->m_criteria->m_id;
                        tie = false;
                    } else if (node->m_counter == highest) {
                        tie = true;
                    }
                }

                if ((flags & 0x20) && node->m_counter >= spec->m_criteria->m_quantity) {
                    count++;
                }

                value = node->m_counter;
                Achievement_CriteriaRec* rec = g_achievement_CriteriaDB.GetRecord(node->m_id);

                if (rec) {
                    criteriaFlags = rec->m_flags;
                }

                if (!earliest || node->m_timer2 < earliest->m_timer2) {
                    earliest = node;
                }

                break;
            }
        }

        if (tie && (source->m_rec->m_flags & 0x400)) {
            highestId = 0;
        }

        if (earliest) {
            earliestRec = g_achievement_CriteriaDB.GetRecord(earliest->m_id);
        }
    }

    char text[3000];
    uint64_t required = 0;
    CGAchievementInfo::BuildOutputString(text, sizeof(text), sum, highest, highestId, count, value, earliest, achievement->m_rec->m_flags, criteriaFlags, false, achievement->m_rec, earliestRec, &required);
    lua_pushstring(L, text);
    return 1;
}

// OFFSET: 0x0094CBE0/0x1406BBBE0
static int32_t Script_GetAchievementLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetAchievementLink(achievementID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    AchievementRec* achievement = g_achievementDB.GetRecord(id);

    if (!achievement) {
        return 0;
    }

    int32_t index = 0;
    uint32_t criteriaBits[4] = { 0, 0, 0, 0 };
    CLIENT_ACHIEVEMENTS* done = CGAchievementInfo::GetAchievement(achievement->m_id);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    ACHIEVEMENT* source = s_achievements.Ptr(id, HASHKEY_NONE());

    if (source->m_rec->m_sharesCriteria) {
        source = s_achievements.Ptr(source->m_rec->m_sharesCriteria, HASHKEY_NONE());
    }

    if (source) {
        for (SPECIFIC_CRITERIA* spec = source->m_criteria.Head(); !source->m_criteria.IsEnd(spec); spec = source->m_criteria.RawNext(spec)) {
            Achievement_CriteriaRec* criteria = spec->m_criteria;

            if (criteria->m_flags & 0x2) {
                continue;
            }

            for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head(); !s_criteria.IsEnd(node); node = s_criteria.RawNext(node)) {
                if (node->m_id != criteria->m_id) {
                    continue;
                }

                if (node->m_counter >= spec->m_criteria->m_quantity) {
                    if (index < 32) {
                        criteriaBits[0] |= 1 << index;
                    } else if (index < 64) {
                        criteriaBits[1] |= 1 << (index - 32);
                    } else if (index < 96) {
                        criteriaBits[2] |= 1 << (index - 64);
                    } else if (index < 128) {
                        criteriaBits[3] |= 1 << (index - 96);
                    }
                }

                break;
            }

            index++;
        }
    }

    if (done) {
        if (!source->m_rec->m_minimumCriteria) {
            criteriaBits[0] = 0xFFFFFFFF;
            criteriaBits[1] = 0xFFFFFFFF;
            criteriaBits[2] = 0xFFFFFFFF;
            criteriaBits[3] = 0xFFFFFFFF;
        }

        lua_pushstring(L, CGTooltip::GetAchievementHyperlink(id, player->m_obj->m_guid, 1, done->m_date, criteriaBits[0], criteriaBits[1], criteriaBits[2], criteriaBits[3]));
        return 1;
    }

    WowTime time;
    lua_pushstring(L, CGTooltip::GetAchievementHyperlink(id, player->m_obj->m_guid, 0, time, criteriaBits[0], criteriaBits[1], criteriaBits[2], criteriaBits[3]));
    return 1;
}

// OFFSET: 0x00948400
static int32_t Script_GetGuildCategoryList(lua_State* L) {
    int32_t count = 1;

    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    for (int32_t i = 0; i < g_achievement_CategoryDB.m_numRecords; i++) {
        Achievement_CategoryRec* category = g_achievement_CategoryDB.GetRecordByIndex(i);

        if (!category || category->m_id == 1) {
            continue;
        }

        Achievement_CategoryRec* parent = g_achievement_CategoryDB.GetRecord(category->m_parent);

        while (parent && parent->m_id != 15076) {
            parent = g_achievement_CategoryDB.GetRecord(parent->m_parent);
        }

        if (!parent) {
            continue;
        }

        lua_checkstack(L, 2);
        lua_pushnumber(L, count);
        lua_pushnumber(L, category->m_id);
        lua_settable(L, -3);
        count++;
    }

    return 1;
}

// OFFSET: 0x00948C90
static int32_t Script_SetFocusedAchievement(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGAchievementInfo::SetFocusedAchievement(static_cast<int32_t>(lua_tonumber(L, 1)));
    }

    return 0;
}

// OFFSET: 0x0094BF10
static int32_t Script_IsAchievementEligible(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsAchievementEligible(achievementID)");
        return 0;
    }

    ACHIEVEMENT* achievement = s_achievements.Ptr(static_cast<int32_t>(lua_tonumber(L, 1)), HASHKEY_NONE());

    if (!achievement) {
        return 0;
    }

    if (achievement->m_rec->m_sharesCriteria) {
        achievement = s_achievements.Ptr(achievement->m_rec->m_sharesCriteria, HASHKEY_NONE());
    }

    int32_t eligible = 1;

    if (achievement) {
        for (SPECIFIC_CRITERIA* node = achievement->m_criteria.Head(); !achievement->m_criteria.IsEnd(node); ) {
            Achievement_CriteriaRec* criteria = node->m_criteria;
            SPECIFIC_CRITERIA* next = achievement->m_criteria.RawNext(node);

            if (criteria->unk_0040 && CGWorldStateInfo::m_worldStates.Ptr(criteria->unk_0040, HASHKEY_NONE()) && GetWorldState(criteria->unk_0040) != criteria->unk_0044) {
                eligible = 0;
                break;
            }

            node = next;
        }
    }

    lua_pushboolean(L, eligible);
    return 1;
}

// OFFSET: 0x0094CEB0
static int32_t Script_GetGuildAchievementNumMembers(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildAchievementNumMembers(achievementID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    GUILDACHIEVEMENTMEMBERLIST* list;

    if (player && player->GetGuildGUID() && (list = s_guildAchievementMembers.Ptr(id, HASHKEY_NONE()))) {
        lua_pushnumber(L, list->m_members.m_count);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x0094CF60
static int32_t Script_GetGuildAchievementMemberInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetGuildAchievementMemberInfo(achievementID, index)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!player->GetGuildGUID()) {
        return 0;
    }

    GUILDACHIEVEMENTMEMBERLIST* list = s_guildAchievementMembers.Ptr(id, HASHKEY_NONE());

    if (!list || index >= list->m_members.m_count) {
        return 0;
    }

    GuildAchievementMember* member = &list->m_members.m_data[index];
    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(member->m_guid.m_guid, none, NULL, NULL, false);

    if (name) {
        lua_pushstring(L, name->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0094D080
static int32_t Script_GetAchievementGuildRep(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    ACHIEVEMENT* achievement = s_achievements.Ptr(static_cast<int32_t>(lua_tonumber(L, 1)), HASHKEY_NONE());

    if (!achievement) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int32_t required = 0;
    bool found = false;

    for (SPECIFIC_CRITERIA* node = achievement->m_criteria.Head(); !achievement->m_criteria.IsEnd(node); ) {
        Achievement_CriteriaRec* criteria = node->m_criteria;
        SPECIFIC_CRITERIA* next = achievement->m_criteria.RawNext(node);

        for (uint32_t i = 0; i < 3; i++) {
            if ((&criteria->unk_0048)[i] == 62) {
                if ((&criteria->unk_0054)[i] > required) {
                    required = (&criteria->unk_0054)[i];
                    found = true;
                }

                break;
            }
        }

        node = next;
    }

    if (!found) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    lua_pushboolean(L, GetRepListRepValue(1168) >= required);
    int32_t reaction;

    if (required >= 42000) {
        reaction = 7;
    } else if (required >= 21000) {
        reaction = 6;
    } else if (required >= 9000) {
        reaction = 5;
    } else if (required >= 3000) {
        reaction = 4;
    } else if (required >= 0) {
        reaction = 3;
    } else if (required >= -3000) {
        reaction = 2;
    } else {
        reaction = required >= -6000;
    }

    lua_pushnumber(L, reaction + 1);
    return 3;
}

// OFFSET: 0x0094EFE0
static int32_t Script_GetGuildAchievementMembers(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGuildAchievementMembers(achievementID)");
        return 0;
    }

    int32_t id = static_cast<int32_t>(lua_tonumber(L, 1));
    AchievementRec* achievement = g_achievementDB.GetRecord(id);

    if (!achievement) {
        return 0;
    }

    if (!(achievement->m_flags & 0x4000) || !(achievement->m_flags & 0x8000) || s_guildAchievementMembers.Ptr(id, HASHKEY_NONE())) {
        return 0;
    }

    s_guildAchievementMembers.New(id, HASHKEY_NONE(), 0, 0);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && player->GetGuildGUID()) {
        UserClientGuildAchievementMembers request(player->m_obj->m_guid.m_guid, player->GetGuildGUID().m_guid, id);
        ClientServices::Send(&request);
    }

    return 0;
}

// STATIC: 0x00D126C8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetCategoryList", &Script_GetCategoryList },
    { "GetGuildCategoryList", &Script_GetGuildCategoryList },
    { "GetStatisticsCategoryList", &Script_GetStatisticsCategoryList },
    { "GetCategoryInfo", &Script_GetCategoryInfo },
    { "GetCategoryNumAchievements", &Script_GetCategoryNumAchievements },
    { "GetComparisonCategoryNumAchievements", &Script_GetComparisonCategoryNumAchievements },
    { "GetAchievementInfo", &Script_GetAchievementInfo },
    { "GetAchievementNumRewards", &Script_GetAchievementNumRewards },
    { "GetAchievementReward", &Script_GetAchievementReward },
    { "GetAchievementNumCriteria", &Script_GetAchievementNumCriteria },
    { "GetAchievementCriteriaInfo", &Script_GetAchievementCriteriaInfo },
    { "SetAchievementComparisonUnit", &Script_SetAchievementComparisonUnit },
    { "ClearAchievementComparisonUnit", &Script_ClearAchievementComparisonUnit },
    { "GetAchievementComparisonInfo", &Script_GetAchievementComparisonInfo },
    { "GetPreviousAchievement", &Script_GetPreviousAchievement },
    { "GetNextAchievement", &Script_GetNextAchievement },
    { "GetAchievementCategory", &Script_GetAchievementCategory },
    { "GetAchievementLink", &Script_GetAchievementLink },
    { "GetNumCompletedAchievements", &Script_GetNumCompletedAchievements },
    { "GetNumComparisonCompletedAchievements", &Script_GetNumComparisonCompletedAchievements },
    { "GetLatestCompletedAchievements", &Script_GetLatestCompletedAchievements },
    { "GetLatestUpdatedStats", &Script_GetLatestUpdatedStats },
    { "GetLatestCompletedComparisonAchievements", &Script_GetLatestCompletedComparisonAchievements },
    { "GetLatestUpdatedComparisonStats", &Script_GetLatestUpdatedComparisonStats },
    { "GetTotalAchievementPoints", &Script_GetTotalAchievementPoints },
    { "GetAchievementInfoFromCriteria", &Script_GetAchievementInfoFromCriteria },
    { "IsAchievementEligible", &Script_IsAchievementEligible },
    { "GetStatistic", &Script_GetStatistic },
    { "GetComparisonStatistic", &Script_GetComparisonStatistic },
    { "GetComparisonAchievementPoints", &Script_GetComparisonAchievementPoints },
    { "CanShowAchievementUI", &Script_CanShowAchievementUI },
    { "GetTrackedAchievements", &Script_GetTrackedAchievements },
    { "AddTrackedAchievement", &Script_AddTrackedAchievement },
    { "RemoveTrackedAchievement", &Script_RemoveTrackedAchievement },
    { "IsTrackedAchievement", &Script_IsTrackedAchievement },
    { "GetNumTrackedAchievements", &Script_GetNumTrackedAchievements },
    { "HasCompletedAnyAchievement", &Script_HasCompletedAnyAchievement },
    { "QueryQuestsCompleted", &Script_QueryQuestsCompleted },
    { "GetQuestsCompleted", &Script_GetQuestsCompleted },
    { "GetGuildAchievementMembers", &Script_GetGuildAchievementMembers },
    { "GetGuildAchievementNumMembers", &Script_GetGuildAchievementNumMembers },
    { "GetGuildAchievementMemberInfo", &Script_GetGuildAchievementMemberInfo },
    { "SetFocusedAchievement", &Script_SetFocusedAchievement },
    { "GetAchievementGuildRep", &Script_GetAchievementGuildRep },
};

// OFFSET: 0x009478F0
void AchievementInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00947920
void AchievementInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
