#include "TradeSkillFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaGroupRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellCastingRequirementsRec.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellCategoryRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellFocusObjectRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellTotemsRec.h"
#include "WoW/Source/DB/Rec/TotemCategoryRec.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/UIUtil/Tooltip.h"

// STATIC: 0x00C01DB0
static const char s_difficultyNames[4][32] = { "optimal", "medium", "easy", "trivial" };

// PROVEN: e4622432 IDENTICAL/NONE
// OFFSET: 0x0098D3C0/0x14070B860
static int32_t Script_GetNumTradeSkills(lua_State* L) {
    lua_pushnumber(L, CGTradeSkillInfo::m_filteredSkills);
    return 1;
}

// PROVEN: 0c7b394f IDENTICAL/NONE
// OFFSET: 0x0098D3F0/0x14070B880
static int32_t Script_GetTradeSkillItemNameFilter(lua_State* L) {
    lua_pushstring(L, CGTradeSkillInfo::m_itemNameFilter);
    return 1;
}

// PROVEN: b4c2c069 IDENTICAL/NONE
// OFFSET: 0x0098D410/0x14070B8A0
static int32_t Script_GetTradeSkillItemLevelFilter(lua_State* L) {
    int32_t maxLevel = CGTradeSkillInfo::m_itemMaxLevelFilter;
    lua_pushnumber(L, CGTradeSkillInfo::m_itemMinLevelFilter);
    lua_pushnumber(L, maxLevel);
    return 2;
}

// PROVEN: e28d938a IDENTICAL/NONE
// OFFSET: 0x0098D450/0x14070B8F0
static int32_t Script_GetTradeSkillSubClassFilter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillSubClassFilter(index)");
        return 0;
    }

    uint32_t filter = CGTradeSkillInfo::m_subClassFilter;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        for (uint32_t i = 0; i < CGTradeSkillInfo::m_numSubClasses; i++) {
            if (!((1 << i) & filter)) {
                lua_pushnil(L);
                return 1;
            }
        }

        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (index >= static_cast<int32_t>(CGTradeSkillInfo::m_numSubClasses)) {
        luaL_error(L, "Bad sub class in GetTradeSkillSubClassFilter");
        return 0;
    }

    if ((1 << index) & filter) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: a9940870 IDENTICAL/NONE
