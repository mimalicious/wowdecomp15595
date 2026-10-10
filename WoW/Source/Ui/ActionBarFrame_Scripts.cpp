#include "ActionBarFrame.h"
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/TotemCategoryRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGCooldown.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x009544A0/0x1406C52B0
static int32_t Script_GetActionCount(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionCount(slot)");
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t count = slot > 143 ? 0 : CGActionBar::m_slotUseCount[slot];
    lua_pushnumber(L, static_cast<double>(count));
    return 1;
}

// OFFSET: 0x00954520/0x1406C5330
static int32_t Script_GetActionText(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionText(slot)");
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action) {
        if ((action & 0xF0000000) == 0x40000000) {
            MACRONODE* macro = CGUIMacros::GetMacroByID(action & 0xB1FFFFFF);

            if (macro) {
                lua_pushstring(L, macro->m_name);
                return 1;
            }
        } else if ((action & 0xF0000000) == 0x20000000) {
            const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGActionBar::GetEquipmentSetID(slot));

            if (set) {
                lua_pushstring(L, set->m_name);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009545F0/0x1406C5410
static int32_t Script_IsUsableAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsUsableAction(slot)");
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 143 || !CGActionBar::m_slotUsable[slot]) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    if (CGActionBar::m_slotNoMana[slot]) {
        lua_pushnil(L);
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x009546B0/0x1406C54E0
static int32_t Script_GetBonusBarOffset(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGActionBar::m_bonusBarOffset));
    return 1;
}

// OFFSET: 0x009546F0/0x14065A910
static int32_t Script_GetMultiCastBarOffset(lua_State* L) {
    lua_pushnumber(L, 6.0);
    return 1;
}

// OFFSET: 0x00954720/0x1406C5530
static int32_t Script_ChangeActionBarPage(lua_State* L) {
    uint32_t page = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (page > 5) {
        luaL_error(L, "ChangeActionBarPage() needs a page in the range 1 to %d", 6);
    }

    if (!CGGameUI::CanPerformAction(14) || page == CGActionBar::m_currentPage) {
        return 0;
    }

    CGActionBar::m_currentPage = page;
    FrameScript_SignalEvent(SCRIPT_EVENT_ACTIONBAR_PAGE_CHANGED, NULL);
    return 0;
}

// OFFSET: 0x00954790/0x1406C55B0
static int32_t Script_GetActionBarPage(lua_State* L) {
    if (!CGActionBar::m_tempPageActiveFlags || (CGActionBar::m_tempPageActiveFlags & CGActionBar::TEMP_PAGE_EXTRA)) {
        lua_pushinteger(L, CGActionBar::m_currentPage + 1);
    } else {
        lua_pushinteger(L, 1);
    }

    return 1;
}

