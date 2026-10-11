#include "SpellBookFrame.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"
#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/MountCapabilityRec.h"
#include "WoW/Source/DB/Rec/MountTypeRec.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/GameClientCommands.h"

// STATIC: 0x00BF646C
static const int32_t s_specializationSpells[5] = { 20219, 20222, 28677, 28675, 28672 };

// OFFSET: 0x0090E2E0
static bool GetCompanionTypeFromLua(lua_State* L, int32_t* type) {
    const char* token = lua_tolstring(L, 1, NULL);

    if (!token) {
        return false;
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (!_stricmp(token, s_companionTypes[i])) {
            *type = i;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00914CD0
static bool GetSlotFromLua(lua_State* L, int32_t* slot, int32_t* petSpell, int32_t* type, int32_t allowName) {
    *petSpell = 0;

    if (lua_isnumber(L, 1) && lua_isstring(L, 2)) {
        *slot = static_cast<int32_t>(lua_tonumber(L, 1) - 1.0);

        if (static_cast<uint32_t>(*slot) <= 0x3FF) {
            if (!_stricmp("pet", lua_tolstring(L, 2, NULL))) {
                *petSpell = 1;
            }

            if (type) {
                *type = 3;
            }

            return true;
        }
    } else if (allowName && lua_isstring(L, 1)) {
        WOWGUID target;

        if (!lua_isstring(L, 2) || Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, false, false)) {
            *slot = CGSpellBook::FindSlotByName(lua_tolstring(L, 1, NULL), *petSpell);

            if (type) {
                *type = 2;
            }
        } else {
            *slot = CGSpellBook::FindSlotByName(lua_tolstring(L, 1, NULL), lua_tolstring(L, 2, NULL), *petSpell);

            if (type) {
                *type = 3;
            }
        }

        return *slot >= 0;
    }

    char function[1024];
    luaL_error(L, "%s(): Invalid spell slot", FrameScript_GetCurrentFunction(L, function, sizeof(function)));
    return false;
}

// OFFSET: 0x0090E330/0x1406695B0
static int32_t Script_UnlearnSpecialization(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: UnlearnSpecialization(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1));

    if (index < 5) {
        UserClientUnlearnSpecialization msg;
        msg.m_index = index;
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x0090F180/0x14066A6B0
static int32_t Script_GetNumSpellTabs(lua_State* L) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < s_numSkillInfos; i++) {
        SpellBookSkillInfo* info = s_skillInfos.m_data[i];

        if (info && !info->m_isProfession) {
            count++;
        }
    }

    lua_pushnumber(L, static_cast<double>(count));
    return 1;
}

// OFFSET: 0x0090F1E0/0x14066A720
static int32_t Script_GetSpellTabInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetSpellTabInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    SpellBookSkillInfo* info;

    if (index >= s_numSkillInfos || !(info = s_skillInfos.m_data[index])) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushboolean(L, 0);
        return 5;
    }

    if (info->m_skillLine) {
        const SkillLineRec* skill = g_skillLineDB.GetRecord(info->m_skillLine);

        if (skill) {
            lua_pushstring(L, skill->m_displayName);
            const SpellIconRec* icon = g_spellIconDB.GetRecord(skill->m_spellIconID);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
            } else {
                lua_pushnil(L);
            }
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
        }
    } else {
        lua_pushstring(L, FrameScript_GetText("GENERAL_SPELLS", -1, GENDER_NOT_APPLICABLE));
        lua_pushstring(L, "Interface\\Spellbook\\Spellbook-Icon");
    }

    lua_pushnumber(L, static_cast<double>(CGSpellBook::GetSkillLineOffset(index)));
    lua_pushnumber(L, static_cast<double>(info->m_numVisible + info->m_numFuture + info->m_numFlyouts));
    lua_pushboolean(L, info->m_isGuild);
    return 5;
}

// OFFSET: 0x0090F380/0x14066A920
static int32_t Script_GetSpellBaseCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1) || lua_gettop(L) != 1) {
        luaL_error(L, "Usage: GetSpellBaseCooldown(spellID)");
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(static_cast<int32_t>(lua_tonumber(L, 1)));

    if (!spell || !spell->m_spellCooldownsID) {
        lua_pushnil(L);
        return 1;
    }

    int32_t cooldown = 0;
    const SpellCooldownsRec* cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID);

    if (cooldowns) {
        cooldown = cooldowns->m_categoryRecoveryTime;

        if (cooldowns->m_recoveryTime > cooldown) {
            cooldown = cooldowns->m_recoveryTime;
        }
    }

    lua_pushnumber(L, static_cast<double>(cooldown));
    return 1;
}

// OFFSET: 0x0090F460/0x14066AA20
static int32_t Script_SetSpellbookPetAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSpellbookPetAction(slot, target)");
        return 0;
    }

    int32_t slot = static_cast<int64_t>(lua_tonumber(L, 1));

    if (static_cast<uint32_t>(slot - 1) > 7) {
        luaL_error(L, "Invalid slot in SetSpellbookPetAction");
        return 0;
    }

    static const uint32_t s_actions[8] = { 0x7000002, 0x7000001, 0x7000004, 0x7000000, 0x6000003, 0x6000001, 0x6000000, 0 };
    PetAction action;
    action.m_data = s_actions[slot - 1];
    WOWGUID target;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true, false)) {
        CGPetInfo::SendPetAction(action, target, 0);
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("GAMEABILITYACTIVATE", NULL, &properties, NULL, __FILE__, __LINE__, "Script_SetSpellbookPetAction");
    }

    return 0;
}

