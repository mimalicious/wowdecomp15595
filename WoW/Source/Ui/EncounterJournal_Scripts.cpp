#include "WoW/Source/Ui/EncounterJournal.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/FileDataRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterCreatureRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterItemRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterRec.h"
#include "WoW/Source/DB/Rec/JournalEncounterSectionRec.h"
#include "WoW/Source/DB/Rec/JournalInstanceRec.h"
#include "WoW/Source/DB/Rec/ScalingStatDistributionRec.h"
#include "WoW/Source/DB/Rec/ScalingStatValuesRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00F77174
static const char* s_itemQualityColorStrings[8] = { "ff9d9d9d", "ffffffff", "ff1eff00", "ff0070dd", "ffa335ee", "ffff8000", "ffe6cc80", "ffe6cc80" };

// OFFSET: [0x00842700/0x1407FDD80]
static int32_t Script_EJ_SelectTier(lua_State* L) {
    return 0;
}

// OFFSET: [0x00842700/0x1407FDD80]
static int32_t Script_EJ_GetTierInfo(lua_State* L) {
    return 0;
}

// OFFSET: 0x00A06D50/0x1407A6ED0
int32_t PushEncounterInfo(lua_State* L, JournalEncounterRec* encounter) {
    if (!encounter) {
        return 0;
    }

    lua_pushstring(L, encounter->m_name);
    lua_pushstring(L, encounter->m_description);
    lua_pushnumber(L, encounter->m_id);
    lua_pushnumber(L, encounter->m_firstSectionID);
    lua_pushstring(L, CGEncounterJournal::BuildLink(1, encounter->m_id, encounter->m_name));
    return 5;
}

// OFFSET: 0x00A06DF0/0x1407A6C80
static int32_t Script_EJ_SetDifficulty(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_SetFlag(difficulty)");
        return 0;
    }

    CGEncounterJournal::s_difficulty = lua_tointeger(L, 1) - 1;
    CGEncounterJournal::s_lootDirty = true;
    FrameScript_SignalEvent(SCRIPT_EVENT_EJ_DIFFICULTY_UPDATE, "%d", CGEncounterJournal::s_difficulty + 1);
    return 0;
}

// OFFSET: 0x00A06E50/0x1407A6CF0
static int32_t Script_EJ_IsValidInstanceDifficulty(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_IsValidInstanceDifficulty(difficulty)");
        return 0;
    }

    int32_t difficulty = lua_tointeger(L, 1) - 1;
    uint32_t mask = 0;

    if (CGEncounterJournal::s_instance) {
        mask = CGEncounterJournal::s_instance->m_difficultyMask;
    }

    lua_pushboolean(L, (mask & (1 << difficulty)) != 0);
    return 1;
}

// OFFSET: 0x00A06EC0/0x1407A6D60
static int32_t Script_EJ_GetDifficulty(lua_State* L) {
    lua_pushnumber(L, static_cast<uint32_t>(CGEncounterJournal::s_difficulty + 1));
    return 1;
}

// OFFSET: 0x00A07020/0x1407A6FF0
static int32_t Script_EJ_SelectEncounter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_SelectEncounter(ID)");
        return 0;
    }

    int32_t encounterID = lua_tointeger(L, 1);

    if (g_journalEncounterDB.GetRecord(encounterID)) {
        CGEncounterJournal::s_encounterID = encounterID;
        CGEncounterJournal::s_lootDirty = true;
        return 0;
    }

    luaL_error(L, "Bad Encounter ID.  Usage: EJ_SelectEncounter(ID)");
    return 0;
}

// OFFSET: 0x00A070A0/0x1407A7080
static int32_t Script_EJ_GetEncounterInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetEncounterInfo(ID)");
        return 0;
    }

    return PushEncounterInfo(L, g_journalEncounterDB.GetRecord(lua_tointeger(L, 1)));
}

