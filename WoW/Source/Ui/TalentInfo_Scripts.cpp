#include "TalentInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/TalentRec.h"
#include "WoW/Source/DB/Rec/TalentTabRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/DB/Rec/NumTalentsAtLevelRec.h"
#include "WoW/Source/DB/Rec/TalentTreePrimarySpellsRec.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Ui/GameUI.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/QuestTextParser.h"

// STATIC: 0x00D14CD0/0x140BCDBF0
static int32_t s_activateTalentGroupSpells[2] = { 63645, 63644 };

// OFFSET: 0x0099A300/0x14071C170
static int32_t Script_GetNumTalentTabs(lua_State* L) {
    int32_t inspect = StringToBOOL(L, 1, 0);
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t count;

    if (inspect) {
        if (pet) {
            lua_pushnumber(L, 0.0);
            return 1;
        }

        count = CGTalentInfo_Inspect::m_tabs.m_count;
    } else {
        count = pet ? CGTalentInfo::m_petTabs.m_count : CGTalentInfo::m_tabs.m_count;
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: b29659ef IDENTICAL/DIFFERENT
// OFFSET: 0x0099A480/0x14071C350
static int32_t Script_GetNumTalents(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNumTalents(tabIndex[, isInspect[, isPet]])");
        return 0;
    }

    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t inspect = StringToBOOL(L, 2, 0);
    int32_t pet = StringToBOOL(L, 3, 0);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, inspect, pet);

    if (tab) {
        lua_pushnumber(L, tab->m_numTalents);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// PROVEN: 5037c252 IDENTICAL/UNPROVEN
// OFFSET: 0x0099A530/0x14071C450
static int32_t Script_GetNumTalentGroups(lua_State* L) {
    int32_t inspect = StringToBOOL(L, 1, 0);
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t count;

    if (inspect) {
        count = CGTalentInfo_Inspect::m_numTalentGroups;
    } else {
        count = CGTalentInfo::GetNumTalentGroups(pet);
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x0099AF30/0x14071D1B0
static int32_t Script_SetActiveTalentGroup(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetActiveTalentGroup(groupIndex)");
        return 0;
    }

    uint32_t group = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (group != CGTalentInfo::m_activeTalentGroup && group < CGTalentInfo::m_numTalentGroups && group < 2) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player && player->CanChangeTalentsInRaid()) {
            int32_t spellId = s_activateTalentGroupSpells[group];
            Spell_C_ClickSpell(spellId, NULL, ClntObjMgrGetActivePlayer());
            return 0;
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_UI_ERROR_MESSAGE, "%s", FrameScript_GetText("SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW", -1, GENDER_NOT_APPLICABLE));
    }

    return 0;
}

// OFFSET: 0x0099AB80/0x14071CC50
static int32_t Script_GetTalentTabInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTalentTabInfo(tabIndex[, isInspect[, isPet[, groupIndex]]])");
        return 0;
    }

    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t pet = StringToBOOL(L, 3, 0);
    int32_t inspect = StringToBOOL(L, 2, 0);
    TalentTabInfo* info = CGTalentInfo::GetTab(tabIndex, inspect, pet);
    TalentTabRec* tab;

    if (!info || !(tab = g_talentTabDB.GetRecord(info->m_tabId))) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushinteger(L, 0);
        lua_pushnil(L);
        lua_pushinteger(L, 0);
        lua_pushboolean(L, 0);
        return 8;
    }

    char description[1024];
    lua_pushinteger(L, tab->m_id);
    lua_pushstring(L, tab->m_name);
    QuestParserParseText(tab->unk_001C, description, sizeof(description), ClntObjMgrGetActivePlayer(), NULL, 0);
    lua_pushstring(L, description);
    SpellIconRec* icon = g_spellIconDB.GetRecord(tab->m_spellIconID);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    CGTalentGroup_C* group = TalentInfoGetTalentGroup(TalentInfoGetScriptGroupIndex(L, inspect, pet, 4), inspect, pet);
    TalentGroupTab_C* groupTab;

    if (group && (groupTab = group->GetTab(tabIndex))) {
        lua_pushinteger(L, groupTab->m_pointsSpent);
        lua_pushstring(L, tab->m_backgroundFile);
        lua_pushinteger(L, groupTab->m_previewPointsSpent);
        int32_t primaryTree = group->m_previewPrimaryTree;

        if (!primaryTree) {
            primaryTree = group->m_primaryTree;
        }

        if (pet || primaryTree == groupTab->m_tabId) {
            lua_pushboolean(L, 1);
        } else {
            uint32_t spent = 0;

            for (uint32_t i = 0; i < 3; i++) {
                TalentGroupTab_C* other = group->GetTab(i);

                if (other && other->m_tabId == primaryTree) {
                    spent = other->m_pointsSpent + other->m_previewPointsSpent;
                    break;
                }
            }

            lua_pushboolean(L, spent >= 31);
        }
    } else {
        lua_pushinteger(L, 0);
        lua_pushstring(L, tab->m_backgroundFile);
        lua_pushinteger(L, 0);
        lua_pushboolean(L, 0);
    }

    return 8;
}