// OFFSET: 0x0090F570/0x14066AB50
static int32_t Script_GetNumShapeshiftForms(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(s_shapeshiftSpells.m_count));
    return 1;
}

// OFFSET: 0x0090F5B0/0x14066AB80
static int32_t Script_CastShapeshiftForm(lua_State* L) {
    if (lua_tonumber(L, 1) == 0.0) {
        luaL_error(L, "Usage: CastShapeshiftForm(index)");
        return 0;
    }

    int32_t index = static_cast<int64_t>(lua_tonumber(L, 1));

    if (index - 1 < static_cast<int32_t>(s_shapeshiftSpells.m_count)) {
        Spell_C_ClickSpell(s_shapeshiftSpells.m_data[index - 1].m_spellId, NULL, WOWGUID(), 0, 0);
    }

    return 0;
}

// OFFSET: 0x0090F640/0x14066AC00
static int32_t Script_GetShapeshiftFormCooldown(lua_State* L) {
    if (lua_tonumber(L, 1) == 0.0) {
        luaL_error(L, "Usage: GetShapeshiftFormCooldown(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;

    if (index >= s_shapeshiftSpells.m_count) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 1.0);
        return 3;
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(s_shapeshiftSpells.m_data[index].m_spellId, 0, &duration, &start, &enable, NULL);
    lua_pushnumber(L, static_cast<double>(start) * 0.001);
    lua_pushnumber(L, static_cast<double>(duration) * 0.001);
    lua_pushnumber(L, static_cast<double>(enable));
    return 3;
}

// OFFSET: 0x0090F760/0x14066AD20
static int32_t Script_GetNumCompanions(lua_State* L) {
    int32_t type;

    if (!GetCompanionTypeFromLua(L, &type)) {
        luaL_error(L, "Usage: GetNumCompanions(type)");
        return 0;
    }

    lua_pushnumber(L, static_cast<double>(CGSpellBook::m_companions[type].m_count));
    return 1;
}

// OFFSET: 0x0090F7D0/0x14066ADC0
static int32_t Script_FindSpellBookSlotBySpellID(lua_State* L) {
    int32_t spellId;

    if (!lua_isnumber(L, 1) || (spellId = lua_tointeger(L, 1)) <= 0) {
        luaL_error(L, "Usage: FindSpellBookSlotByID(spellID[, isPet])");
        return 0;
    }

    int32_t slot = CGSpellBook::FindSlotBySpellID(spellId, StringToBOOL(L, 2, 0));

    if (slot >= 0) {
        lua_pushinteger(L, slot + 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0090F850/0x14066AE60
static int32_t Script_GetCallPetSpellInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCallPetSpellInfo(spellID)");
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(lua_tointeger(L, 1));

    if (!spell) {
        return 0;
    }

    uint32_t index = CGStableInfo::FindPetIndexBySpellID(spell->m_id, 0);

    if (index > 4) {
        return 0;
    }

    lua_pushnumber(L, static_cast<double>(index));
    StablePetInfo* pet = CGStableInfo::GetPetInfo(index);

    if (pet) {
        lua_pushstring(L, pet->m_name);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x00910C20/0x14066C8B0
static int32_t Script_HasPetSpells(lua_State* L) {
    if (!s_petSpellBookItems.m_count) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    lua_pushnumber(L, static_cast<double>(s_petSpellBookItems.m_count));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushstring(L, g_chrClassesDB.GetRecord(player->m_unit->m_unitFieldBytes0[1])->m_petNameToken);
    } else {
        lua_pushstring(L, "PET");
    }

    return 2;
}

// PROVEN: 07fe615a IDENTICAL/NONE
// OFFSET: 0x00910D00/0x14066C9A0
static int32_t Script_GetShapeshiftForm(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGSpellBook::GetShapeshiftForm(StringToBOOL(L, 1, 0))));
    return 1;
}

// OFFSET: 0x00910D50/0x14066C9E0
static int32_t Script_GetShapeshiftFormID(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t form;

    if (player && (form = player->GetShapeshiftForm())) {
        lua_pushnumber(L, static_cast<double>(form));
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00910DC0/0x14066CA50
static int32_t Script_CancelShapeshiftForm(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

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

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (index >= s_shapeshiftSpells.m_count || !player) {
        return 0;
    }

    SpellUsableInfo& info = s_shapeshiftSpells.m_data[index];
    const SpellRec* spell = g_spellDB.GetRecord(info.m_spellId);

    if (!spell) {
        return 0;
    }

    bool active = Spell_C_IsActiveSpell(spell, player);
    bool castable = active || (info.m_usable && !info.m_noMana);
    int32_t iconId = active && spell->m_activeIconID ? spell->m_activeIconID : spell->m_spellIconID;
    const SpellIconRec* icon = g_spellIconDB.GetRecord(iconId);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    lua_pushstring(L, spell->m_name);

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

// OFFSET: 0x00910F90/0x14066CCA0
static int32_t Script_CastSpellByID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CastSpellByID(spellID[, target])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    WOWGUID target;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true, false)) {
        return 0;
    }

    int32_t spellId = lua_tointeger(L, 1);

    if (!spellId) {
        return 0;
    }

    if (CGSpellBook::FindSlotBySpellID(spellId, 0) >= 0) {
        lua_pushboolean(L, Spell_C_CastSpell(spellId, NULL, target, false));
        return 1;
    }

    if (!CGPetInfo::PetActionsUsable(false, false)) {
        return 0;
    }

    PetAction fallback;
    PetAction* action = CGPetInfo::FindSpellByID(spellId);

    if (!action) {
        if (CGSpellBook::FindSlotBySpellID(spellId, 1) < 0) {
            return 0;
        }

        fallback.m_data = 0x1000000 | spellId;
        action = &fallback;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 0;
    }

    if ((spell->m_attributesExD & 0x20) || player->GetPossessedUnit() == pet) {
        lua_pushboolean(L, Spell_C_CastSpell(pet, spell->m_id, NULL, target, NULL, false, false, 0, 0));
        return 1;
    }

    Spell_C_StartSpellCooldown(spell, pet, 0, 0);
    CDataStore msg;
    C3Vector position = C3Vector();
    msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(action->m_data);
    msg.Put(target.m_guid);
    msg << position;
    msg.Finalize();
    ClientServices::Send2(&msg);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x009111E0/0x14066D020
static int32_t Script_GetCompanionInfo(lua_State* L) {
    int32_t type;

    if (!GetCompanionTypeFromLua(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: GetCompanionInfo(type, index)");
        return 0;
    }

    TSGrowableArray<int32_t>& companions = CGSpellBook::m_companions[type];
    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 2)) - 1;

    if (index >= companions.m_count) {
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(companions.m_data[index]);

    if (!spell) {
        return 0;
    }

    const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);
    int32_t creature = effect ? effect->m_effectMiscValue : 0;
    lua_pushinteger(L, creature);
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(creature, WOWGUID(), NULL, NULL, false);

    if (stats) {
        lua_pushstring(L, stats->GetName(0));
    } else {
        lua_pushnil(L);
    }

    lua_pushinteger(L, spell->m_id);
    bool active = Spell_C_IsActiveSpell(spell, ClntObjMgrGetActivePlayerObj());
    int32_t iconId = active && spell->m_activeIconID ? spell->m_activeIconID : spell->m_spellIconID;
    const SpellIconRec* icon = g_spellIconDB.GetRecord(iconId);

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

    if (!effect || effect->m_effect != 6 || effect->m_effectAura != 78) {
        return 5;
    }

    const MountTypeRec* mountType = g_mountTypeDB.GetRecord(effect->m_effectMiscValueB);

    if (!mountType) {
        return 5;
    }

    uint32_t flags = 0;

    for (int32_t i = 23; i >= 0; i--) {
        const MountCapabilityRec* capability = g_mountCapabilityDB.GetRecord(mountType->m_capability[i]);

        if (capability) {
            flags |= capability->m_flags;
        }
    }

    lua_pushnumber(L, static_cast<double>(flags));
    return 6;
}

// OFFSET: 0x00911410/0x14066D2D0
static int32_t Script_GetCompanionCooldown(lua_State* L) {
    int32_t type;

    if (!GetCompanionTypeFromLua(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: GetCompanionCooldown(type, index)");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    TSGrowableArray<int32_t>& companions = CGSpellBook::m_companions[type];
    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 2)) - 1;

    if (index >= companions.m_count) {
        return 0;
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(companions.m_data[index], 0, &duration, &start, &enable, NULL);
    lua_pushnumber(L, static_cast<double>(start) * 0.001);
    lua_pushnumber(L, static_cast<double>(duration) * 0.001);
    lua_pushnumber(L, static_cast<double>(enable));
    return 3;
}

// OFFSET: 0x00911560/0x14066D440
static int32_t Script_CallCompanion(lua_State* L) {
    int32_t type;

    if (!GetCompanionTypeFromLua(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: CallCompanion(type, index)");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    TSGrowableArray<int32_t>& companions = CGSpellBook::m_companions[type];
    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 2)) - 1;

    if (index < companions.m_count) {
        Spell_C_CastSpell(companions.m_data[index], NULL, WOWGUID(), false);
    }

    return 0;
}

// OFFSET: 0x00911630/0x14066D550
static int32_t Script_DismissCompanion(lua_State* L) {
    int32_t type;

    if (!GetCompanionTypeFromLua(L, &type)) {
        luaL_error(L, "Usage: DismissCompanion(type)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    TSGrowableArray<int32_t>& companions = CGSpellBook::m_companions[type];

    for (uint32_t i = 0; i < companions.m_count; i++) {
        const SpellRec* spell = g_spellDB.GetRecord(companions.m_data[i]);

        if (spell && Spell_C_IsActiveSpell(spell, player)) {
            Spell_C_CancelAura(spell, player, -1);
        }
    }

    return 0;
}

// OFFSET: 0x009116F0/0x14066D680
static int32_t Script_IsSpellKnown(lua_State* L) {
    int32_t spellId;

    if (!lua_isnumber(L, 1) || (spellId = lua_tointeger(L, 1)) <= 0) {
        luaL_error(L, "Usage: IsSpellKnown(spellID[, isPet])");
        return 0;
    }

    int32_t petSpell = StringToBOOL(L, 2, 0);
    int32_t known;

    if (CGSpellBook::FindSlotBySpellID(spellId, petSpell) >= 0) {
        known = 1;
    } else if (petSpell) {
        known = CGPetInfo::FindSpellByID(spellId) != NULL;
    } else {
        known = 0;
    }

    lua_pushboolean(L, known);
    return 1;
}

// OFFSET: 0x00911780/0x14066D700
static int32_t Script_SummonRandomCritter(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->CastRandomCritter();
    }

    return 1;
}

// OFFSET: 0x009117B0/0x14066D740
static int32_t Script_GetProfessions(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t professions[6] = { 0, 0, 0, 0, 0, 0 };

    if (player) {
        for (uint32_t i = 0; i < s_numSkillInfos; i++) {
            SpellBookSkillInfo* info = s_skillInfos.m_data[i];

            if (!info || !info->m_isProfession) {
                continue;
            }

            const SkillLineRec* skill = g_skillLineDB.GetRecord(info->m_skillLine);

            if (!skill) {
                continue;
            }

            if (skill->m_categoryID == 11) {
                if (professions[0]) {
                    professions[1] = i;
                } else {
                    professions[0] = i;
                }

                continue;
            }

            const SkillRaceClassInfoRec* raceClass = SpellTableLookupInfo(player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[1], info->m_skillLine);

            if (!raceClass) {
                continue;
            }

            switch (raceClass->m_skillTierID) {
                case 23:
                    professions[3] = i;
                    break;

                case 61:
                    professions[4] = i;
                    break;

                case 63:
                    professions[5] = i;
                    break;

                case 224:
                    professions[2] = i;
                    break;

                default:
                    break;
            }
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (professions[i]) {
            lua_pushnumber(L, static_cast<double>(professions[i] + 1));
        } else {
            lua_pushnil(L);
        }
    }

    return 6;
}

// OFFSET: 0x009119F0/0x14066D8E0
static int32_t Script_GetProfessionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetProfessionSkillLine(index)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;

    if (index >= s_numSkillInfos) {
        return 0;
    }

    SpellBookSkillInfo* info = s_skillInfos.m_data[index];

    if (!info || !info->m_isProfession) {
        return 0;
    }

    const SkillLineRec* skill = g_skillLineDB.GetRecord(info->m_skillLine);

    if (!skill) {
        return 0;
    }

    int32_t rank;
    int32_t bonus;
    player->GetExpandedSkillRank(info->m_skillLine, rank, bonus);
    int32_t skillIndex = player->GetSkillIndex(info->m_skillLine);
    uint32_t offset = CGSpellBook::GetSkillLineOffset(index);
    int32_t specialization = -1;
    int32_t specializationIndex = -1;

    for (uint32_t i = 0; i < info->m_count; i++) {
        int32_t slot = offset + i;

        if (slot >= 0 && slot < static_cast<int32_t>(s_spellBookItems.m_count)) {
            SpellBookItem* item = s_spellBookItems.m_data[slot];

            if (item && item->m_type == SPELLBOOK_ITEM_SPELL) {
                for (uint32_t j = 0; j < 5; j++) {
                    if (s_specializationSpells[j] == item->m_id) {
                        specialization = j;
                        specializationIndex = i;
                        break;
                    }
                }
            }
        }

        if (specialization >= 0) {
            break;
        }
    }

    lua_pushstring(L, skill->m_displayName);
    const SpellIconRec* icon = g_spellIconDB.GetRecord(skill->m_spellIconID);

    if (icon) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, static_cast<double>(rank));
    lua_pushnumber(L, static_cast<double>(player->GetSkillMaxRank(skillIndex)));
    lua_pushnumber(L, static_cast<double>(info->m_count));
    lua_pushnumber(L, static_cast<double>(offset));
    lua_pushnumber(L, static_cast<double>(info->m_skillLine));
    lua_pushnumber(L, static_cast<double>(bonus));
    lua_pushnumber(L, static_cast<double>(specialization));
    lua_pushnumber(L, static_cast<double>(specializationIndex + 1));
    return 10;
}

// OFFSET: 0x00911C60/0x14066DBD0
static int32_t Script_GetCurrentLevelSpells(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCurrentLevelSpells(level)");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    int32_t level = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t count = 0;

    for (uint32_t i = 0; i < s_spellBookItems.m_count; i++) {
        SpellBookItem* item = s_spellBookItems.m_data[i];

        if (!item->m_skillLine || item->m_level != level || (item->m_type != SPELLBOOK_ITEM_FUTURESPELL && item->m_type != SPELLBOOK_ITEM_SPELL)) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(item->m_id);

        if (spell && !(spell->m_attributesExH & 0x2000000)) {
            lua_pushnumber(L, static_cast<double>(item->m_id));
            count++;
        }
    }

    return count;
}

// OFFSET: 0x00911D70/0x14066DD20
static int32_t Script_GetCurrentLevelFeatures(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCurrentLevelSpells(level)");
        return 0;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    int32_t level = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t count = 0;

    for (uint32_t i = 0; i < s_spellBookItems.m_count; i++) {
        SpellBookItem* item = s_spellBookItems.m_data[i];

        if (item->m_skillLine || item->m_level != level || (item->m_type != SPELLBOOK_ITEM_FUTURESPELL && item->m_type != SPELLBOOK_ITEM_SPELL)) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(item->m_id);

        if (spell && !(spell->m_attributesExH & 0x2000000)) {
            lua_pushnumber(L, static_cast<double>(item->m_id));
            count++;
        }
    }

    return count;
}

// OFFSET: 0x00912F90/0x14066F690
static int32_t Script_PickupSpell(lua_State* L) {
    if (!lua_isnumber(L, 1) || lua_gettop(L) != 1) {
        return 0;
    }

    int32_t spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    CGPlayer_C* player;

    if (spellId && (player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) && player->LookupAbility(spellId)) {
        CGSpellBook::PickupSpell(spellId);
    }

    return 0;
}

// OFFSET: 0x00913010/0x14066F710
static int32_t Script_PickupCompanion(lua_State* L) {
    int32_t type;

    if (!GetCompanionTypeFromLua(L, &type) || lua_tonumber(L, 2) == 0.0) {
        luaL_error(L, "Usage: PickupCompanion(type, index)");
        return 0;
    }

    CGSpellBook::PickupCompanion(type, static_cast<int64_t>(lua_tonumber(L, 2)) - 1);
    return 0;
}

// OFFSET: 0x00913AE0/0x140670630
static int32_t Script_CastSpellByName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: CastSpellByName(name[, target])");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    WOWGUID target;

    if (!Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true, false)) {
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    int32_t petSpell = 0;
    int32_t spellId = CGSpellBook::FindSpellByName(name, petSpell);

    if (!spellId && !(spellId = CGActionBar::GetOverrideBaseSpellByName(name))) {
        return 0;
    }

    if (*name != '!') {
        int32_t slot = CGSpellBook::FindSlotBySpellID(spellId, petSpell);

        if (slot < 0) {
            int32_t base = CGActionBar::GetOverrideBaseSpell(spellId);

            if (base) {
                slot = CGSpellBook::FindSlotBySpellID(base, petSpell);
            }
        }

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
            Spell_C_ClickSpell(spellId, NULL, target, 0, 0);
        }

        return 0;
    }

    if (!petSpell) {
        int32_t baseSpell = 0;
        int32_t ignoreCost = 0;
        HASHKEY_NONE key;
        OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.Ptr(spellId, key);

        if (node) {
            baseSpell = spellId;
            spellId = node->m_overrideSpellID;
            ignoreCost = (node->m_flags >> 1) & 0x1;
        }

        Spell_C_CastSpell(spellId, NULL, target, false, false, baseSpell, ignoreCost);
        return 0;
    }

    if (!CGPetInfo::PetActionsUsable(false, false)) {
        return 0;
    }

    PetAction fallback;
    PetAction* action = CGPetInfo::FindSpellByID(spellId);

    if (!action) {
        if (CGSpellBook::FindSlotBySpellID(spellId, 1) < 0) {
            return 0;
        }

        fallback.m_data = 0x1000000 | spellId;
        action = &fallback;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 0;
    }

    if ((spell->m_attributesExD & 0x20) || player->GetPossessedUnit() == pet) {
        Spell_C_CastSpell(pet, spell->m_id, NULL, target, NULL, false, false, 0, 0);
        return 0;
    }

    Spell_C_StartSpellCooldown(spell, pet, 0, 0);
    CDataStore msg;
    C3Vector position = C3Vector();
    msg.Put(static_cast<uint32_t>(CMSG_PET_ACTION));
    msg.Put(CGPetInfo::GetPet(0).m_guid);
    msg.Put(action->m_data);
    msg.Put(target.m_guid);
    msg << position;
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x00913DD0/0x1406709F0
static int32_t Script_SetBarSlotFromIntro(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetBarSlotFromIntro(slot)");
        return 0;
    }

    CGSpellBook::SetBarSlotFromIntro(lua_tointeger(L, 1) - 1);
    return 0;
}

// OFFSET: 0x00914E60/0x140671F10
static int32_t Script_GetSpellBookItemName(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);

    if (!item) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    switch (item->m_type) {
        case SPELLBOOK_ITEM_SPELL:
        case SPELLBOOK_ITEM_FUTURESPELL: {
            const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetOverrideSpell(item->m_id));

            if (!spell) {
                break;
            }

            StablePetInfo* pet = CGStableInfo::GetPetInfo(CGStableInfo::FindPetIndexBySpellID(spell->m_id, 0));

            if (pet && *pet->m_name) {
                char name[128];
                SStrPrintf(name, sizeof(name), FrameScript_GetText("CALL_PET_SPELL_NAME", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(pet->m_name));
                lua_pushstring(L, name);
            } else {
                lua_pushstring(L, spell->m_name);
            }

            lua_pushstring(L, spell->m_nameSubtext);
            return 2;
        }

        case SPELLBOOK_ITEM_PETACTION: {
            uint32_t data = item->m_id;

            switch ((data >> 24) & 0x3F) {
                case 1:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17: {
                    const SpellRec* spell = g_spellDB.GetRecord(data & 0xFFFFFF);

                    if (!spell) {
                        break;
                    }

                    lua_pushstring(L, spell->m_name);
                    lua_pushstring(L, spell->m_nameSubtext);
                    return 2;
                }

                case 6: {
                    char token[64];
                    SStrPrintf(token, sizeof(token), "PET_MODE_%s", CGPetInfo::GetModeToken(data & 0xFFFFFF));
                    lua_pushstring(L, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));
                    lua_pushstring(L, FrameScript_GetText("PET_MODE_SUBTEXT", -1, GENDER_NOT_APPLICABLE));
                    return 2;
                }

                case 7: {
                    char token[64];
                    SStrPrintf(token, sizeof(token), "PET_ACTION_%s", CGPetInfo::GetOrdersToken(data & 0xFFFFFF));
                    lua_pushstring(L, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));
                    lua_pushstring(L, FrameScript_GetText("PET_ACTION_SUBTEXT", -1, GENDER_NOT_APPLICABLE));
                    return 2;
                }

                default:
                    break;
            }

            break;
        }

        case SPELLBOOK_ITEM_FLYOUT: {
            const SpellFlyoutRec* flyout = g_spellFlyoutDB.GetRecord(item->m_id);

            if (!flyout) {
                break;
            }

            lua_pushstring(L, flyout->m_name);
            lua_pushnil(L);
            return 2;
        }

        default:
            break;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x00915130
