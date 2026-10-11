#include "Spell_C.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/Rec/SpellAuraVisibilityRec.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include <cstdlib>
#include <cstring>

// STATIC: 0x00D0ADC8
static const char* s_visibilityTypes[] = { "RAID_INCOMBAT", "RAID_OUTOFCOMBAT", "ENEMY_TARGET" };

// OFFSET: 0x008A5A60/0x1405E8660
static int32_t Script_SpellIsTargeting(lua_State* L) {
    if (s_targetingSpell) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 0c6e96e2 IDENTICAL/NONE
// OFFSET: 0x008A5AA0/0x1405E86A0
static int32_t Script_SpellCanTargetItem(lua_State* L) {
    if (s_targetingSpell && (s_needTargets & 0x4010)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008A5AF0/0x1405E86E0
static int32_t Script_SpellCanTargetGlyph(lua_State* L) {
    if (s_targetingSpell && (s_needTargets & 0x20000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008AB020
static int32_t Script_CannotBeResurrected(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_auraEffectMask[314 / 8] & (1 << (314 % 8)))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008A85D0/0x1405EB870
static int32_t Script_SpellTargetItem(lua_State* L) {
    if (!CGGameUI::CanPerformAction(2) || !s_targetingSpell || !(s_needTargets & 0x4010)) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGItem_C* item;

    if (lua_isnumber(L, 1)) {
        item = player->m_bag.FindItemOfType(static_cast<int32_t>(lua_tonumber(L, 1)), 0);
    } else if (lua_isstring(L, 1)) {
        item = player->m_bag.FindItemByName(lua_tolstring(L, 1, NULL), 0);
    } else {
        luaL_error(L, "Usage: SpellTargetItem(itemID|\"name\"|\"itemlink\")");
        return 0;
    }

    if (item) {
        CGGameUI::CastItemSpell(item->m_obj->m_guid, 0);
    }

    return 0;
}

// OFFSET: 0x008B7A00/0x1405FF4E0
static int32_t Script_SpellTargetUnit(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SpellTargetUnit(\"unit\")");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(2) || !s_targetingSpell) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(s_targetingSpell->m_data.m_spellId);
    WOWGUID guid = WOWGUID::Make(0);

    if (Script_GetGUIDFromToken(lua_tostring(L, 1), guid, false, false) && guid) {
        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!(guid.m_guid & 0xF000000000000000ull) && (guid.m_guid & 0xF07FFFFFFFFFFFFFull) && ((s_needTargets & 0x8000) || (spell && (spell->m_attributesExI & 0x8000000)))) {
            CGCorpse_C* corpse = ClntObjMgrFindCorpseByOwner(guid);

            if (corpse) {
                object = corpse;
            }
        }

        if (object) {
            Spell_C_HandleSpriteClick(object, NULL);
            return 0;
        }
    }

    Spell_C_SpellFailed(s_targetingSpell, spell, SPELL_FAILED_OUT_OF_RANGE, -1, -1, false);
    return 0;
}

// OFFSET: 0x008B7970/0x1405FF450
static int32_t Script_SpellCanTargetUnit(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return luaL_error(L, "Usage: SpellCanTargetUnit(\"unit\")");
        return 0;
    }

    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));

    if (guid && Spell_C_HandleSpriteRay(guid, true)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008B47E0/0x1405FBAC0
static int32_t Script_SpellStopTargeting(lua_State* L) {
    if (CGGameUI::CanPerformAction(2)) {
        bool targeting = s_targetingSpell != NULL;

        if (s_targetingSpell) {
            Spell_C_CancelSpell(s_targetingSpell, true, true, 27);
        }

        if (targeting) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008B4860/0x1405FBB40
static int32_t Script_SpellStopCasting(lua_State* L) {
    if (!CGGameUI::CanPerformAction(2)) {
        lua_pushnil(L);
        return 1;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && player->m_unit->m_unitFieldCharmedby) {
        lua_pushnil(L);
        return 1;
    }

    if (OnCancelCombat()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (Spell_C_IsActivePlayerCasting()) {
        Spell_C_StopCasting();
        lua_pushnumber(L, 1.0);
        return 1;
    }

    if (!Spell_C_GetAutoRepeatingSpell()) {
        lua_pushnil(L);
        return 1;
    }

    Spell_C_CancelAutoRepeat(true);
    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x008A86C0/0x1405EB980
static int32_t Script_CancelItemTempEnchantment(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    uint32_t slot;

    if (!lua_isnumber(L, 1) || (slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, slot > 1)) {
        luaL_error(L, "Usage: CancelItemTempEnchantment(slot)");
        return 0;
    }

    int32_t invSlot = slot + 15;
    CGItem_C* item = player->m_bag.GetItemPointer(invSlot);

    if (!item || !item->GetEnchantmentId(1)) {
        return 0;
    }

    CGPlayer_C::RemoveTempEnchantment(invSlot);
    return 0;
}

// OFFSET: 0x008AAE40/0x1405EE9F0
static int32_t Script_CancelUnitBuff(lua_State* L) {
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 1, &info)) {
        luaL_error(L, "Usage: CancelUnitBuff(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(24)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_obj->m_guid == info.m_guid) {
        int32_t slot = static_cast<int32_t>(player->GetSortedAuraSlot(info.m_index, info.m_name, info.m_rank, info.m_filter));

        if (slot < 0) {
            return 0;
        }

        SpellRec* spell = g_spellDB.GetRecord(player->GetAuraSpellId(slot));

        if (spell && ((player->GetAuraFlags(slot) & 0x10) || (spell->m_attributesEx & 0x4))) {
            Spell_C_CancelAura(spell, player, slot);
        }

        return 0;
    }

    CGInputControl* input = CGInputControl::GetActive();
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!input || !mover || !input->IsVehicleSeatFlagSet(0x2000000)) {
        return 0;
    }

    int32_t slot = static_cast<int32_t>(mover->GetSortedAuraSlot(info.m_index, info.m_name, info.m_rank, info.m_filter));

    if (slot < 0 || mover->GetAuraCaster(slot) != ClntObjMgrGetActivePlayer().m_guid) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(mover->GetAuraSpellId(slot));

    if (!spell) {
        return 0;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effectAura == 236) {
            mover->VehiclePassengerRequestExit();
            break;
        }
    }

    return 0;
}

// OFFSET: 0x008A5B40/0x1405E8730
static int32_t Script_SpellCancelQueuedSpell(lua_State* L) {
    UserClientCancelQueuedSpell msg;
    ClientServices::Send(&msg);
    return 0;
}

// OFFSET: 0x008A8770/0x1405EBA60
static int32_t Script_SpellIsSelfBuff(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SpellIsSelfBuff(spellID)");
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(lua_tointeger(L, 1));

    if (!spell) {
        return 0;
    }

    bool selfBuff = false;

    for (uint32_t i = 0; i < 3; i++) {
        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, i);

        if (!effect || effect->m_effect != 6) {
            continue;
        }

        if (effect->m_implicitTargetA != 1 && effect->m_implicitTargetB != 1) {
            lua_pushboolean(L, false);
            return 1;
        }

        selfBuff = true;
    }

    lua_pushboolean(L, selfBuff);
    return 1;
}

// OFFSET: 0x008AB060/0x1405EED50
static int32_t Script_SpellGetVisibilityInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SpellGetVisibilityInfo(spellID, \"visType\")");
        return 0;
    }

    int32_t type = 0;

    while (_stricmp(lua_tostring(L, 2), s_visibilityTypes[type])) {
        if (++type >= 3) {
            luaL_error(L, "SpellGetVisibilityInfo: Invalid visType. Valid types: \"RAID_INCOMBAT\", \"RAID_OUTOFCOMBAT\", \"ENEMY_TARGET\"");
            return 0;
        }
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        SpellAuraVisibilityRec key;
        key.m_spellID = lua_tointeger(L, 1);
        key.m_type = type;
        const SpellAuraVisibilityRec* rec = static_cast<const SpellAuraVisibilityRec*>(bsearch(&key, g_spellAuraVisibilityDB.m_numRecords <= 0 ? NULL : g_spellAuraVisibilityDB.m_records, g_spellAuraVisibilityDB.m_numRecords, sizeof(SpellAuraVisibilityRec), CompareSpellAuraVisibility));

        if (rec) {
            lua_pushboolean(L, true);
            lua_pushboolean(L, rec->m_flags & 0x1);
            int32_t talentTab = TalentInfoGetPrimaryTalentTree(CGTalentInfo::GetActiveTalentGroup(0), 0, 0);
            lua_pushboolean(L, SpellAuraVisibilityHasTalentTab(rec->m_id, talentTab));
            return 3;
        }
    }

    lua_pushboolean(L, false);
    return 1;
}

// STATIC: 0x00D0AE00
static FrameScript_Method s_ScriptFunctions[] = {
    { "SpellIsTargeting", &Script_SpellIsTargeting },
    { "SpellCanTargetItem", &Script_SpellCanTargetItem },
    { "SpellTargetItem", &Script_SpellTargetItem },
    { "SpellCanTargetUnit", &Script_SpellCanTargetUnit },
    { "SpellTargetUnit", &Script_SpellTargetUnit },
    { "SpellCanTargetGlyph", &Script_SpellCanTargetGlyph },
    { "SpellStopTargeting", &Script_SpellStopTargeting },
    { "SpellStopCasting", &Script_SpellStopCasting },
    { "CancelUnitBuff", &Script_CancelUnitBuff },
    { "CancelItemTempEnchantment", &Script_CancelItemTempEnchantment },
    { "CannotBeResurrected", &Script_CannotBeResurrected },
    { "SpellCancelQueuedSpell", &Script_SpellCancelQueuedSpell },
    { "SpellGetVisibilityInfo", &Script_SpellGetVisibilityInfo },
    { "SpellIsSelfBuff", &Script_SpellIsSelfBuff },
};

// OFFSET: 0x008A5B70
void SpellRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x008A5BA0
void SpellUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