// OFFSET: 0x00A07110/0x1407A7100
static int32_t Script_EJ_GetCreatureInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetCreatureInfo(index, [encounterID]");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;
    int32_t encounterID = lua_isnumber(L, 2) ? lua_tointeger(L, 2) : CGEncounterJournal::s_encounterID;
    JournalEncounterCreatureRec* creature = CGEncounterJournal::GetCreature(encounterID, index);

    if (!creature) {
        return 0;
    }

    lua_pushnumber(L, creature->m_id);
    lua_pushstring(L, creature->m_name);
    lua_pushstring(L, creature->m_description);
    lua_pushnumber(L, creature->m_creatureDisplayInfoID);
    FileDataRec* fileData = g_fileDataDB.GetRecord(creature->m_fileDataID);

    if (fileData) {
        char path[2048];
        SStrCopy(path, static_cast<const char*>(fileData->m_filepath), sizeof(path));
        SStrPack(path, static_cast<const char*>(fileData->m_filename), sizeof(path));
        lua_pushstring(L, path);
    } else {
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: 0x00A07290/0x1407A7310
static int32_t Script_EJ_GetSectionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetSectionInfo(ID)");
        return 0;
    }

    JournalEncounterSectionRec* section = g_journalEncounterSectionDB.GetRecord(lua_tointeger(L, 1));

    if (!section) {
        return 0;
    }

    char buffer[2048];
    lua_pushstring(L, section->m_title);

    if (JournalParserParseText(section->m_bodyText, buffer, sizeof(buffer), CGEncounterJournal::s_difficulty)) {
        lua_pushstring(L, buffer);
    } else {
        lua_pushstring(L, "Parsing Error");
    }

    lua_pushnumber(L, section->m_type);
    FileDataRec* fileData = g_fileDataDB.GetRecord(section->m_iconFileDataID);

    if (fileData) {
        SStrCopy(buffer, static_cast<const char*>(fileData->m_filepath), sizeof(buffer));
        SStrPack(buffer, static_cast<const char*>(fileData->m_filename), sizeof(buffer));
        lua_pushstring(L, buffer);
    } else {
        lua_pushstring(L, "");
    }

    lua_pushnumber(L, section->m_iconCreatureDisplayInfoID);

    if (section->m_nextSiblingSectionID) {
        lua_pushnumber(L, section->m_nextSiblingSectionID);
    } else {
        lua_pushnil(L);
    }

    if (section->m_firstChildSectionID) {
        lua_pushnumber(L, section->m_firstChildSectionID);
    } else {
        lua_pushnil(L);
    }

    lua_pushboolean(L, (section->m_difficultyMask & (1 << CGEncounterJournal::s_difficulty)) == 0);
    lua_pushstring(L, CGEncounterJournal::BuildLink(2, section->m_id, section->m_title));
    lua_pushboolean(L, section->m_flags & 1);
    int32_t count = 0;
    int32_t bit = 0;

    for (int32_t flags = section->m_iconFlags; flags; flags >>= 1) {
        if (flags & 1) {
            lua_pushnumber(L, bit);
            count++;
        }

        bit++;
    }

    return count + 10;
}

// OFFSET: 0x00A074F0/0x1407A7650
static int32_t Script_EJ_SetClassLootFilter(lua_State* L) {
    ChrClassesRec* rec;
    int32_t index;

    if (lua_isnumber(L, 1) && (index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, index >= 0) && index < g_chrClassesDB.m_numRecords && (rec = g_chrClassesDB.GetRecordByIndex(index)) != NULL) {
        CGEncounterJournal::s_classFilter = rec->m_id;
        CGEncounterJournal::s_lootDirty = true;
        return 0;
    }

    CGEncounterJournal::s_classFilter = -1;
    CGEncounterJournal::s_lootDirty = true;
    return 0;
}