static int32_t Script_GetSpellBookItemInfo(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1) || slot < 0) {
        return 0;
    }

    SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);

    if (!item) {
        return 0;
    }

    switch (item->m_type) {
        case SPELLBOOK_ITEM_SPELL:
            lua_pushstring(L, "SPELL");
            lua_pushinteger(L, item->m_id);
            return 2;

        case SPELLBOOK_ITEM_FUTURESPELL:
            lua_pushstring(L, "FUTURESPELL");
            lua_pushinteger(L, item->m_id);
            return 2;

        case SPELLBOOK_ITEM_PETACTION:
            lua_pushstring(L, "PETACTION");
            return 1;

        case SPELLBOOK_ITEM_FLYOUT:
            lua_pushstring(L, "FLYOUT");
            lua_pushinteger(L, item->m_id);
            return 2;

        default:
            return 0;
    }
}

// OFFSET: 0x00915260
static int32_t Script_GetSpellLink(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || ((spell->m_attributes & 0x80) && (spell->m_attributes & 0x100))) {
        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    lua_pushstring(L, CGTooltip::GetSpellHyperlinkString(spellId));
    char link[256];

    if (CGTradeSkillInfo::GetTradeSkillListLink(spellId, link, sizeof(link))) {
        lua_pushstring(L, link);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x00915380/0x140672960
static int32_t Script_GetSpellInfo(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 0;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGUnit_C* caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : player;
    lua_pushstring(L, spell->m_name);
    lua_pushstring(L, spell->m_nameSubtext);
    lua_pushstring(L, CGSpellBook::GetSpellTexture(spellId));
    int32_t cost = Spell_C_GetPowerCost(spell, caster) / static_cast<int32_t>(Spell_C_GetPowerDisplayMod(spell->m_powerType));
    int32_t costPerSecond = Spell_C_GetPowerCostPerSecond(spell, caster) / static_cast<int32_t>(Spell_C_GetPowerDisplayMod(spell->m_powerType));

    if (costPerSecond <= 0) {
        lua_pushnumber(L, static_cast<double>(cost));
        lua_pushboolean(L, 0);
    } else {
        lua_pushnumber(L, static_cast<double>(costPerSecond));
        lua_pushboolean(L, 1);
    }

    lua_pushnumber(L, static_cast<double>(spell->m_powerType));

    if (spell->m_attributes & 0x40) {
        lua_pushnumber(L, 0.0);
    } else {
        lua_pushnumber(L, static_cast<double>(Spell_C_GetCastTime(spell, petSpell, 0, 1)));
    }

    if (!player || !caster) {
        return 7;
    }

    if (SpellRec::IsMeleeRange(spell)) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 9;
    }

    float minRange = 0.0f;
    float maxRange = 0.0f;
    Spell_C_GetMinMaxRange(caster, spell, &minRange, &maxRange, NULL);

    if (SpellRec::IsMinimumRange(spell)) {
        Spell_C_GetDefaultMinRange(spell, &minRange);
    }

    lua_pushnumber(L, minRange);
    lua_pushnumber(L, maxRange);
    return 9;
}

// OFFSET: 0x009155F0
static int32_t Script_GetSpellDescription(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 0;
    }

    lua_pushstring(L, spell->m_description);
    return 1;
}

