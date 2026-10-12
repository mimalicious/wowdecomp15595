#include "QuestFrame.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CharTitlesRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/PageTextMaterialRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// PROVEN: d43b213a IDENTICAL/DIFFERENT
// OFFSET: 0x00944670/0x1406B1550
static int32_t Script_GetQuestID(lua_State* L) {
    lua_pushinteger(L, CGQuestInfo::m_currentQuest);
    return 1;
}

// PROVEN: fe7f026f IDENTICAL/IDENTICAL
// OFFSET: 0x00944690/0x1406B1570
static int32_t Script_GetTitleText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_questTitle);
    return 1;
}

// PROVEN: b60cc7a8 IDENTICAL/IDENTICAL
// OFFSET: 0x009446B0/0x1406B1590
static int32_t Script_GetGreetingText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_greetingText);
    return 1;
}

// PROVEN: a23cf335 IDENTICAL/IDENTICAL
// OFFSET: 0x009446D0/0x1406B15B0
static int32_t Script_GetQuestText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_questText);
    return 1;
}

// PROVEN: 6ea84157 IDENTICAL/IDENTICAL
// OFFSET: 0x009446F0/0x1406B15D0
static int32_t Script_GetObjectiveText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_questLogText);
    return 1;
}

// PROVEN: 32cfbccd IDENTICAL/IDENTICAL
// OFFSET: 0x00944710/0x1406B15F0
static int32_t Script_GetProgressText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_progressText);
    return 1;
}

// PROVEN: 116f097b IDENTICAL/IDENTICAL
// OFFSET: 0x00944730/0x1406B1610
static int32_t Script_GetRewardText(lua_State* L) {
    lua_pushstring(L, CGQuestInfo::m_rewardText);
    return 1;
}

// OFFSET: 0x00944750/0x1406B1630
static int32_t Script_GetNumAvailableQuests(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_numQuests);
    return 1;
}

// OFFSET: 0x00944780/0x1406B1650
static int32_t Script_GetNumActiveQuests(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_numInProgress);
    return 1;
}

// PROVEN: 5af4df45 IDENTICAL/DIFFERENT
// OFFSET: 0x009447B0/0x1406B1670
static int32_t Script_GetAvailableTitle(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        lua_pushstring(L, index < CGQuestInfo::m_numQuests ? CGQuestInfo::m_availableQuests[index].m_title : NULL);
        return 1;
    }

    luaL_error(L, "Usage: GetAvailableTitle(index)");
    return 0;
}

// OFFSET: 0x00944850/0x1406B1700
static int32_t Script_GetActiveTitle(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        lua_pushstring(L, index < CGQuestInfo::m_numInProgress ? CGQuestInfo::m_inProgressQuests[index].m_title : NULL);
        lua_pushboolean(L, CGQuestInfo::IsQuestCompleteAt(index));
        return 2;
    }

    luaL_error(L, "Usage: GetActiveTitle(index)");
    return 0;
}

// PROVEN: d8183c4b IDENTICAL/DIFFERENT
// OFFSET: 0x009448F0/0x1406B1790
static int32_t Script_GetAvailableLevel(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t level;

        if (index < CGQuestInfo::m_numQuests) {
            level = CGQuestInfo::m_availableQuests[index].m_level;
        } else {
            level = 0;
        }

        lua_pushnumber(L, level);
        return 1;
    }

    luaL_error(L, "Usage: GetGetAvailableLevel(index)");
    return 0;
}

// PROVEN: ae97175b IDENTICAL/DIFFERENT
// OFFSET: 0x00944990/0x1406B1810
static int32_t Script_GetActiveLevel(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t level;

        if (index < CGQuestInfo::m_numInProgress) {
            level = CGQuestInfo::m_inProgressQuests[index].m_level;
        } else {
            level = 0;
        }

        lua_pushnumber(L, level);
        return 1;
    }

    luaL_error(L, "Usage: GetGetActiveLevel(index)");
    return 0;
}

// PROVEN: 37e5c769 IDENTICAL/UNPROVEN
// OFFSET: 0x00944A30/0x1406B1890
static int32_t Script_GetRewardMoney(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewards.m_money);
    return 1;
}