// OFFSET: 0x00A07580/0x1407A76D0
static int32_t Script_EJ_GetAvailableClasses(lua_State* L) {
    uint32_t count = g_chrClassesDB.m_numRecords;

    for (uint32_t i = 0; i < count; i++) {
        ChrClassesRec* rec = g_chrClassesDB.GetRecordByIndex(i);
        const char* name = CGUnit_C::GetDisplayClassNameFromRecord(rec, 2, NULL);

        if (rec && name) {
            lua_pushstring(L, name);
            lua_pushstring(L, rec->m_filename);
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }

    return 2 * count;
}

// OFFSET: 0x00A07610/0x1407A77C0
static int32_t Script_EJ_GetClassFilter(lua_State* L) {
    int32_t index = CGEncounterJournal::s_classFilter - 1;

    if (index >= 0) {
        ChrClassesRec* rec = g_chrClassesDB.GetRecordByIndex(index);
        const char* name = CGUnit_C::GetDisplayClassNameFromRecord(rec, 2, NULL);

        if (!rec || !name) {
            return 0;
        }

        lua_pushinteger(L, index + 1);
        lua_pushstring(L, name);
        return 2;
    }

    return 0;
}

// OFFSET: 0x00A07680/0x1407A7860
static int32_t Script_EJ_GetNumSearchResults(lua_State* L) {
    lua_pushinteger(L, CGEncounterJournal::s_searchResults.size());
    lua_pushboolean(L, CGEncounterJournal::s_searchTruncated);
    return 2;
}

// OFFSET: 0x00A076B0/0x1407A7890
static int32_t Script_EJ_HandleLinkPath(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: EJ_HandleLinkPath(type, id)");
        return 0;
    }

    int32_t type = lua_tointeger(L, 1);
    int32_t id = lua_tointeger(L, 2);
    JournalInstanceRec* instance = NULL;
    JournalEncounterRec* encounter = NULL;
    JournalEncounterSectionRec* section = NULL;

    if (type == 0) {
        instance = g_journalInstanceDB.GetRecord(id);
    } else if (type == 1) {
        encounter = g_journalEncounterDB.GetRecord(id);

        if (encounter) {
            instance = g_journalInstanceDB.GetRecord(encounter->m_journalInstanceID);
        }
    } else if (type == 2) {
        section = g_journalEncounterSectionDB.GetRecord(id);

        if (section) {
            encounter = g_journalEncounterDB.GetRecord(section->m_journalEncounterID);

            if (encounter) {
                instance = g_journalInstanceDB.GetRecord(encounter->m_journalInstanceID);
            }
        }
    }

    if (!instance) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushinteger(L, instance->m_id);

    if (encounter) {
        lua_pushinteger(L, encounter->m_id);

        if (section) {
            lua_pushinteger(L, section->m_id);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x00A078A0
static int32_t PushSectionPath(lua_State* L, int32_t sectionID) {
    JournalEncounterSectionRec* section = g_journalEncounterSectionDB.GetRecord(sectionID);

    if (section) {
        lua_pushinteger(L, sectionID);
        return PushSectionPath(L, section->m_parentSectionID) + 1;
    }

    return 0;
}

// OFFSET: 0x00A078F0/0x1407A7B00
static int32_t Script_EJ_GetSectionPath(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetSectionPath(id)");
        return 0;
    }

    return PushSectionPath(L, lua_tointeger(L, 1));
}

// OFFSET: 0x00A07AE0
static int32_t Script_EJ_ClearSearch(lua_State* L) {
    CGEncounterJournal::s_searchResults.clear();

    if (CGEncounterJournal::s_searchString) {
        SMemFree(CGEncounterJournal::s_searchString, __FILE__, __LINE__, 0);
    }

    CGEncounterJournal::s_searchString = NULL;
    CGEncounterJournal::s_searchTruncated = false;
    return 0;
}

// OFFSET: 0x00A07E60/0x1407A8020
static int32_t PushInstanceInfo(lua_State* L, JournalInstanceRec* instance) {
    char path[2048];
    lua_pushstring(L, instance->m_name);
    lua_pushstring(L, instance->m_description);
    FileDataRec* fileData = g_fileDataDB.GetRecord(instance->m_backgroundFileDataID);

    if (fileData) {
        SStrCopy(path, static_cast<const char*>(fileData->m_filepath), sizeof(path));
        SStrPack(path, static_cast<const char*>(fileData->m_filename), sizeof(path));
        lua_pushstring(L, path);
    } else {
        lua_pushnil(L);
    }

    fileData = g_fileDataDB.GetRecord(instance->m_buttonFileDataID);

    if (fileData) {
        SStrCopy(path, static_cast<const char*>(fileData->m_filepath), sizeof(path));
        SStrPack(path, static_cast<const char*>(fileData->m_filename), sizeof(path));
        lua_pushstring(L, path);
    } else {
        lua_pushnil(L);
    }

    fileData = g_fileDataDB.GetRecord(instance->m_loreFileDataID);

    if (fileData) {
        SStrCopy(path, static_cast<const char*>(fileData->m_filepath), sizeof(path));
        SStrPack(path, static_cast<const char*>(fileData->m_filename), sizeof(path));
        lua_pushstring(L, path);
    } else {
        lua_pushnil(L);
    }

    JournalEncounterRec* encounter = CGEncounterJournal::GetEncounterByIndex(instance->m_id, 0);

    if (encounter) {
        lua_pushinteger(L, encounter->m_worldMapAreaID);
    } else {
        lua_pushinteger(L, -1);
    }

    lua_pushstring(L, CGEncounterJournal::BuildLink(0, instance->m_id, instance->m_name));
    return 7;
}

// OFFSET: 0x00A08120/0x1407A84B0
static int32_t Script_EJ_GetInstanceInfo(lua_State* L) {
    int32_t instanceID = lua_isnumber(L, 1) ? lua_tointeger(L, 1) : CGEncounterJournal::s_instanceID;
    JournalInstanceRec* instance = g_journalInstanceDB.GetRecord(instanceID);

    if (!instance) {
        return 0;
    }

    return PushInstanceInfo(L, instance);
}

// OFFSET: 0x00A08180/0x1407A8520
static int32_t Script_EJ_GetInstanceByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1) || lua_type(L, 2) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: EJ_GetInstanceByIndex(index, isRaid)");
        return 0;
    }

    int32_t index = lua_tointeger(L, 1) - 1;
    bool isRaid = lua_toboolean(L, 2) != 0;
    JournalInstanceRec* instance = g_journalInstanceDB.GetRecord(CGEncounterJournal::GetInstanceByIndex(index, (isRaid != 0) + 1));

    if (!instance) {
        return 0;
    }

    lua_pushnumber(L, instance->m_id);
    return PushInstanceInfo(L, instance) + 1;
}

