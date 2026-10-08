#include "SpellBookFrame.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Net/Types.h"
#include "Engine/Source/Base/CDataStore.h"

// STATIC: #0x00ACCCC4
const char* s_companionTypes[2] = { "CRITTER", "MOUNT" };

// OFFSET: 0x0090F570/0x14066AB50
static int32_t Script_GetNumShapeshiftForms(lua_State* L) {
    lua_pushnumber(L, s_shapeshiftSpells.m_count);
    return 1;
}

// OFFSET: #0x0053DD10
static int32_t Script_UpdateSpells(lua_State* L) {
    CGSpellBook::UpdateSpells(0, 0, 0);
    return 0;
}

// OFFSET: 0x0090F180/0x14066A6B0
static int32_t Script_GetNumSpellTabs(lua_State* L) {
    lua_pushnumber(L, s_numSkillInfos);
    return 1;
}

// OFFSET: #0x0053B6C0
static int32_t Script_FindSpellBookSlotByID(lua_State* L) {
    int32_t spellId;

    if (!lua_isnumber(L, 1) || (spellId = static_cast<int32_t>(lua_tointeger(L, 1))) <= 0) {
        luaL_error(L, "Usage: FindSpellBookSlotByID(spellID[, isPet])");
        return 0;
    }

    int32_t slot = GetSpellSlot(spellId, StringToBOOL(L, 2, 0));

    if (slot >= 0) {
        lua_pushinteger(L, slot);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: #0x0053B650
static int32_t Script_GetKnownSlotFromHighestRankSlot(lua_State* L) {
    uint32_t slot = lua_tointeger(L, 1) - 1;
    int32_t index = -1;

    if (slot < s_unkBE8D9C) {
        int32_t spell = s_unkBE5D88[slot];

        for (uint32_t i = 0; i < s_numSpells; i++) {
            if (s_spellList[i] == spell) {
                index = i;
                break;
            }
        }
    }

    lua_pushnumber(L, index + 1);
    return 1;
}

// OFFSET: 0x0090F1E0/0x14066A720
static int32_t Script_GetSpellTabInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetSpellTabInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    SpellBookSkillInfo* info;

    if (static_cast<uint32_t>(index) >= s_numSkillInfos || !(info = s_skillInfos.m_data[index])) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 6;
    }

    if (!info->m_skillLine) {
        lua_pushstring(L, FrameScript_GetText("GENERAL_SPELLS", -1, GENDER_NOT_APPLICABLE));
        lua_pushstring(L, "Interface\\Icons\\Ability_Kick");
    } else {
        SkillLineRec* skill = g_skillLineDB.GetRecord(info->m_skillLine);

        if (skill) {
            lua_pushstring(L, skill->m_displayName);
            SpellIconRec* icon = g_spellIconDB.GetRecord(skill->m_spellIconID);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
            } else {
                lua_pushnil(L);
            }
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }

    uint32_t offset = 0;

    if (index && static_cast<uint32_t>(index) < s_numSkillInfos) {
        for (int32_t i = index - 1; i >= 0; i--) {
            offset += s_skillInfos.m_data[i]->m_count;
        }
    }

    lua_pushnumber(L, offset);
    lua_pushnumber(L, info->m_count);
    uint32_t visibleOffset = 0;

    if (index && static_cast<uint32_t>(index) < s_numSkillInfos) {
        for (int32_t i = index - 1; i >= 0; i--) {
            visibleOffset += s_skillInfos.m_data[i]->m_unk08;
        }
    }

    lua_pushnumber(L, visibleOffset);
    lua_pushnumber(L, info->m_unk08);
    return 6;
}

