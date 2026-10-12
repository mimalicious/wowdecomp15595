#include "PetInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include <cstdint>
#include <cfloat>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/DB/Rec/ItemPetFoodRec.h"
#include "WoW/Source/DB/Rec/TalentTabRec.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Client.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/SoundEngine/SE3.h"

// OFFSET: 0x00950080/0x1406C03E0
static int32_t Script_IsPetAttackActive(lua_State* L) {
    lua_pushboolean(L, CGPetInfo::m_attackingTarget);
    return 1;
}

// OFFSET: 0x009500A0/0x1406C0400
static int32_t Script_GetPetTimeRemaining(lua_State* L) {
    uint32_t expiration = CGPetInfo::m_expirationTime;

    if (expiration) {
        int32_t remaining = expiration != FrameTime::GetCurTimeMs() ? expiration - FrameTime::GetCurTimeMs() : 0;
        lua_pushnumber(L, remaining);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00950AE0/0x1406C0F40
static int32_t Script_PetCanBeAbandoned(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && pet->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer() && (pet->m_unit->m_unitFieldBytes2[2] & 2)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00950B80/0x1406C0FF0
static int32_t Script_PetCanBeDismissed(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    SummonPropertiesRec* properties;

    if (pet && pet->m_unit->m_unitFieldCreatedby == ClntObjMgrGetActivePlayer() && (properties = pet->GetSummonProperties()) && (properties->m_flags & 0x20)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x00950C20/0x1406C10A0
static int32_t Script_PetCanBeRenamed(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && pet->m_unit->m_unitFieldSummonedby == ClntObjMgrGetActivePlayer() && (pet->m_unit->m_unitFieldBytes2[2] & 1)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00950CC0/0x1406C1150
static int32_t Script_HasPetUI(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && !(pet->m_obj->m_type & TYPEMASK_PLAYER) && pet->m_unit->m_unitFieldPetnumber) {
        lua_pushnumber(L, 1.0);

        if (pet->CanGainXP()) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x00950D80/0x1406C1210
static int32_t Script_GetPetExperience(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet && pet->CanGainXP()) {
        lua_pushnumber(L, pet->m_unit->m_unitFieldPetexperience);
        lua_pushnumber(L, pet->m_unit->m_unitFieldPetnextlevelexp);
        return 2;
    }

    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 2;
}

// OFFSET: 0x00950E60/0x1406C12C0
static int32_t Script_GetPetFoodTypes(lua_State* L) {
    int32_t results = 0;
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet || !pet->CanGainXP()) {
        return 0;
    }

    CreatureFamilyRec* family = g_creatureFamilyDB.GetRecord(pet->GetCreatureFamily());

    if (!family) {
        return 0;
    }

    for (int32_t i = 0; i < g_itemPetFoodDB.m_numRecords; i++) {
        ItemPetFoodRec* food = g_itemPetFoodDB.GetRecordByIndex(i);

        if (food && ((1 << (food->m_id - 1)) & family->m_petFoodMask)) {
            lua_checkstack(L, 1);
            lua_pushstring(L, food->m_name);
            results++;
        }
    }

    return results;
}

// OFFSET: 0x00950F70/0x1406C1420
static int32_t Script_GetPetIcon(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    CreatureFamilyRec* family;

    if (pet && pet->CanGainXP() && (family = g_creatureFamilyDB.GetRecord(pet->GetCreatureFamily()))) {
        lua_pushstring(L, family->m_iconFile);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00951010/0x1406C14F0
static int32_t Script_GetPetTalentTree(lua_State* L) {
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    CreatureFamilyRec* family;

    if (pet && pet->CanGainXP() && (family = g_creatureFamilyDB.GetRecord(pet->GetCreatureFamily())) && family->m_petTalentType >= 0) {
        int32_t talentType = family->m_petTalentType;

        for (uint32_t i = 0; i < static_cast<uint32_t>(g_talentTabDB.m_numRecords); i++) {
            TalentTabRec* tab = g_talentTabDB.GetRecordByIndex(i);

            if (tab->m_petTalentMask && ((1 << talentType) & tab->m_petTalentMask)) {
                lua_pushstring(L, tab->m_name);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00952370/0x1406C2B60
static int32_t Script_PetHasActionBar(lua_State* L) {
    if (s_pets.m_count > 0 && s_pets.m_data[0] && !(CGActionBar::m_tempPageActiveFlags & 1) && CGPetInfo::IsPetBarUsed()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009523D0/0x1406C2BC0
static int32_t Script_GetPetActionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPetActionInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;

    if (!s_pets.m_count || !s_pets.m_data[0] || index >= 10) {
        return 0;
    }

    PetAction* action = &CGPetInfo::m_actions[index];

    if (!action || !action->m_data) {
        return 0;
    }

    bool autoCastAllowed = false;
    bool autoCastEnabled = false;
    int32_t spellId = 0;
    char buffer[64];

    switch ((action->m_data >> 24) & 0x3F) {
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
            SpellRec* spell = g_spellDB.GetRecord(action->m_data & 0xFFFFFF);

            if (spell) {
                lua_pushstring(L, spell->m_name);
                lua_pushstring(L, spell->m_nameSubtext);
                CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
                int32_t iconId;

                if (!Spell_C_IsActiveSpell(spell, pet) || !(iconId = spell->m_activeIconID)) {
                    iconId = spell->m_spellIconID;
                }

                SpellIconRec* icon = g_spellIconDB.GetRecord(iconId);
                autoCastEnabled = (action->m_data & 0x40000000) != 0;
                autoCastAllowed = (action->m_data >> 31) & 1;
                spellId = spell->m_id;

                if (icon) {
                    lua_pushstring(L, icon->m_textureFilename);
                } else {
                    lua_pushnil(L);
                }

                lua_pushnil(L);
            } else {
                lua_pushnil(L);
                lua_pushnil(L);
                lua_pushnil(L);
                lua_pushnil(L);
            }

            break;
        }

        case 6: {
            SStrPrintf(buffer, sizeof(buffer), "PET_MODE_%s", CGPetInfo::GetModeToken(action->m_data & 0xFFFFFF));
            lua_pushstring(L, buffer);
            lua_pushnil(L);
            SStrPrintf(buffer, sizeof(buffer), "PET_%s_TEXTURE", CGPetInfo::GetModeToken(action->m_data & 0xFFFFFF));
            lua_pushstring(L, buffer);
            lua_pushnumber(L, 1.0);
            break;
        }

        case 7: {
            SStrPrintf(buffer, sizeof(buffer), "PET_ACTION_%s", CGPetInfo::GetOrdersToken(action->m_data & 0xFFFFFF));
            lua_pushstring(L, buffer);
            lua_pushnil(L);
            SStrPrintf(buffer, sizeof(buffer), "PET_%s_TEXTURE", CGPetInfo::GetOrdersToken(action->m_data & 0xFFFFFF));
            lua_pushstring(L, buffer);
            lua_pushnumber(L, 1.0);
            break;
        }

        default: {
            break;
        }
    }

    if (CGPetInfo::PetActionIsActive(*action)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (autoCastAllowed) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (autoCastEnabled) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, spellId);
    return 8;
}

// OFFSET: 0x00952700/0x1406C2FD0
static int32_t Script_GetPetActionCooldown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPetActionCooldown(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    PetAction* action = s_pets.m_count && s_pets.m_data[0] && index < 10 ? &CGPetInfo::m_actions[index] : NULL;
    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;

    if (action && action->m_data) {
        uint32_t type = (action->m_data >> 24) & 0x3F;

        if (type == 1 || type - 8 <= 9) {
            Spell_C_GetSpellCooldown(action->m_data & 0xFFFFFF, 1, &duration, &start, &enable, NULL);
        }
    }

    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// OFFSET: 0x00952840/0x1406C3100
static int32_t Script_GetPetActionsUsable(lua_State* L) {
    if (CGPetInfo::PetActionsUsable(false, false)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00952890/0x1406C3150
static int32_t Script_GetPetActionSlotUsable(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPetActionSlotsUsable(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;
    PetAction* action = s_pets.m_count && s_pets.m_data[0] && index < 10 ? &CGPetInfo::m_actions[index] : NULL;

    if (CGPetInfo::PetActionSlotUsable(action)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00952950/0x1406C31F0
static int32_t Script_PetPassiveMode(lua_State* L) {
    PetAction action;
    action.m_data = 0x6000000;
    CGPetInfo::SendPetAction(action, WOWGUID(), 0);
    return 0;
}

// OFFSET: 0x00952980/0x1406C3220
static int32_t Script_PetDefensiveMode(lua_State* L) {
    PetAction action;
    action.m_data = 0x6000001;
    CGPetInfo::SendPetAction(action, WOWGUID(), 0);
    return 0;
}

// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_PetAggressiveMode(lua_State* L) {
    return 0;
}

// OFFSET: 0x009529B0/0x1406C3250
static int32_t Script_PetAssistMode(lua_State* L) {
    PetAction action;
    action.m_data = 0x6000003;
    CGPetInfo::SendPetAction(action, WOWGUID(), 0);
    return 0;
}

// OFFSET: 0x009529E0/0x1406C3280
static int32_t Script_PetWait(lua_State* L) {
    PetAction action;
    action.m_data = 0x7000000;
    CGPetInfo::SendPetAction(action, WOWGUID(), 0);
    return 0;
}

// OFFSET: 0x00952A10/0x1406C32B0
static int32_t Script_PetFollow(lua_State* L) {
    PetAction action;
    action.m_data = 0x7000001;
    CGPetInfo::SendPetAction(action, WOWGUID(), 0);
    return 0;
}

// OFFSET: 0x00952A40/0x1406C32E0
static int32_t Script_PetMoveTo(lua_State* L) {
    PetAction action;
    action.m_data = 0x7000004;
    CGPetInfo::SendPetAction(action, WOWGUID(), 0);
    return 0;
}

// OFFSET: 0x00952A70/0x1406C3310
static int32_t Script_PetAttack(lua_State* L) {
    int32_t useGiven = 0;
    WOWGUID target = WOWGUID();
    const char* token = lua_tolstring(L, 1, NULL);

    if (token && *token) {
        if (!Script_GetGUIDFromToken(token, target, false, false)) {
            int32_t exact = StringToBOOL(L, 2, 0);
            target = CGGameUI::ClosestObjectMatch(token, TYPEMASK_UNIT, 1, exact, FLT_MAX, 1);
        }

        useGiven = 1;
    }

    PetAction action;
    action.m_data = 0x7000002;
    CGPetInfo::SendPetAction(action, target, useGiven);
    return 0;
}

// OFFSET: 0x00952B10/0x1406C33D0
static int32_t Script_PetStopAttack(lua_State* L) {
    CGPetInfo::PetClearTarget();
    return 0;
}

// OFFSET: 0x00952B20/0x1406C33E0
static int32_t Script_PetAbandon(lua_State* L) {
    CGPetInfo::PetAbandon();
    return 0;
}

// OFFSET: 0x00952B30/0x1406C33F0
static int32_t Script_PetDismiss(lua_State* L) {
    CGPetInfo::PetDismiss();
    return 0;
}

// OFFSET: 0x00952B40/0x1406C3400
static int32_t Script_PetRename(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);
    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet) {
        CGGameUI::DisplayError(ERR_NO_PET);
        return 0;
    }

    if (pet->m_unit->m_unitFieldSummonedby != ClntObjMgrGetActivePlayer()) {
        CGGameUI::DisplayError(ERR_NOTYOURPET);
        return 0;
    }

    if (!(pet->m_unit->m_unitFieldBytes2[2] & 0x1)) {
        CGGameUI::DisplayError(ERR_PET_NOT_RENAMEABLE);
        return 0;
    }

    if (!name || !*name) {
        CGGameUI::DisplayNameError(2);
        return 0;
    }

    int32_t error = ValidatePetName(s_locale, name, true, true, true, true, 0);

    if (error) {
        CGGameUI::DisplayNameError(error);
        return 0;
    }

    if (NameNeedsDeclensions(s_locale, name)) {
        char declinedNames[5][96];

        for (uint32_t i = 0; i < 5; i++) {
            const char* declined = lua_tolstring(L, i + 2, NULL);

            if (!declined) {
                FrameScript_SignalEvent(SCRIPT_EVENT_PET_FORCE_NAME_DECLENSION, "%s", name);
                return 0;
            }

            if (!*declined) {
                FrameScript_SignalEvent(SCRIPT_EVENT_PET_FORCE_NAME_DECLENSION, "%s", name);
                return 0;
            }

            SStrCopy(declinedNames[i], declined, 96);
        }

        CGPetInfo::PetRename(name, declinedNames[0]);
        return 0;
    }

    CGPetInfo::PetRename(name, NULL);
    return 0;
}

// OFFSET: 0x00952D10/0x1406C35F0
static int32_t Script_GetPossessInfo(lua_State* L) {
    if (lua_tonumber(L, 1) == 0.0) {
        luaL_error(L, "Usage: GetPossessInfo(index)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && CGPetInfo::m_possessSpell) {
        int32_t index = static_cast<int64_t>(lua_tonumber(L, 1));

        if (index == 1) {
            SpellRec* spell = g_spellDB.GetRecord(CGPetInfo::m_possessSpell);

            if (spell) {
                int32_t iconId = spell->m_activeIconID;

                if (!iconId) {
                    iconId = spell->m_spellIconID;
                }

                SpellIconRec* icon = g_spellIconDB.GetRecord(iconId);

                if (icon) {
                    lua_pushstring(L, icon->m_textureFilename);
                } else {
                    lua_pushnil(L);
                }

                lua_pushstring(L, spell->m_name);
                lua_pushboolean(L, 1);
                return 3;
            }
        } else if (index == 2) {
            SpellIconRec* icon = g_spellIconDB.GetRecord(693);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
            } else {
                lua_pushnil(L);
            }

            bool cancelable = true;
            SpellRec* spell = g_spellDB.GetRecord(CGPetInfo::m_possessSpell);

            if (spell && spell->m_attributes < 0) {
                cancelable = false;
            }

            lua_pushstring(L, spell->m_name);
            lua_pushboolean(L, cancelable);
            return 3;
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

// OFFSET: 0x00952EB0/0x1406C3850
static int32_t Script_IsPetAttackAction(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        int32_t index = lua_tointeger(L, 1);
        lua_pushboolean(L, CGPetInfo::IsAttackAction(index - 1));
        return 1;
    }

    luaL_error(L, "Usage: IsPetAttackAction(index)");
    return 0;
}

// OFFSET: 0x00953C70/0x1406C4920
static int32_t Script_PickupPetAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PickupPetAction(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;

    if (index > 10) {
        luaL_error(L, "Invalid slot in PickupPetAction");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(14)) {
        return 0;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!pet || (pet->m_unit->m_unitFieldFlags & 0x1000000) || !pet->m_unit->m_unitFieldPetnumber || (CGGameUI::m_cursorItemType != 4 && CGGameUI::m_cursorItemType)) {
        return 0;
    }

    PetAction cursor = CGGameUI::m_cursorPetAction;
    CGGameUI::ClearCursor(1, 1);

    if (cursor.m_data) {
        CGPetInfo::PlaceCursorAction(cursor, index);
        return 0;
    }

    PetAction* action = GetPetAction(index);

    if (action) {
        CGGameUI::SetCursorPetAction(action);
        CGPetInfo::PickupAction(index, 1);
    }

    return 0;
}

// OFFSET: 0x00953DA0/0x1406C4B80
static int32_t Script_TogglePetAutocast(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TogglePetAutocast(index)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;

    if (index > 10) {
        luaL_error(L, "Invalid slot in TogglePetAutocast");
        return 0;
    }

    PetAction cursor = CGGameUI::m_cursorPetAction;
    CGGameUI::ClearCursor(1, 1);

    if (cursor.m_data) {
        CGPetInfo::PlaceAction(cursor, index);
    } else {
        CGPetInfo::SetAutocast(index, -1);
    }

    return 0;
}

// OFFSET: 0x00953E60/0x1406C4C40
static int32_t Script_CastPetAction(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CastPetAction(index, target)");
        return 0;
    }

    uint32_t index = static_cast<int64_t>(lua_tonumber(L, 1)) - 1;

    if (index > 10) {
        luaL_error(L, "Invalid slot in CastPetAction");
        return 0;
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));
    PetAction cursor;

    if (pet && !(pet->m_unit->m_unitFieldFlags & 0x1000000) && (cursor = CGGameUI::m_cursorPetAction).m_data) {
        CGGameUI::ClearCursor(1, 1);
        CGPetInfo::PlaceCursorAction(cursor, index);
        return 0;
    }

    if (!s_pets.m_count || !s_pets.m_data[0] || index >= 10) {
        return 0;
    }

    PetAction* action = &CGPetInfo::m_actions[index];

    if (!action) {
        return 0;
    }

    WOWGUID target;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true, false)) {
        CGPetInfo::SendPetAction(*action, target, 0);
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("GAMEABILITYACTIVATE", NULL, &properties, NULL, __FILE__, __LINE__, "Script_CastPetAction");
    }

    return 0;
}

// STATIC: 0x00D12B78/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "PetHasActionBar", &Script_PetHasActionBar },
    { "GetPetActionInfo", &Script_GetPetActionInfo },
    { "GetPetActionCooldown", &Script_GetPetActionCooldown },
    { "GetPetActionsUsable", &Script_GetPetActionsUsable },
    { "GetPetActionSlotUsable", &Script_GetPetActionSlotUsable },
    { "IsPetAttackActive", &Script_IsPetAttackActive },
    { "PickupPetAction", &Script_PickupPetAction },
    { "TogglePetAutocast", &Script_TogglePetAutocast },
    { "CastPetAction", &Script_CastPetAction },
    { "PetPassiveMode", &Script_PetPassiveMode },
    { "PetDefensiveMode", &Script_PetDefensiveMode },
    { "PetAggressiveMode", &Script_PetAggressiveMode },
    { "PetAssistMode", &Script_PetAssistMode },
    { "PetWait", &Script_PetWait },
    { "PetFollow", &Script_PetFollow },
    { "PetMoveTo", &Script_PetMoveTo },
    { "PetAttack", &Script_PetAttack },
    { "PetStopAttack", &Script_PetStopAttack },
    { "PetAbandon", &Script_PetAbandon },
    { "PetDismiss", &Script_PetDismiss },
    { "PetRename", &Script_PetRename },
    { "PetCanBeAbandoned", &Script_PetCanBeAbandoned },
    { "PetCanBeDismissed", &Script_PetCanBeDismissed },
    { "PetCanBeRenamed", &Script_PetCanBeRenamed },
    { "GetPetTimeRemaining", &Script_GetPetTimeRemaining },
    { "HasPetUI", &Script_HasPetUI },
    { "GetPetExperience", &Script_GetPetExperience },
    { "GetPetFoodTypes", &Script_GetPetFoodTypes },
    { "GetPetIcon", &Script_GetPetIcon },
    { "GetPetTalentTree", &Script_GetPetTalentTree },
    { "GetPossessInfo", &Script_GetPossessInfo },
    { "IsPetAttackAction", &Script_IsPetAttackAction },
};

// OFFSET: 0x00950110/0x1406C0470
void PetInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x00950140/0x1406C04C0
void PetInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
