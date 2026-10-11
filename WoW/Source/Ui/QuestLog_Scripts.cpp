#include "QuestLog.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include <ctime>
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/DB/Rec/CharTitlesRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/QuestLogLeaderBoardParams.h"
#include "WoW/Source/Ui/WorldMap.h"
#include <cmath>
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/DB/Rec/QuestSortRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"

// STATIC: 0x00D13108
static float s_rafQuestDistanceSq = 10000.0f;

// INLINED
static int32_t GetQuestIDArg(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return CGQuestLog::m_selectedQuest;
    }

    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    return CGQuestLog::m_quests[index].m_questId;
}

// OFFSET: 0x0095C680/0x1406CF480
static int32_t Script_GetNumQuestLogEntries(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::m_numShownQuests);
    uint32_t count = 0;

    for (uint32_t i = 0; i < CGQuestLog::m_numQuest; i++) {
        int32_t index = i;

        if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || !CGQuestLog::m_quests[index].m_isHeader) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 2;
}

// PROVEN: d5d4f689 IDENTICAL/NONE
// OFFSET: 0x0095C700/0x1406CF500
static int32_t Script_GetQuestLogSelection(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::GetSelectedLogEntry() + 1);
    return 1;
}

// PROVEN: ab8052b5 IDENTICAL/NONE
// OFFSET: 0x0095C730/0x1406CF560
static int32_t Script_SetAbandonQuest(lua_State* L) {
    CGQuestLog::m_abandonQuest = CGQuestLog::m_selectedQuest;
    return 0;
}

// OFFSET: 0x0095C740/0x1406CF570
static int32_t Script_GetAbandonQuestName(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_abandonQuest, none, NULL, NULL, false);

    if (quest) {
        lua_pushstring(L, quest->m_name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0095C7B0/0x1406CF5E0
static int32_t Script_GetNumQuestLogRewards(lua_State* L) {
    uint32_t count = 0;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);

    if (quest) {
        uint32_t i = 0;

        for (; i < 4; i++) {
            if (!quest->m_rewardItem[i]) {
                break;
            }
        }

        if (i <= 4) {
            count = i;
        }
    }

    lua_pushnumber(L, static_cast<int32_t>(count));
    return 1;
}

// OFFSET: 0x0095C830/0x1406CF660
static int32_t Script_GetNumQuestLogRewardCurrencies(lua_State* L) {
    uint32_t count = 0;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);

    if (quest) {
        uint32_t i = 0;

        for (; i < 4; i++) {
            if (!quest->m_rewardCurrencyId[i]) {
                break;
            }
        }

        if (i <= 4) {
            count = i;
        }
    }

    lua_pushnumber(L, static_cast<int32_t>(count));
    return 1;
}

// OFFSET: 0x0095C8B0/0x1406CF6E0
static int32_t Script_GetNumQuestLogChoices(lua_State* L) {
    uint32_t count = 0;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);

    if (quest) {
        uint32_t i = 0;

        for (; i < 6; i++) {
            if (!quest->m_rewardChoiceItem[i]) {
                break;
            }
        }

        if (i <= 6) {
            count = i;
        }
    }

    lua_pushnumber(L, static_cast<int32_t>(count));
    return 1;
}

// OFFSET: 0x0095C930/0x1406CF760
static int32_t Script_GetQuestLogRewardTalents(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    int32_t talents = 0;

    if (quest && static_cast<int32_t>(quest->m_rewardTalentPoints) >= 0) {
        talents = quest->m_rewardTalentPoints;
    }

    lua_pushinteger(L, talents);
    return 1;
}

// OFFSET: 0x0095C990/0x1406CF7D0
static int32_t Script_GetQuestLogRequiredMoney(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(GetQuestIDArg(L), none, NULL, NULL, false);

    if (quest && quest->m_rewardMoney < 0) {
        lua_pushnumber(L, -quest->m_rewardMoney);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x0095CA50/0x1406CF8A0
static int32_t Script_GetQuestLogPushable(lua_State* L) {
    int32_t questId;
    int32_t arg = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1)));

    if (arg) {
        int32_t index = arg - 1;
        questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;
    } else {
        questId = CGQuestLog::m_selectedQuest;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);

    if (!quest || !(quest->m_flags & 0x8)) {
        return 0;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// PROVEN: 5a751288 IDENTICAL/NONE
// OFFSET: 0x0095CB00/0x1406CF940
static int32_t Script_GetNumQuestWatches(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::m_numQuestWatches);
    return 1;
}

// OFFSET: 0x0095CB40/0x1406CF970
static int32_t Script_IsQuestWatched(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsQuestWatched(index)");
        return 0;
    }

    bool watched = false;
    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;

    if (index >= 0 && index < static_cast<int32_t>(CGQuestLog::m_numQuest) && !CGQuestLog::m_quests[index].m_isHeader) {
        int32_t questId = CGQuestLog::m_quests[index].m_questId;

        if (questId) {
            for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches; i++) {
                if ((i < 25 ? CGQuestLog::m_questWatches[i].m_questId : 0) == questId) {
                    watched = true;
                }
            }

            if (watched) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0095CC30/0x1406CFA70
static int32_t Script_RemoveQuestWatch(lua_State* L) {
    int32_t questId = GetQuestIDArg(L);

    if (questId) {
        CGQuestLog::RemoveQuestWatch(questId);
    }

    return 0;
}

// OFFSET: 0x0095CCB0/0x1406CFAE0
static int32_t Script_GetQuestIndexForWatch(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestIndexForWatch(index)");
        return 0;
    }

    uint32_t watch = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;
    int32_t questId;

    if (watch < 25 && (questId = CGQuestLog::m_questWatches[watch].m_questId) != 0) {
        for (int32_t i = 0; i < static_cast<int32_t>(CGQuestLog::m_numQuest);) {
            int32_t current = i < 0 || i >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[i].m_isHeader ? 0 : CGQuestLog::m_quests[i].m_questId;
            i++;

            if (current == questId) {
                lua_pushnumber(L, static_cast<uint32_t>(i));
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0095CD90/0x1406CFBB0
static int32_t Script_ProcessQuestLogRewardFactions(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);

    if (quest) {
        for (int32_t i = 0; i < 5; i++) {
            CGQuestInfo::SetRewardFactionInfo(i, quest->m_rewardReputationFaction[i], quest->m_factionRewardId[i], quest->m_rewardReputationOverride[i]);
        }

        CGQuestInfo::SetRewardFactionFlags(quest->m_rewardReputationMask);
        CGQuestInfo::ProcessFactionRewards();
    }

    return 0;
}

// OFFSET: 0x0095CE00/0x1406CFC40
static int32_t Script_GetNumQuestLogRewardFactions(lua_State* L) {
    lua_pushnumber(L, static_cast<int32_t>(CGQuestInfo::m_numRewardFactions));
    return 1;
}

// OFFSET: 0x0095CE30/0x1406CFC60
static int32_t Script_GetQuestLogRewardFactionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_GetQuestLogRewardFactionInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1) - 1.0));
    lua_pushnumber(L, index > 9 ? 0 : CGQuestInfo::m_rewardFactionList[index]);
    lua_pushnumber(L, index > 9 ? 0 : CGQuestInfo::m_rewardFactionAmounts[index]);
    return 2;
}

