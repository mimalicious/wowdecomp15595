#include "ClassTrainerFrame.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/Types.h"

// PROVEN: 5332716c IDENTICAL/NONE
// OFFSET: 0x009B9080/0x140743EC0
static int32_t Script_GetNumTrainerServices(lua_State* L) {
    lua_pushnumber(L, CGClassTrainer::m_filteredServices);
    return 1;
}

// PROVEN: 2f9014ab IDENTICAL/NONE
// OFFSET: 0x009B90C0/0x140743EF0
static int32_t Script_IsTradeskillTrainer(lua_State* L) {
    if (CGClassTrainer::m_trainerType == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 788fe43f IDENTICAL/NONE
// OFFSET: 0x009B9100/0x140743F30
static int32_t Script_GetTrainerGreetingText(lua_State* L) {
    lua_pushstring(L, CGClassTrainer::m_greetingText);
    return 1;
}

// OFFSET: 0x009B9170/0x140743FC0
static int32_t Script_GetTrainerServiceStepIndex(lua_State* L) {
    if (CGClassTrainer::m_stepIndex >= 0) {
        lua_pushinteger(L, CGClassTrainer::m_stepIndex + 1);
        return 1;
    }

    return 0;
}

// PROVEN: d8087372 IDENTICAL/NONE
// OFFSET: 0x009B9580/0x140744580
static int32_t Script_CloseTrainer(lua_State* L) {
    CGClassTrainer::SetTrainer(WOWGUID(), 0, 0);
    return 0;
}

// OFFSET: 0x009B95B0/0x1407445D0
static int32_t Script_SelectTrainerService(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectTrainerService(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGClassTrainer::m_numServices || (CGClassTrainer::m_currentSelection = CGClassTrainer::m_services.m_data[index]->m_spellID) < 0) {
        CGClassTrainer::m_currentSelection = 0;
    }

    return 0;
}

// OFFSET: 0x009B9640/0x140744650
static int32_t Script_GetTrainerSelectionIndex(lua_State* L) {
    int32_t index = CGClassTrainer::GetSelectionIndex();

    if (index < 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, CGClassTrainer::GetSelectionIndex() + 1);
    return 1;
}

// OFFSET: 0x009B96C0/0x1407446F0
static int32_t Script_GetTrainerServiceLevelReq(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceLevelReq(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t levelReq = 0;

    if (static_cast<uint32_t>(index) < CGClassTrainer::m_numServices) {
        TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[index];

        if (service) {
            levelReq = service->m_levelReq;
        }
    }

    lua_pushnumber(L, levelReq);
    return 1;
}

// OFFSET: 0x009B9740/0x140744770
static int32_t Script_GetTrainerServiceNumAbilityReq(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceAbilityReq(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service = static_cast<uint32_t>(index) >= CGClassTrainer::m_numServices ? NULL : CGClassTrainer::m_services.m_data[index];
    uint32_t count = 0;

    if (service) {
        if (service->m_abilityReq[0] > 0) {
            count = 1;
        }

        if (service->m_abilityReq[1] > 0) {
            count++;
        }

        if (service->m_abilityReq[2] > 0) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x009B97E0/0x140744810
static int32_t Script_GetTrainerServiceTypeFilter(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceTypeFilter(\"type\")");
        return 0;
    }

    int32_t type = GetServiceTypeFromString(lua_tolstring(L, 1, NULL));

    if (type == 6) {
        luaL_error(L, "Bad service type in GetTrainerServiceTypeFilter");
        return 0;
    }

    if ((1 << type) & g_serviceTypeFilterCVar->m_intValue) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009B9BC0/0x140744CB0
static int32_t Script_OpenTrainer(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        WOWGUID guid = player->m_obj->m_guid;
        CGPlayer_C::TalkToTrainer(guid);
    }

    return 0;
}

// OFFSET: 0x009B9C10/0x140744D00
static int32_t Script_GetTrainerServiceSkillLine(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceSkillLine(index)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service = static_cast<uint32_t>(index) < CGClassTrainer::m_numServices ? CGClassTrainer::m_services.m_data[index] : NULL;

    if (player && service) {
        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(service->m_spellID, count);

        for (uint32_t i = 0; i < count; i++) {
            const SpellEffectRec* effect = effects->m_data[i];

            if (!effect || !effect->m_effect) {
                continue;
            }

            SkillLineRec* skill;

            if (effect->m_effect == 44) {
                skill = g_skillLineDB.GetRecord(effect->m_effectMiscValue);
            } else {
                int32_t spellId = effect->m_effect != 36 || effect->m_implicitTargetA == 5 ? effect->m_spellID : effect->m_effectTriggerSpell;
                SkillLineAbilityRec* ability = SpellTableLookupAbility(player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[1], spellId);

                if (!ability) {
                    continue;
                }

                skill = g_skillLineDB.GetRecord(ability->m_skillLine);
            }

            if (skill) {
                lua_pushstring(L, skill->m_displayName);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009B9D80/0x140744F20
static int32_t Script_GetTrainerServiceCost(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceCost(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service = static_cast<uint32_t>(index) < CGClassTrainer::m_numServices ? CGClassTrainer::m_services.m_data[index] : NULL;
    int32_t moneyCost = service ? service->m_moneyCost : 0;
    lua_pushnumber(L, moneyCost);

    if (service->m_skillLine != 2) {
        return 1;
    }

    SkillLineRec* skill = g_skillLineDB.GetRecord(GetServiceSkillLine(service->m_spellID));

    if (!skill || skill->m_categoryID != 11) {
        return 1;
    }

    lua_pushboolean(L, service->m_skillRankReq == 0);
    return 2;
}

// OFFSET: 0x009B9E60/0x140745020
static int32_t Script_GetTrainerServiceSkillReq(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceSkillReq(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t met = 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    TrainerServiceInfo* service = static_cast<uint32_t>(index) < CGClassTrainer::m_numServices ? CGClassTrainer::m_services.m_data[index] : NULL;

    if (player && service && service->m_skillReq && service->m_skillRankReq) {
        if (SkillLineRec* skill = g_skillLineDB.GetRecord(service->m_skillReq)) {
            uint32_t i = 0;

            while (player->GetSkillLineID(i) != service->m_skillReq) {
                if (++i >= 128) {
                    break;
                }
            }

            if (i >= 128 || player->GetSkillValue(i) < service->m_skillRankReq) {
                met = 0;
            }

            uint32_t rank = service->m_skillRankReq;

            if (skill->m_displayName && *skill->m_displayName) {
                lua_pushstring(L, skill->m_displayName);
                lua_pushnumber(L, rank);

                if (met) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                return 3;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnumber(L, 0.0);

    if (met) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 3;
}

// OFFSET: 0x009B9FD0/0x1407451F0
static int32_t Script_GetTrainerServiceAbilityReq(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetTrainerServiceAbilityReq(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    char name[256] = { 0 };
    uint32_t abilityIndex = static_cast<int64_t>(lua_tonumber(L, 2)) - 1;
    int32_t met = 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    TrainerServiceInfo* service = index < CGClassTrainer::m_numServices ? CGClassTrainer::m_services.m_data[index] : NULL;

    if (player && service && abilityIndex < 3) {
        if (SpellRec* spell = g_spellDB.GetRecord(service->m_abilityReq[abilityIndex])) {
            if (spell->m_nameSubtext && *spell->m_nameSubtext) {
                SStrPrintf(name, sizeof(name), "%s (%s)", static_cast<const char*>(spell->m_name), static_cast<const char*>(spell->m_nameSubtext));
            } else {
                SStrCopy(name, spell->m_name, sizeof(name));
            }

            if (!player->IsSpellKnown(service->m_abilityReq[abilityIndex]) && !player->IsSpellSuperceded(service->m_abilityReq[abilityIndex])) {
                met = 0;
            }
        }
    }

    if (name[0]) {
        lua_pushstring(L, name);
    } else {
        lua_pushnil(L);
    }

    if (met) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x009BA1C0/0x140745480
static int32_t Script_BuyTrainerService(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BuyTrainerService(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0) {
        CGClassTrainer::BuyAllServices();
    } else {
        CGClassTrainer::BuyService(index);
    }

    return 0;
}

// OFFSET: 0x009BA220/0x140745500
static int32_t Script_SetTrainerServiceTypeFilter(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetTrainerServiceTypeFilter(\"type\", on/off [, exclusive])");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);

    if (!_stricmp(name, "all")) {
        g_serviceTypeFilterCVar->Set(7, true, false, false, true);
        CGClassTrainer::FilterAndSortServices();
        FrameScript_SignalEvent(SCRIPT_EVENT_TRAINER_UPDATE, NULL);
        return 0;
    }

    int32_t type = GetServiceTypeFromString(name);

    if (type == 6) {
        luaL_error(L, "Bad service type in SetTrainerServiceTypeFilter");
        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Missing on/off parameter in SetTrainerServiceTypeFilter");
        return 0;
    }

    int32_t filter = g_serviceTypeFilterCVar->m_intValue;

    if (!static_cast<int32_t>(lua_tonumber(L, 2))) {
        CGClassTrainer::SetServiceTypeFilter(filter & ~(1 << type));
        return 0;
    }

    if (lua_isnumber(L, 3) && static_cast<int32_t>(lua_tonumber(L, 3))) {
        CGClassTrainer::SetServiceTypeFilter(1 << type);
        return 0;
    }

    CGClassTrainer::SetServiceTypeFilter(filter | (1 << type));
    return 0;
}

// OFFSET: 0x009BA370/0x140745690
static int32_t Script_GetTrainerTradeskillRankValues(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || CGClassTrainer::m_trainerType != 2 || !CGClassTrainer::m_numServices || !CGClassTrainer::m_services.m_data[0]) {
        return 0;
    }

    int32_t skillLine = GetServiceSkillLine(CGClassTrainer::m_services.m_data[0]->m_spellID);

    if (!skillLine) {
        return 0;
    }

    int32_t rank;
    int32_t bonus;
    player->GetExpandedSkillRank(skillLine, rank, bonus);
    int32_t index = player->GetSkillIndex(skillLine);
    lua_pushnumber(L, rank);
    lua_pushnumber(L, static_cast<int32_t>(player->GetSkillMaxRank(index)));
    lua_pushnumber(L, bonus);
    return 3;
}

// OFFSET: 0x009BABC0/0x140746250
static int32_t Script_GetTrainerServiceInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceInfo(index)");
        return 0;
    }

    int64_t index = static_cast<int64_t>(lua_tonumber(L, 1));

    if (static_cast<uint32_t>(index - 1) >= CGClassTrainer::m_numServices) {
        return 0;
    }

    TrainerServiceInfo* service = CGClassTrainer::m_services.m_data[index - 1];

    if (!service || service->m_spellID < 0) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(service->m_spellID);

    if (!spell) {
        return 0;
    }

    lua_pushstring(L, spell->m_name);
    lua_pushstring(L, spell->m_nameSubtext);

    if (!service->m_state) {
        lua_pushstring(L, "used");
    } else if (service->m_state == 1) {
        lua_pushstring(L, "available");
    } else {
        lua_pushstring(L, "unavailable");
    }

    const char* icon = "";
    char texture[260];

    if (CGClassTrainer::m_trainerType == 2) {
        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

        if (effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59)) {
            WOWGUID requester = WOWGUID::Make(0);

            if (ItemRec* item = g_itemDB.GetRecord(effect->m_effectItemType, requester, NULL, NULL, false)) {
                const char* path = ClientDBStringLookup(3);
                SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
                icon = texture;
                lua_pushstring(L, icon);
                lua_pushnumber(L, service->m_levelReq);
                lua_pushnumber(L, service->m_skillLine);
                return 6;
            }
        }
    }

    if (SpellIconRec* iconRec = g_spellIconDB.GetRecord(spell->m_spellIconID)) {
        icon = iconRec->m_textureFilename;
    }

    lua_pushstring(L, icon);
    lua_pushnumber(L, service->m_levelReq);
    lua_pushnumber(L, service->m_skillLine);
    return 6;
}

// OFFSET: 0x009BADD0/0x1407464F0
static int32_t Script_GetTrainerServiceIcon(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceIcon(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service;
    SpellRec* spell;

    if (static_cast<uint32_t>(index) >= CGClassTrainer::m_numServices || !(service = CGClassTrainer::m_services.m_data[index]) || !(spell = g_spellDB.GetRecord(service->m_spellID))) {
        lua_pushnil(L);
        return 1;
    }

    if (CGClassTrainer::m_trainerType == 2) {
        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

        if (effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59)) {
            WOWGUID requester = WOWGUID::Make(0);

            if (ItemRec* item = g_itemDB.GetRecord(effect->m_effectItemType, requester, NULL, NULL, false)) {
                const char* path = ClientDBStringLookup(3);
                char texture[260];
                SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
                lua_pushstring(L, texture);
                return 1;
            }
        }
    }

    if (SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID)) {
        lua_pushstring(L, icon->m_textureFilename);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009BAF70/0x1407466E0
static int32_t Script_GetTrainerServiceDescription(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceDescription(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service;
    SpellRec* spell;

    if (static_cast<uint32_t>(index) >= CGClassTrainer::m_numServices || !(service = CGClassTrainer::m_services.m_data[index]) || !(spell = g_spellDB.GetRecord(service->m_spellID))) {
        lua_pushnil(L);
        return 1;
    }

    if (spell->m_description && *spell->m_description) {
        char text[1024];
        SpellParserParseText(spell, text, sizeof(text), 0, 0, 0, 0, 1, 0, NULL);
        lua_pushstring(L, text);
        return 1;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);
    int32_t type = effect ? effect->m_effect : 0;
    int32_t itemId;
    ItemRecSparse* item;

    if ((spell->m_attributes & 0x20) && (type == 24 || type == 157 || type == 59) && (itemId = effect->m_effectItemType) != 0) {
        WOWGUID requester = WOWGUID::Make(static_cast<int64_t>(spell->m_id) | 0x1FE0000000000000LL);

        if ((item = g_itemSparseDB.GetRecord(itemId, requester, &TrainerItemCallback, NULL, false)) && item->m_description && *item->m_description) {
            lua_pushstring(L, item->m_description);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009BB0E0/0x1407468E0
static int32_t Script_GetTrainerServiceItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTrainerServiceItemLink(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    TrainerServiceInfo* service;

    if (static_cast<uint32_t>(index) >= CGClassTrainer::m_numServices || !(service = CGClassTrainer::m_services.m_data[index])) {
        lua_pushnil(L);
        return 1;
    }

    const SpellEffectRec* effect = SpellGetEffect(service->m_spellID, 0);
    int32_t type = effect ? effect->m_effect : 0;
    int32_t itemId;

    if (CGClassTrainer::m_trainerType == 2 && (type == 24 || type == 157 || type == 59) && (itemId = effect->m_effectItemType) != 0) {
        WOWGUID requester = WOWGUID::Make(0);

        if (ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false)) {
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(effect->m_effectItemType, item->m_quality, 0, 0, 0, 0));
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// STATIC: 0x00D15A30
static FrameScript_Method s_ScriptFunctions[] = {
    { "OpenTrainer", &Script_OpenTrainer },
    { "CloseTrainer", &Script_CloseTrainer },
    { "GetNumTrainerServices", &Script_GetNumTrainerServices },
    { "GetTrainerServiceInfo", &Script_GetTrainerServiceInfo },
    { "SelectTrainerService", &Script_SelectTrainerService },
    { "IsTradeskillTrainer", &Script_IsTradeskillTrainer },
    { "GetTrainerSelectionIndex", &Script_GetTrainerSelectionIndex },
    { "GetTrainerGreetingText", &Script_GetTrainerGreetingText },
    { "GetTrainerServiceIcon", &Script_GetTrainerServiceIcon },
    { "GetTrainerServiceSkillLine", &Script_GetTrainerServiceSkillLine },
    { "GetTrainerServiceCost", &Script_GetTrainerServiceCost },
    { "GetTrainerServiceLevelReq", &Script_GetTrainerServiceLevelReq },
    { "GetTrainerServiceSkillReq", &Script_GetTrainerServiceSkillReq },
    { "GetTrainerServiceNumAbilityReq", &Script_GetTrainerServiceNumAbilityReq },
    { "GetTrainerServiceAbilityReq", &Script_GetTrainerServiceAbilityReq },
    { "GetTrainerServiceDescription", &Script_GetTrainerServiceDescription },
    { "BuyTrainerService", &Script_BuyTrainerService },
    { "SetTrainerServiceTypeFilter", &Script_SetTrainerServiceTypeFilter },
    { "GetTrainerServiceTypeFilter", &Script_GetTrainerServiceTypeFilter },
    { "GetTrainerServiceItemLink", &Script_GetTrainerServiceItemLink },
    { "GetTrainerServiceStepIndex", &Script_GetTrainerServiceStepIndex },
    { "GetTrainerTradeskillRankValues", &Script_GetTrainerTradeskillRankValues },
};

// PROVEN: 4c2fdd6d IDENTICAL/NONE
// OFFSET: 0x009B91A0/0x140743FF0
void ClassTrainerRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 373c2929 IDENTICAL/NONE
// OFFSET: 0x009B91D0/0x140744040
void ClassTrainerUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