// OFFSET: 0x00A08230/0x1407A8630
static int32_t Script_EJ_InstanceIsRaid(lua_State* L) {
    bool isRaid = false;

    if (CGEncounterJournal::s_instanceID) {
        EJInstance* instance = CGEncounterJournal::FindInstance(CGEncounterJournal::s_instanceID);

        if (instance) {
            isRaid = instance->m_instanceType == 2;
        }
    }

    lua_pushboolean(L, isRaid);
    return 1;
}

// OFFSET: 0x00A08270/0x1407A86A0
static int32_t Script_EJ_GetSearchResult(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetSearchResult(index)");
        return 0;
    }

    int32_t index = lua_tointeger(L, 1) - 1;
    int32_t current = 0;
    EJSearchResultList::iterator it = CGEncounterJournal::s_searchResults.begin();

    while (it != CGEncounterJournal::s_searchResults.end() && current != index) {
        ++it;
        current++;
    }

    if (it == CGEncounterJournal::s_searchResults.end()) {
        return 0;
    }

    lua_pushinteger(L, it->m_id);
    lua_pushinteger(L, it->m_type);
    JournalEncounterRec* encounter = NULL;
    uint32_t mask = 0;

    switch (it->m_type) {
        case 0: {
            JournalEncounterItemRec* item = g_journalEncounterItemDB.GetRecord(it->m_id);
            mask = item->m_difficultyMask;
            encounter = g_journalEncounterDB.GetRecord(item->m_journalEncounterID);
            break;
        }

        case 1: {
            encounter = g_journalEncounterDB.GetRecord(it->m_id);
            mask = encounter->m_difficultyMask;
            break;
        }

        case 2: {
            JournalEncounterCreatureRec* creature = g_journalEncounterCreatureDB.GetRecord(it->m_id);

            if (creature) {
                encounter = g_journalEncounterDB.GetRecord(creature->m_journalEncounterID);
                mask = encounter->m_difficultyMask;
            }

            break;
        }

        case 3: {
            JournalEncounterSectionRec* section = g_journalEncounterSectionDB.GetRecord(it->m_id);
            mask = section->m_difficultyMask;
            encounter = g_journalEncounterDB.GetRecord(section->m_journalEncounterID);
            break;
        }
    }

    int32_t difficulty = CGEncounterJournal::s_difficulty;

    if (mask && !((1 << difficulty) & mask)) {
        for (int32_t i = 0; i <= 4; i++) {
            if ((1 << i) & mask) {
                difficulty = i;
                break;
            }
        }
    }

    lua_pushinteger(L, difficulty + 1);
    JournalInstanceRec* instance;

    if (encounter && (instance = g_journalInstanceDB.GetRecord(encounter->m_journalInstanceID)) != NULL) {
        lua_pushinteger(L, instance->m_id);
        lua_pushinteger(L, encounter->m_id);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: 0x00A08420/0x1407A8950
static int32_t Script_EJ_GetEncounterInfoByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetEncounterInfoByIndex(index, [instanceID])");
        return 0;
    }

    int32_t index = lua_tointeger(L, 1) - 1;
    int32_t instanceID = lua_isnumber(L, 2) ? lua_tointeger(L, 2) : CGEncounterJournal::s_instanceID;
    return PushEncounterInfo(L, CGEncounterJournal::GetEncounterByIndex(instanceID, index));
}