// OFFSET: 0x0095CF00/0x1406CFD30
static int32_t Script_GetQuestSortIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestSortIndex(questIndex)");
        return 0;
    }

    lua_pushnumber(L, GetQuestSortId(static_cast<int32_t>(lua_tonumber(L, 1)) - 1) + 1);
    return 1;
}

// OFFSET: 0x0095CF60/0x1406CFD90
static int32_t Script_QuestPOIGetQuestIDByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: QuestPOIGetQuestIDByIndex(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index > 49) {
        return 0;
    }

    int32_t questId = CGQuestLog::m_questPOIs[index].m_questId;

    if (questId < 0) {
        return 0;
    }

    lua_pushnumber(L, questId);
    return 1;
}

// OFFSET: 0x0095CFE0/0x1406CFE10
static int32_t Script_SetPOIIconOverlapDistance(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetPOIIconOverlapDistance(index)");
        return 0;
    }

    CGQuestLog::m_questIconOverlapDist = static_cast<float>(lua_tonumber(L, 1));
    return 0;
}

// OFFSET: 0x0095D020/0x1406CFE60
static int32_t Script_SetPOIIconOverlapPushDistance(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetPOIIconOverlapPushDistance(index)");
        return 0;
    }

    float distance = static_cast<float>(lua_tonumber(L, 1));

    if (distance >= CGQuestLog::m_questIconOverlapDist) {
        CGQuestLog::m_questIconOverlapPushDist = distance;
    }

    return 0;
}

// OFFSET: 0x0095D080/0x1406CFEC0
static int32_t Script_ShiftQuestWatches(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: ShiftQuestWatches(indexFrom, indexTo)");
        return 0;
    }

    uint32_t to = static_cast<int64_t>(lua_tonumber(L, 2)) - 1;
    uint32_t from = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    CGQuestLog::ShiftQuestWatches(from, to);
    return 0;
}

// OFFSET: 0x0095D120/0x1406CFF40
static int32_t Script_GetQuestWatchIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestWatchIndex(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    int32_t questId = CGQuestLog::m_quests[index].m_questId;

    if (!questId) {
        return 0;
    }

    for (uint32_t i = 0; i < CGQuestLog::m_numQuestWatches;) {
        int32_t current = i < 25 ? CGQuestLog::m_questWatches[i].m_questId : 0;
        i++;

        if (current == questId) {
            lua_pushnumber(L, i);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0095D200/0x1406D0010
static int32_t Script_GetQuestLogIndexByID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogIndexByID(questID)");
        return 0;
    }

    lua_pushnumber(L, CGQuestLog::GetQuestIndex(static_cast<int32_t>(lua_tonumber(L, 1))));
    return 1;
}

// OFFSET: 0x0095D260/0x1406D00A0
static int32_t Script_GetQuestLogIsAutoComplete(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(GetQuestIDArg(L), none, NULL, NULL, false);
    lua_pushboolean(L, quest && (quest->m_flags & 0x10000));
    return 1;
}

// OFFSET: 0x0095D330/0x1406D0150
static int32_t Script_QuestLogShouldShowPortrait(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(GetQuestIDArg(L), none, NULL, NULL, false);
    lua_pushboolean(L, quest && (quest->m_flags & 0x8000000));
    return 1;
}

// OFFSET: 0x0095D400/0x1406D0200
static int32_t Script_GetQuestLogPortraitGiver(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(GetQuestIDArg(L), none, NULL, NULL, false);

    if (!quest) {
        return 0;
    }

    lua_pushnumber(L, static_cast<int32_t>(quest->m_questGiverPortrait));
    lua_pushstring(L, quest->m_questGiverTextWindow);
    lua_pushstring(L, quest->m_questGiverTargetName);
    return 3;
}

// OFFSET: 0x0095D4E0/0x1406D02E0
static int32_t Script_GetQuestLogPortraitTurnIn(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(GetQuestIDArg(L), none, NULL, NULL, false);

    if (!quest) {
        return 0;
    }

    lua_pushnumber(L, static_cast<int32_t>(quest->m_questTurnInPortrait));
    lua_pushstring(L, quest->m_questTurnTextWindow);
    lua_pushstring(L, quest->m_questTurnTargetName);
    return 3;
}

// PROVEN: bd357a09 IDENTICAL/NONE
// OFFSET: 0x0095D5C0/0x1406D03C0
static int32_t Script_GetNumAutoQuestPopUps(lua_State* L) {
    lua_pushinteger(L, CGQuestLog::m_numAutoQuestPopUps);
    return 1;
}

// OFFSET: 0x0095D5E0/0x1406D03E0
static int32_t Script_FindAutoQuestPopUp(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: FindAutoQuestPopUp(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (index >= 50 || index >= CGQuestLog::m_numAutoQuestPopUps) {
        return 0;
    }

    AutoQuestPopUp* popUp = &CGQuestLog::m_autoQuestPopUps[index];

    if (!popUp) {
        return 0;
    }

    lua_pushinteger(L, popUp->m_questId);

    if (popUp->m_type == 0) {
        lua_pushstring(L, "OFFER");
    } else if (popUp->m_type == 1) {
        lua_pushstring(L, "COMPLETE");
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// PROVEN: 406f6bef IDENTICAL/NONE
// OFFSET: 0x0095D690/0x1406D04D0
static int32_t Script_SetSuperTrackedQuestID(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGQuestLog::m_superTrackedQuest = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        CGQuestLog::m_superTrackedQuest = CGQuestLog::m_selectedQuest;
    }

    return 0;
}

// PROVEN: 73bb14de IDENTICAL/NONE
// OFFSET: 0x0095D6D0/0x1406D0520
static int32_t Script_GetSuperTrackedQuestID(lua_State* L) {
    lua_pushinteger(L, CGQuestLog::m_superTrackedQuest);
    return 1;
}

// OFFSET: 0x0095DC30/0x1406D0D10
static int32_t Script_SelectQuestLogEntry(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGQuestLog::SetSelectedQuest(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
        return 0;
    }

    luaL_error(L, "Usage: SelectQuestLogEntry(index)");
    return 0;
}

// OFFSET: 0x0095DC80/0x1406D0D60
static int32_t Script_GetQuestLogRewardCurrencyInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogRewardCurrencyInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    CurrencyTypesRec* currency;

    if (quest && index <= 3 && (currency = g_currencyTypesDB.GetRecord(quest->m_rewardCurrencyId[index])) != NULL) {
        lua_pushstring(L, currency->m_name);
        char texture[260];
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(currency));
        lua_pushstring(L, texture);
        lua_pushnumber(L, static_cast<int32_t>(quest->m_rewardCurrencyCount[index]));
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 1.0);
    }

    return 3;
}