// OFFSET: 0x0098D510/0x14070B9C0
static int32_t Script_GetTradeskillRepeatCount(lua_State* L) {
    uint32_t count;

    if (CGTradeSkillInfo::m_recastSpellID && CGTradeSkillInfo::m_recastSpellID == CGTradeSkillInfo::m_selectedSkill) {
        count = CGTradeSkillInfo::m_recastTimes;
    } else if (CGTradeSkillInfo::m_pendingRecastSpellID && CGTradeSkillInfo::m_pendingRecastSpellID == CGTradeSkillInfo::m_selectedSkill) {
        count = CGTradeSkillInfo::m_pendingRecastTimes;
    } else {
        count = 1;
    }

    if (count <= 1) {
        count = 1;
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: c8f95ea9 IDENTICAL/NONE
// OFFSET: 0x0098D580/0x14070BA30
static int32_t Script_StopTradeSkillRepeat(lua_State* L) {
    if (CGTradeSkillInfo::m_recastSpellID || CGTradeSkillInfo::m_pendingRecastSpellID) {
        CGTradeSkillInfo::m_recastSpellID = 0;
        CGTradeSkillInfo::m_pendingRecastSpellID = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TRADESKILL_RECAST, NULL);
    }

    return 0;
}

// OFFSET: 0x0098D5B0/0x14070BA70
static int32_t Script_IsTradeSkillLinked(lua_State* L) {
    if (CGTradeSkillInfo::m_linkData) {
        lua_pushnumber(L, 1.0);
        WOWGUID requester = WOWGUID::Make(0);
        NameCache* name = g_nameDBCache.GetRecord(CGTradeSkillInfo::m_playerGuid.m_guid, requester, NULL, NULL, false);
        lua_pushstring(L, name ? name->m_name : NULL);
        return 2;
    }

    if (CGTradeSkillInfo::m_guildMemberSkill) {
        lua_pushnumber(L, 1.0);
        NameCache* name = g_nameDBCache.GetRecord(CGTradeSkillInfo::m_playerGuid.m_guid, CGTradeSkillInfo::m_playerGuid, TradeSkillNameCallback, NULL, true);

        if (name) {
            lua_pushstring(L, name->m_name);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// PROVEN: 143fad9d IDENTICAL/NONE
// OFFSET: 0x0098D690/0x14070BB50
static int32_t Script_IsTradeSkillGuild(lua_State* L) {
    if (CGTradeSkillInfo::m_guildSkill) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 5cfb81f5 IDENTICAL/NONE
// OFFSET: 0x0098DE50
static int32_t Script_CloseTradeSkill(lua_State* L) {
    CGTradeSkillInfo::Close();
    return 0;
}

// OFFSET: 0x0098DE60/0x14070C4F0
static int32_t Script_GetTradeSkillInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillInfo* info;

    if (index < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[index])) {
        if (info->m_spellId == -1) {
            if (info->m_itemClass == -1 && info->m_itemSubClass == -1) {
                SkillLineRec* skillLine = g_skillLineDB.GetRecord(CGTradeSkillInfo::m_skillLineID);

                if (skillLine) {
                    lua_pushstring(L, skillLine->m_alternateVerb);
                } else {
                    lua_pushnil(L);
                }

                lua_pushstring(L, "header");
                lua_pushnumber(L, 0.0);

                if (CGTradeSkillInfo::IsCollapsedHeader(index)) {
                    lua_pushnil(L);
                } else {
                    lua_pushnumber(L, 1.0);
                }

                lua_pushnil(L);
                lua_pushnumber(L, 0.0);
                return 6;
            }

            for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
                ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

                if (rec->m_classID != info->m_itemClass || rec->m_subClassID != info->m_itemSubClass) {
                    continue;
                }

                const char* name = rec->m_verboseName;

                if (!name || !*name) {
                    name = rec->m_displayName;
                }

                lua_pushstring(L, name);
                lua_pushstring(L, "header");
                lua_pushnumber(L, 0.0);

                if (CGTradeSkillInfo::IsCollapsedHeader(index)) {
                    lua_pushnil(L);
                } else {
                    lua_pushnumber(L, 1.0);
                }

                lua_pushnil(L);
                lua_pushnumber(L, 0.0);
                return 6;
            }
        } else {
            SpellRec* spell = g_spellDB.GetRecord(info->m_spellId);

            if (spell) {
                lua_pushstring(L, spell->m_name);
                lua_pushstring(L, s_difficultyNames[info->m_difficulty]);
                lua_pushnumber(L, info->m_numAvailable);
                lua_pushnil(L);

                const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);
                SkillLineRec* skillLine;

                if ((effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59)) || !(skillLine = g_skillLineDB.GetRecord(CGTradeSkillInfo::m_skillLineID))) {
                    lua_pushnil(L);
                } else {
                    lua_pushstring(L, skillLine->m_alternateVerb);
                }

                lua_pushnumber(L, info->m_numSkillUps);
                return 6;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    return 6;
}

// PROVEN: 3534c9ca IDENTICAL/NONE
// OFFSET: 0x0098E0E0/0x14070C860
static int32_t Script_SelectTradeSkill(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectTradeSkill(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (static_cast<uint32_t>(index) >= CGTradeSkillInfo::m_numDisplayEntries || (CGTradeSkillInfo::m_selectedSkill = CGTradeSkillInfo::m_displayList.m_data[index]->m_spellId) <= 0) {
        CGTradeSkillInfo::m_selectedSkill = 0;
    }

    return 0;
}

// PROVEN: b987c204 IDENTICAL/NONE
// OFFSET: 0x0098E150/0x14070C8E0
static int32_t Script_GetTradeSkillSelectionIndex(lua_State* L) {
    uint32_t i;

    if (CGTradeSkillInfo::m_selectedSkill) {
        for (i = 0; i < CGTradeSkillInfo::m_numDisplayEntries; i++) {
            if (CGTradeSkillInfo::m_displayList.m_data[i]->m_spellId == CGTradeSkillInfo::m_selectedSkill) {
                break;
            }
        }

        if (i == CGTradeSkillInfo::m_numDisplayEntries) {
            i = -1;
        }
    } else {
        i = -1;
    }

    lua_pushnumber(L, static_cast<int32_t>(i + 1));
    return 1;
}

// OFFSET: 0x0098E1B0/0x14070C950
static int32_t Script_GetTradeSkillCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillCooldown(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillInfo* info;

    if (index >= CGTradeSkillInfo::m_numDisplayEntries || !(info = CGTradeSkillInfo::m_displayList.m_data[index]) || info->m_spellId <= 0) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    uint32_t duration;
    uint32_t start;
    uint32_t enable;

    if (Spell_C_GetSpellCooldown(info->m_spellId, 0, &duration, &start, &enable, NULL)) {
        uint32_t remaining = start + duration - FrameTime::GetCurTimeMs();
        lua_pushnumber(L, remaining * 0.001f);
    } else {
        lua_pushnil(L);
    }

    bool daily = false;
    SpellRec* spell = g_spellDB.GetRecord(info->m_spellId);
    SpellCategoriesRec* categories;
    SpellCategoryRec* category;

    if (spell && spell->m_spellCategoriesID && (categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID)) && categories->m_category && (category = g_spellCategoryDB.GetRecord(categories->m_category)) && (category->m_flags & 0x8)) {
        daily = true;
    }

    lua_pushboolean(L, daily);
    return 2;
}

// OFFSET: 0x0098E310/0x14070CB10
static int32_t Script_GetTradeSkillNumMade(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillNumMade(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillInfo* info;
    SpellRec* spell;

    if (index < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[index]) && (spell = g_spellDB.GetRecord(info->m_spellId))) {
        int32_t minMade;
        int32_t maxMade;
        Spell_C_GetMinMaxPoints(spell, 0, &minMade, &maxMade, 0, 0, 0, 0);

        if (minMade < 1) {
            minMade = 1;
        }

        if (maxMade < 1) {
            maxMade = 1;
        }

        lua_pushnumber(L, minMade);
        lua_pushnumber(L, maxMade);
        return 2;
    }

    lua_pushnumber(L, 1.0);
    lua_pushnumber(L, 1.0);
    return 2;
}

// OFFSET: 0x0098E400/0x14070CC30
static int32_t Script_GetTradeSkillRecipeLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillRecipeLink(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillInfo* info = static_cast<uint32_t>(index) < CGTradeSkillInfo::m_numDisplayEntries ? CGTradeSkillInfo::m_displayList.m_data[index] : NULL;

    if (!info) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(info->m_spellId);

    if (!spell || !(spell->m_attributes & 0x20)) {
        return 0;
    }

    uint64_t guid = ClntObjMgrGetActivePlayer().m_guid;
    SkillLineRec* skillLine = CGTradeSkillInfo::GetSkillLineRec(info->m_spellId, guid);
    char link[1024];

    if (skillLine) {
        SStrPrintf(link, sizeof(link), "|c%s|Henchant:%d|h[%s: %s]|h|r", s_tradeLinkColor, spell->m_id, static_cast<const char*>(skillLine->m_displayName), static_cast<const char*>(spell->m_name));
    } else {
        SStrPrintf(link, sizeof(link), "|c%s|Henchant:%d|h[%s]|h|r", s_tradeLinkColor, spell->m_id, static_cast<const char*>(spell->m_name));
    }

    lua_pushstring(L, link);
    return 1;
}

// PROVEN: 1795e244 IDENTICAL/NONE
// OFFSET: 0x0098E530/0x14070CDF0
static int32_t Script_GetTradeSkillNumReagents(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillNumReagents(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t count = 0;
    TradeSkillInfo* info;
    SpellRec* spell;
    SpellReagentsRec* reagents;

    if (index < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[index]) && (spell = g_spellDB.GetRecord(info->m_spellId)) && spell->m_spellReagentsID && (reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        for (uint32_t i = 0; i < 8; i++) {
            if (reagents->m_reagent[i]) {
                count++;
            }
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: 3995aa09 IDENTICAL/NONE
// OFFSET: 0x0098E640/0x14070CEE0
static int32_t Script_GetTradeSkillDescription(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillDescription(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillInfo* info;
    SpellRec* spell;

    if (index < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[index]) && (spell = g_spellDB.GetRecord(info->m_spellId)) && spell->m_description && *spell->m_description) {
        char description[1024];
        SpellParserParseText(spell, description, sizeof(description), 0, 0, 0, 0, 1, 0, NULL);
        lua_pushstring(L, description);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0098E720
static int32_t Script_GetTradeSkillSubClasses(lua_State* L) {
    uint32_t count = CGTradeSkillInfo::m_numSubClasses;

    for (uint32_t i = 0; i < count; i++) {
        TradeSkillSubClassInfo* subClass = i < CGTradeSkillInfo::m_numSubClasses ? CGTradeSkillInfo::m_subClassList.m_data[i] : NULL;

        if (subClass->m_itemClass == -1 && subClass->m_itemSubClass == -1) {
            SkillLineRec* skillLine = g_skillLineDB.GetRecord(CGTradeSkillInfo::m_skillLineID);

            if (skillLine) {
                lua_pushstring(L, skillLine->m_alternateVerb);
            } else {
                lua_pushnil(L);
            }

            continue;
        }

        for (uint32_t j = 0; j < static_cast<uint32_t>(g_itemSubClassDB.m_numRecords); j++) {
            ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(j);

            if (rec->m_classID == subClass->m_itemClass && rec->m_subClassID == subClass->m_itemSubClass) {
                if (rec->m_verboseName && *rec->m_verboseName) {
                    lua_pushstring(L, rec->m_verboseName);
                } else {
                    lua_pushstring(L, rec->m_displayName);
                }

                break;
            }
        }
    }

    return count;
}

// OFFSET: 0x0098E820/0x14070D190
static int32_t Script_GetFirstTradeSkill(lua_State* L) {
    for (uint32_t i = 0; i < static_cast<uint32_t>(CGTradeSkillInfo::m_filteredSkills); i++) {
        TradeSkillInfo* info;

        if (i < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[i]) && info->m_spellId > 0) {
            lua_pushnumber(L, i + 1);
            return 1;
        }
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x0098E8B0/0x14070D210
static int32_t Script_DoTradeSkill(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: DoTradeSkill(index [, repeat])");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t times = 1;

    if (lua_isnumber(L, 2)) {
        times = static_cast<uint32_t>(lua_tonumber(L, 2));
    }

    if (index < CGTradeSkillInfo::m_numDisplayEntries) {
        TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[index];

        if (info) {
            uint32_t available = info->m_numAvailable;

            if (available != 0xFFFFFFFF && times >= available) {
                times = info->m_numAvailable;
            }

            if (CGTradeSkillInfo::m_recastSpellID == info->m_spellId || Spell_C_CastSpell(info->m_spellId, NULL, WOWGUID(), false)) {
                CGTradeSkillInfo::DoTradeSkill(info->m_spellId, times);
            }
        }
    }

    return 0;
}

// PROVEN: c4d9b014 IDENTICAL/NONE
// OFFSET: 0x0098E990/0x14070D310
static int32_t Script_GetTradeSkillSubClassFilteredSlots(lua_State* L) {
    int32_t count = 0;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillSubClassInfo* subClass = static_cast<uint32_t>(index) < CGTradeSkillInfo::m_numSubClasses ? CGTradeSkillInfo::m_subClassList.m_data[index] : NULL;
    uint32_t mask = CGTradeSkillInfo::m_invSlots;

    if (subClass) {
        mask = subClass->m_invTypeMask;
    }

    for (uint32_t i = 0; i < 27; i++) {
        if ((1 << i) & mask) {
            lua_pushstring(L, FrameScript_GetText(s_invSlotTokens[i], -1, GENDER_NOT_APPLICABLE));
            count++;
        }
    }

    return count;
}

// PROVEN: 55e36a30 IDENTICAL/NONE
// OFFSET: 0x0098EA20/0x14070D3C0
static int32_t Script_GetTradeSkillTexture(lua_State* L) {
    SkillLineRec* skillLine = g_skillLineDB.GetRecord(CGTradeSkillInfo::m_skillLineID);
    SpellIconRec* icon;

    if (skillLine && (icon = g_spellIconDB.GetRecord(skillLine->m_spellIconID))) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: ccc9b98d IDENTICAL/NONE
// OFFSET: 0x0098EE10/0x14070D820
static int32_t Script_GetTradeSkillLine(lua_State* L) {
    SkillLineRec* skillLine = g_skillLineDB.GetRecord(CGTradeSkillInfo::m_skillLineID);

    if (skillLine) {
        lua_pushstring(L, skillLine->m_displayName);
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        int32_t rank;

        if (player && CGTradeSkillInfo::m_playerGuid == player->m_obj->m_guid) {
            rank = player->GetSkillValue(player->GetSkillIndex(CGTradeSkillInfo::m_skillLineID));
        } else {
            rank = CGTradeSkillInfo::m_skillRank;
        }

        lua_pushnumber(L, rank);
        int32_t maxRank = CGTradeSkillInfo::GetSkillMaxRank();
        lua_pushnumber(L, maxRank);
        int32_t modifier = CGTradeSkillInfo::GetSkillModifier();
        lua_pushnumber(L, modifier);
        return 4;
    }

    lua_pushstring(L, "UNKNOWN");
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 4;
}

// PROVEN: fc435f9d IDENTICAL/NONE
// OFFSET: 0x0098EF40/0x14070D970
static int32_t Script_GetTradeSkillInvSlots(lua_State* L) {
    int32_t count = 0;
    TradeSkillSubClassInfo* subClass;
    uint32_t mask;

    if (CGTradeSkillInfo::m_subClassFilterIndex < CGTradeSkillInfo::m_numSubClasses && (subClass = CGTradeSkillInfo::m_subClassList.m_data[CGTradeSkillInfo::m_subClassFilterIndex])) {
        mask = subClass->m_invTypeMask;
    } else {
        mask = 0xFFFFFFFF;
    }

    uint32_t slots = CGTradeSkillInfo::m_invSlots & mask;

    for (uint32_t i = 0; i < 27; i++) {
        if ((1 << i) & slots) {
            lua_pushstring(L, FrameScript_GetText(s_invSlotTokens[i], -1, GENDER_NOT_APPLICABLE));
            count++;
        }
    }

    return count;
}

// OFFSET: 0x0098EFC0/0x14070DA20
static int32_t Script_GetTradeSkillInvSlotFilter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillInvSlotFilter(index)");
        return 0;
    }

    uint32_t filter = CGTradeSkillInfo::m_invTypeFilter;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        if (CGTradeSkillInfo::m_invTypeFilter == 0xFFFFFFFF) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 1;
    }

    if (CGTradeSkillInfo::m_invTypeFilter == 0xFFFFFFFF) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t slots = CGTradeSkillInfo::GetInvSlotFilter();
    int32_t count = 0;
    int32_t slot = -1;

    for (uint32_t i = 0; i < 27; i++) {
        if ((1 << i) & slots) {
            if (count == index) {
                slot = i;
                break;
            }

            count++;
        }
    }

    if (slot == -1) {
        luaL_error(L, "Bad inv type in GetTradeSkillInvSlotFilter");
        return 0;
    }

    if ((1 << slot) & filter) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 1acd808e IDENTICAL/NONE
// OFFSET: 0x0098F0E0/0x14070DB60
static int32_t Script_GetTradeSkillListLink(lua_State* L) {
    char link[256];

    if (CGTradeSkillInfo::GetTradeSkillListLink(CGTradeSkillInfo::m_spellID, link, sizeof(link))) {
        lua_pushstring(L, link);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0098F300/0x14070DE30
static int32_t Script_GetTradeSkillIcon(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillIcon(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillInfo* info;
    SpellRec* spell;

    if (index < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[index]) && (spell = g_spellDB.GetRecord(info->m_spellId))) {
        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

        if (effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59) && effect->m_effectItemType) {
            WOWGUID requester = WOWGUID::Make(0);
            ItemRec* item = g_itemDB.GetRecord(effect->m_effectItemType, requester, NULL, NULL, false);

            if (item) {
                const char* path = ClientDBStringLookup(3);
                const char* separator = *path ? "\\" : "";
                char texture[260];
                SStrPrintf(texture, sizeof(texture), "%s%s%s", path, separator, CGItem_C::GetInventoryArt(item->m_displayInfoID));
                lua_pushstring(L, texture);
                return 1;
            }
        } else {
            SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0098F490/0x14070E020
static int32_t Script_GetTradeSkillItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillItemLink(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGTradeSkillInfo::m_numDisplayEntries) {
        return 0;
    }

    TradeSkillInfo* info = CGTradeSkillInfo::m_displayList.m_data[index];

    if (!info) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(info->m_spellId);

    if (!spell) {
        return 0;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

    if (effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59)) {
        int32_t itemId = effect->m_effectItemType;
        WOWGUID requester = WOWGUID::Make(0);
        ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false);

        if (item) {
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, item->m_quality, 0, 0, 0, 0));
            return 1;
        }
    }

    char link[1024];
    SStrPrintf(link, sizeof(link), "|c%s|Henchant:%d|h[%s]|h|r", s_tradeLinkColor, spell->m_id, static_cast<const char*>(spell->m_name));
    lua_pushstring(L, link);
    return 1;
}

// OFFSET: 0x0098F5B0/0x14070E1E0
static int32_t Script_GetTradeSkillReagentItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetTradeReagentSkillItemLink(index, reagentIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t reagentIndex = static_cast<int32_t>(lua_tonumber(L, 2));
    TradeSkillInfo* info;
    SpellRec* spell;
    SpellReagentsRec* reagents;

    if (index < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[index]) && (spell = g_spellDB.GetRecord(info->m_spellId)) && spell->m_spellReagentsID && (reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        int32_t count = 0;

        for (uint32_t i = 0; i < 8; i++) {
            int32_t itemId = reagents->m_reagent[i];

            if (!itemId || ++count != reagentIndex) {
                continue;
            }

            WOWGUID requester = WOWGUID::Make(0);
            ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false);

            if (item) {
                lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, item->m_quality, 0, 0, 0, 0));
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// PROVEN: 3988d40e IDENTICAL/NONE
// OFFSET: 0x00990720/0x14070F850
static int32_t Script_SetTradeSkillItemNameFilter(lua_State* L) {
    CGTradeSkillInfo::SetItemNameFilter(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x00990740/0x14070F870
static int32_t Script_SetTradeSkillItemLevelFilter(lua_State* L) {
    if (lua_isnumber(L, 1) && lua_isnumber(L, 2)) {
        CGTradeSkillInfo::SetItemLevelFilter(lua_tointeger(L, 1), lua_tointeger(L, 2));
        return 0;
    }

    luaL_error(L, "Usage: SetTradeSkillItemLevelFilter(minLevel, maxLevel)");
    return 0;
}

// PROVEN: 556ea418 IDENTICAL/NONE
// OFFSET: 0x009907A0/0x14070F910
static int32_t Script_SetTradeSkillSubClassFilter(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetTradeSkillSubClassFilter(index [, on\\off, exclusive])");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        CGTradeSkillInfo::m_subClassFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::m_subClassFilterIndex = 0xFFFFFFFF;
        CGTradeSkillInfo::FilterAndSortSkills(1);
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
        return 0;
    }

    if (index >= static_cast<int32_t>(CGTradeSkillInfo::m_numSubClasses)) {
        luaL_error(L, "Bad sub class in SetTradeSkillSubClassFilter");
        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Missing on//off parameter in SetTradeSkillSubClassFilter");
        return 0;
    }

    uint32_t filter = CGTradeSkillInfo::m_subClassFilter;

    if (!static_cast<int32_t>(lua_tonumber(L, 2))) {
        CGTradeSkillInfo::SetSubClassFilter(filter & ~(1 << index), index);
        return 0;
    }

    if (lua_isnumber(L, 3) && static_cast<int32_t>(lua_tonumber(L, 3))) {
        CGTradeSkillInfo::SetSubClassFilter(1 << index, index);
        return 0;
    }

    CGTradeSkillInfo::SetSubClassFilter(filter | (1 << index), index);
    return 0;
}

// OFFSET: 0x009908E0/0x14070FA80
static int32_t Script_SetTradeSkillInvSlotFilter(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetTradeSkillInvSlotFilter(index [, on\\off, exclusive])");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        CGTradeSkillInfo::m_invTypeFilter = 0xFFFFFFFF;
        CGTradeSkillInfo::FilterAndSortSkills(1);
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
        return 0;
    }

    TradeSkillSubClassInfo* subClass;
    uint32_t mask;

    if (CGTradeSkillInfo::m_subClassFilterIndex < CGTradeSkillInfo::m_numSubClasses && (subClass = CGTradeSkillInfo::m_subClassList.m_data[CGTradeSkillInfo::m_subClassFilterIndex])) {
        mask = subClass->m_invTypeMask;
    } else {
        mask = 0xFFFFFFFF;
    }

    uint32_t slots = CGTradeSkillInfo::m_invSlots & mask;
    int32_t count = 0;
    int32_t slot = -1;

    for (uint32_t i = 0; i < 27; i++) {
        if ((1 << i) & slots) {
            if (count == index) {
                slot = i;
                break;
            }

            count++;
        }
    }

    if (slot == -1) {
        luaL_error(L, "Bad inv slot in SetTradeSkillInvSlotFilter");
        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Missing on//off parameter in SetTradeSkillInvSlotFilter");
        return 0;
    }

    uint32_t filter = CGTradeSkillInfo::m_invTypeFilter;

    uint32_t result;

    if (!static_cast<int32_t>(lua_tonumber(L, 2))) {
        result = ~(1 << slot) & filter;
    } else if (lua_isnumber(L, 3) && static_cast<int32_t>(lua_tonumber(L, 3))) {
        CGTradeSkillInfo::SetInvSlotFilter(1 << slot);
        return 0;
    } else {
        result = (1 << slot) | filter;
    }

    CGTradeSkillInfo::m_invTypeFilter = result;
    CGTradeSkillInfo::FilterAndSortSkills(1);
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
    return 0;
}

// PROVEN: 7e8f6066 IDENTICAL/NONE
// OFFSET: 0x00990A90/0x14070FC20
static int32_t Script_TradeSkillOnlyShowMakeable(lua_State* L) {
    int32_t value = StringToBOOL(L, 1, 1);

    if (value != CGTradeSkillInfo::m_showOnlyMakeable) {
        CGTradeSkillInfo::m_showOnlyMakeable = value;
        CGTradeSkillInfo::FilterAndSortSkills(1);
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
    }

    return 0;
}

// PROVEN: 62c3cf8f IDENTICAL/NONE
// OFFSET: 0x00990AD0/0x14070FC60
static int32_t Script_TradeSkillOnlyShowSkillUps(lua_State* L) {
    int32_t value = StringToBOOL(L, 1, 1);

    if (value != CGTradeSkillInfo::m_showOnlySkillUps) {
        CGTradeSkillInfo::m_showOnlySkillUps = value;
        CGTradeSkillInfo::FilterAndSortSkills(1);
        FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
    }

    return 0;
}

// OFFSET: 0x00990B10/0x14070FCA0
static int32_t Script_CollapseTradeSkillSubClass(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CollapseTradeSkillSubClass(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        CGTradeSkillInfo::m_collapseFilter = 0;
    } else {
        int32_t subClass = CGTradeSkillInfo::GetSubClassIndexFromSkill(index);

        if (subClass < 0) {
            luaL_error(L, "Bad sub class in CollapseTradeSkillSubClass");
            return 0;
        }

        CGTradeSkillInfo::m_collapseFilter &= ~(1 << subClass);
    }

    CGTradeSkillInfo::FilterAndSortSkills(1);
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
    return 0;
}

// OFFSET: 0x00990BB0/0x14070FD40
static int32_t Script_ExpandTradeSkillSubClass(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ExpandTradeSkillSubClass(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        CGTradeSkillInfo::m_collapseFilter = 0xFFFFFFFF;
    } else {
        int32_t subClass = CGTradeSkillInfo::GetSubClassIndexFromSkill(index);

        if (subClass < 0) {
            luaL_error(L, "Bad skill line in ExpandTradeSkillSubClass");
            return 0;
        }

        CGTradeSkillInfo::m_collapseFilter |= 1 << subClass;
    }

    CGTradeSkillInfo::FilterAndSortSkills(1);
    FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_SKILL_UPDATE, NULL);
    return 0;
}

// PROVEN: bc5925d2 IDENTICAL/NONE
// OFFSET: 0x00991940/0x140710D40
static void TradeSkillItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGTradeSkillInfo::RefreshList(0);
    }
}

// OFFSET: 0x00991E30/0x1407112F0
static int32_t Script_GetTradeSkillReagentInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetTradeSkillReagentInfo(index, reagentIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t reagentIndex = static_cast<int32_t>(lua_tonumber(L, 2));
    TradeSkillInfo* info;
    SpellRec* spell;
    SpellReagentsRec* reagents;

    if (index < CGTradeSkillInfo::m_numDisplayEntries && (info = CGTradeSkillInfo::m_displayList.m_data[index]) && (spell = g_spellDB.GetRecord(info->m_spellId)) && spell->m_spellReagentsID && (reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        int32_t count = 0;

        for (uint32_t i = 0; i < 8; i++) {
            if (!reagents->m_reagent[i] || ++count != reagentIndex) {
                continue;
            }

            int32_t itemId = reagents->m_reagent[i];
            WOWGUID requester = WOWGUID::Make(info->m_spellId | 0x1FE0000000000000ULL);
            ItemRecSparse* sparse = g_itemSparseDB.GetRecord(itemId, requester, TradeSkillItemCallback, NULL, false);
            ItemRec* item;

            if (sparse && (item = sparse->GetNonSparse())) {
                lua_pushstring(L, sparse->GetName(0));
                const char* path = ClientDBStringLookup(3);
                const char* separator = *path ? "\\" : "";
                char texture[260];
                SStrPrintf(texture, sizeof(texture), "%s%s%s", path, separator, CGItem_C::GetInventoryArt(item->m_displayInfoID));
                lua_pushstring(L, texture);
            } else {
                lua_pushnil(L);
                lua_pushnil(L);
            }

            lua_pushnumber(L, reagents->m_reagentCount[i]);
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                int32_t playerCount = player->m_bag.GetItemTypeCount(itemId, 0);
                lua_pushnumber(L, playerCount);
            } else {
                lua_pushnumber(L, 0.0);
            }

            return 4;
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 4;
}

// OFFSET: 0x00992060/0x1407115D0
static int32_t Script_GetTradeSkillTools(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTradeSkillTools(index)");
        return 0;
    }

    int32_t count = 0;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TradeSkillInfo* info = index >= CGTradeSkillInfo::m_numDisplayEntries ? NULL : CGTradeSkillInfo::m_displayList.m_data[index];
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    SpellRec* spell;

    if (!info || !player || !(spell = g_spellDB.GetRecord(info->m_spellId))) {
        return 2 * count;
    }

    SpellCastingRequirementsRec* requirements;

    if (spell->m_spellCastingRequirementsID && (requirements = g_spellCastingRequirementsDB.GetRecord(spell->m_spellCastingRequirementsID))) {
        SpellFocusObjectRec* focus;

        if (requirements->m_requiresSpellFocus && (focus = g_spellFocusObjectDB.GetRecord(requirements->m_requiresSpellFocus))) {
            lua_checkstack(L, 2);
            lua_pushstring(L, focus->m_name);
            lua_pushnumber(L, 1.0);
            count = 1;
        }

        AreaGroupRec* group;
        AreaTableRec* area;

        if (requirements->m_requiredAreasID > 0 && (group = g_areaGroupDB.GetRecord(requirements->m_requiredAreasID)) && (area = g_areaTableDB.GetRecord(group->m_areaID[0]))) {
            lua_checkstack(L, 2);
            lua_pushstring(L, area->m_areaName);
            lua_pushnumber(L, 1.0);
            count++;
        }
    }

    SpellTotemsRec* totems;

    if (spell->m_spellTotemsID && (totems = g_spellTotemsDB.GetRecord(spell->m_spellTotemsID))) {
        for (uint32_t i = 0; i < 2; i++) {
            if (!totems->m_totem[i]) {
                continue;
            }

            WOWGUID requester = WOWGUID::Make(info->m_spellId | 0x1FE0000000000000ULL);

            ItemRecSparse* totem = g_itemSparseDB.GetRecord(totems->m_totem[i], requester, TradeSkillItemCallback, NULL, false);

            if (totem) {
                lua_checkstack(L, 2);
                lua_pushstring(L, totem->GetName(0));

                if (player->m_bag.FindItemOfType(totems->m_totem[i], 0)) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                count++;
            }
        }

        for (uint32_t i = 0; i < 2; i++) {
            TotemCategoryRec* category;

            if (totems->m_requiredTotemCategoryID[i] && (category = g_totemCategoryDB.GetRecord(totems->m_requiredTotemCategoryID[i]))) {
                lua_checkstack(L, 2);
                lua_pushstring(L, category->m_name);

                if (player->m_bag.FindTotemCategory(totems->m_requiredTotemCategoryID[i], 0)) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                count++;
            }
        }
    }

    return 2 * count;
}

// STATIC: 0x00D146A0
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseTradeSkill", &Script_CloseTradeSkill },
    { "GetNumTradeSkills", &Script_GetNumTradeSkills },
    { "GetTradeSkillInfo", &Script_GetTradeSkillInfo },
    { "SelectTradeSkill", &Script_SelectTradeSkill },
    { "GetTradeSkillSelectionIndex", &Script_GetTradeSkillSelectionIndex },
    { "GetTradeSkillCooldown", &Script_GetTradeSkillCooldown },
    { "GetTradeSkillIcon", &Script_GetTradeSkillIcon },
    { "GetTradeSkillNumMade", &Script_GetTradeSkillNumMade },
    { "GetTradeSkillLine", &Script_GetTradeSkillLine },
    { "GetTradeSkillItemLink", &Script_GetTradeSkillItemLink },
    { "SetTradeSkillItemNameFilter", &Script_SetTradeSkillItemNameFilter },
    { "GetTradeSkillItemNameFilter", &Script_GetTradeSkillItemNameFilter },
    { "SetTradeSkillItemLevelFilter", &Script_SetTradeSkillItemLevelFilter },
    { "GetTradeSkillItemLevelFilter", &Script_GetTradeSkillItemLevelFilter },
    { "GetTradeSkillNumReagents", &Script_GetTradeSkillNumReagents },
    { "GetTradeSkillReagentInfo", &Script_GetTradeSkillReagentInfo },
    { "GetTradeSkillReagentItemLink", &Script_GetTradeSkillReagentItemLink },
    { "GetTradeSkillTools", &Script_GetTradeSkillTools },
    { "GetTradeSkillDescription", &Script_GetTradeSkillDescription },
    { "GetTradeSkillSubClasses", &Script_GetTradeSkillSubClasses },
    { "GetTradeSkillInvSlots", &Script_GetTradeSkillInvSlots },
    { "SetTradeSkillSubClassFilter", &Script_SetTradeSkillSubClassFilter },
    { "GetTradeSkillSubClassFilter", &Script_GetTradeSkillSubClassFilter },
    { "SetTradeSkillInvSlotFilter", &Script_SetTradeSkillInvSlotFilter },
    { "GetTradeSkillInvSlotFilter", &Script_GetTradeSkillInvSlotFilter },
    { "TradeSkillOnlyShowMakeable", &Script_TradeSkillOnlyShowMakeable },
    { "TradeSkillOnlyShowSkillUps", &Script_TradeSkillOnlyShowSkillUps },
    { "CollapseTradeSkillSubClass", &Script_CollapseTradeSkillSubClass },
    { "ExpandTradeSkillSubClass", &Script_ExpandTradeSkillSubClass },
    { "GetFirstTradeSkill", &Script_GetFirstTradeSkill },
    { "GetTradeskillRepeatCount", &Script_GetTradeskillRepeatCount },
    { "DoTradeSkill", &Script_DoTradeSkill },
    { "GetTradeSkillRecipeLink", &Script_GetTradeSkillRecipeLink },
    { "StopTradeSkillRepeat", &Script_StopTradeSkillRepeat },
    { "GetTradeSkillListLink", &Script_GetTradeSkillListLink },
    { "IsTradeSkillLinked", &Script_IsTradeSkillLinked },
    { "IsTradeSkillGuild", &Script_IsTradeSkillGuild },
    { "GetTradeSkillSubClassFilteredSlots", &Script_GetTradeSkillSubClassFilteredSlots },
    { "GetTradeSkillTexture", &Script_GetTradeSkillTexture },
};

// PROVEN: af9a7239 IDENTICAL/NONE
// OFFSET: 0x0098D6D0
void TradeSkillRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 6aa63387 IDENTICAL/NONE
// OFFSET: 0x0098D700
void TradeSkillUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