// OFFSET: 0x0099AE80/0x14071D100
static int32_t Script_GetUnspentTalentPoints(lua_State* L) {
    int32_t inspect = StringToBOOL(L, 1, 0);
    int32_t pet = StringToBOOL(L, 2, 0);
    CGTalentGroup_C* group = TalentInfoGetTalentGroup(TalentInfoGetScriptGroupIndex(L, inspect, pet, 3), inspect, pet);
    lua_pushinteger(L, group ? group->m_unspentPoints : 0);
    return 1;
}

// OFFSET: 0x0099B090/0x14071D360
static int32_t Script_GetGroupPreviewTalentPointsSpent(lua_State* L) {
    int32_t pet = StringToBOOL(L, 1, 0);
    uint32_t group = TalentInfoGetScriptGroupIndex(L, 0, pet, 2);
    lua_pushinteger(L, CGTalentInfo::GetPreviewPointsSpent(group, pet));
    return 1;
}

// OFFSET: 0x0099D080/0x14071FEB0
static int32_t Script_LearnPreviewTalents(lua_State* L) {
    int32_t pet = StringToBOOL(L, 1, 0);
    CGTalentGroup_C* group = CGTalentInfo::GetTalentGroup(CGTalentInfo::GetActiveTalentGroup(pet), pet);

    if (!group) {
        return 0;
    }

    if (group->GetPreviewPointsSpent() <= 0 && !group->m_previewPrimaryTree) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(pet ? CGPetInfo::GetPet(0) : ClntObjMgrGetActivePlayer(), pet ? TYPEMASK_UNIT : TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    CDataStore msg;

    if (pet) {
        msg.Put(static_cast<uint32_t>(CMSG_LEARN_PREVIEW_TALENTS_PET));
        msg.Put(unit->m_obj->m_guid.m_guid);
    } else {
        msg.Put(static_cast<uint32_t>(CMSG_LEARN_PREVIEW_TALENTS));
        TalentTabRec* tabRec = g_talentTabDB.GetRecord(group->m_previewPrimaryTree);

        if (tabRec) {
            msg.Put(static_cast<uint32_t>(tabRec->m_orderIndex));
        } else {
            msg.Put(static_cast<uint32_t>(-1));
        }
    }

    uint32_t count = 0;
    uint32_t countPos = msg.m_size;
    msg.Put(0u);
    int32_t primaryTree = group->m_previewPrimaryTree;

    if (!primaryTree) {
        primaryTree = group->m_primaryTree;
    }

    for (uint32_t i = 0; i < group->m_tabs.m_count; i++) {
        TalentGroupTab_C* tab = &group->m_tabs.m_data[i];

        if (tab->m_tabId != primaryTree) {
            continue;
        }

        for (uint32_t j = 0; j < tab->m_tiers.m_count; j++) {
            TalentGroupTier_C* tier = tab->m_tiers.m_data[j];

            for (TalentGroupTierItem_C* node = tier->m_items.m_fulllist.Head(); node; node = tier->m_items.Next(node)) {
                TalentGroupItem_C* item = node->m_item;

                if (item && item->m_previewRank > item->m_rank) {
                    msg.Put(static_cast<uint32_t>(item->m_talentId));
                    msg.Put(static_cast<uint32_t>(item->m_previewRank));
                    count++;
                }
            }
        }
    }

    for (uint32_t i = 0; i < group->m_tabs.m_count; i++) {
        TalentGroupTab_C* tab = &group->m_tabs.m_data[i];

        if (tab->m_tabId == primaryTree) {
            continue;
        }

        for (uint32_t j = 0; j < tab->m_tiers.m_count; j++) {
            TalentGroupTier_C* tier = tab->m_tiers.m_data[j];

            for (TalentGroupTierItem_C* node = tier->m_items.m_fulllist.Head(); node; node = tier->m_items.Next(node)) {
                TalentGroupItem_C* item = node->m_item;

                if (item && item->m_previewRank > item->m_rank) {
                    msg.Put(static_cast<uint32_t>(item->m_talentId));
                    msg.Put(static_cast<uint32_t>(item->m_previewRank));
                    count++;
                }
            }
        }
    }

    if (count || group->m_previewPrimaryTree) {
        msg.Set(countPos, count);
        msg.Finalize();
        ClientServices::Send2(&msg);

        if (count) {
            g_talentPointsSpentCVar->Set("1", true, false, false, true);
        }
    }

    return 0;
}

// OFFSET: 0x0099CEE0/0x14071FA60
static int32_t Script_ResetPreviewTalentPoints(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ResetPreviewTalentPoints(tabIndex[, isPet[, groupIndex]])");
        return 0;
    }

    uint32_t tab = lua_tointeger(L, 1) - 1;
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t group = TalentInfoGetScriptGroupIndex(L, 0, pet, 3);
    int32_t before = CGTalentInfo::GetPreviewPointsSpent(group, pet);
    CGTalentInfo::ResetPreviewTalentPoints(group, tab, pet);
    int32_t after = CGTalentInfo::GetPreviewPointsSpent(group, pet);

    if (after != before) {
        FrameScript_SignalEvent(pet ? SCRIPT_EVENT_PREVIEW_PET_TALENT_POINTS_CHANGED : SCRIPT_EVENT_PREVIEW_TALENT_POINTS_CHANGED, "%d%d%d%d", 0, tab + 1, group + 1, after - before);
    }

    return 0;
}