// OFFSET: 0x00A08540/0x1407A8AE0
static int32_t Script_EJ_SelectInstance(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_SelectInstance(ID)");
        return 0;
    }

    if (!CGEncounterJournal::SelectInstance(lua_tointeger(L, 1))) {
        luaL_error(L, "Bad Instance ID.  Usage: EJ_SelectInstance(ID)");
        return 0;
    }

    return 0;
}

// OFFSET: 0x00A08F00/0x1407A98A0
static int32_t Script_EJ_GetMapEncounter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetEncounterInfo(index)");
        return 0;
    }

    JournalEncounterRec* encounter = CGEncounterJournal::GetMapEncounter(lua_tointeger(L, 1) - 1);

    if (encounter) {
        lua_pushnumber(L, encounter->m_mapX);
        lua_pushnumber(L, 1.0 - encounter->m_mapY);
        lua_pushinteger(L, encounter->m_journalInstanceID);
        return PushEncounterInfo(L, encounter) + 3;
    }

    return 0;
}

// OFFSET: 0x00A08F90/0x1407A9950
static int32_t Script_EJ_GetCurrentInstance(lua_State* L) {
    lua_pushinteger(L, CGEncounterJournal::GetCurrentInstance());
    return 1;
}

// OFFSET: 0x00A09340/0x1407A9F80
static int32_t PushLootInfo(lua_State* L, JournalEncounterItemRec* item) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t itemId = 0;
    ItemRec* rec;
    ItemRecSparse* sparse;

    if (!item || (itemId = item->m_itemID, rec = g_itemDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false), sparse = g_itemSparseDB.GetRecord(itemId, WOWGUID(), CGEncounterJournal::ItemLoadedCallback, NULL, true), !rec) || !sparse || !player) {
        lua_pushstring(L, FrameScript_GetText("RETRIEVING_ITEM_INFO", -1, GENDER_NOT_APPLICABLE));
        lua_pushstring(L, "Interface\\Icons\\INV_Misc_QuestionMark");
        lua_pushstring(L, "");
        lua_pushstring(L, "");
        lua_pushinteger(L, itemId);
        lua_pushstring(L, "");
        lua_pushinteger(L, 0);
        return 7;
    }

    char armorType[1024];
    char slot[1024];
    char temp[1024];

    if (CGItem_C::BuildItemName(armorType, sizeof(armorType), itemId, 0)) {
        SStrPrintf(slot, sizeof(slot), "|c%s%s|r", s_itemQualityColorStrings[sparse->m_quality], armorType);
        lua_pushstring(L, slot);
    } else {
        lua_pushstring(L, FrameScript_GetText("RETRIEVING_ITEM_INFO", -1, GENDER_NOT_APPLICABLE));
    }

    const char* directory = ClientDBStringLookup(3);
    SStrPrintf(slot, sizeof(slot), "%s%s%s", directory, *directory ? "\\" : "", CGItem_C::GetInventoryArt(rec->m_displayInfoID));
    lua_pushstring(L, slot);
    slot[0] = 0;
    armorType[0] = 0;
    ScalingStatValuesRec* scalingValues = NULL;
    ScalingStatDistributionRec* scalingDistribution = NULL;

    if (sparse->m_scalingStatDistribution) {
        scalingDistribution = g_scalingStatDistributionDB.GetRecord(sparse->m_scalingStatDistribution);
        scalingValues = g_scalingStatValuesDB.GetRecordByIndex(CGTooltip::GetDislayedScalingItemLevel(sparse, NULL, WOWGUID(), 1) - 1);
    }

    ItemSubClassRec* subClass = NULL;

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* candidate = g_itemSubClassDB.GetRecordByIndex(i);

        if (candidate && candidate->m_classID == rec->m_classID && candidate->m_subClassID == rec->m_subclassID) {
            subClass = candidate;

            if (sparse->m_inventoryType != 16 && !(subClass->m_displayFlags & 1) && subClass->m_displayName && *subClass->m_displayName) {
                SStrCopy(armorType, subClass->m_displayName, sizeof(armorType));
            }

            break;
        }
    }

    const char* slotName = FrameScript_GetText(s_inventoryTypeTokens[sparse->m_inventoryType], -1, GENDER_NOT_APPLICABLE);
    SStrCopy(slot, slotName, sizeof(slot));

    if (!*slotName) {
        slot[0] = 0;
    }

    bool unusableType = false;
    bool unusableSlot = false;
    uint32_t proficiency = CGPlayer_C::GetProficiency(static_cast<uint8_t>(rec->m_classID));

    if (proficiency && !((1 << rec->m_subclassID) & proficiency)) {
        if (rec->m_classID == 2) {
            if (subClass->m_prerequisiteProficiency == -1) {
                if (subClass->m_postrequisiteProficiency != -1 && ((1 << subClass->m_postrequisiteProficiency) & proficiency)) {
                    unusableSlot = true;
                } else {
                    unusableType = true;
                }
            } else if ((1 << subClass->m_prerequisiteProficiency) & proficiency) {
                unusableSlot = true;
            } else {
                unusableType = true;
            }
        } else if (!scalingValues || !scalingDistribution || rec->m_classID != 4 || (rec->m_subclassID != 4 && rec->m_subclassID != 3)) {
            unusableType = true;
        } else {
            ChrClassesRec* classRec = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);
            int32_t armor = -1;

            if (classRec) {
                if (rec->m_subclassID == 4 && (classRec->m_flags & 0x20)) {
                    armor = ((proficiency & 8) | 0x10) >> 3;
                } else if (rec->m_subclassID == 3 && (classRec->m_flags & 0x10) && (proficiency & 4)) {
                    armor = 2;
                }
            }

            if (armor == -1) {
                unusableType = true;
            } else {
                for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
                    ItemSubClassRec* candidate = g_itemSubClassDB.GetRecordByIndex(i);

                    if (candidate && candidate->m_classID == 4 && candidate->m_subClassID == armor) {
                        if (candidate->m_displayName && *candidate->m_displayName) {
                            SStrCopy(armorType, candidate->m_displayName, sizeof(armorType));
                        }

                        break;
                    }
                }
            }
        }
    }

    if ((sparse->m_inventoryType == 22 && !CGPlayer_C::CanDualWield()) || unusableSlot) {
        SStrCopy(temp, slot, sizeof(temp));
        SStrPrintf(slot, sizeof(slot), "|cff990000%s|r", temp);
    }

    if (unusableType) {
        SStrCopy(temp, armorType, sizeof(temp));
        SStrPrintf(armorType, sizeof(armorType), "|cff990000%s|r", temp);
    }

    lua_pushstring(L, slot);
    lua_pushstring(L, armorType);
    lua_pushinteger(L, itemId);
    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, sparse->m_quality, 0, 0, 0, 0));
    lua_pushinteger(L, item->m_journalEncounterID);
    return 7;
}