// OFFSET: 0x0095DDB0/0x1406D0F40
static int32_t Script_GetQuestLogSpellLink(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    int32_t spellId;

    if (quest && (spellId = quest->m_rewardSpellId) != 0 && g_spellDB.GetRecord(spellId)) {
        lua_pushstring(L, CGTooltip::GetSpellHyperlinkString(spellId));
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0095DFB0/0x1406D1160
static int32_t Script_GetQuestLogRewardSkillPoints(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    SkillLineRec* skill = quest ? g_skillLineDB.GetRecord(quest->m_rewardSkillId) : NULL;

    if (!skill) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushstring(L, skill->m_displayName);
    SpellIconRec* icon = g_spellIconDB.GetRecord(skill->m_spellIconID);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    lua_pushinteger(L, quest->m_rewardSkillPoints);
    return 3;
}

// OFFSET: 0x0095E0A0/0x1406D1290
static int32_t Script_GetQuestLogRewardSpell(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    int32_t spellId;
    SpellRec* spell;

    if (!quest || (spellId = quest->m_rewardSpellId) == 0 || (spell = g_spellDB.GetRecord(spellId)) == NULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 4;
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, spell->m_name);

    if (spell->m_attributes & 0x20) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    SpellRec* cast;
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects;

    if (quest->m_effectOnPlayer && (cast = g_spellDB.GetRecord(quest->m_effectOnPlayer)) != NULL && (effects = SpellGetEffects(cast->m_id, count), count)) {
        for (uint32_t i = 0; i < count; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (effect && (effect->m_effect == 36 || effect->m_effect == 57)) {
                lua_pushnumber(L, 1.0);
                return 4;
            }
        }
    }

    lua_pushnil(L);
    return 4;
}

// OFFSET: 0x0095E230/0x1406D1480
static int32_t Script_QuestPOIGetIconInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: QuestPOIGetIconInfo(questID)");
        return 0;
    }

    bool complete;
    int32_t mapId;
    int32_t floor;
    int32_t x;
    int32_t y;
    int32_t objectiveIndex;

    if (!CGQuestLog::GetQuestPOILoc(static_cast<int32_t>(lua_tonumber(L, 1)), complete, mapId, floor, x, y, objectiveIndex)) {
        return 0;
    }

    C3Vector position(static_cast<float>(x), static_cast<float>(y), 0.0f);
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());
    float mapX;
    float mapY;
    bool outland;

    if (!CGWorldMap::GetWorldPosition(position, mapId, &mapX, &mapY, &outland, false, dungeonMap ? dungeonMap->m_id : 0) || outland || fabs(fabs(mapX * mapY)) < 0.001f) {
        return 0;
    }

    lua_pushboolean(L, complete);
    lua_pushnumber(L, mapX);
    lua_pushnumber(L, mapY);
    lua_pushinteger(L, objectiveIndex);
    return 4;
}

// OFFSET: 0x0095E370/0x1406D1650
static int32_t Script_GetQuestLogCompletionText(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(GetQuestIDArg(L), none, NULL, NULL, false);

    if (!quest) {
        return 0;
    }

    char text[3000];
    WOWGUID player = ClntObjMgrGetActivePlayer();

    if ((quest->m_flags & 0x40000) || !quest->m_completionText[0]) {
        QuestParserParseText(quest->m_objText, text, sizeof(text), player, NULL, 0);
    } else {
        QuestParserParseText(quest->m_completionText, text, sizeof(text), player, NULL, 0);
    }

    lua_pushstring(L, text);
    return 1;
}

// PROVEN: 4ca66562 IDENTICAL/NONE
// OFFSET: 0x009608A0/0x1406D41A0
static int32_t Script_AbandonQuest(lua_State* L) {
    CGQuestLog::AbandonSelectedQuest();
    return 0;
}

