#include "CharacterModelBase.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// PROVEN: 76393933 IDENTICAL/NONE
// OFFSET: 0x00A02EA0
void CGCharacterModelBase::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGCharacterModelBase::s_metatable);
    CGCharacterModelBase::s_metatable = LUA_NOREF;
}

// PROVEN: 0a99bf25 IDENTICAL/NONE
// OFFSET: 0x00A035F0/0x1407A2830
static int32_t Script_ModelSetUnit(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetUnit(\"unit\")");
        return 0;
    }

    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 2, NULL));

    if (guid) {
        model->RefreshUnit(guid);
    }

    return 0;
}

// OFFSET: 0x00A03670/0x1407A28D0
static int32_t Script_SetRotation(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetRotation(rotation (in radians))");
        return 0;
    }

    bool animate = true;

    if (lua_type(L, 3) != LUA_TNONE) {
        animate = lua_toboolean(L, 3) != 0;
    }

    model->RotateModel(static_cast<float>(lua_tonumber(L, 2)), animate);
    return 0;
}

// PROVEN: d238e798 IDENTICAL/NONE
// OFFSET: 0x00A03710
void CGCharacterModelBase::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGCharacterModelBase::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGCharacterModelBase::RegisterScriptMethods);
}

// OFFSET: 0x00A03F80/0x1407A3410
static int32_t Script_SetCreature(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetCreature(creatureID)");
        return 0;
    }

    int32_t creatureID = lua_tointeger(L, 2);
    WOWGUID requester = WOWGUID::Make(0);
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(creatureID, requester, NULL, NULL, false);

    if (stats) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        model->SetCreature(stats, player);
    }

    return 0;
}

// PROVEN: e4c6fba3 IDENTICAL/NONE
// OFFSET: 0x00A04040/0x1407A34F0
static int32_t Script_SetDisplayInfo(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetDisplayInfo(displayID)");
        return 0;
    }

    int32_t displayID = lua_tointeger(L, 2);

    if (displayID) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
        model->SetDisplayInfo(displayID, player);
    }

    return 0;
}

// PROVEN: 724df049 IDENTICAL/NONE
// OFFSET: 0x00A040D0/0x1407A35A0
static int32_t Script_SetPortraitZoom(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetPortraitZoom(zoom)");
        return 0;
    }

    float zoom = static_cast<float>(lua_tonumber(L, 2));

    if (model->m_portraitZoom != zoom) {
        model->m_portraitZoom = zoom;

        if (model->m_model) {
            model->ConfigureCamera();
        }
    }

    return 0;
}

// OFFSET: 0x00A04170/0x1407A3640
static int32_t Script_SetCamDistanceScale(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetCamDistanceScale(scale)");
        return 0;
    }

    double value = lua_tonumber(L, 2);

    if (value <= 0.0) {
        luaL_error(L, "SetCamDistanceScale: Scale value must be greater than 0");
        return 0;
    }

    float scale = value >= 0.1 ? static_cast<float>(value) : 0.1f;

    if (model->m_camDistanceScale != scale) {
        model->m_camDistanceScale = scale;

        if (model->m_model) {
            model->ConfigureCamera();
        }
    }

    return 0;
}

// OFFSET: 0x00A04240/0x1407A3720
static int32_t Script_RefreshUnit(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (model->m_unitGuid) {
        model->RefreshUnit(model->m_unitGuid);
    }

    if (model->m_model) {
        model->ConfigureCamera();
    }

    return 0;
}

// OFFSET: 0x00A042A0/0x1407A3780
static int32_t Script_RefreshCamera(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));

    if (model->m_model) {
        model->ConfigureCamera();
    }

    return 0;
}

// OFFSET: 0x00A04530/0x1407A3B30
static int32_t Script_SetCustomRace(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t race = player->m_unit->m_unitFieldBytes0[0];

    if (lua_isnumber(L, 2)) {
        int32_t value = lua_tointeger(L, 2);

        if (value != -1) {
            race = value;
        }
    }

    int32_t sex = player->m_unit->m_unitFieldBytes0[2];

    if (lua_isnumber(L, 3)) {
        sex = lua_tointeger(L, 3);
    }

    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(race);

    if (raceRec && !(raceRec->m_flags & 1) && model->m_visible) {
        model->SetCustomRace(race, sex != 0);
    }

    return 0;
}

// OFFSET: 0x00A04800/0x1407A3E30
static int32_t Script_SetBarberShopAlternateForm(lua_State* L) {
    CGCharacterModelBase* model = static_cast<CGCharacterModelBase*>(FrameScript_GetObjectThis(L, CGCharacterModelBase::GetObjectType()));
    model->SetBarberShopAlternateForm();
    return 0;
}

// STATIC: 0x00D17690
static FrameScript_Method s_ScriptMethods[] = {
    { "SetUnit", &Script_ModelSetUnit },
    { "SetCreature", &Script_SetCreature },
    { "SetDisplayInfo", &Script_SetDisplayInfo },
    { "SetPortraitZoom", &Script_SetPortraitZoom },
    { "SetCamDistanceScale", &Script_SetCamDistanceScale },
    { "RefreshUnit", &Script_RefreshUnit },
    { "RefreshCamera", &Script_RefreshCamera },
    { "SetRotation", &Script_SetRotation },
    { "SetBarberShopAlternateForm", &Script_SetBarberShopAlternateForm },
    { "SetCustomRace", &Script_SetCustomRace },
};

// PROVEN: 4c0fb303 IDENTICAL/NONE
// OFFSET: 0x00A02E80
void CGCharacterModelBase::RegisterScriptMethods(lua_State* L) {
    CSimpleModel::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