// OFFSET: 0x009156A0
static int32_t Script_GetSpellBookItemTexture(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    const char* texture = CGSpellBook::GetSpellBookItemTexture(slot, petSpell);

    if (texture && *texture) {
        lua_pushstring(L, texture);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00915720
static int32_t Script_GetSpellTexture(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    const char* texture = CGSpellBook::GetSpellTexture(spellId);

    if (texture && *texture) {
        lua_pushstring(L, texture);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009157D0
static int32_t Script_GetSpellCount(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    lua_pushnumber(L, static_cast<double>(Spell_C_GetSpellReagentUses(spellId)));
    return 1;
}

// OFFSET: 0x00915870
static int32_t Script_GetSpellCooldown(lua_State* L) {
    int32_t spellId = 0;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));

        if (spellId && CGSpellBook::FindSlotBySpellID(spellId, 0) >= 0) {
            petSpell = 0;
        } else if (spellId && CGSpellBook::FindSlotBySpellID(spellId, 1) >= 0) {
            petSpell = 1;
        } else if (CGPetInfo::FindSpellByID(spellId)) {
            petSpell = 1;
        }
    } else {
        int32_t slot = 0;

        if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);

            if (item && item->m_type == SPELLBOOK_ITEM_SPELL) {
                spellId = item->m_id;
                HASHKEY_NONE key;
                OVERRIDE_SPELL_NODE* node = CGActionBar::m_overrideSpells.Ptr(spellId, key);

                if (node) {
                    spellId = node->m_overrideSpellID;
                }
            }
        } else {
            WOWGUID target;

            if (!lua_isstring(L, 2) || Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, false, false)) {
                spellId = CGSpellBook::FindSpellByName(lua_tolstring(L, 1, NULL), petSpell);
            } else {
                spellId = CGSpellBook::FindSpellByName(lua_tolstring(L, 1, NULL), lua_tolstring(L, 2, NULL), petSpell);
            }

            if (!spellId) {
                const char* name = lua_tolstring(L, 1, NULL);
                OVERRIDE_SPELL_BY_NAME_NODE* node;

                if (!name || !*name || !(node = CGActionBar::m_overrideSpellsByName.Ptr(name)) || !(spellId = node->m_overrideSpellID)) {
                    return 0;
                }
            }
        }
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(spellId, petSpell, &duration, &start, &enable, NULL);
    lua_pushnumber(L, static_cast<double>(start) * 0.001);
    lua_pushnumber(L, static_cast<double>(duration) * 0.001);
    lua_pushnumber(L, static_cast<double>(enable));
    return 3;
}