// PROVEN: 42e13d84 IDENTICAL/UNPROVEN
// OFFSET: 0x00944A60/0x1406B18B0
static int32_t Script_GetRewardXP(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewards.m_xp);
    return 1;
}

// PROVEN: 905cb92b IDENTICAL/DIFFERENT
// OFFSET: 0x00944A90/0x1406B18D0
static int32_t Script_GetRewardHonor(lua_State* L) {
    lua_pushinteger(L, CGQuestInfo::m_rewards.m_honor);
    return 1;
}

// PROVEN: 7bc8a746 IDENTICAL/DIFFERENT
// OFFSET: 0x00944AB0/0x1406B18F0
static int32_t Script_GetRewardTalents(lua_State* L) {
    lua_pushinteger(L, CGQuestInfo::m_rewards.m_talents);
    return 1;
}

// PROVEN: 237a49c4 IDENTICAL/IDENTICAL
// OFFSET: 0x00944AD0/0x1406B1910
static int32_t Script_GetQuestMoneyToGet(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_requiredMoney);
    return 1;
}

// PROVEN: 52bb2948 IDENTICAL/DIFFERENT
// OFFSET: 0x00944B00/0x1406B1930
static int32_t Script_GetNumQuestRewards(lua_State* L) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (!CGQuestInfo::m_questItems[i].m_reward.m_itemID) {
            break;
        }

        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: b755215b IDENTICAL/DIFFERENT
// OFFSET: 0x00944B50/0x1406B1970
static int32_t Script_GetNumQuestChoices(lua_State* L) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (!CGQuestInfo::m_questItems[i].m_choice.m_itemID) {
            break;
        }

        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: f9e73193 IDENTICAL/DIFFERENT
// OFFSET: 0x00944BA0/0x1406B19B0
static int32_t Script_GetNumQuestItems(lua_State* L) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (!CGQuestInfo::m_questItems[i].m_required.m_displayID) {
            break;
        }

        count++;
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: 939a2f7a IDENTICAL/IDENTICAL
// OFFSET: 0x00944BF0/0x1406B19F0
static int32_t Script_QuestChooseRewardError(lua_State* L) {
    CGGameUI::DisplayError(ERR_QUEST_MUST_CHOOSE);
    return 0;
}

// PROVEN: 9fc66b01 IDENTICAL/IDENTICAL
// OFFSET: 0x00944C00/0x1406B1A10
static int32_t Script_GetSuggestedGroupNum(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_suggestedGroupNum);
    return 1;
}