// OFFSET: 0x0099CFB0/0x14071FC60
static int32_t Script_ResetGroupPreviewTalentPoints(lua_State* L) {
    int32_t pet = StringToBOOL(L, 1, 0);
    uint32_t group = TalentInfoGetScriptGroupIndex(L, 0, pet, 2);
    int32_t before = CGTalentInfo::GetPreviewPointsSpent(group, pet);
    CGTalentInfo::ResetGroupPreviewTalentPoints(group, pet);
    int32_t after = CGTalentInfo::GetPreviewPointsSpent(group, pet);

    if (after != before) {
        FrameScript_SignalEvent(pet ? SCRIPT_EVENT_PREVIEW_PET_TALENT_POINTS_CHANGED : SCRIPT_EVENT_PREVIEW_TALENT_POINTS_CHANGED, "%d%d%d%d", 0, 0, group + 1, after - before);
    }

    CGTalentGroup_C* talentGroup = CGTalentInfo::GetTalentGroup(group, pet);

    if (talentGroup && talentGroup->m_previewPrimaryTree) {
        CGTalentInfo::SetPreviewPrimaryTalentTree(group, pet != 0, 0);
        FrameScript_SignalEvent(SCRIPT_EVENT_PREVIEW_TALENT_PRIMARY_TREE_CHANGED, "%d", 0);
    }

    return 0;
}