// OFFSET: 0x00954B50/0x1406C5A90
static int32_t Script_HasAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: HasAction(slot)");
    }

    if (CGActionBar::HasAction(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00954BC0/0x1406C5B10
static int32_t Script_SetActionBarToggles(lua_State* L) {
    uint8_t toggles = 0;

    for (int32_t i = 0; i < 4; i++) {
        if (StringToBOOL(L, i + 1, 0)) {
            toggles |= 1 << i;
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_ACTION_BAR_TOGGLES));
    msg.Put(toggles);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00954C60/0x1406C5C00
static int32_t Script_IsPossessBarVisible(lua_State* L) {
    if (!CGPetInfo::m_possessSpell) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const SpellRec* spell = g_spellDB.GetRecord(CGPetInfo::m_possessSpell);
    lua_pushboolean(L, !(spell && SpellHasAuraEffect(spell, 236)));
    return 1;
}

// OFFSET: 0x00954CE0/0x1406C5C90
static int32_t Script_GetMultiCastTotemSpells(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMultiCastTotemSpells(slot)");
    }

    uint32_t slot = (static_cast<uint8_t>(lua_tointeger(L, 1)) - 1) & 0x3;
    TSGrowableArray<int32_t>& spells = CGSpellBook::m_totemSpells[slot];

    for (uint32_t i = 0; i < spells.m_count; i++) {
        lua_pushinteger(L, spells.m_data[i]);
    }

    return spells.m_count;
}

// OFFSET: 0x00955100/0x1406C6230
static int32_t Script_GetActionBarToggles(lua_State* L) {
    uint8_t toggles = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        toggles = player->m_player->m_playerFieldBytes[2];
    }

    lua_checkstack(L, 4);

    for (uint32_t i = 0; i < 4; i++) {
        if (toggles & (1 << i)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    }

    return 4;
}

// OFFSET: 0x00955190/0x1406C62E0
static int32_t Script_GetBonusBarOverrideBarType(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint16_t overrideId = *reinterpret_cast<const uint16_t*>(player->m_player->m_playerFieldBytes2);

    if (!overrideId) {
        return 0;
    }

    const OverrideSpellDataRec* rec = g_overrideSpellDataDB.GetRecord(overrideId);

    if (!rec || !rec->m_overrideBarType || !*rec->m_overrideBarType || (rec->m_flags & 0x8)) {
        return 0;
    }

    lua_pushstring(L, rec->m_overrideBarType);
    return 1;
}

// OFFSET: 0x00955210/0x1406C63A0
static int32_t Script_HasExtraActionBar(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    lua_pushboolean(L, (CGActionBar::m_tempPageActiveFlags & CGActionBar::TEMP_PAGE_EXTRA) != 0);
    return 1;
}

// OFFSET: 0x00955810/0x1406C6BF0
static int32_t Script_GetActionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionInfo(slot)");
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t action = CGActionBar::m_slotActions[slot];

    if (action) {
        if (!(action & 0xF0000000)) {
            int32_t spellId = 0;

            if (slot <= 143) {
                spellId = action & 0xF1FFFFFF;
                HASHKEY_NONE key;
                OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.Ptr(spellId, key);

                if (node) {
                    spellId = node->m_overrideSpellID;
                }
            }

            const SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (spell && SpellIsCallCompanion(spell)) {
                int32_t type = CGSpellBook::GetCompanionType(spell);
                lua_pushstring(L, "companion");
                lua_pushnumber(L, static_cast<double>(spellId));

                if (type == 0) {
                    lua_pushstring(L, "CRITTER");
                } else if (type == 1) {
                    lua_pushstring(L, "MOUNT");
                } else {
                    lua_pushstring(L, "UNKNOWN");
                }

                return 3;
            }

            lua_pushstring(L, "spell");
            lua_pushnumber(L, static_cast<double>(spellId));
            lua_pushstring(L, "spell");
            return 3;
        }

        if ((action & 0xF0000000) == 0x10000000) {
            int32_t spellId = CGActionBar::GetPetActionSpell(slot);
            lua_pushstring(L, "spell");
            lua_pushnumber(L, static_cast<double>(spellId));
            lua_pushstring(L, "pet");
            return 3;
        }
    }

    if (CGActionBar::IsItem(slot)) {
        lua_pushstring(L, "item");
        lua_pushnumber(L, static_cast<double>(CGActionBar::GetItem(slot)));
        return 2;
    }

    if (CGActionBar::IsMacro(slot)) {
        lua_pushstring(L, "macro");
        lua_pushnumber(L, static_cast<double>(CGUIMacros::GetMacroIndexByID(CGActionBar::GetMacroID(slot)) + 1));
        return 2;
    }

    if (CGActionBar::IsEquipmentSet(slot)) {
        const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGActionBar::GetEquipmentSetID(slot));

        if (set) {
            lua_pushstring(L, "equipmentset");
            lua_pushstring(L, set->m_name);
            return 2;
        }
    } else if (CGActionBar::IsFlyout(slot)) {
        lua_pushstring(L, "flyout");
        lua_pushinteger(L, CGActionBar::GetFlyout(slot));
        return 2;
    }

    return 0;
}

