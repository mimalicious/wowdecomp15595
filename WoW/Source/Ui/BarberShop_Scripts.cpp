#include "BarberShop.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/DB/Rec/BarberShopStyleRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// PROVEN: fe191053 IDENTICAL/NONE
// OFFSET: 0x008FF630/0x1406570B0
static int32_t Script_CanAlterSkin(lua_State* L) {
    lua_pushboolean(L, CGBarberShop::m_currentSkinColor != 0);
    return 1;
}

// OFFSET: 0x008FF920/0x140657470
static int32_t Script_GetBarberShopStyleInfo(lua_State* L) {
    if (!CGBarberShop::m_barberShopEnabled) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBarberShopInfo(type)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index > 3) {
        return 0;
    }

    if (index == 3 && !CGBarberShop::m_currentSkinColor) {
        return 0;
    }

    int32_t cost;

    if (index == 1) {
        lua_pushnil(L);
        lua_pushnil(L);
        float value = GameFormulaEvaluateTable(0, ClntObjMgrGetActivePlayerObj()->m_unit->m_unitFieldLevel - 1, 0) * 0.5f;
        cost = static_cast<int32_t>(value);
    } else {
        const BarberShopStyleRec* style = CGBarberShop::GetCurrentStyle(index);
        lua_pushstring(L, style->m_displayName);
        lua_pushstring(L, style->m_description);
        float value = GameFormulaEvaluateTable(0, ClntObjMgrGetActivePlayerObj()->m_unit->m_unitFieldLevel - 1, 0) * style->m_costModifier;
        cost = static_cast<int32_t>(value);
    }

    lua_pushinteger(L, cost);

    if (CGBarberShop::IsCurrentStyle(index)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 4;
}

// OFFSET: 0x008FFEE0/0x140657B40
static int32_t Script_GetBarberShopTotalCost(lua_State* L) {
    lua_pushinteger(L, CGBarberShop::GetTotalCost());
    return 1;
}

// OFFSET: 0x00900420/0x140658220
static int32_t Script_CancelBarberShop(lua_State* L) {
    CGBarberShop::DisableBarberShop();
    return 0;
}

// OFFSET: 0x00900410/0x140658210
static int32_t Script_ApplyBarberShopStyle(lua_State* L) {
    CGBarberShop::ApplyAppearanceChange();
    return 0;
}

// OFFSET: 0x00900430/0x140658230
static int32_t Script_BarberShopReset(lua_State* L) {
    CGBarberShop::Reset();
    return 1;
}

// OFFSET: 0x00900AC0/0x140658A70
static int32_t Script_SetNextBarberShopStyle(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetNextBarberShopStyle(type[, backward])");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index <= 3 && (index != 3 || CGBarberShop::m_currentSkinColor)) {
        bool backward = StringToBOOL(L, 2, 0) != 0;
        CGBarberShop::SetNextBarberShopStyle(index, backward);
    }

    return 0;
}

// OFFSET: 0x008FFAA0/0x140657630
static int32_t Script_GetHairCustomization(lua_State* L) {
    lua_pushstring(L, CGPlayer_C::GetHairCustomizationName());
    return 1;
}

// OFFSET: 0x008FFAC0/0x140657660
static int32_t Script_GetFacialHairCustomization(lua_State* L) {
    lua_pushstring(L, CGPlayer_C::GetFacialHairCustomizationName());
    return 1;
}

// OFFSET: 0x008FFF00/0x140657B70
static int32_t Script_SetBarberShopAlternateFormFrame(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetAlternateFormFrame(\"frameName\")");
        return 0;
    }

    CGCharacterModelBase* frame = static_cast<CGCharacterModelBase*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 1, NULL), CGCharacterModelBase::GetObjectType()));

    if (frame) {
        CGBarberShop::m_alternateFormFrame = frame;

        if (CGBarberShop::m_barberShopEnabled) {
            CGBarberShop::SetAlternateForm();
        }
    }

    return 0;
}

// OFFSET: 0x008FFAE0/0x140657690
static int32_t Script_HasAlternateForm(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t race = ClientServices::GetCharacterRace();
    const ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(race);

    if (raceRec && raceRec->m_alteredFormRaceID) {
        lua_pushboolean(L, 1);
        lua_pushboolean(L, race != player->GetCurrentClientRace());
        return 2;
    }

    lua_pushboolean(L, 0);
    lua_pushboolean(L, 0);
    return 2;
}

// STATIC: 0x00D108A8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetBarberShopStyleInfo", &Script_GetBarberShopStyleInfo },
    { "SetNextBarberShopStyle", &Script_SetNextBarberShopStyle },
    { "GetBarberShopTotalCost", &Script_GetBarberShopTotalCost },
    { "ApplyBarberShopStyle", &Script_ApplyBarberShopStyle },
    { "CancelBarberShop", &Script_CancelBarberShop },
    { "GetHairCustomization", &Script_GetHairCustomization },
    { "GetFacialHairCustomization", &Script_GetFacialHairCustomization },
    { "BarberShopReset", &Script_BarberShopReset },
    { "CanAlterSkin", &Script_CanAlterSkin },
    { "SetBarberShopAlternateFormFrame", &Script_SetBarberShopAlternateFormFrame },
    { "HasAlternateForm", &Script_HasAlternateForm },
};

// OFFSET: 0x008FF660
void CGBarberShop::RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x008FF690
void CGBarberShop::UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