// OFFSET: 0x0099C460/0x14071ED50
static int32_t Script_GetTalentInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetTalentInfo(tabIndex, talentIndex[, isInspect[, isPet[, groupIndex]]])");
        return 0;
    }

    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t talentIndex = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    int32_t inspect = StringToBOOL(L, 3, 0);
    int32_t pet = StringToBOOL(L, 4, 0);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, inspect, pet);

    if (tab && talentIndex < tab->m_numTalents) {
        CGUnit_C* unit = pet ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : ClntObjMgrGetActivePlayerObj();
        const TalentRec* talent = &tab->m_talents[talentIndex];

        if (talent && (!pet || CGTalentInfo::IsTalentAvailableForPet(unit, talent))) {
            int32_t maxRank = -1;

            for (int32_t i = 4; i >= 0; i--) {
                if (talent->m_spellRank[i] && maxRank < 0) {
                    maxRank = i;
                }
            }

            int32_t rank = -1;
            int32_t previewRank = -1;
            TalentGroupItem_C* item = TalentInfoGetTalentItem(TalentInfoGetScriptGroupIndex(L, inspect, pet, 5), talent->m_id, inspect, pet);

            if (item) {
                rank = item->m_rank;
                previewRank = item->m_previewRank;
            }

            SpellRec* spell;

            if ((spell = g_spellDB.GetRecord(rank < 0 ? talent->m_spellRank[0] : talent->m_spellRank[rank]))) {
                lua_pushstring(L, spell->m_name);
                SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);

                if (icon) {
                    lua_pushstring(L, icon->m_textureFilename);
                } else {
                    lua_pushnil(L);
                }

                lua_pushnumber(L, talent->m_tierID + 1);
                lua_pushnumber(L, talent->m_columnIndex + 1);
                lua_pushnumber(L, rank + 1);
                lua_pushnumber(L, maxRank + 1);

                int32_t available = 0;

                if (inspect) {
                    available = !pet && rank >= 0;
                } else if (unit) {
                    int32_t requiredSpell = talent->m_requiredSpellID;
                    available = 1;

                    if (requiredSpell && !pet && !TalentInfoIsSpellKnown(requiredSpell, 0, 0) && !unit->IsSpellSuperceded(talent->m_requiredSpellID)) {
                        available = 0;
                    }
                }

                if (available) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                lua_pushnumber(L, previewRank + 1);

                int32_t previewAvailable = 0;

                if (inspect) {
                    previewAvailable = !pet && previewRank >= 0;
                } else if (unit) {
                    int32_t requiredSpell = talent->m_requiredSpellID;
                    previewAvailable = 1;

                    if (requiredSpell && !pet && !TalentInfoIsSpellKnown(requiredSpell, 0, 0) && !unit->IsSpellSuperceded(talent->m_requiredSpellID)) {
                        previewAvailable = 0;
                    }
                }

                if (previewAvailable) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                if (talent->m_flags & 1) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                if (talent->m_flags & 4) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                return 11;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 1.0);
    lua_pushnumber(L, 1.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 11;
}

// PROVEN: fbe20d27 IDENTICAL/DIFFERENT
// OFFSET: 0x0099CAB0/0x14071F510
static int32_t Script_GetTalentPrereqs(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetTalentPrereqs(tabIndex, talentIndex[, isInspect[, isPet[, groupIndex]]])");
        return 0;
    }

    int32_t count = 0;
    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t talentIndex = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    int32_t inspect = StringToBOOL(L, 3, 0);
    int32_t pet = StringToBOOL(L, 4, 0);
    uint32_t group = TalentInfoGetScriptGroupIndex(L, inspect, pet, 5);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, inspect, pet);

    if (tab && talentIndex < tab->m_numTalents) {
        const TalentRec* talent = &tab->m_talents[talentIndex];

        if (talent) {
            for (uint32_t i = 0; i < 3; i++) {
                int32_t prereqId = talent->m_prereqTalent[i];

                if (!prereqId) {
                    continue;
                }

                TalentRec* prereq = g_talentDB.GetRecord(prereqId);

                if (!prereq) {
                    continue;
                }

                lua_checkstack(L, 4);
                lua_pushnumber(L, prereq->m_tierID + 1);
                lua_pushnumber(L, prereq->m_columnIndex + 1);
                TalentGroupItem_C* item = TalentInfoGetTalentItem(group, prereqId, inspect, pet);

                if (item) {
                    if (item->m_rank >= talent->m_prereqRank[i]) {
                        lua_pushnumber(L, 1.0);
                    } else {
                        lua_pushnil(L);
                    }

                    if (item->m_previewRank >= talent->m_prereqRank[i]) {
                        lua_pushnumber(L, 1.0);
                    } else {
                        lua_pushnil(L);
                    }
                } else {
                    lua_pushnil(L);
                    lua_pushnil(L);
                }

                count++;
            }
        }
    }

    return 4 * count;
}