// OFFSET: 0x009608B0/0x1406D41B0
static int32_t Script_IsUnitOnQuest(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: IsUnitOnQuest(index, \"unit\")");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t questId;
    WOWGUID guid;
    CGPlayer_C* player;

    if (index >= 0 && index < static_cast<int32_t>(CGQuestLog::m_numQuest) && !CGQuestLog::m_quests[index].m_isHeader && (questId = CGQuestLog::m_quests[index].m_questId) != 0 && (guid = Script_GetGUIDFromName(lua_tolstring(L, 2, NULL)), CGPartyInfo::IsMember(guid)) && (player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__))) != NULL && !(player->m_modelFlags & 0x10000)) {
        for (uint32_t slot = 0; slot < 50; slot++) {
            PlayerQuestLogEntry* entry = player->GetQuestLogEntry(slot);

            if (entry && static_cast<int32_t>(entry->m_questId) == questId) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009609E0/0x1406D4310
static int32_t Script_GetQuestLogQuestText(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, QuestQueryCallback, NULL, false);

    char description[3000];
    char objectives[3000];
    WOWGUID player;

    if (quest) {
        player = ClntObjMgrGetActivePlayer();
        QuestParserParseText(quest->m_description, description, sizeof(description), player, NULL, 0);
        player = ClntObjMgrGetActivePlayer();
        QuestParserParseText(quest->m_objText, objectives, sizeof(objectives), player, NULL, 0);
        lua_pushstring(L, description);
        lua_pushstring(L, objectives);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x00960AC0/0x1406D4430
static int32_t Script_GetNumQuestItemDrops(lua_State* L) {
    int32_t count = 0;

    if (ClntObjMgrGetActivePlayerObj()) {
        count = CountQuestItemDropsNeeded(GetQuestIDArg(L), QuestQueryCallback, NULL);
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x00960B80/0x1406D44E0
static int32_t Script_GetQuestLogTimeLeft(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        int32_t index = CGQuestLog::GetSelectedLogEntry();
        uint32_t slot = index < 0 ? 0 : CGQuestLog::m_quests[index].m_logSlot;

        if (slot <= 50) {
            PlayerQuestLogEntry* entry = &player->m_player->m_playerQuestLog[slot];
            int32_t expire;

            if (entry && (expire = entry->m_time) != 0 && !(entry->m_state & 0x2) && CGQuestLog::m_timeOffset) {
                int32_t left = CGQuestLog::m_timeOffset + expire - _time32(NULL) - 1;

                if (left < 0) {
                    left = 0;
                }

                lua_pushnumber(L, left);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00960C50/0x1406D4600
static int32_t Script_IsCurrentQuestFailed(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        int32_t index = CGQuestLog::GetSelectedLogEntry();
        uint32_t slot = index < 0 ? 0 : CGQuestLog::m_quests[index].m_logSlot;

        if (slot <= 50) {
            PlayerQuestLogEntry* entry = &player->m_player->m_playerQuestLog[slot];

            if (entry && ((entry->m_state & 0x2) || CGQuestLog::IsSelectedQuestExpired())) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00960CF0/0x1406D4700
static int32_t Script_GetQuestLogRewardMoney(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    bool maxLevel = player && player->m_unit->m_unitFieldLevel >= player->m_player->m_playerFieldMaxLevel;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    int32_t money = 0;

    if (quest) {
        int32_t reward = 0;

        if (quest->m_rewardMoney > 0) {
            reward = quest->m_rewardMoney;
            money = reward;
        }

        if (maxLevel && reward <= static_cast<int32_t>(quest->m_rewardMoneyInsteadOfXp)) {
            money = quest->m_rewardMoneyInsteadOfXp;
        }
    }

    lua_pushnumber(L, money);
    return 1;
}

// OFFSET: 0x00960DB0/0x1406D47C0
static int32_t Script_GetQuestLogRewardXP(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t questId = CGQuestLog::m_selectedQuest;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);
    int32_t experience = 0;

    if (player && quest) {
        int32_t level = player->m_unit->m_unitFieldLevel;

        if (level < static_cast<int32_t>(player->m_player->m_playerFieldMaxLevel)) {
            experience = CalcQuestExp(questId, level);

            for (uint32_t i = 0; i < 5; i++) {
                WOWGUID guid = CGPartyInfo::GetMember(i);
                CGPlayer_C* member = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

                if (!member || !CGPlayer_C::IsRAFPlayer(guid)) {
                    continue;
                }

                if (s_rafQuestDistanceSq <= player->GetDistanceToPos(member->GetPosition())) {
                    continue;
                }

                int32_t questLevel = quest->m_level;

                if (questLevel == -1) {
                    if (static_cast<int32_t>(player->m_unit->m_unitFieldLevel) <= 79) {
                        experience *= 3;
                    }

                    continue;
                }

                int32_t playerLevel = player->m_unit->m_unitFieldLevel;

                if (playerLevel > questLevel + player->GetQuestGreenRange()) {
                    continue;
                }

                int32_t memberLevel = member->m_unit->m_unitFieldLevel;

                if (memberLevel > questLevel + member->GetQuestGreenRange()) {
                    continue;
                }

                int32_t requiredLevel = quest->m_requiredLevel;

                if (playerLevel < requiredLevel || memberLevel < requiredLevel) {
                    continue;
                }

                if (static_cast<int32_t>(player->m_unit->m_unitFieldLevel) <= 79) {
                    experience *= 3;
                }
            }

            experience = static_cast<int32_t>(static_cast<float>(player->GetQuestExperienceModifier() * static_cast<double>(experience)));
        }
    }

    lua_pushnumber(L, experience);
    return 1;
}

// OFFSET: 0x00960F90/0x1406D49C0
static int32_t Script_GetQuestLogRewardTitle(lua_State* L) {
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    int32_t titleId = quest ? quest->m_rewardTitleId : 0;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    CharTitlesRec* title = g_charTitlesDB.GetRecord(titleId);

    if (!title) {
        lua_pushnil(L);
        return 1;
    }

    int32_t gender;
    const char* name;

    if (player) {
        gender = player->m_player->m_playerBytes3[0];
        name = player->GetUnitName(NULL, 1);
    } else {
        gender = 0;
        name = "";
    }

    const char* format = title->m_name1;

    if (!*format || (gender != 1 && *title->m_name)) {
        format = title->m_name;
    }

    char text[256];
    SStrPrintf(text, sizeof(text), format, name);
    lua_pushstring(L, text);
    return 1;
}

// OFFSET: 0x00961090/0x1406D4B10
static int32_t Script_QuestLogPushQuest(lua_State* L) {
    int32_t questId;
    int32_t arg = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1)));

    if (arg) {
        int32_t index = arg - 1;
        questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;
    } else {
        questId = CGQuestLog::m_selectedQuest;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);

    if (player && quest && (quest->m_flags & 0x8) && (CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers)) {
        CGPlayer_C::PushQuestToParty(questId);
    }

    return 0;
}

// OFFSET: 0x00961170/0x1406D4BF0
static int32_t Script_GetQuestIndexForTimer(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestIndexForTimer(index)");
        return 0;
    }

    int32_t timer = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t found = 0;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && CGQuestLog::m_numQuest) {
        uint32_t count = CGQuestLog::m_numQuest;
        int32_t offset = CGQuestLog::m_timeOffset;

        for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
            if (i < 0 || i >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[i].m_isHeader) {
                continue;
            }

            uint32_t slot = CGQuestLog::m_quests[i].m_logSlot;

            if (static_cast<int32_t>(slot) < 0 || slot > 50) {
                continue;
            }

            PlayerQuestLogEntry* entry = &player->m_player->m_playerQuestLog[slot];

            if (!entry || !entry->m_time || (entry->m_state & 0x2) || offset - _time32(NULL) + static_cast<int32_t>(entry->m_time) - 1 < 0) {
                continue;
            }

            if (found == timer) {
                lua_pushnumber(L, static_cast<uint32_t>(i + 1));
                return 1;
            }

            found++;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009612C0
static int32_t Script_CollapseQuestHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CollapseQuestHeader(index)");
        return 0;
    }

    CGQuestLog::CollapseHeader(static_cast<int32_t>(lua_tonumber(L, 1)) - 1, 1);
    return 0;
}

// OFFSET: 0x00961310
static int32_t Script_ExpandQuestHeader(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ExpandQuestHeader(index)");
        return 0;
    }

    CGQuestLog::CollapseHeader(static_cast<int32_t>(lua_tonumber(L, 1)) - 1, 0);
    return 0;
}

// PROVEN: 078f9aad IDENTICAL/NONE
// OFFSET: 0x00961360
static int32_t Script_GetQuestGreenRange(lua_State* L) {
    lua_pushnumber(L, CGQuestLog::GetQuestGreenRange());
    return 1;
}

// PROVEN: 7c377ff0 IDENTICAL/NONE
// OFFSET: 0x00961390
static int32_t Script_GetQuestLogGroupNum(lua_State* L) {
    lua_pushnumber(L, GetQuestSuggestedPlayers(CGQuestLog::GetSelectedLogEntry()));
    return 1;
}

// OFFSET: 0x009613C0/0x1406D5080
static int32_t Script_GetQuestLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLink(questID)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t questId;
    QuestCache* quest;
    WOWGUID none = WOWGUID::Make(0);

    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader || (questId = CGQuestLog::m_quests[index].m_questId) == 0 || (quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false)) == NULL) {
        lua_pushnil(L);
        return 1;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t level = player && quest->m_level == static_cast<uint32_t>(-1) ? player->m_unit->m_unitFieldLevel : quest->m_level;
    lua_pushstring(L, CGTooltip::GetQuestHyperlinkString(questId, level));
    return 1;
}

// OFFSET: 0x009614C0/0x1406D51A0
static int32_t Script_GetQuestLogSpecialItemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogSpecialItemInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;
    CGItem_C* item = CGQuestLog::GetQuestSpecialItem(index);
    int32_t questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(questId, none, NULL, NULL, false);

    if (!quest || !item) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
    char texture[260];
    const char* path = ClientDBStringLookup(3);
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", item->GetInventoryArt());
    lua_pushstring(L, texture);
    lua_pushnumber(L, item->GetCurrentSpellCharges());
    lua_pushboolean(L, (quest->m_flags >> 28) & 0x1);
    return 4;
}