// OFFSET: #0x0053B600
static bool GetLuaCompanionType(lua_State* L, int32_t* type) {
    const char* token = lua_tolstring(L, 1, NULL);

    if (!token) {
        return false;
    }

    for (int32_t i = 0; i < 2; i++) {
        if (!SStrCmpI(token, s_companionTypes[i], STORM_MAX_STR)) {
            *type = i;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00910C20/0x14066C8B0
static int32_t Script_HasPetSpells(lua_State* L) {
    if (!CGSpellBook::m_numPetSpells) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    lua_pushnumber(L, CGSpellBook::m_numPetSpells);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        lua_pushstring(L, "PET");
        return 2;
    }

    ChrClassesRec* rec = g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1]);
    lua_pushstring(L, rec->m_petNameToken);
    return 2;
}

// OFFSET: 0x009111E0/0x14066D020
static int32_t Script_GetCompanionInfo(lua_State* L) {
    int32_t type;

    if (!GetLuaCompanionType(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: GetCompanionInfo(type, index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    TSGrowableArray<int32_t>& companions = CGSpellBook::m_companions[type];

    if (index >= companions.m_count) {
        return 0;
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(companions.m_data[index], &spell)) {
        return 0;
    }

    int32_t creatureId = spell.m_effectMiscValue[0];
    lua_pushinteger(L, creatureId);
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(creatureId, WOWGUID(), NULL, NULL, false);

    if (stats) {
        lua_pushstring(L, stats->GetName(0));
    } else {
        lua_pushnil(L);
    }

    lua_pushinteger(L, spell.m_id);
    bool active = Spell_C_IsActiveSpell(&spell, ClntObjMgrGetActivePlayerObj());
    SpellIconRec* icon = g_spellIconDB.GetRecord(active && spell.m_activeIconID ? spell.m_activeIconID : spell.m_spellIconID);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    if (active) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: #0x005407F0
static int32_t Script_GetSpellName(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellRec spell;

    if (g_spellDB.GetRecord(static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0), &spell)) {
        lua_pushstring(L, spell.m_name);
        lua_pushstring(L, spell.m_nameSubtext);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x00915E90
static int32_t Script_IsPassiveSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellRec spell;

    if (g_spellDB.GetRecord(static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0), &spell) && (spell.m_attributes & 0x40)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00915F60
static int32_t Script_IsAttackSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellRec spell;

    if (g_spellDB.GetRecord(static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0), &spell) && spell.m_effect[0] == 78) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009160A0
static int32_t Script_IsAutoRepeatSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    int32_t spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);

    if (spellId && !petSpell && (Spell_C_GetAutoRepeatingSpell() == spellId || Spell_C_GetAutoRangedCombatSpell() == spellId)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00916490
static int32_t Script_IsConsumableSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellRec spell;

    if (g_spellDB.GetRecord(static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0), &spell)) {
        for (uint32_t i = 0; i < 8; i++) {
            if (spell.m_reagent[i] && spell.m_reagentCount[i] > 0) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0090F760/0x14066AD20
static int32_t Script_GetNumCompanions(lua_State* L) {
    int32_t type;

    if (!GetLuaCompanionType(L, &type)) {
        luaL_error(L, "Usage: GetNumCompanions(type)");
        return 0;
    }

    lua_pushnumber(L, CGSpellBook::m_companions[type].Count());
    return 1;
}

// OFFSET: 0x009116F0/0x14066D680
static int32_t Script_IsSpellKnown(lua_State* L) {
    int32_t spellId;

    if (!lua_isnumber(L, 1) || (spellId = lua_tointeger(L, 1)) <= 0) {
        luaL_error(L, "Usage: IsSpellKnown(spellID[, isPet])");
        return 0;
    }

    int32_t isPet = StringToBOOL(L, 2, 0);
    bool known;

    if (!(GetSpellSlot(spellId, isPet) & 0x80000000)) {
        known = true;
    } else if (isPet) {
        known = CGPetInfo::FindSpellByID(spellId) != NULL;
    } else {
        known = false;
    }

    lua_pushboolean(L, known);
    return 1;
}

// OFFSET: 0x00915720
static int32_t Script_GetSpellTexture(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    const char* texture = CGSpellBook::GetSpellTexture(slot, petSpell);

    if (texture && *texture) {
        lua_pushstring(L, texture);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: #0x005412C0
static int32_t Script_IsSelectedSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    if (CGSpellBook::IsSelectedSlot(slot, petSpell)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00916330
static int32_t Script_IsHelpfulSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellRec spell;

    if (g_spellDB.GetRecord(static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0), &spell) && SpellRec::IsHelpfulOrHarmful(&spell) == 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009163E0
static int32_t Script_IsHarmfulSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellRec spell;

    if (g_spellDB.GetRecord(static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0), &spell) && SpellRec::IsHelpfulOrHarmful(&spell) == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009157D0
static int32_t Script_GetSpellCount(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    lua_pushnumber(L, Spell_C_GetSpellReagentUses(static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0)));
    return 1;
}

// OFFSET: 0x00915870
static int32_t Script_GetSpellCooldown(lua_State* L) {
    int32_t petSpell = 0;
    int32_t spellId;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));

        if (GetSpellSlot(spellId, 0) >= 0) {
            petSpell = 0;
        } else if (GetSpellSlot(spellId, 1) >= 0 || CGPetInfo::FindSpellByID(spellId)) {
            petSpell = 1;
        }
    } else {
        uint32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(spellId, petSpell, &duration, &start, &enable);
    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// OFFSET: 0x00915B70
static int32_t Script_GetSpellAutocast(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    PetAction* action = NULL;
    int32_t spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);

    if (petSpell && (action = CGPetInfo::GetPetSpellAction(spellId)) && (action->m_data & 0x80000000)) {
        lua_pushnumber(L, 1.0);
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

// OFFSET: 0x00915C30
static int32_t Script_ToggleSpellAutocast(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1) && petSpell && slot < 1024 && s_petSpellList[slot]) {
        CGPetInfo::SetSpellAutocast(s_petSpellList[slot], -1);
    }

    return 0;
}

// OFFSET: 0x00915C90
static int32_t Script_EnableSpellAutocast(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1) && petSpell && slot < 1024 && s_petSpellList[slot]) {
        CGPetInfo::SetSpellAutocast(s_petSpellList[slot], 1);
    }

    return 0;
}

// OFFSET: 0x00915CF0
static int32_t Script_DisableSpellAutocast(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1) && petSpell && slot < 1024 && s_petSpellList[slot]) {
        CGPetInfo::SetSpellAutocast(s_petSpellList[slot], 0);
    }

    return 0;
}

// OFFSET: 0x00912F90/0x14066F690
static int32_t Script_PickupSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        CGSpellBook::PickupSpell(slot, petSpell);
    }

    return 0;
}