// OFFSET: 0x0099C8F0/0x14071F2B0
static int32_t Script_GetTalentLink(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetTalentLink(tabIndex, talentIndex[, isInspect[, isPet[, groupIndex[, isPreview]]]])");
        return 0;
    }

    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t talentIndex = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    int32_t inspect = StringToBOOL(L, 3, 0);
    int32_t pet = StringToBOOL(L, 4, 0);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, inspect, pet);

    if (tab && talentIndex < tab->m_numTalents) {
        CGUnit_C* unit = pet ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : NULL;
        const TalentRec* talent = &tab->m_talents[talentIndex];

        if (talent && (!pet || CGTalentInfo::IsTalentAvailableForPet(unit, talent))) {
            int32_t rank = -1;
            TalentGroupItem_C* item = TalentInfoGetTalentItem(TalentInfoGetScriptGroupIndex(L, inspect, pet, 5), talent->m_id, inspect, pet);

            if (item) {
                rank = StringToBOOL(L, 5, 0) ? item->m_previewRank : item->m_rank;
            }

            SpellRec* spell;

            if ((spell = g_spellDB.GetRecord(talent->m_spellRank[rank < 0 ? 0 : rank]))) {
                lua_pushstring(L, CGTooltip::GetTalentHyperlinkString(talent, spell->m_name, rank));
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0099CCA0/0x14071F7C0
static int32_t Script_LearnTalent(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: LearnTalent(tabIndex, talentIndex[, isPet])");
        return 0;
    } else {
        uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        uint32_t talentIndex = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        int32_t pet = StringToBOOL(L, 3, 0);
        CGUnit_C* unit = pet ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, pet);

        if (!unit || !tab || talentIndex >= tab->m_numTalents) {
            return 0;
        }

        const TalentRec* talent = &tab->m_talents[talentIndex];

        if (!talent || (pet && !CGTalentInfo::IsTalentAvailableForPet(unit, talent))) {
            return 0;
        }

        int32_t rank = -1;
        int32_t maxRank = -1;

        for (int32_t i = 4; i >= 0; i--) {
            if (talent->m_spellRank[i]) {
                maxRank = i;
                break;
            }
        }

        TalentGroupItem_C* item = CGTalentInfo::GetTalentItem(CGTalentInfo::GetActiveTalentGroup(pet), talent->m_id, pet);

        if (item) {
            rank = item->m_rank;
        }

        if (rank < maxRank) {
            CDataStore msg;

            if (pet) {
                msg.Put(static_cast<uint32_t>(CMSG_PET_LEARN_TALENT));
                msg.Put(unit->m_obj->m_guid.m_guid);
            } else {
                msg.Put(static_cast<uint32_t>(CMSG_LEARN_TALENT));
            }

            msg.Put(static_cast<uint32_t>(talent->m_id));
            msg.Put(static_cast<uint32_t>(rank + 1));
            msg.Finalize();
            ClientServices::Send2(&msg);
            g_talentPointsSpentCVar->Set("1", true, false, false, true);
        }
    }

    return 0;
}

// OFFSET: 0x0099EB30/0x1407225E0
static int32_t Script_AddPreviewTalentPoints(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: AddPreviewTalentPoints(tabIndex, talentIndex, points[, isPet[, groupIndex]])");
        return 0;
    }

    if (!lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: AddPreviewTalentPoints(tabIndex, talentIndex, points[, isPet[, groupIndex]])");
        return 0;
    }

    uint32_t tab = lua_tointeger(L, 1) - 1;
    uint32_t talent = lua_tointeger(L, 2) - 1;
    int32_t points = lua_tointeger(L, 3);
    int32_t pet = StringToBOOL(L, 4, 0);
    uint32_t group = TalentInfoGetScriptGroupIndex(L, 0, pet, 5);
    int32_t before = CGTalentInfo::GetPreviewPointsSpent(group, pet);
    CGTalentInfo::AddPreviewTalentPoints(group, tab, talent, points, pet);
    int32_t after = CGTalentInfo::GetPreviewPointsSpent(group, pet);

    if (after != before) {
        FrameScript_SignalEvent(pet ? SCRIPT_EVENT_PREVIEW_PET_TALENT_POINTS_CHANGED : SCRIPT_EVENT_PREVIEW_TALENT_POINTS_CHANGED, "%d%d%d%d", talent + 1, tab + 1, group + 1, after - before);
    }

    return 0;
}

// PROVEN: c38e45bd IDENTICAL/IDENTICAL
// OFFSET: 0x00999D10/0x14071BAB0
static int32_t Script_GetActiveTalentGroup(lua_State* L) {
    int32_t inspect = StringToBOOL(L, 1, 0);
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t group;

    if (inspect) {
        group = CGTalentInfo_Inspect::m_activeTalentGroup;
    } else {
        group = pet ? 0 : CGTalentInfo::m_activeTalentGroup;
    }

    lua_pushnumber(L, group + 1);
    return 1;
}