// OFFSET: 0x00A098B0/0x1407AA700
static int32_t Script_EJ_GetLootInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetLootInfo(id)");
        return 0;
    }

    return PushLootInfo(L, g_journalEncounterItemDB.GetRecord(lua_tointeger(L, 1)));
}

// OFFSET: 0x00A09920/0x1407AA780
static int32_t Script_EJ_GetLootInfoByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EJ_GetLootInfo(index)");
        return 0;
    }

    int32_t index = lua_tointeger(L, 1) - 1;
    EJLootList::iterator it = CGEncounterJournal::s_loot.begin();

    while (it != CGEncounterJournal::s_loot.end() && index) {
        ++it;
        index--;
    }

    if (it == CGEncounterJournal::s_loot.end()) {
        return PushLootInfo(L, NULL);
    }

    return PushLootInfo(L, *it);
}

// OFFSET: 0x00A09990/0x1407AA800
static int32_t Script_EJ_SetSearch(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: EJ_SetSearch(search string)");
        return 0;
    }

    CGEncounterJournal::SetSearch(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x00A09D40/0x1407AACC0
static int32_t Script_EJ_GetNumLoot(lua_State* L) {
    lua_pushinteger(L, CGEncounterJournal::BuildLoot());
    return 1;
}

// STATIC: 0x00D17838
static FrameScript_Method s_ScriptFunctions[] = {
    { "EJ_SelectTier", &Script_EJ_SelectTier },
    { "EJ_SelectInstance", &Script_EJ_SelectInstance },
    { "EJ_SelectEncounter", &Script_EJ_SelectEncounter },
    { "EJ_GetTierInfo", &Script_EJ_GetTierInfo },
    { "EJ_GetInstanceInfo", &Script_EJ_GetInstanceInfo },
    { "EJ_GetInstanceByIndex", &Script_EJ_GetInstanceByIndex },
    { "EJ_GetEncounterInfo", &Script_EJ_GetEncounterInfo },
    { "EJ_GetEncounterInfoByIndex", &Script_EJ_GetEncounterInfoByIndex },
    { "EJ_GetCreatureInfo", &Script_EJ_GetCreatureInfo },
    { "EJ_GetSectionInfo", &Script_EJ_GetSectionInfo },
    { "EJ_SetClassLootFilter", &Script_EJ_SetClassLootFilter },
    { "EJ_GetAvailableClasses", &Script_EJ_GetAvailableClasses },
    { "EJ_GetClassFilter", &Script_EJ_GetClassFilter },
    { "EJ_GetNumLoot", &Script_EJ_GetNumLoot },
    { "EJ_GetLootInfoByIndex", &Script_EJ_GetLootInfoByIndex },
    { "EJ_GetLootInfo", &Script_EJ_GetLootInfo },
    { "EJ_SetDifficulty", &Script_EJ_SetDifficulty },
    { "EJ_GetDifficulty", &Script_EJ_GetDifficulty },
    { "EJ_InstanceIsRaid", &Script_EJ_InstanceIsRaid },
    { "EJ_IsValidInstanceDifficulty", &Script_EJ_IsValidInstanceDifficulty },
    { "EJ_SetSearch", &Script_EJ_SetSearch },
    { "EJ_ClearSearch", &Script_EJ_ClearSearch },
    { "EJ_GetNumSearchResults", &Script_EJ_GetNumSearchResults },
    { "EJ_GetSearchResult", &Script_EJ_GetSearchResult },
    { "EJ_GetMapEncounter", &Script_EJ_GetMapEncounter },
    { "EJ_GetCurrentInstance", &Script_EJ_GetCurrentInstance },
    { "EJ_HandleLinkPath", &Script_EJ_HandleLinkPath },
    { "EJ_GetSectionPath", &Script_EJ_GetSectionPath },
};

// OFFSET: 0x00A06F00
void EncounterJournalRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00A06F30
void EncounterJournalUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