// OFFSET: 0x00915DA0
static int32_t Script_CastSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 0)) {
        WOWGUID target = CGGameUI::m_lockedTarget;

        if (StringToBOOL(L, 3, 0)) {
            target = ClntObjMgrGetActivePlayer();
        }

        CGSpellBook::CastSpell(slot, petSpell, target);
    }

    return 0;
}

// OFFSET: 0x00915FF0
static int32_t Script_IsCurrentSpell(lua_State* L) {
    int32_t spellId;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = lua_tointeger(L, 1);
    } else {
        uint32_t slot = 0;
        int32_t petSpell;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);
    }

    if (Spell_C_IsCurrentSpell(spellId)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00916130
static int32_t Script_IsUsableSpell(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    bool noMana = false;
    SpellRec spell;
    int32_t spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);
    CGUnit_C* caster;

    if (g_spellDB.GetRecord(spellId, &spell)
        && (caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : static_cast<CGUnit_C*>(ClntObjMgrGetActivePlayerObj()))
        && Spell_C_IsUsableAction(caster, &spell, WOWGUID::Make(0), NULL, false)
        && !(noMana = !Spell_C_HaveSpellPower(caster, &spell))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (noMana) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x009165A0
static int32_t Script_SpellHasRange(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellRec spell;
    int32_t spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);
    CGUnit_C* caster;
    float minRange;
    float maxRange;

    if (g_spellDB.GetRecord(spellId, &spell)
        && (caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : static_cast<CGUnit_C*>(ClntObjMgrGetActivePlayerObj()))) {
        Spell_C_GetMinMaxRange(caster, &spell, &minRange, &maxRange, NULL);

        if (fnotequal(minRange, 0.0f) || fnotequal(maxRange, 0.0f)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009166D0/0x140675ED0
static int32_t Script_IsSpellInRange(lua_State* L) {
    uint32_t slot = 0;
    int32_t petSpell;
    int32_t unitIndex;

    if (!GetSlotFromLua(L, &slot, &petSpell, &unitIndex, 1)) {
        return 0;
    }

    SpellRec spell;
    int32_t spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);
    CGUnit_C* caster;
    WOWGUID target;
    uint8_t inRange;

    if (g_spellDB.GetRecord(spellId, &spell)
        && (caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : static_cast<CGUnit_C*>(ClntObjMgrGetActivePlayerObj()))
        && Script_GetGUIDFromToken(lua_tolstring(L, unitIndex, NULL), target, true)
        && Spell_C_RangeCheckSelected(caster, &spell, target, &inRange, false, NULL)) {
        lua_pushnumber(L, inRange ? 1.0 : 0.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0090F5B0/0x14066AB80
static int32_t Script_CastShapeshiftForm(lua_State* L) {
    if (lua_tonumber(L, 1) == 0.0) {
        luaL_error(L, "Usage: CastShapeshiftForm(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < s_shapeshiftSpells.m_count) {
        Spell_C_ClickSpell(s_shapeshiftSpells.m_data[index].m_spellId, NULL, WOWGUID::Make(0));
    }

    return 0;
}

// OFFSET: 0x0090F640/0x14066AC00
static int32_t Script_GetShapeshiftFormCooldown(lua_State* L) {
    if (lua_tonumber(L, 1) == 0.0) {
        luaL_error(L, "Usage: GetShapeshiftFormCooldown(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= s_shapeshiftSpells.m_count) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
        return 3;
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(s_shapeshiftSpells.m_data[index].m_spellId, 0, &duration, &start, &enable);
    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// OFFSET: 0x00913010/0x14066F710
static int32_t Script_PickupCompanion(lua_State* L) {
    int32_t type;

    if (!GetLuaCompanionType(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: PickupCompanion(type, index)");
        return 0;
    }

    CGSpellBook::PickupCompanion(type, static_cast<uint32_t>(lua_tonumber(L, 2)) - 1);
    return 0;
}

// PROVEN: 981c6d15 IDENTICAL/NONE
// OFFSET: 0x00910D00/0x14066C9A0
static int32_t Script_GetShapeshiftForm(lua_State* L) {
    lua_pushnumber(L, CGSpellBook::GetShapeshiftForm(StringToBOOL(L, 1, 0)));
    return 1;
}

// OFFSET: 0x00910DC0/0x14066CA50
static int32_t Script_CancelShapeshiftForm(lua_State* L) {
    CGUnit_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->CancelShapeshift();
    }

    return 0;
}

// OFFSET: 0x00910DF0/0x14066CA90
static int32_t Script_GetShapeshiftFormInfo(lua_State* L) {
    if (lua_tonumber(L, 1) == 0.0) {
        luaL_error(L, "Usage: GetShapeshiftFormInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGUnit_C* player = ClntObjMgrGetActivePlayerObj();

    if (index >= s_shapeshiftSpells.m_count || !player) {
        return 0;
    }

    SpellRec spell;
    SpellUsableInfo* info = &s_shapeshiftSpells.m_data[index];

    if (!g_spellDB.GetRecord(info->m_spellId, &spell)) {
        return 0;
    }

    bool active = Spell_C_IsActiveSpell(&spell, player);
    bool castable;
    int32_t iconId;

    if (!active && (!info->m_usable || info->m_noMana)) {
        castable = false;
        iconId = spell.m_spellIconID;
    } else {
        castable = true;
        iconId = active && spell.m_activeIconID ? spell.m_activeIconID : spell.m_spellIconID;
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(iconId);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, spell.m_name);

    if (active) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (castable) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 4;
}

// OFFSET: 0x00911410/0x14066D2D0
static int32_t Script_GetCompanionCooldown(lua_State* L) {
    int32_t type;

    if (!GetLuaCompanionType(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: GetCompanionCooldown(type, index)");
        return 0;
    }

    if (!ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (index >= CGSpellBook::m_companions[type].m_count) {
        return 0;
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(CGSpellBook::m_companions[type].m_data[index], 0, &duration, &start, &enable);
    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// OFFSET: 0x00911560/0x14066D440
static int32_t Script_CallCompanion(lua_State* L) {
    int32_t type;

    if (!GetLuaCompanionType(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: CallCompanion(type, index)");
        return 0;
    }

    if (!ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (index >= CGSpellBook::m_companions[type].m_count) {
        return 0;
    }

    Spell_C::CastSpell(CGSpellBook::m_companions[type].m_data[index], NULL, WOWGUID::Make(0), false);
    return 0;
}

// OFFSET: 0x00911630/0x14066D550
static int32_t Script_DismissCompanion(lua_State* L) {
    int32_t type;

    if (!GetLuaCompanionType(L, &type)) {
        luaL_error(L, "Usage: DismissCompanion(type)");
        return 0;
    }

    CGUnit_C* player = ClntObjMgrGetActivePlayerObj();

    for (uint32_t i = 0; i < CGSpellBook::m_companions[type].m_count; i++) {
        SpellRec spell;

        if (g_spellDB.GetRecord(CGSpellBook::m_companions[type].m_data[i], &spell) && Spell_C_IsActiveSpell(&spell, player)) {
            Spell_C_CancelAura(&spell, player, -1);
        }
    }

    return 0;
}

// OFFSET: 0x00911780/0x14066D700
static int32_t Script_SummonRandomCritter(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->CastRandomCritter();
    }

    return 1;
}

// OFFSET: 0x00910F90/0x14066CCA0
static int32_t Script_CastSpellByID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CastSpellByID(spellID[, target])");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    WOWGUID target;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true)) {
        return 0;
    }

    int32_t spellId = lua_tointeger(L, 1);

    if (!spellId) {
        return 0;
    }

    if (GetSpellSlot(spellId, 0) >= 0) {
        Spell_C::CastSpell(spellId, NULL, target, false);
        return 0;
    }

    if (!CGPetInfo::PetActionsUsable(false)) {
        return 0;
    }

    PetAction petAction;
    petAction.m_data = 0;
    PetAction* action = CGPetInfo::FindSpellByID(spellId);

    if (!action) {
        if (GetSpellSlot(spellId, 1) < 0) {
            return 0;
        }

        petAction.m_data = spellId | 0x1000000;
        action = &petAction;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        return 0;
    }

    SpellRec spell;
    g_spellDB.GetRecord(spellId, &spell);

    if (!(spell.m_attributesExD & 0x20) && player->GetPossessedUnit() != pet) {
        Spell_C_StartSpellCooldown(&spell, pet);

        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
        msg.Put(CGPetInfo::GetPet(0).m_guid);
        msg.Put(action->m_data);
        msg.Put(target.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        return 0;
    }

    Spell_C_CastSpell(pet, spell.m_id, NULL, target, NULL, false);
    return 0;
}

// OFFSET: 0x00913AE0/0x140670630
static int32_t Script_CastSpellByName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: CastSpellByName(name[, target])");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    WOWGUID target;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true)) {
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    int32_t petSpell;
    int32_t spellId = GetSpellIdByName(name, &petSpell);

    if (!spellId) {
        return 0;
    }

    if (*name != '!') {
        int32_t slot = GetSpellSlot(spellId, petSpell);

        if (slot >= 0) {
            CGSpellBook::CastSpell(slot, petSpell, target);
            return 0;
        }

        PetAction* action = CGPetInfo::FindSpellByID(spellId);

        if (action) {
            CGPetInfo::SendPetAction(*action, target, 0);
            return 0;
        }

        if (!petSpell) {
            Spell_C_ClickSpell(spellId, NULL, target);
        }

        return 0;
    }

    if (!petSpell) {
        Spell_C::CastSpell(spellId, NULL, target, false);
        return 0;
    }

    if (!CGPetInfo::PetActionsUsable(false)) {
        return 0;
    }

    PetAction petAction;
    petAction.m_data = 0;
    PetAction* action = CGPetInfo::FindSpellByID(spellId);

    if (!action) {
        if (GetSpellSlot(spellId, petSpell) < 0) {
            return 0;
        }

        petAction.m_data = spellId | 0x1000000;
        action = &petAction;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        return 0;
    }

    SpellRec spell;
    g_spellDB.GetRecord(spellId, &spell);

    if ((spell.m_attributesExD & 0x20) || player->GetPossessedUnit() == pet) {
        Spell_C_CastSpell(pet, spell.m_id, NULL, target, NULL, false);
        return 0;
    }

    Spell_C_StartSpellCooldown(&spell, pet);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(action->m_data);
    msg.Put(target.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00915260
static int32_t Script_GetSpellLink(lua_State* L) {
    int32_t spellId;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        uint32_t slot = 0;
        int32_t petSpell;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);
    }

    SpellRec spell;
    char link[256];

    if (!g_spellDB.GetRecord(spellId, &spell) || ((spell.m_attributes & 0x80) && (spell.m_attributes & 0x100))) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    lua_pushstring(L, CGTooltip::GetSpellLink(spellId));

    if (!CGTradeSkillInfo::GetTradeSkillListLink(spellId, link, sizeof(link))) {
        lua_pushnil(L);
        return 2;
    }

    lua_pushstring(L, link);
    return 2;
}

// OFFSET: 0x00915380/0x140672960
static int32_t Script_GetSpellInfo(lua_State* L) {
    int32_t petSpell = 0;
    int32_t spellId;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        uint32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = static_cast<int32_t>(slot) < 0 ? 0 : petSpell ? (slot < 1024 ? s_petSpellList[slot] : 0) : (slot < s_numSpells ? s_spellList[slot] : 0);
    }

    SpellRec spell;

    if (!g_spellDB.GetRecord(spellId, &spell)) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    CGUnit_C* caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : player;

    lua_pushstring(L, spell.m_name);
    lua_pushstring(L, spell.m_nameSubtext);

    const char* texture = "";

    if (spell.m_effect[0] == 78) {
        texture = CGActionBar::GetAttackTexture();
    } else if ((spell.m_attributes & 0x2) && (spell.m_attributesExB & 0x20)) {
        texture = CGActionBar::GetRangedTexture();
    } else {
        SpellIconRec* icon = g_spellIconDB.GetRecord(spell.m_spellIconID);

        if (icon) {
            texture = icon->m_textureFilename;
        }
    }

    lua_pushstring(L, texture);

    uint32_t powerCost = Spell_C_GetPowerCost(&spell, caster);
    uint32_t powerCostPerSecond = Spell_C_GetPowerCostPerSecond(&spell, caster);
    uint32_t cost = powerCost / Spell_C_GetPowerDisplayMod(spell.m_powerType);
    int32_t costPerSecond = powerCostPerSecond / Spell_C_GetPowerDisplayMod(spell.m_powerType);

    if (costPerSecond <= 0) {
        lua_pushnumber(L, cost);
        lua_pushboolean(L, 0);
    } else {
        lua_pushnumber(L, costPerSecond);
        lua_pushboolean(L, 1);
    }

    lua_pushnumber(L, spell.m_powerType);

    if (spell.m_attributes & 0x40) {
        lua_pushnumber(L, 0.0);
    } else {
        lua_pushnumber(L, static_cast<uint32_t>(SpellRec::GetCastTime(&spell, petSpell, 0, 1)));
    }

    if (!player || !caster) {
        return 7;
    }

    if (SpellRec::IsMeleeRange(&spell)) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 9;
    }

    float minRange = 0.0f;
    float maxRange = 0.0f;
    Spell_C_GetMinMaxRange(caster, &spell, &minRange, &maxRange, NULL);

    if (SpellRec::IsMinimumRange(&spell)) {
        SpellRec::GetMinRange(&spell, &minRange);
    }

    lua_pushnumber(L, minRange);
    lua_pushnumber(L, maxRange);
    return 9;
}

// STATIC: #0x00ACCCE0
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumSpellTabs", &Script_GetNumSpellTabs },
    { "GetSpellTabInfo", &Script_GetSpellTabInfo },
    { "GetSpellName", &Script_GetSpellName },
    { "GetSpellLink", &Script_GetSpellLink },
    { "GetSpellInfo", &Script_GetSpellInfo },
    { "GetSpellTexture", &Script_GetSpellTexture },
    { "GetSpellCount", &Script_GetSpellCount },
    { "GetSpellCooldown", &Script_GetSpellCooldown },
    { "GetSpellAutocast", &Script_GetSpellAutocast },
    { "ToggleSpellAutocast", &Script_ToggleSpellAutocast },
    { "EnableSpellAutocast", &Script_EnableSpellAutocast },
    { "DisableSpellAutocast", &Script_DisableSpellAutocast },
    { "PickupSpell", &Script_PickupSpell },
    { "CastSpell", &Script_CastSpell },
    { "IsSelectedSpell", &Script_IsSelectedSpell },
    { "IsPassiveSpell", &Script_IsPassiveSpell },
    { "IsAttackSpell", &Script_IsAttackSpell },
    { "IsCurrentSpell", &Script_IsCurrentSpell },
    { "IsAutoRepeatSpell", &Script_IsAutoRepeatSpell },
    { "IsUsableSpell", &Script_IsUsableSpell },
    { "IsHelpfulSpell", &Script_IsHelpfulSpell },
    { "IsHarmfulSpell", &Script_IsHarmfulSpell },
    { "IsConsumableSpell", &Script_IsConsumableSpell },
    { "SpellHasRange", &Script_SpellHasRange },
    { "IsSpellInRange", &Script_IsSpellInRange },
    { "UpdateSpells", &Script_UpdateSpells },
    { "HasPetSpells", &Script_HasPetSpells },
    { "GetNumShapeshiftForms", &Script_GetNumShapeshiftForms },
    { "GetShapeshiftForm", &Script_GetShapeshiftForm },
    { "CancelShapeshiftForm", &Script_CancelShapeshiftForm },
    { "GetShapeshiftFormInfo", &Script_GetShapeshiftFormInfo },
    { "CastShapeshiftForm", &Script_CastShapeshiftForm },
    { "GetShapeshiftFormCooldown", &Script_GetShapeshiftFormCooldown },
    { "CastSpellByName", &Script_CastSpellByName },
    { "CastSpellByID", &Script_CastSpellByID },
    { "GetNumCompanions", &Script_GetNumCompanions },
    { "GetCompanionInfo", &Script_GetCompanionInfo },
    { "GetCompanionCooldown", &Script_GetCompanionCooldown },
    { "PickupCompanion", &Script_PickupCompanion },
    { "CallCompanion", &Script_CallCompanion },
    { "DismissCompanion", &Script_DismissCompanion },
    { "GetKnownSlotFromHighestRankSlot", &Script_GetKnownSlotFromHighestRankSlot },
    { "IsSpellKnown", &Script_IsSpellKnown },
    { "FindSpellBookSlotByID", &Script_FindSpellBookSlotByID },
    { "SummonRandomCritter", &Script_SummonRandomCritter },
};

// OFFSET: #0x0053B740
void SpellBookRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: #0x0053B770
void SpellBookUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