// OFFSET: 0x00961620/0x1406D5330
static int32_t Script_GetQuestLogSpecialItemCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogSpecialItemCooldown(index)");
        return 0;
    }

    CGItem_C* item = CGQuestLog::GetQuestSpecialItem(static_cast<int64_t>(lua_tonumber(L, 1)) - 1);

    if (!item) {
        return 0;
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetItemCooldown(item, &duration, &start, &enable);
    lua_pushnumber(L, static_cast<double>(start) * 0.001);
    lua_pushnumber(L, static_cast<double>(duration) * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// OFFSET: 0x00961710/0x1406D5400
static int32_t Script_IsQuestLogSpecialItemInRange(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsQuestLogSpecialItemInRange(index [,target])");
        return 0;
    }

    WOWGUID target;
    target.m_guid = static_cast<int64_t>(lua_tonumber(L, 1));
    CGItem_C* item = CGQuestLog::GetQuestSpecialItem(static_cast<int32_t>(target.m_guid) - 1);
    int32_t spellId = item ? item->GetEnchantmentSpell(0) : 0;
    SpellRec* spell;
    CGPlayer_C* player;
    uint8_t inRange;

    if ((spell = g_spellDB.GetRecord(spellId)) != NULL && (player = ClntObjMgrGetActivePlayerObj()) != NULL && Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true, false) && Spell_C_RangeCheckSelected(player, spell, target, &inRange, false, NULL)) {
        lua_pushnumber(L, inRange ? 1.0 : 0.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00961860/0x1406D5560
static int32_t Script_GetQuestWorldMapAreaID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestWorldMapAreaID(questID)");
        return 0;
    }

    int32_t questId = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t floor = 0;
    int32_t areaId = CGQuestLog::GetQuestWorldMapAreaID(questId, floor);
    lua_pushnumber(L, areaId);
    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(areaId);

    if (floor && area && (area->m_flags & 0x4)) {
        floor++;
    }

    lua_pushnumber(L, floor);
    return 2;
}

// OFFSET: 0x00961920/0x1406D5630
static int32_t Script_GetDistanceSqToQuest(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetDistanceSqToQuest(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader) {
        return 0;
    }

    int32_t questId = CGQuestLog::m_quests[index].m_questId;

    if (questId <= 0) {
        return 0;
    }

    C2Vector position;
    uint32_t distance = static_cast<int64_t>(CGQuestLog::GetQuestDistSqr(questId, position));
    lua_pushnumber(L, distance);
    return 1;
}

// OFFSET: 0x00962E50/0x1406D7A00
static int32_t Script_GetQuestLogTitle(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogTitle(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    lua_pushstring(L, CGQuestLog::GetQuestName(index));
    int32_t level = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : GetQuestLevel(CGQuestLog::m_quests[index].m_questId);
    lua_pushnumber(L, level);
    lua_pushstring(L, GetQuestTag(index));
    lua_pushnumber(L, GetQuestSuggestedPlayers(index));

    if (index >= 0 && index < static_cast<int32_t>(CGQuestLog::m_numQuest) && CGQuestLog::m_quests[index].m_isHeader) {
        lua_pushnumber(L, 1.0);
        uint32_t sortId = GetQuestSortId(index);

        if (sortId < 50 && ((1 << sortId) & g_questLogCollapseHeaderFilterCVar->m_intValue)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        lua_pushnil(L);
    } else {
        bool failed = false;

        if (index >= 0 && index < static_cast<int32_t>(CGQuestLog::m_numQuest) && !CGQuestLog::m_quests[index].m_isHeader) {
            uint32_t slot = CGQuestLog::m_quests[index].m_logSlot;

            if (slot <= 50) {
                PlayerQuestLogEntry* entry = &player->m_player->m_playerQuestLog[slot];

                if (entry && (entry->m_state & 0x2)) {
                    failed = true;
                }
            }
        }

        if (failed || (static_cast<uint32_t>(index) < CGQuestLog::m_numQuest && !CGQuestLog::m_quests[index].m_isHeader && CGQuestLog::m_quests[index].m_expired)) {
            lua_pushnumber(L, -1.0);
        } else if (CGQuestLog::IsQuestComplete(index)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    }

    if (IsDailyQuest(index)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    int32_t questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;
    lua_pushnumber(L, questId);

    if (IsQuestStartEvent(index)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    CVar* displayQuestId = CVar::Lookup("displayQuestID");
    lua_pushboolean(L, displayQuestId && displayQuestId->m_intValue);
    return 11;
}

// OFFSET: 0x00963120/0x1406D7CD0
static int32_t Script_GetNumQuestLeaderBoards(lua_State* L) {
    int32_t count = 0;

    if (ClntObjMgrGetActivePlayerObj()) {
        count = CGQuestLog::GetNumQuestLeaderBoards(GetQuestIDArg(L), QuestQueryCallback, NULL);
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x009631E0/0x1406D7D80
static int32_t Script_GetQuestLogCriteriaSpell(lua_State* L) {
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;
    int32_t spellId = CGQuestLog::GetQuestCriteriaSpell(questId);
    SpellRec* spell;

    if (!spellId || (spell = g_spellDB.GetRecord(spellId)) == NULL) {
        return 0;
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);
    lua_pushinteger(L, spellId);
    lua_pushstring(L, spell->m_name);
    lua_pushstring(L, icon ? static_cast<const char*>(icon->m_textureFilename) : NULL);
    bool known = static_cast<uint32_t>(spellId) > static_cast<uint32_t>(g_spellDB.m_maxID) ? false : ((1 << (spellId & 0x1F)) & s_knownSpellBits.m_bits.m_data[static_cast<uint32_t>(spellId) >> 5]) != 0;
    lua_pushboolean(L, known);
    return 4;
}

// OFFSET: 0x00963300/0x1406D7EF0
static int32_t Script_QuestMapUpdateAllQuests(lua_State* L) {
    int32_t zoneId;

    if (CGWorldMap::m_currentContinent < 0) {
        zoneId = CGWorldMap::m_currentOrphanZone;
    } else if (CGWorldMap::m_currentZone < 0) {
        zoneId = CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_areaID;
    } else {
        zoneId = CGWorldMap::m_continents.m_data[CGWorldMap::m_currentContinent].m_zones.m_data[CGWorldMap::m_currentZone];
    }

    WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(zoneId);
    DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());
    int32_t count = 0;

    if ((area && (CGWorldMap::m_currentZone != -1 || CGWorldMap::m_currentContinent == -2)) || dungeonMap) {
        count = CGQuestLog::UpdateAllQuestPOI();
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: ed7c9622 IDENTICAL/NONE
// OFFSET: 0x009633E0/0x1406D7FF0
static int32_t Script_QuestPOIUpdateIcons(lua_State* L) {
    CGQuestLog::UpdatePOIIcons();
    return 0;
}

// OFFSET: 0x009633F0/0x1406D8000
static int32_t Script_QuestPOIGetQuestIDByVisibleIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_QuestPOIGetQuestIDByVisibleIndex(index)");
        return 0;
    }

    int32_t questId = CGQuestLog::GetPOIQuestIDByVisibleIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (questId >= 0) {
        lua_pushnumber(L, questId);
        lua_pushnumber(L, CGQuestLog::GetQuestIndex(questId));
        return 2;
    }

    return 0;
}

// PROVEN: 3c87843e IDENTICAL/NONE
// OFFSET: 0x00963480/0x1406D80C0
static int32_t Script_SortQuestWatches(lua_State* L) {
    lua_pushboolean(L, CGQuestLog::SortQuestWatches());
    return 1;
}

// OFFSET: 0x009634A0/0x1406D80F0
static int32_t Script_AddAutoQuestPopUp(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: AddAutoQuestPopUp(questID, type)");
        return 0;
    }

    int32_t questId = static_cast<int32_t>(lua_tonumber(L, 1));
    const char* name = lua_tolstring(L, 2, NULL);
    int32_t type;

    if (!_stricmp(name, "OFFER")) {
        type = 0;
    } else if (!_stricmp(name, "COMPLETE")) {
        type = 1;
    } else {
        luaL_error(L, "AddAutoQuestPopUp: Unknown pop-up type %s", name);
        return 0;
    }

    if (questId) {
        lua_pushboolean(L, CGQuestLog::AddAutoQuestPopUp(questId, type));
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// PROVEN: 78e02b3a IDENTICAL/NONE
// OFFSET: 0x00963580/0x1406D81F0
static int32_t Script_RemoveAutoQuestPopUp(lua_State* L) {
    int32_t questId = lua_isnumber(L, 1) ? static_cast<int32_t>(lua_tonumber(L, 1)) : CGQuestLog::m_selectedQuest;

    if (questId) {
        CGQuestLog::RemoveAutoQuestPopUp(questId);
    }

    return 0;
}

// OFFSET: 0x00963A60/0x1406D8830
static int32_t Script_GetQuestTimers(lua_State* L) {
    int32_t pushed = 0;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    uint32_t count = CGQuestLog::m_numQuest;
    int32_t offset = CGQuestLog::m_timeOffset;
    int32_t expired = 0;

    if (!count) {
        return 0;
    }

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        if (i < 0 || i >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[i].m_isHeader) {
            continue;
        }

        uint32_t slot = CGQuestLog::m_quests[i].m_logSlot;

        if (slot > 50) {
            continue;
        }

        PlayerQuestLogEntry* entry = &player->m_player->m_playerQuestLog[slot];

        if (!entry || !entry->m_time || (entry->m_state & 0x2) || (static_cast<uint32_t>(i) < CGQuestLog::m_numQuest && CGQuestLog::m_quests[i].m_expired)) {
            continue;
        }

        int32_t left = offset + entry->m_time - _time32(NULL) - 1;

        if (left < 0) {
            if (CGQuestLog::SetQuestExpired(i)) {
                WOWGUID none = WOWGUID::Make(0);
                QuestCache* quest = g_questDBCache.GetRecord(entry->m_questId, none, DisplayQuestFailed, NULL, false);

                if (quest) {
                    CGGameUI::DisplayError(ERR_QUEST_FAILED_S, quest->m_name);
                }

                expired = 1;
            }
        } else {
            pushed++;
            lua_checkstack(L, 1);
            lua_pushnumber(L, left);
        }
    }

    if (expired) {
        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
    }

    return pushed;
}

// OFFSET: 0x00963BF0/0x1406D8A20
static int32_t Script_AddQuestWatch(lua_State* L) {
    int32_t questId = GetQuestIDArg(L);

    if (questId) {
        int32_t duration = 0;

        if (lua_isnumber(L, 2)) {
            duration = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));
        }

        CGQuestLog::AddQuestWatch(questId, duration);
    }

    return 0;
}

// OFFSET: 0x00966230/0x1406DBFF0
static int32_t Script_GetAbandonQuestItems(lua_State* L) {
    uint32_t count = 0;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    WOWGUID none = WOWGUID::Make(0);
    QuestCache* abandon = g_questDBCache.GetRecord(CGQuestLog::m_abandonQuest, none, NULL, NULL, false);

    if (!abandon) {
        return 0;
    }

    int32_t items[11];
    memset(items, 0, sizeof(items));

    for (uint32_t i = 0; i < 6; i++) {
        int32_t itemId = abandon->m_collectItemId[i];

        if (itemId > 0 && player->m_bag.GetItemTypeCount(itemId, 8) > 0) {
            WOWGUID itemGuid = WOWGUID::Make(0);
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, itemGuid, NULL, NULL, false);

            if (stats && stats->m_bonding == 4) {
                uint32_t j = 0;

                for (; j < count && items[j] != itemId; j++) {
                }

                if (j >= count) {
                    items[count++] = itemId;
                }
            }
        }
    }

    int32_t startingItem = abandon->m_startingItemId;

    if (startingItem > 0 && player->m_bag.GetItemTypeCount(startingItem, 8) > 0) {
        WOWGUID itemGuid = WOWGUID::Make(0);
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(startingItem, itemGuid, NULL, NULL, false);

        if (stats && stats->m_bonding == 4) {
            uint32_t j = 0;

            for (; j < count && items[j] != startingItem; j++) {
            }

            if (j >= count) {
                items[count++] = startingItem;
            }
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        int32_t itemId = abandon->m_intermediateItemId[i];

        if (itemId > 0 && player->m_bag.GetItemTypeCount(itemId, 8) > 0) {
            WOWGUID itemGuid = WOWGUID::Make(0);
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, itemGuid, NULL, NULL, false);

            if (stats && stats->m_bonding == 4) {
                uint32_t j = 0;

                for (; j < count && items[j] != itemId; j++) {
                }

                if (j >= count) {
                    items[count++] = itemId;
                }
            }
        }
    }

    uint32_t total = count;

    for (int32_t i = 0; i < static_cast<int32_t>(CGQuestLog::m_numQuest); i++) {
        if (!count) {
            return 0;
        }

        WOWGUID questGuid = WOWGUID::Make(0);
        int32_t questId = i < 0 || i >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[i].m_isHeader ? 0 : CGQuestLog::m_quests[i].m_questId;
        QuestCache* quest = g_questDBCache.GetRecord(questId, questGuid, NULL, NULL, false);

        if (!quest || quest == abandon) {
            continue;
        }

        for (uint32_t j = 0; j < 6; j++) {
            int32_t itemId = quest->m_collectItemId[j];

            for (uint32_t k = 0; k < 11; k++) {
                if (itemId > 0 && itemId == items[k]) {
                    items[k] = 0;
                    count--;
                }
            }
        }

        int32_t startingId = quest->m_startingItemId;

        for (uint32_t k = 0; k < 11; k++) {
            if (startingId > 0 && startingId == items[k]) {
                items[k] = 0;
                count--;
            }
        }

        total = count;
    }

    if (!count) {
        return 0;
    }

    bool first = true;

    for (uint32_t k = 0; k < 11; k++) {
        if (!items[k]) {
            continue;
        }

        WOWGUID itemGuid = WOWGUID::Make(0);
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(items[k], itemGuid, NULL, NULL, false);

        if (first) {
            first = false;
        } else {
            lua_pushstring(L, ", ");
        }

        lua_pushstring(L, stats->GetName(0));
    }

    if (total > 1) {
        lua_concat(L, 2 * total - 1);
    }

    return 1;
}

// OFFSET: 0x00966540/0x1406DC410
static int32_t Script_GetQuestLogRewardInfo(lua_State* L) {
    // STATIC: 0x00F467B0
    static char s_texture[260];

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogRewardInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    ItemRecSparse* stats;
    ItemRec* item;
    WOWGUID itemGuid;

    if (!quest || index > 3 || (itemGuid = WOWGUID::Make(0), (stats = g_itemSparseDB.GetRecord(quest->m_rewardItem[index], itemGuid, ItemQueryCallback, NULL, false)) == NULL) || (item = stats->GetNonSparse()) == NULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 5;
    }

    const char* path = ClientDBStringLookup(3);
    SStrPrintf(s_texture, sizeof(s_texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
    lua_pushstring(L, stats->GetName(0));
    lua_pushstring(L, s_texture);
    lua_pushnumber(L, static_cast<int32_t>(quest->m_rewardItemCount[index]));
    lua_pushnumber(L, stats->m_inventoryType ? stats->m_quality : -1);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t error;

    if (!player || player->CanUseItem(stats, error)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: 0x00966730/0x1406DC630
static int32_t Script_GetQuestLogChoiceInfo(lua_State* L) {
    // STATIC: 0x00F468B8
    static char s_texture[260];

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogRewardInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);
    ItemRecSparse* stats;
    ItemRec* item;
    WOWGUID itemGuid;

    if (!quest || index > 5 || (itemGuid = WOWGUID::Make(0), (stats = g_itemSparseDB.GetRecord(quest->m_rewardChoiceItem[index], itemGuid, ItemQueryCallback, NULL, false)) == NULL) || (item = stats->GetNonSparse()) == NULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 5;
    }

    const char* path = ClientDBStringLookup(3);
    SStrPrintf(s_texture, sizeof(s_texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
    lua_pushstring(L, stats->GetName(0));
    lua_pushstring(L, s_texture);
    lua_pushnumber(L, static_cast<int32_t>(quest->m_rewardChoiceItemCount[index]));
    lua_pushnumber(L, stats->m_inventoryType ? stats->m_quality : -1);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t error;

    if (!player || player->CanUseItem(stats, error)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: 0x00966920/0x1406DC850
static int32_t Script_GetQuestLogItemLink(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetQuestLogItemLink(\"type\", index)");
        return 0;
    }

    const char* type = lua_tolstring(L, 1, NULL);
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    WOWGUID none = WOWGUID::Make(0);
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, none, NULL, NULL, false);

    if (quest && type && *type && index >= 0) {
        int32_t itemId;
        int32_t count;
        bool valid = true;

        if (!_stricmp(type, "choice") && index < 6) {
            itemId = quest->m_rewardChoiceItem[index];
            count = quest->m_rewardChoiceItemCount[index];
        } else if (!_stricmp(type, "reward") && index < 4) {
            itemId = quest->m_rewardItem[index];
            count = quest->m_rewardItemCount[index];
        } else if (!_stricmp(type, "itemToGet") && index < 6) {
            itemId = quest->m_collectItemId[index];
            count = quest->m_collectItemCount[index];
        } else if (!_stricmp(type, "itemDrop") && index < 4) {
            itemId = quest->m_intermediateItemId[index];
            count = quest->m_intermediateItemCount[index];
        } else {
            valid = false;
        }

        if (valid && itemId) {
            WOWGUID itemGuid = WOWGUID::Make(0);
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, itemGuid, NULL, NULL, false);

            if (stats) {
                lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, stats->m_quality, 0, 0, 0, 0));
                lua_pushinteger(L, count);
                return 2;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x00966AE0/0x1406DCA70
static int32_t Script_UseQuestLogSpecialItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: UseQuestLogSpecialItem(index)");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    CGItem_C* item = CGQuestLog::GetQuestSpecialItem(static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1)) - 1));

    if (!item) {
        return 0;
    }

    ItemRecSparse* stats = item->GetStats();

    if (stats) {
        if (!player) {
            return 0;
        }

        int32_t itemId = stats->m_ID;
        WOWGUID none = WOWGUID::Make(0);
        ItemRec* rec = g_itemDB.GetRecord(itemId, none, NULL, NULL, false);

        if (rec && rec->m_inventoryType) {
            uint32_t flags = 0x81;
            int32_t charges = item->GetUsableSpellCharges();

            if (charges != -1 && charges) {
                flags = 0xA1;
            }

            CGItem_C* found = player->m_bag.FindItemOfType(stats->m_ID, flags);
            WOWGUID target = WOWGUID::Make(0);

            if (found) {
                found->UseItem(&target, 0);
                return 0;
            }

            uint32_t slot = 0;
            found = player->m_bag.FindItemOfType(itemId, target, slot, 0x80);

            if (found && !(found->m_itemFlags & 0x1)) {
                player->AutoEquipItem(target, slot, 0);
            }

            return 0;
        }
    }

    WOWGUID target = WOWGUID::Make(0);
    item->UseItem(&target, 0);
    return 0;
}

// OFFSET: 0x009671C0
int32_t CGQuestLog::GetQuestLeaderBoardForItem(int32_t questId, int32_t itemId, QuestLogLeaderBoardParams* params) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    return CGQuestLog::GetQuestLeaderBoardForItem(player, questId, itemId, params);
}

// OFFSET: 0x009674D0/0x1406DD8F0
static int32_t Script_GetQuestLogLeaderBoard(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogLeaderBoard(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    QuestLogLeaderBoardParams params;
    params.m_index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    params.m_requireLogEntry = 1;
    int32_t questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;

    if (!CGQuestLog::GetQuestLeaderBoardByID(questId, &params)) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushstring(L, params.m_text);
    lua_pushstring(L, params.m_type);

    if (params.m_complete) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x00967640/0x1406DDA80
static int32_t Script_GetQuestLogItemDrop(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogItemDrop(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    QuestLogLeaderBoardParams params;
    params.m_index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    params.m_requireLogEntry = 1;
    int32_t questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;

    if (!CGQuestLog::GetQuestLogItemDropByID(questId, &params)) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushstring(L, params.m_text);
    lua_pushstring(L, params.m_type);

    if (params.m_complete) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x00967A00/0x1406DDE20
static int32_t Script_GetQuestPOILeaderBoard(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetQuestLogLeaderBoard(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    QuestLogLeaderBoardParams params;
    params.m_index = static_cast<int32_t>(lua_tonumber(L, 1));
    params.m_requireLogEntry = 1;
    int32_t questId = index < 0 || index >= static_cast<int32_t>(CGQuestLog::m_numQuest) || CGQuestLog::m_quests[index].m_isHeader ? 0 : CGQuestLog::m_quests[index].m_questId;

    if (!CGQuestLog::GetQuestPOILeaderBoardByID(questId, &params)) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushstring(L, params.m_text);
    lua_pushstring(L, params.m_type);

    if (params.m_complete) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// PROVEN: 7f6279b4 IDENTICAL/NONE
// OFFSET: 0x009648A0/0x1406D9E60
static int32_t Script_GetQuestResetTime(lua_State* L) {
    int32_t now = static_cast<int32_t>(time(NULL));

    if (CGQuestLog::m_nextQuestUpdate && now >= CGQuestLog::m_nextQuestUpdate) {
        CGQuestLog::Update(1);
    }

    lua_pushnumber(L, CGQuestLog::m_questResetTime - now);
    return 1;
}

// STATIC: 0x00D12E88
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumQuestLogEntries", &Script_GetNumQuestLogEntries },
    { "GetQuestLogTitle", &Script_GetQuestLogTitle },
    { "SelectQuestLogEntry", &Script_SelectQuestLogEntry },
    { "GetQuestLogSelection", &Script_GetQuestLogSelection },
    { "SetAbandonQuest", &Script_SetAbandonQuest },
    { "GetAbandonQuestName", &Script_GetAbandonQuestName },
    { "GetAbandonQuestItems", &Script_GetAbandonQuestItems },
    { "AbandonQuest", &Script_AbandonQuest },
    { "IsUnitOnQuest", &Script_IsUnitOnQuest },
    { "GetQuestLogQuestText", &Script_GetQuestLogQuestText },
    { "GetNumQuestLeaderBoards", &Script_GetNumQuestLeaderBoards },
    { "GetQuestLogLeaderBoard", &Script_GetQuestLogLeaderBoard },
    { "GetQuestLogCriteriaSpell", &Script_GetQuestLogCriteriaSpell },
    { "GetNumQuestItemDrops", &Script_GetNumQuestItemDrops },
    { "GetQuestLogItemDrop", &Script_GetQuestLogItemDrop },
    { "GetQuestPOILeaderBoard", &Script_GetQuestPOILeaderBoard },
    { "GetQuestLogTimeLeft", &Script_GetQuestLogTimeLeft },
    { "IsCurrentQuestFailed", &Script_IsCurrentQuestFailed },
    { "GetNumQuestLogRewards", &Script_GetNumQuestLogRewards },
    { "GetNumQuestLogChoices", &Script_GetNumQuestLogChoices },
    { "GetNumQuestLogRewardCurrencies", &Script_GetNumQuestLogRewardCurrencies },
    { "GetQuestLogRewardInfo", &Script_GetQuestLogRewardInfo },
    { "GetQuestLogRewardCurrencyInfo", &Script_GetQuestLogRewardCurrencyInfo },
    { "GetQuestLogChoiceInfo", &Script_GetQuestLogChoiceInfo },
    { "GetQuestLogItemLink", &Script_GetQuestLogItemLink },
    { "GetQuestLogSpellLink", &Script_GetQuestLogSpellLink },
    { "GetQuestLogRewardMoney", &Script_GetQuestLogRewardMoney },
    { "GetQuestLogRewardXP", &Script_GetQuestLogRewardXP },
    { "GetQuestLogRewardSpell", &Script_GetQuestLogRewardSpell },
    { "GetQuestLogRequiredMoney", &Script_GetQuestLogRequiredMoney },
    { "GetQuestLogPushable", &Script_GetQuestLogPushable },
    { "QuestLogPushQuest", &Script_QuestLogPushQuest },
    { "GetQuestTimers", &Script_GetQuestTimers },
    { "GetQuestIndexForTimer", &Script_GetQuestIndexForTimer },
    { "CollapseQuestHeader", &Script_CollapseQuestHeader },
    { "ExpandQuestHeader", &Script_ExpandQuestHeader },
    { "GetQuestGreenRange", &Script_GetQuestGreenRange },
    { "GetNumQuestWatches", &Script_GetNumQuestWatches },
    { "IsQuestWatched", &Script_IsQuestWatched },
    { "AddQuestWatch", &Script_AddQuestWatch },
    { "RemoveQuestWatch", &Script_RemoveQuestWatch },
    { "GetQuestIndexForWatch", &Script_GetQuestIndexForWatch },
    { "GetQuestLogGroupNum", &Script_GetQuestLogGroupNum },
    { "GetQuestResetTime", &Script_GetQuestResetTime },
    { "GetQuestLink", &Script_GetQuestLink },
    { "GetQuestLogRewardTitle", &Script_GetQuestLogRewardTitle },
    { "GetQuestLogRewardTalents", &Script_GetQuestLogRewardTalents },
    { "GetQuestLogRewardSkillPoints", &Script_GetQuestLogRewardSkillPoints },
    { "GetQuestLogSpecialItemInfo", &Script_GetQuestLogSpecialItemInfo },
    { "GetQuestLogSpecialItemCooldown", &Script_GetQuestLogSpecialItemCooldown },
    { "IsQuestLogSpecialItemInRange", &Script_IsQuestLogSpecialItemInRange },
    { "UseQuestLogSpecialItem", &Script_UseQuestLogSpecialItem },
    { "ProcessQuestLogRewardFactions", &Script_ProcessQuestLogRewardFactions },
    { "GetNumQuestLogRewardFactions", &Script_GetNumQuestLogRewardFactions },
    { "GetQuestLogRewardFactionInfo", &Script_GetQuestLogRewardFactionInfo },
    { "SortQuestWatches", &Script_SortQuestWatches },
    { "ShiftQuestWatches", &Script_ShiftQuestWatches },
    { "GetQuestWatchIndex", &Script_GetQuestWatchIndex },
    { "GetQuestLogIndexByID", &Script_GetQuestLogIndexByID },
    { "GetQuestLogIsAutoComplete", &Script_GetQuestLogIsAutoComplete },
    { "QuestLogShouldShowPortrait", &Script_QuestLogShouldShowPortrait },
    { "GetQuestLogPortraitGiver", &Script_GetQuestLogPortraitGiver },
    { "GetQuestLogPortraitTurnIn", &Script_GetQuestLogPortraitTurnIn },
    { "QuestMapUpdateAllQuests", &Script_QuestMapUpdateAllQuests },
    { "GetQuestSortIndex", &Script_GetQuestSortIndex },
    { "GetQuestWorldMapAreaID", &Script_GetQuestWorldMapAreaID },
    { "QuestPOIUpdateIcons", &Script_QuestPOIUpdateIcons },
    { "QuestPOIGetIconInfo", &Script_QuestPOIGetIconInfo },
    { "QuestPOIGetQuestIDByIndex", &Script_QuestPOIGetQuestIDByIndex },
    { "QuestPOIGetQuestIDByVisibleIndex", &Script_QuestPOIGetQuestIDByVisibleIndex },
    { "GetQuestLogCompletionText", &Script_GetQuestLogCompletionText },
    { "SetPOIIconOverlapDistance", &Script_SetPOIIconOverlapDistance },
    { "SetPOIIconOverlapPushDistance", &Script_SetPOIIconOverlapPushDistance },
    { "GetDistanceSqToQuest", &Script_GetDistanceSqToQuest },
    { "GetNumAutoQuestPopUps", &Script_GetNumAutoQuestPopUps },
    { "GetAutoQuestPopUp", &Script_FindAutoQuestPopUp },
    { "AddAutoQuestPopUp", &Script_AddAutoQuestPopUp },
    { "RemoveAutoQuestPopUp", &Script_RemoveAutoQuestPopUp },
    { "SetSuperTrackedQuestID", &Script_SetSuperTrackedQuestID },
    { "GetSuperTrackedQuestID", &Script_GetSuperTrackedQuestID },
};

// PROVEN: cb18a370 IDENTICAL/NONE
// OFFSET: 0x0095D6F0
void QuestLogRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 0e94e32e IDENTICAL/NONE
// OFFSET: 0x0095D720
void QuestLogUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