// OFFSET: 0x0099B000/0x14071D2A0
static int32_t Script_GetPreviewTalentPointsSpent(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPreviewTalentPointsSpent(tabIndex[, isPet[, groupIndex]])");
        return 0;
    }

    uint32_t tab = lua_tointeger(L, 1) - 1;
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t group = lua_isnumber(L, 3) ? lua_tointeger(L, 3) - 1 : CGTalentInfo::GetActiveTalentGroup(pet);
    lua_pushinteger(L, CGTalentInfo::GetTabPreviewPointsSpent(group, tab, pet));
    return 1;
}

// PROVEN: bb9955cf IDENTICAL/DIFFERENT
// OFFSET: 0x0099A380/0x14071C1F0
static int32_t Script_GetTalentTreeRoles(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTalentTreeRoles(treeIndex[, isInspect[, isPet]])");
        return 0;
    }

    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t pet = StringToBOOL(L, 3, 0);
    int32_t inspect = StringToBOOL(L, 2, 0);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, inspect, pet);

    if (tab) {
        TalentTabRec* tabRec = g_talentTabDB.GetRecord(tab->m_tabId);

        if (!tabRec) {
            return 0;
        }

        int32_t count = 0;

        if (tabRec->m_roleMask & 2) {
            lua_pushstring(L, s_roleTokens[0]);
            count = 1;
        }

        if (tabRec->m_roleMask & 8) {
            lua_pushstring(L, s_roleTokens[2]);
            count++;
        }

        if (tabRec->m_roleMask & 4) {
            lua_pushstring(L, s_roleTokens[1]);
            count++;
        }

        return count;
    }

    return 0;
}

// OFFSET: 0x0099A5B0/0x14071C4D0
static int32_t Script_GetNextPetTalentLevel(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet || !pet->CanGainXP()) {
        lua_pushnil(L);
        return 1;
    }

    int32_t level = pet->m_unit->m_unitFieldLevel;

    if (lua_isnumber(L, 1)) {
        level = lua_tointeger(L, 1);
    }

    if (level >= 20) {
        lua_pushinteger(L, 4 * ((level - 20) / 4) + 24);
    } else {
        lua_pushinteger(L, 20);
    }

    return 1;
}

// OFFSET: 0x0099A670/0x14071C5B0
static int32_t Script_SetPrimaryTalentTree(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetPrimaryTalentTree(talentTabID)");
        return 0;
    }

    uint32_t tree = lua_tointeger(L, 1) - 1;

    if (tree <= 2) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_PRIMARY_TALENT_TREE));
        msg.Put(tree);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// PROVEN: e9d6469f IDENTICAL/DIFFERENT
// OFFSET: 0x0099A730/0x14071C6A0
static int32_t Script_GetTalentTreeMasterySpells(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTalentTreeEarlySpells(tabIndex[, isInspect[, isPet]])");
        return 0;
    }

    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t inspect = StringToBOOL(L, 2, 0);
    int32_t pet = StringToBOOL(L, 3, 0);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, inspect, pet);

    if (tab) {
        TalentTabRec* tabRec = g_talentTabDB.GetRecord(tab->m_tabId);

        if (!tabRec) {
            return 0;
        }

        int32_t count = 0;

        if (tabRec->m_masterySpellID[0]) {
            lua_pushinteger(L, tabRec->m_masterySpellID[0]);
            count = 1;
        }

        if (tabRec->m_masterySpellID[1]) {
            lua_pushinteger(L, tabRec->m_masterySpellID[1]);
            count++;
        }

        return count;
    }

    return 0;
}

// OFFSET: 0x0099AE20/0x14071D070
static int32_t Script_GetNumTalentPoints(lua_State* L) {
    uint32_t group = CGTalentInfo::m_activeTalentGroup;
    CGTalentGroup_C* talentGroup = CGTalentInfo::GetTalentGroup(group, 0);
    uint32_t spent = talentGroup ? talentGroup->GetPointsSpent() : 0;
    lua_pushinteger(L, spent + CGTalentInfo::GetUnspentTalentPoints(group, 0));
    return 1;
}