// OFFSET: 0x00955AE0/0x1406C6F20
static int32_t Script_GetActionAutocast(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionAutocast(slot)");
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    PetAction* action = NULL;

    if (slot <= 143) {
        int32_t petSpell;
        int32_t spellId = CGActionBar::GetSpell(slot, petSpell);

        if (petSpell && (action = CGPetInfo::GetPetSpellAction(spellId)) && static_cast<int32_t>(action->m_data) < 0) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }

    if (action && (action->m_data & 0x40000000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x00956450/0x1406C7A60
static int32_t Script_IsAttackAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsAttackAction(slot)");
    }

    if (CGActionBar::IsAttackAction(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009564C0/0x1406C7AE0
static int32_t Script_IsAutoRepeatAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsAutorepeatAction(slot)");
    }

    if (CGActionBar::IsAutoRepeatAction(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00956530/0x1406C7B60
static int32_t Script_ActionHasRange(lua_State* L) {
    if (lua_isnumber(L, 1) && CGActionBar::ActionHasRange(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00956590/0x1406C7BC0
static int32_t Script_IsActionInRange(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
        int32_t inRange = 0;
        WOWGUID target = WOWGUID();
        Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, false);

        if (CGActionBar::HasValidTarget(slot, target, inRange)) {
            lua_pushnumber(L, inRange ? 1.0 : 0.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00956660/0x1406C7C90
static int32_t Script_SetActionUIButton(lua_State* L) {
    CSimpleCheckbox* checkbox = NULL;
    CGCooldown* cooldown = NULL;
    uint32_t slot = 0;

    if (lua_type(L, 1) == LUA_TTABLE && lua_isnumber(L, 2) && lua_type(L, 3) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        checkbox = static_cast<CSimpleCheckbox*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
        slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
        lua_rawgeti(L, 3, 0);
        cooldown = static_cast<CGCooldown*>(lua_touserdata(L, -1));
        lua_settop(L, -2);
    }

    if (!checkbox || !checkbox->IsA(CSimpleCheckbox::GetObjectType()) || !cooldown || !cooldown->IsA(CGCooldown::GetObjectType())) {
        luaL_error(L, "Usage: SetActionUIButton(checkboxFrame, actionSlot, cooldownFrame)");
    }

    CGActionBar::SetActionUIButton(checkbox, slot, cooldown);
    return 0;
}

// OFFSET: 0x00956960/0x1406C8170
static int32_t Script_IsCurrentAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsCurrentAction(slot)");
    }

    if (CGActionBar::IsCurrentAction(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00957F40/0x1406C9E10
static int32_t Script_GetActionTexture(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionTexture(slot)");
    }

    const char* texture = CGActionBar::GetTexture(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (texture) {
        lua_pushstring(L, texture);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00957FB0/0x1406C9E80
static int32_t Script_GetActionCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActionCooldown(slot)");
    }

    uint32_t start;
    uint32_t duration;
    uint32_t enable = 0;
    CGActionBar::GetCooldown(static_cast<int32_t>(lua_tonumber(L, 1)) - 1, start, duration, enable);
    lua_pushnumber(L, static_cast<double>(start) * 0.001);
    lua_pushnumber(L, static_cast<double>(duration) * 0.001);
    lua_pushnumber(L, static_cast<double>(enable));
    return 3;
}

// OFFSET: 0x00958080/0x1406C9F40
static int32_t Script_IsConsumableAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsConsumableAction(slot)");
    }

    if (CGActionBar::IsConsumableAction(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009580F0/0x1406C9FC0
static int32_t Script_IsStackableAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsStackableAction(slot)");
    }

    if (IsStackableAucItem(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00958160/0x1406CA040
static int32_t Script_IsEquippedAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsEquippedAction(slot)");
    }

    if (CGActionBar::IsEquippedAction(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00959090/0x1406CB410
static int32_t Script_PlaceAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PlaceAction(slot)");
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (slot >= 120 && slot < 132) {
        return 0;
    }

    CGActionBar::PutActionInSlot(slot, 0);
    return 0;
}

// OFFSET: 0x009590F0/0x1406CB470
static int32_t Script_SetMultiCastSpell(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetMultiCastSpell(slot, spellID)");
    }

    if (!CGGameUI::CanPerformAction(14)) {
        return 0;
    }

    int32_t slot = lua_tointeger(L, 1) - 1;

    if (slot < 132 || static_cast<uint32_t>(slot) >= 144) {
        return 0;
    }

    int32_t spellId = lua_tointeger(L, 2);

    if (!spellId) {
        CGActionBar::RemoveAction(slot, 0);
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 0;
    }

    int32_t category;

    if (IsMultiCastSpellForSlot(slot, spell, &category)) {
        CGActionBar::SetAction(slot, spellId, 1, 0);
    } else {
        const TotemCategoryRec* rec = g_totemCategoryDB.GetRecord(category);

        if (rec) {
            CGGameUI::DisplayError(ERR_MULTI_CAST_ACTION_TOTEM_S, rec->m_name);
        }
    }

    return 0;
}

// OFFSET: 0x00959850/0x1406CBE60
static int32_t Script_UseAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: UseAction(slot, [, target] [, button])");
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    WOWGUID target;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true)) {
        CGActionBar::UseAction(slot, target, lua_tolstring(L, 3, NULL));
    }

    return 0;
}

// OFFSET: 0x009598E0/0x1406CBF00
static int32_t Script_PickupAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PickupAction(slot)");
    }

    CGActionBar::PickupAction(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x009C8210/0x140756A70
static int32_t Script_GetExtraBarOffset(lua_State* L) {
    lua_pushnumber(L, 5.0);
    return 1;
}

// STATIC: 0x00D12C98
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetActionInfo", &Script_GetActionInfo },
    { "GetActionTexture", &Script_GetActionTexture },
    { "GetActionCount", &Script_GetActionCount },
    { "GetActionCooldown", &Script_GetActionCooldown },
    { "GetActionAutocast", &Script_GetActionAutocast },
    { "GetActionText", &Script_GetActionText },
    { "HasAction", &Script_HasAction },
    { "UseAction", &Script_UseAction },
    { "PickupAction", &Script_PickupAction },
    { "PlaceAction", &Script_PlaceAction },
    { "IsAttackAction", &Script_IsAttackAction },
    { "IsCurrentAction", &Script_IsCurrentAction },
    { "IsAutoRepeatAction", &Script_IsAutoRepeatAction },
    { "IsUsableAction", &Script_IsUsableAction },
    { "IsConsumableAction", &Script_IsConsumableAction },
    { "IsStackableAction", &Script_IsStackableAction },
    { "IsEquippedAction", &Script_IsEquippedAction },
    { "ActionHasRange", &Script_ActionHasRange },
    { "IsActionInRange", &Script_IsActionInRange },
    { "GetBonusBarOffset", &Script_GetBonusBarOffset },
    { "GetMultiCastBarOffset", &Script_GetMultiCastBarOffset },
    { "ChangeActionBarPage", &Script_ChangeActionBarPage },
    { "GetActionBarPage", &Script_GetActionBarPage },
    { "GetActionBarToggles", &Script_GetActionBarToggles },
    { "SetActionBarToggles", &Script_SetActionBarToggles },
    { "IsPossessBarVisible", &Script_IsPossessBarVisible },
    { "GetMultiCastTotemSpells", &Script_GetMultiCastTotemSpells },
    { "SetMultiCastSpell", &Script_SetMultiCastSpell },
    { "GetBonusBarOverrideBarType", &Script_GetBonusBarOverrideBarType },
    { "HasExtraActionBar", &Script_HasExtraActionBar },
    { "GetExtraBarOffset", &Script_GetExtraBarOffset },
    { "SetActionUIButton", &Script_SetActionUIButton },
};

// OFFSET: 0x009547E0
void ActionBarRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00954810
void ActionBarUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