// PROVEN: 4837c60d IDENTICAL/IDENTICAL
// OFFSET: 0x00944C30/0x1406B1A30
static int32_t Script_QuestFlagsPVP(lua_State* L) {
    if (s_isPvPRealm || !(CGQuestInfo::m_questFlags & 0x2000)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// PROVEN: 2a112e5c IDENTICAL/IDENTICAL
// OFFSET: 0x00944C80/0x1406B1A70
static int32_t Script_QuestGetAutoAccept(lua_State* L) {
    if (CGQuestInfo::m_questFlags & 0x80000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 5f8babe0 IDENTICAL/IDENTICAL
// OFFSET: 0x00944CC0/0x1406B1AB0
static int32_t Script_QuestGetAutoLaunched(lua_State* L) {
    if (CGQuestInfo::m_autoLaunched) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: c390cf8e IDENTICAL/IDENTICAL
// OFFSET: 0x00944D00/0x1406B1AF0
static int32_t Script_QuestIsFromAreaTrigger(lua_State* L) {
    if (CGQuestInfo::m_fromAreaTrigger) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 4036c54c IDENTICAL/IDENTICAL
// OFFSET: 0x00944D40/0x1406B1B30
static int32_t Script_GetMaxDailyQuests(lua_State* L) {
    lua_pushnumber(L, 25.0);
    return 1;
}

// PROVEN: ec9fc4c2 IDENTICAL/UNPROVEN
// OFFSET: 0x00944D70/0x1406B1B50
static int32_t Script_GetRewardArenaPoints(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewards.m_arenaPoints);
    return 1;
}

// PROVEN: e2814cca IDENTICAL/IDENTICAL
// OFFSET: 0x00944DA0/0x1406B1B70
static int32_t Script_QuestIsDaily(lua_State* L) {
    if (CGQuestInfo::m_questFlags & 0x1000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: a81be9ce IDENTICAL/IDENTICAL
// OFFSET: 0x00944DE0/0x1406B1BB0
static int32_t Script_QuestIsWeekly(lua_State* L) {
    if (CGQuestInfo::m_questFlags & 0x8000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: d1e6f53a IDENTICAL/IDENTICAL
// OFFSET: 0x00944E20/0x1406B1BF0
static int32_t Script_GetQuestPortraitGiver(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_portraitGiver);
    lua_pushstring(L, CGQuestInfo::m_portraitGiverText);
    lua_pushstring(L, CGQuestInfo::m_portraitGiverName);
    return 3;
}

// PROVEN: 01a0570d IDENTICAL/IDENTICAL
// OFFSET: 0x00944E60/0x1406B1C40
static int32_t Script_GetQuestPortraitTurnIn(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_portraitTurnIn);
    lua_pushstring(L, CGQuestInfo::m_portraitTurnInText);
    lua_pushstring(L, CGQuestInfo::m_portraitTurnInName);
    return 3;
}

// PROVEN: 0807fe1b IDENTICAL/UNPROVEN
// OFFSET: 0x00944EA0/0x1406B1C90
static int32_t Script_GetRewardSkillLineID(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewards.m_skillLine);
    return 1;
}

// PROVEN: 1cbae2c5 IDENTICAL/UNPROVEN
// OFFSET: 0x00944ED0/0x1406B1CB0
static int32_t Script_GetRewardNumSkillUps(lua_State* L) {
    lua_pushnumber(L, CGQuestInfo::m_rewards.m_skillPoints);
    return 1;
}

// PROVEN: b9fbf9cc IDENTICAL/DIFFERENT
// OFFSET: 0x00944F00/0x1406B1CD0
static int32_t Script_GetNumQuestCurrencies(lua_State* L) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (CGQuestInfo::m_questRequest.m_currencies[i].m_currencyID) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: 969ab72b IDENTICAL/DIFFERENT
// OFFSET: 0x00944F60/0x1406B1D10
static int32_t Script_GetNumRewardCurrencies(lua_State* L) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (CGQuestInfo::m_rewards.m_currencyIDs[i]) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: ba48af13 IDENTICAL/IDENTICAL
// OFFSET: 0x00944FC0/0x1406B1D50
static int32_t Script_PlayAutoAcceptQuestSound(lua_State* L) {
    SI3::PlayUISound(CGQuestInfo::m_autoAcceptSound, NULL, true);
    CGQuestInfo::m_autoAcceptSound = 0;
    return 0;
}

// PROVEN: 2116af7d IDENTICAL/IDENTICAL
// OFFSET: 0x00944FE0/0x1406B1D80
static int32_t Script_ClearAutoAcceptQuestSound(lua_State* L) {
    CGQuestInfo::m_autoAcceptSound = 0;
    return 0;
}

// OFFSET: 0x009458C0/0x1406B27C0
static int32_t Script_GetRewardSpell(lua_State* L) {
    int32_t spellId = CGQuestInfo::m_rewards.m_spell;
    const SpellRec* spell;

    if (!spellId || !(spell = g_spellDB.GetRecord(spellId))) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 4;
    }

    if (SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID)) {
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

    int32_t castId = CGQuestInfo::m_rewards.m_spellCast;
    const SpellRec* cast;
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects;

    if (castId && (cast = g_spellDB.GetRecord(castId)) && (effects = SpellGetEffects(cast->m_id, count), count)) {
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

// PROVEN: 4837df81 IDENTICAL/DIFFERENT
// OFFSET: 0x00945A30/0x1406B2990
static int32_t Script_GetRewardSkillPoints(lua_State* L) {
    SkillLineRec* skill = g_skillLineDB.GetRecord(CGQuestInfo::m_rewards.m_skillLine);

    if (skill) {
        lua_pushstring(L, skill->m_displayName);

        if (SpellIconRec* icon = g_spellIconDB.GetRecord(skill->m_spellIconID)) {
            lua_pushstring(L, icon->m_textureFilename);
        } else {
            lua_pushnil(L);
        }

        lua_pushinteger(L, CGQuestInfo::m_rewards.m_skillPoints);
        return 3;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

// PROVEN: 3e90b3b9 IDENTICAL/UNPROVEN
// OFFSET: 0x00945AE0/0x1406B2AA0
static int32_t Script_GetQuestSpellLink(lua_State* L) {
    if (g_spellDB.GetRecord(CGQuestInfo::m_rewards.m_spell)) {
        lua_pushstring(L, CGTooltip::GetSpellHyperlinkString(CGQuestInfo::m_rewards.m_spell));
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00945B40/0x1406B2B10
static int32_t Script_ConfirmAcceptQuest(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_QUEST_CONFIRM_ACCEPT));
    msg.Put(static_cast<uint32_t>(CGQuestInfo::m_pendingQuest));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00945BC0/0x1406B2BA0
static int32_t Script_GetQuestCurrencyInfo(lua_State* L) {
    char texture[260];
    char name[256];
    uint32_t count;

    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !CGQuestInfo::GetQuestCurrencyInfo(lua_tolstring(L, 1, NULL), static_cast<int32_t>(lua_tonumber(L, 2)) - 1, name, sizeof(name), texture, sizeof(texture), count)) {
        luaL_error(L, "Invalid quest item in GetQuestCurrencyInfo(\"type\", index)");
        return 0;
    }

    lua_pushstring(L, name);
    lua_pushstring(L, texture);
    lua_pushnumber(L, count);
    return 3;
}

// PROVEN: af5e9950 IDENTICAL/IDENTICAL
// OFFSET: 0x00946450/0x1406B36A0
static int32_t Script_CloseQuest(lua_State* L) {
    CGQuestInfo::QuestGiverFinished(0, CGQuestInfo::m_currentQuest, 1, 0);
    return 0;
}

// OFFSET: 0x00946470/0x1406B36C0
static int32_t Script_IsAvailableQuestTrivial(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t level = index < CGQuestInfo::m_numQuests ? CGQuestInfo::m_availableQuests[index].m_level : 0;

        if (CGQuestInfo::IsTrivialQuest(level)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    luaL_error(L, "Usage: IsAvailableQuestTrivial(index)");
    return 0;
}

// OFFSET: 0x00946520/0x1406B3750
static int32_t Script_IsActiveQuestTrivial(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t level = index < CGQuestInfo::m_numInProgress ? CGQuestInfo::m_inProgressQuests[index].m_level : 0;

        if (CGQuestInfo::IsTrivialQuest(level)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    luaL_error(L, "Usage: IsActiveQuestTrivial(index)");
    return 0;
}

// PROVEN: 2f47eb7e IDENTICAL/DIFFERENT
// OFFSET: 0x009465D0/0x1406B37E0
static int32_t Script_SelectAvailableQuest(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGQuestInfo::QueryQuest(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
        return 0;
    }

    luaL_error(L, "Usage: SelectAvailableQuest(index)");
    return 0;
}

// PROVEN: cd3b6283 IDENTICAL/DIFFERENT
// OFFSET: 0x00946640/0x1406B3830
static int32_t Script_SelectActiveQuest(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGQuestInfo::CompleteQuest(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
        return 0;
    }

    luaL_error(L, "Usage: SelectActiveQuest(index)");
    return 0;
}

// OFFSET: 0x009466B0/0x1406B3880
static int32_t Script_ShowQuestComplete(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CompleteQuest(index)");
        return 0;
    }

    int32_t questId = CGQuestLog::GetQuestID(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

    if (questId) {
        WOWGUID requester = WOWGUID();
        QuestCache* quest = g_questDBCache.GetRecord(questId, requester, NULL, NULL, false);

        if (quest && (quest->m_flags & 0x10000)) {
            if (CGPlayer_C* player = ClntObjMgrGetActivePlayerObj()) {
                WOWGUID guid = player->m_obj->m_guid;
                CGPlayer_C::CompleteQuest(guid, questId, 1);
            }
        }
    }

    return 0;
}

// PROVEN: a8d6436a IDENTICAL/IDENTICAL
// OFFSET: 0x00946770/0x1406B3950
static int32_t Script_AcceptQuest(lua_State* L) {
    CGQuestInfo::AcceptQuest();
    return 0;
}

// PROVEN: b6d91b1b IDENTICAL/IDENTICAL
// OFFSET: 0x00946780/0x1406B3960
static int32_t Script_DeclineQuest(lua_State* L) {
    CGQuestInfo::DeclineQuest();
    return 0;
}

// PROVEN: 81c4ff82 IDENTICAL/IDENTICAL
// OFFSET: 0x00946790/0x1406B3970
static int32_t Script_IsQuestCompletable(lua_State* L) {
    if (CGQuestInfo::IsCompletable()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: c68bfb03 IDENTICAL/IDENTICAL
// OFFSET: 0x009467D0/0x1406B39B0
static int32_t Script_CompleteQuest(lua_State* L) {
    CGQuestInfo::GiveQuestItems();
    return 0;
}

// PROVEN: 70f76374 IDENTICAL/IDENTICAL
// OFFSET: 0x009467E0/0x1406B39C0
static int32_t Script_GetQuestReward(lua_State* L) {
    int32_t choice = -1;

    if (lua_isnumber(L, 1)) {
        choice = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    }

    if (!CGQuestInfo::GetReward(choice)) {
        luaL_error(L, "Invalid reward choice in GetQuestReward([choice])");
        return 0;
    }

    return 0;
}

// PROVEN: e51beb74 IDENTICAL/DIFFERENT
// OFFSET: 0x00946830/0x1406B3A20
static int32_t Script_GetRewardTitle(lua_State* L) {
    int32_t titleId = CGQuestInfo::m_rewards.m_title;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CharTitlesRec* title = g_charTitlesDB.GetRecord(titleId);

    if (title) {
        int32_t gender = player ? player->m_player->m_playerBytes3[0] : 0;
        char format[256];
        format[0] = 0;

        if (gender == 1) {
            if (strcmp(title->m_name1, "")) {
                strcpy(format, title->m_name1);
            } else {
                strcpy(format, title->m_name);
            }
        } else {
            if (strcmp(title->m_name, "")) {
                strcpy(format, title->m_name);
            } else {
                strcpy(format, title->m_name1);
            }
        }

        char text[256];
        SStrPrintf(text, sizeof(text), format, player ? player->GetUnitName(NULL, 1) : "");
        lua_pushstring(L, text);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// PROVEN: c4b74217 IDENTICAL/DIFFERENT
// OFFSET: 0x009469C0/0x1406B3C30
static int32_t Script_GetDailyQuestsCompleted(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t count = 0;

    if (player) {
        for (uint32_t i = 0; i < 25; i++) {
            if (player->m_player->m_playerFieldDailyQuests1[i]) {
                count++;
            }
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x00946A50/0x1406B3CA0
static int32_t Script_GetAvailableQuestInfo(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t level = index < CGQuestInfo::m_numQuests ? CGQuestInfo::m_availableQuests[index].m_level : 0;

        if (CGQuestInfo::IsTrivialQuest(level)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        if (index < CGQuestInfo::m_numQuests && (CGQuestInfo::m_availableQuests[index].m_flags & 0x1000)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        if (index < CGQuestInfo::m_numQuests && CGQuestInfo::m_availableQuests[index].m_repeatable) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 3;
    }

    luaL_error(L, "Usage: GetAvailableQuestInfo(index)");
    return 0;
}

// OFFSET: 0x00946D90/0x1406B4050
static int32_t Script_GetCriteriaSpell(lua_State* L) {
    uint32_t spellId = CGQuestInfo::m_criteriaSpell;
    const SpellRec* spell;

    if (!spellId || !(spell = g_spellDB.GetRecord(spellId))) {
        return 0;
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);
    lua_pushinteger(L, spellId);
    lua_pushstring(L, spell->m_name);
    lua_pushstring(L, icon ? static_cast<const char*>(icon->m_textureFilename) : NULL);
    lua_pushboolean(L, spellId <= static_cast<uint32_t>(g_spellDB.m_maxID) && ((1 << (spellId & 0x1F)) & s_knownSpellBits.m_bits.m_data[spellId >> 5]));
    return 4;
}

// PROVEN: b12b104f IDENTICAL/DIFFERENT
// OFFSET: 0x00947490/0x1406B49C0
static int32_t Script_ShowQuestOffer(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        WOWGUID guid = player->m_obj->m_guid;
        int32_t questId = CGQuestLog::GetQuestID(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
        CGQuestInfo::ShowOfferFromQuest(player, guid, questId);
        return 0;
    }

    luaL_error(L, "Usage: ShowQuestOffer(index)");
    return 0;
}

// OFFSET: 0x00947510/0x1406B4A60
static int32_t Script_GetQuestItemInfo(lua_State* L) {
    char texture[260];
    char name[256];
    uint32_t count;
    int32_t quality;
    int32_t usable;

    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !CGQuestInfo::GetQuestItemInfo(lua_tolstring(L, 1, NULL), static_cast<int32_t>(lua_tonumber(L, 2)) - 1, name, sizeof(name), texture, sizeof(texture), count, quality, usable)) {
        luaL_error(L, "Invalid quest item in GetQuestItemInfo(\"type\", index)");
        return 0;
    }

    lua_pushstring(L, name);
    lua_pushstring(L, texture);
    lua_pushnumber(L, count);
    lua_pushnumber(L, quality);

    if (usable) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: 0x00947640/0x1406B4BD0
static int32_t Script_GetQuestItemLink(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetQuestItemLink(\"type\", index)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 2));
    int32_t itemId = CGQuestInfo::GetQuestItemID(lua_tolstring(L, 1, NULL), index - 1);
    ItemRecSparse* rec;
    WOWGUID requester;
    requester.m_guid = 0;

    if (itemId && (rec = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false)) != NULL) {
        lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, rec->m_quality, 0, 0, 0, 0));
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00947730/0x1406B4CE0
static int32_t Script_GetQuestBackgroundMaterial(lua_State* L) {
    CGObject_C* object = ClntObjMgrObjectPtr(CGQuestInfo::m_npc, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        int32_t material = 0;
        bool found = false;

        if (object->m_obj->m_type & TYPEMASK_ITEM) {
            WOWGUID requester;
            requester.m_guid = 0;

            if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(object->m_obj->m_entryID, requester, NULL, NULL, false)) {
                material = rec->m_pageMaterial;
                found = true;
            }
        } else if (object->m_obj->m_type & TYPEMASK_GAMEOBJECT) {
            material = static_cast<CGGameObject_C*>(object)->GetPageTextMaterial();
            found = true;
        }

        if (found && material > 0) {
            if (PageTextMaterialRec* rec = g_pageTextMaterialDB.GetRecord(material)) {
                lua_pushstring(L, rec->m_name);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// STATIC: 0x00D124B8/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseQuest", &Script_CloseQuest },
    { "GetQuestID", &Script_GetQuestID },
    { "GetTitleText", &Script_GetTitleText },
    { "GetGreetingText", &Script_GetGreetingText },
    { "GetQuestText", &Script_GetQuestText },
    { "GetObjectiveText", &Script_GetObjectiveText },
    { "GetProgressText", &Script_GetProgressText },
    { "GetRewardText", &Script_GetRewardText },
    { "GetNumAvailableQuests", &Script_GetNumAvailableQuests },
    { "GetNumActiveQuests", &Script_GetNumActiveQuests },
    { "GetCriteriaSpell", &Script_GetCriteriaSpell },
    { "GetAvailableTitle", &Script_GetAvailableTitle },
    { "GetActiveTitle", &Script_GetActiveTitle },
    { "GetAvailableLevel", &Script_GetAvailableLevel },
    { "GetActiveLevel", &Script_GetActiveLevel },
    { "IsAvailableQuestTrivial", &Script_IsAvailableQuestTrivial },
    { "IsActiveQuestTrivial", &Script_IsActiveQuestTrivial },
    { "SelectAvailableQuest", &Script_SelectAvailableQuest },
    { "SelectActiveQuest", &Script_SelectActiveQuest },
    { "ShowQuestOffer", &Script_ShowQuestOffer },
    { "ShowQuestComplete", &Script_ShowQuestComplete },
    { "AcceptQuest", &Script_AcceptQuest },
    { "DeclineQuest", &Script_DeclineQuest },
    { "IsQuestCompletable", &Script_IsQuestCompletable },
    { "CompleteQuest", &Script_CompleteQuest },
    { "GetQuestReward", &Script_GetQuestReward },
    { "GetRewardMoney", &Script_GetRewardMoney },
    { "GetRewardXP", &Script_GetRewardXP },
    { "GetRewardHonor", &Script_GetRewardHonor },
    { "GetRewardSpell", &Script_GetRewardSpell },
    { "GetQuestMoneyToGet", &Script_GetQuestMoneyToGet },
    { "GetNumQuestRewards", &Script_GetNumQuestRewards },
    { "GetNumQuestChoices", &Script_GetNumQuestChoices },
    { "GetNumQuestItems", &Script_GetNumQuestItems },
    { "GetQuestItemInfo", &Script_GetQuestItemInfo },
    { "GetQuestItemLink", &Script_GetQuestItemLink },
    { "GetQuestSpellLink", &Script_GetQuestSpellLink },
    { "QuestChooseRewardError", &Script_QuestChooseRewardError },
    { "ConfirmAcceptQuest", &Script_ConfirmAcceptQuest },
    { "GetQuestBackgroundMaterial", &Script_GetQuestBackgroundMaterial },
    { "GetSuggestedGroupNum", &Script_GetSuggestedGroupNum },
    { "QuestFlagsPVP", &Script_QuestFlagsPVP },
    { "QuestGetAutoAccept", &Script_QuestGetAutoAccept },
    { "QuestGetAutoLaunched", &Script_QuestGetAutoLaunched },
    { "QuestIsFromAreaTrigger", &Script_QuestIsFromAreaTrigger },
    { "GetDailyQuestsCompleted", &Script_GetDailyQuestsCompleted },
    { "GetMaxDailyQuests", &Script_GetMaxDailyQuests },
    { "GetRewardTitle", &Script_GetRewardTitle },
    { "GetRewardTalents", &Script_GetRewardTalents },
    { "GetRewardSkillPoints", &Script_GetRewardSkillPoints },
    { "GetRewardArenaPoints", &Script_GetRewardArenaPoints },
    { "GetAvailableQuestInfo", &Script_GetAvailableQuestInfo },
    { "QuestIsDaily", &Script_QuestIsDaily },
    { "QuestIsWeekly", &Script_QuestIsWeekly },
    { "GetQuestPortraitGiver", &Script_GetQuestPortraitGiver },
    { "GetQuestPortraitTurnIn", &Script_GetQuestPortraitTurnIn },
    { "GetRewardSkillLineID", &Script_GetRewardSkillLineID },
    { "GetRewardNumSkillUps", &Script_GetRewardNumSkillUps },
    { "GetNumQuestCurrencies", &Script_GetNumQuestCurrencies },
    { "GetNumRewardCurrencies", &Script_GetNumRewardCurrencies },
    { "GetQuestCurrencyInfo", &Script_GetQuestCurrencyInfo },
    { "PlayAutoAcceptQuestSound", &Script_PlayAutoAcceptQuestSound },
    { "ClearAutoAcceptQuestSound", &Script_ClearAutoAcceptQuestSound },
};

// PROVEN: a761f101 IDENTICAL/UNPROVEN
// OFFSET: 0x00944FF0/0x1406B1D90
void QuestInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 6869f314 IDENTICAL/UNPROVEN
// OFFSET: 0x00945020/0x1406B1DE0
void QuestInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