// OFFSET: 0x0099B0F0/0x14071D420
static int32_t Script_GetNextTalentLevel(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t level = player->m_unit->m_unitFieldLevel;

    if (lua_isnumber(L, 1)) {
        level = lua_tointeger(L, 1);
    }

    NumTalentsAtLevelRec* current = g_numTalentsAtLevelDB.GetRecord(level);

    if (!current) {
        return 0;
    }

    int32_t maxLevel = player->m_player->m_playerFieldMaxLevel;

    for (int32_t next = level + 1; next < maxLevel; next++) {
        NumTalentsAtLevelRec* rec = g_numTalentsAtLevelDB.GetRecord(next);

        if (rec && rec->m_numTalents > current->m_numTalents) {
            lua_pushinteger(L, next);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0099B1E0/0x14071D530
static int32_t Script_SetPreviewPrimaryTalentTree(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetPreviewPrimaryTalentTree(talentTree, groupIndex)");
        return 0;
    }

    uint32_t tree = lua_tointeger(L, 1) - 1;

    if (tree > 2) {
        return 0;
    }

    uint32_t group = lua_isnumber(L, 2) ? lua_tointeger(L, 2) - 1 : CGTalentInfo::m_activeTalentGroup;
    CGTalentGroup_C* talentGroup = CGTalentInfo::GetTalentGroup(group, 0);

    if (!talentGroup) {
        return 0;
    }

    TalentTabInfo* tab = CGTalentInfo::GetTab(tree, 0);
    int32_t previous = talentGroup->m_previewPrimaryTree;
    CGTalentInfo::SetPreviewPrimaryTalentTree(group, false, tab->m_tabId);

    if (previous != tab->m_tabId) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PREVIEW_TALENT_PRIMARY_TREE_CHANGED, "%d", tree + 1);
    }

    return 0;
}