// OFFSET: 0x00915B00
static int32_t Script_GetSpellAvailableLevel(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    int32_t level = 0;

    if (slot >= 0 && slot < static_cast<int32_t>(s_spellBookItems.m_count) && s_spellBookItems.m_data[slot]->m_type == SPELLBOOK_ITEM_FUTURESPELL) {
        level = s_spellBookItems.m_data[slot]->m_level;
    }

    lua_pushinteger(L, level);
    return 1;
}

// OFFSET: 0x00915B70
static int32_t Script_GetSpellAutocast(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    PetAction* action = NULL;
    int32_t spellId = CGSpellBook::GetSpell(slot, petSpell);

    if (petSpell && (action = CGPetInfo::GetPetSpellAction(spellId)) && static_cast<int32_t>(action->m_data) < 0) {
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
    int32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1) && petSpell) {
        int32_t spellId = CGSpellBook::GetSpell(slot, petSpell);

        if (spellId) {
            CGPetInfo::SetSpellAutocast(spellId, -1);
        }
    }

    return 0;
}

// OFFSET: 0x00915C90
static int32_t Script_EnableSpellAutocast(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1) && petSpell) {
        int32_t spellId = CGSpellBook::GetSpell(slot, petSpell);

        if (spellId) {
            CGPetInfo::SetSpellAutocast(spellId, 1);
        }
    }

    return 0;
}

