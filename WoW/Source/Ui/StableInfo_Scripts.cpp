#include "StableInfo.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/ItemPetFoodRec.h"
#include "WoW/Source/DB/Rec/TalentTabRec.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// OFFSET: 0x0096C840/0x1406E35F0
static void StablePetFoodCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_UPDATE, NULL);
    }
}

// OFFSET: 0x0096C860/0x1406E3610
static void StablePaperdollCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PET_STABLE_UPDATE_PAPERDOLL, NULL);
    }
}

// OFFSET: 0x0096CA00/0x1406E3770
static int32_t Script_PickupStablePet(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PickupStablePet(slot)");
        return 0;
    }

    double value = lua_tonumber(L, 1);
    uint32_t index = static_cast<int32_t>(value) - 1;

    if (index < 25) {
        StablePetInfo* pet = &CGStableInfo::m_pets[index];

        if (pet && pet->m_petNumber) {
            CGGameUI::SetCursorPet(static_cast<int32_t>(value) - 1);
        }
    }

    return 0;
}

// OFFSET: 0x0096CA60/0x1406E37E0
static int32_t Script_IsAtStableMaster(lua_State* L) {
    if (CGStableInfo::m_stableMaster) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: 0x0096CAA0/0x1406E3800
static int32_t Script_SetPetSlot(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetPetSlot(index, slot)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1;
    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (index <= 24 && slot <= 24) {
        CGStableInfo::SetPetSlot(index, slot);
    }

    return 0;
}

// OFFSET: 0x0096CF90/0x1406E3E70
static int32_t Script_GetStablePetInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetStablePetInfo(slot)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    StablePetInfo* pet;
    CreatureStats_C* stats;
    CreatureFamilyRec* family;
    WOWGUID requester;

    if (index >= 25 || (pet = &CGStableInfo::m_pets[index]) == NULL || (requester = ClntObjMgrGetActivePlayer(), (stats = g_creatureDBCache.GetRecord(pet->m_entry, requester, StablePetFoodCallback, NULL, true)) == NULL) || (family = g_creatureFamilyDB.GetRecord(stats->m_family)) == NULL) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        return 3;
    }

    lua_pushstring(L, family->m_iconFile);
    lua_pushstring(L, pet->m_name);
    lua_pushnumber(L, pet->m_level);
    lua_pushstring(L, family->m_name);
    int32_t talentType = family->m_petTalentType;

    if (talentType < 0 || !g_talentTabDB.m_numRecords) {
        return 4;
    }

    for (int32_t i = 0; i < g_talentTabDB.m_numRecords; i++) {
        TalentTabRec* tab = g_talentTabDB.GetRecordByIndex(i);

        if ((1 << talentType) & tab->m_petTalentMask) {
            lua_pushstring(L, tab->m_name);
            return 5;
        }
    }

    return 4;
}

// OFFSET: 0x0096D110/0x1406E4060
static int32_t Script_GetStablePetFoodTypes(lua_State* L) {
    int32_t index = 0;
    int32_t count = 0;

    if (lua_isnumber(L, 1)) {
        uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

        if (slot >= 25) {
            return count;
        }

        StablePetInfo* pet = &CGStableInfo::m_pets[slot];

        if (!pet) {
            return count;
        }

        WOWGUID requester = ClntObjMgrGetActivePlayer();
        CreatureStats_C* stats = g_creatureDBCache.GetRecord(pet->m_entry, requester, StablePetFoodCallback, NULL, true);
        CreatureFamilyRec* family;

        if (!stats || (family = g_creatureFamilyDB.GetRecord(stats->m_family)) == NULL) {
            return count;
        }

        int32_t numFoods = g_itemPetFoodDB.m_numRecords;

        for (; index < numFoods; index++) {
            ItemPetFoodRec* food = g_itemPetFoodDB.GetRecordByIndex(index);

            if (food && ((1 << (food->m_id - 1)) & family->m_petFoodMask)) {
                lua_checkstack(L, 1);
                lua_pushstring(L, food->m_name);
                count++;
            }
        }

        return count;
    }

    luaL_error(L, "Usage: GetStablePetFoodTypes(slot)");
    return 0;
}

// OFFSET: 0x0096D380/0x1406E4390
static int32_t Script_ClosePetStables(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->CancelOpenStable();
    }

    CGStableInfo::CloseStables(false);
    return 0;
}

// OFFSET: 0x0096D3F0/0x1406E4400
static int32_t Script_SetPetStablePaperdoll(lua_State* L) {
    if (lua_type(L, 1) == LUA_TTABLE) {
        lua_rawgeti(L, 1, 0);
        CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!model) {
            luaL_error(L, "SetPetStablePaperdoll(): Couldn't find 'this' in frame object");
            return 0;
        }

        if (!model->IsA(CGCharacterModelBase::GetObjectType())) {
            luaL_error(L, "SetPetStablePaperdoll(): Wrong object type, expected frame");
            return 0;
        }

        if (!lua_isnumber(L, 2)) {
            luaL_error(L, "Usage: SetPetStablePaperdoll(characterModel, slot)");
            return 0;
        }

        StablePetInfo* pet = CGStableInfo::GetPetInfo(static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2))) - 1);

        if (!pet) {
            return 0;
        }

        WOWGUID requester = ClntObjMgrGetActivePlayer();
        CreatureStats_C* stats = g_creatureDBCache.GetRecord(pet->m_entry, requester, StablePaperdollCallback, NULL, false);

        if (!stats) {
            return 0;
        }

        model->SetCreature(stats, ClntObjMgrGetActivePlayerObj());
        return 0;
    }

    luaL_error(L, "Usage: SetPetStablePaperdoll(characterModel, slot)");
    return 0;
}

// STATIC: 0x00D13308
static FrameScript_Method s_ScriptFunctions[] = {
    { "ClosePetStables", &Script_ClosePetStables },
    { "GetStablePetInfo", &Script_GetStablePetInfo },
    { "PickupStablePet", &Script_PickupStablePet },
    { "SetPetStablePaperdoll", &Script_SetPetStablePaperdoll },
    { "GetStablePetFoodTypes", &Script_GetStablePetFoodTypes },
    { "IsAtStableMaster", &Script_IsAtStableMaster },
    { "SetPetSlot", &Script_SetPetSlot },
};

// OFFSET: 0x0096CB30
void StableInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0096CB60
void StableInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