// OFFSET: 0x0099B2C0/0x14071D640
static int32_t Script_GetPrimaryTalentTree(lua_State* L) {
    int32_t inspect = StringToBOOL(L, 1, 0);
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t group = lua_isnumber(L, 3) ? lua_tointeger(L, 3) - 1 : TalentInfoGetActiveTalentGroup(inspect, pet);
    CGTalentGroup_C* talentGroup = TalentInfoGetTalentGroup(group, inspect, pet);
    TalentTabRec* tabRec = g_talentTabDB.GetRecord(talentGroup ? talentGroup->m_primaryTree : 0);

    if (tabRec) {
        lua_pushinteger(L, tabRec->m_orderIndex + 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0099B3A0/0x14071D730
static int32_t Script_GetPreviewPrimaryTalentTree(lua_State* L) {
    int32_t inspect = StringToBOOL(L, 1, 0);
    int32_t pet = StringToBOOL(L, 2, 0);
    uint32_t group = lua_isnumber(L, 3) ? lua_tointeger(L, 3) - 1 : TalentInfoGetActiveTalentGroup(inspect, pet);
    CGTalentGroup_C* talentGroup = TalentInfoGetTalentGroup(group, inspect, pet);
    TalentTabRec* tabRec = g_talentTabDB.GetRecord(talentGroup ? talentGroup->m_previewPrimaryTree : 0);

    if (tabRec) {
        lua_pushinteger(L, tabRec->m_orderIndex + 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0099B480/0x14071D820
static int32_t PushTalentTreeBonuses(lua_State* L, bool major, bool early) {
    uint32_t tabIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t inspect = StringToBOOL(L, 2, 0);
    int32_t pet = StringToBOOL(L, 3, 0);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, inspect, pet);
    int32_t count = 0;

    if (!tab) {
        return count;
    }

    TalentTabRec* tabRec = g_talentTabDB.GetRecord(tab->m_tabId);

    if (!tabRec) {
        return count;
    }

    for (int32_t i = 0; i < g_talentTreePrimarySpellsDB.m_numRecords; i++) {
        TalentTreePrimarySpellsRec* rec = g_talentTreePrimarySpellsDB.GetRecordByIndex(i);

        if (!rec || rec->m_talentTabID != tabRec->m_id) {
            continue;
        }

        if (early) {
            if (!(rec->m_flags & 1)) {
                continue;
            }

            if (inspect || pet) {
                lua_pushinteger(L, rec->m_spellID);
                count++;
            } else {
                CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

                if (player) {
                    int32_t level = 0;
                    player->CanLearnSpell(rec->m_spellID, level, false);

                    if (level > static_cast<int32_t>(player->m_unit->m_unitFieldLevel)) {
                        lua_pushinteger(L, rec->m_spellID);
                        count++;
                    }
                }
            }
        } else {
            int32_t flags = rec->m_flags;

            if (!(flags & 1)) {
                int32_t spellID = rec->m_spellID;
                SpellRec* spell = g_spellDB.GetRecord(spellID);

                if (spell) {
                    bool isMajor = (flags & 2) || !(spell->m_attributes & 0x40);

                    if (isMajor == major) {
                        lua_pushinteger(L, spellID);
                        count++;
                    }
                }
            }
        }
    }

    return count;
}

// PROVEN: 1dbf99ca DIFFERENT/IDENTICAL
// OFFSET: 0x0099B630/0x14071DB20
static int32_t Script_GetMajorTalentTreeBonuses(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMajorTalentTreeBonuses(tabIndex[, isInspect[, isPet]])");
        return 0;
    }

    return PushTalentTreeBonuses(L, true, false);
}

// PROVEN: c85e46d8 DIFFERENT/IDENTICAL
// OFFSET: 0x0099B670/0x14071DB60
static int32_t Script_GetMinorTalentTreeBonuses(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMinorTalentTreeBonuses(tabIndex[, isInspect[, isPet]])");
        return 0;
    }

    return PushTalentTreeBonuses(L, false, false);
}

// PROVEN: eb3917fd DIFFERENT/IDENTICAL
// OFFSET: 0x0099B6B0/0x14071DBA0
static int32_t Script_GetTalentTreeEarlySpells(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTalentTreeEarlySpells(tabIndex[, isInspect[, isPet]])");
        return 0;
    }

    return PushTalentTreeBonuses(L, false, true);
}

// STATIC: 0x00D14CD8/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumTalentTabs", &Script_GetNumTalentTabs },
    { "GetTalentTabInfo", &Script_GetTalentTabInfo },
    { "GetTalentTreeRoles", &Script_GetTalentTreeRoles },
    { "GetNumTalents", &Script_GetNumTalents },
    { "GetTalentInfo", &Script_GetTalentInfo },
    { "GetTalentLink", &Script_GetTalentLink },
    { "GetTalentPrereqs", &Script_GetTalentPrereqs },
    { "GetNumTalentPoints", &Script_GetNumTalentPoints },
    { "LearnTalent", &Script_LearnTalent },
    { "GetUnspentTalentPoints", &Script_GetUnspentTalentPoints },
    { "GetNumTalentGroups", &Script_GetNumTalentGroups },
    { "GetActiveTalentGroup", &Script_GetActiveTalentGroup },
    { "SetActiveTalentGroup", &Script_SetActiveTalentGroup },
    { "GetPreviewTalentPointsSpent", &Script_GetPreviewTalentPointsSpent },
    { "GetGroupPreviewTalentPointsSpent", &Script_GetGroupPreviewTalentPointsSpent },
    { "AddPreviewTalentPoints", &Script_AddPreviewTalentPoints },
    { "ResetPreviewTalentPoints", &Script_ResetPreviewTalentPoints },
    { "ResetGroupPreviewTalentPoints", &Script_ResetGroupPreviewTalentPoints },
    { "LearnPreviewTalents", &Script_LearnPreviewTalents },
    { "GetNextTalentLevel", &Script_GetNextTalentLevel },
    { "GetNextPetTalentLevel", &Script_GetNextPetTalentLevel },
    { "SetPrimaryTalentTree", &Script_SetPrimaryTalentTree },
    { "SetPreviewPrimaryTalentTree", &Script_SetPreviewPrimaryTalentTree },
    { "GetPrimaryTalentTree", &Script_GetPrimaryTalentTree },
    { "GetPreviewPrimaryTalentTree", &Script_GetPreviewPrimaryTalentTree },
    { "GetMajorTalentTreeBonuses", &Script_GetMajorTalentTreeBonuses },
    { "GetMinorTalentTreeBonuses", &Script_GetMinorTalentTreeBonuses },
    { "GetTalentTreeEarlySpells", &Script_GetTalentTreeEarlySpells },
    { "GetTalentTreeMasterySpells", &Script_GetTalentTreeMasterySpells },
};

// PROVEN: d23793e6 IDENTICAL/UNPROVEN
// OFFSET: 0x00999D80/0x14071BB20
void TalentInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: e368c2c4 IDENTICAL/UNPROVEN
// OFFSET: 0x00999DB0/0x14071BB70
void TalentInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