// OFFSET: 0x00915CF0
static int32_t Script_DisableSpellAutocast(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1) && petSpell) {
        int32_t spellId = CGSpellBook::GetSpell(slot, petSpell);

        if (spellId) {
            CGPetInfo::SetSpellAutocast(spellId, 0);
        }
    }

    return 0;
}

// OFFSET: 0x00915D50
static int32_t Script_PickupSpellBookItem(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        CGSpellBook::PickupSpellBookItem(slot, petSpell);
    }

    return 0;
}

// OFFSET: 0x00915DA0
static int32_t Script_CastSpell(lua_State* L) {
    int32_t slot = 0;
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

// OFFSET: 0x00915E10
static int32_t Script_IsSelectedSpellBookItem(lua_State* L) {
    int32_t slot = 0;
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

// OFFSET: 0x00915E90
static int32_t Script_IsPassiveSpell(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell && (spell->m_attributes & 0x40)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00915F60
static int32_t Script_IsAttackSpell(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    const SpellEffectRec* effect = SpellGetEffect(CGSpellBook::GetSpell(slot, petSpell), 0);

    if (effect && effect->m_effect == 78) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00915FF0
static int32_t Script_IsCurrentSpell(lua_State* L) {
    int32_t spellId;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = lua_tointeger(L, 1);
    } else {
        int32_t slot = 0;
        int32_t petSpell;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    if (Spell_C_IsCurrentSpell(spellId)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009160A0
static int32_t Script_IsAutoRepeatSpell(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    int32_t spellId = CGSpellBook::GetSpell(slot, petSpell);

    if (spellId && !petSpell && (Spell_C_GetAutoRepeatingSpell() == spellId || Spell_C_GetAutoRangedCombatSpell() == spellId)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00916130
static int32_t Script_IsUsableSpell(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));

        if (spellId && CGSpellBook::FindSlotBySpellID(spellId, 0) >= 0) {
            petSpell = 0;
        } else if ((spellId && CGSpellBook::FindSlotBySpellID(spellId, 1) >= 0) || CGPetInfo::FindSpellByID(spellId)) {
            petSpell = 1;
        }
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    bool noMana = false;
    bool usable = false;
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        CGUnit_C* caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (caster && Spell_C_IsUsableAction(caster, spell, WOWGUID(), NULL, false, false, false)) {
            int32_t powerType;
            int32_t requirement;
            usable = Spell_C_HaveSpellPower(caster, spell, powerType, requirement);
            noMana = !usable;
        }
    }

    if (usable) {
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

// OFFSET: 0x00916330
static int32_t Script_IsHelpfulSpell(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetSpell(slot, petSpell));

    if (spell && SpellFaction(spell) == 1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009163E0
static int32_t Script_IsHarmfulSpell(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetSpell(slot, petSpell));

    if (spell && SpellFaction(spell) == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00916490
static int32_t Script_IsConsumableSpell(lua_State* L) {
    int32_t spellId;
    int32_t petSpell = 0;

    if (lua_isnumber(L, 1) && lua_gettop(L) == 1) {
        spellId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        int32_t slot = 0;

        if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
            return 0;
        }

        spellId = CGSpellBook::GetSpell(slot, petSpell);
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);
    const SpellReagentsRec* reagents;

    if (spell && spell->m_spellReagentsID && (reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID))) {
        for (uint32_t i = 0; i < 8; i++) {
            if (reagents->m_reagent[i] && reagents->m_reagentCount[i] > 0) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009165A0
static int32_t Script_SpellHasRange(lua_State* L) {
    int32_t slot = 0;
    int32_t petSpell;

    if (!GetSlotFromLua(L, &slot, &petSpell, NULL, 1)) {
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetSpell(slot, petSpell));
    CGUnit_C* caster;

    if (spell && (caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)))) {
        float minRange;
        float maxRange;
        Spell_C_GetMinMaxRange(caster, spell, &minRange, &maxRange, NULL);

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
    int32_t slot = 0;
    int32_t petSpell;
    int32_t type;

    if (!GetSlotFromLua(L, &slot, &petSpell, &type, 1)) {
        return 0;
    }

    const SpellRec* spell = g_spellDB.GetRecord(CGSpellBook::GetSpell(slot, petSpell));
    CGUnit_C* caster;
    WOWGUID target;
    uint8_t inRange;

    if (spell && (caster = petSpell ? static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__)) : static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) && Script_GetGUIDFromToken(lua_tolstring(L, type, NULL), target, true, false) && Spell_C_RangeCheckSelected(caster, spell, target, &inRange, false, NULL)) {
        lua_pushnumber(L, inRange ? 1.0 : 0.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// STATIC: 0x00D10F08
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumSpellTabs", &Script_GetNumSpellTabs },
    { "GetSpellTabInfo", &Script_GetSpellTabInfo },
    { "GetSpellBookItemName", &Script_GetSpellBookItemName },
    { "GetSpellBookItemInfo", &Script_GetSpellBookItemInfo },
    { "GetSpellLink", &Script_GetSpellLink },
    { "GetSpellInfo", &Script_GetSpellInfo },
    { "GetSpellDescription", &Script_GetSpellDescription },
    { "GetSpellBookItemTexture", &Script_GetSpellBookItemTexture },
    { "GetSpellTexture", &Script_GetSpellTexture },
    { "GetSpellCount", &Script_GetSpellCount },
    { "GetSpellCooldown", &Script_GetSpellCooldown },
    { "GetSpellAvailableLevel", &Script_GetSpellAvailableLevel },
    { "GetSpellAutocast", &Script_GetSpellAutocast },
    { "ToggleSpellAutocast", &Script_ToggleSpellAutocast },
    { "EnableSpellAutocast", &Script_EnableSpellAutocast },
    { "DisableSpellAutocast", &Script_DisableSpellAutocast },
    { "SetSpellbookPetAction", &Script_SetSpellbookPetAction },
    { "PickupSpellBookItem", &Script_PickupSpellBookItem },
    { "PickupSpell", &Script_PickupSpell },
    { "CastSpell", &Script_CastSpell },
    { "IsSelectedSpellBookItem", &Script_IsSelectedSpellBookItem },
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
    { "HasPetSpells", &Script_HasPetSpells },
    { "GetNumShapeshiftForms", &Script_GetNumShapeshiftForms },
    { "GetShapeshiftForm", &Script_GetShapeshiftForm },
    { "GetShapeshiftFormID", &Script_GetShapeshiftFormID },
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
    { "IsSpellKnown", &Script_IsSpellKnown },
    { "FindSpellBookSlotBySpellID", &Script_FindSpellBookSlotBySpellID },
    { "SummonRandomCritter", &Script_SummonRandomCritter },
    { "GetProfessions", &Script_GetProfessions },
    { "GetProfessionInfo", &Script_GetProfessionInfo },
    { "UnlearnSpecialization", &Script_UnlearnSpecialization },
    { "GetCurrentLevelSpells", &Script_GetCurrentLevelSpells },
    { "GetCurrentLevelFeatures", &Script_GetCurrentLevelFeatures },
    { "GetCallPetSpellInfo", &Script_GetCallPetSpellInfo },
    { "SetBarSlotFromIntro", &Script_SetBarSlotFromIntro },
    { "GetSpellBaseCooldown", &Script_GetSpellBaseCooldown },
};

// OFFSET: 0x0090E3B0
void SpellBookRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0090E3E0
void SpellBookUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
