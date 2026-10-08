#include "CharacterSelection.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "WoW/Source/Glue/CharacterSelectionDisplay.h"
#include "WoW/Source/Glue/Types.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// PROVEN: 4988e952 IDENTICAL/NONE
// OFFSET: 0x008087D0/0x14051E300
static int32_t Script_SetCharSelectModelFrame(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCharSelectModelFrame(\"frameName\")");
        return 0;
    }

    CSimpleModelFFX* frame = static_cast<CSimpleModelFFX*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 1, NULL), CSimpleModel::GetObjectType()));

    if (frame) {
        CCharacterSelection::m_modelFrame = frame;
    }

    return 0;
}

// PROVEN: 01149b5d IDENTICAL/NONE
// OFFSET: 0x00808840/0x14051E390
static int32_t Script_UpdateSelectionCustomizationScene(lua_State* L) {
    CCharacterSelection::RenderPrep();
    return 0;
}

// PROVEN: 98a3238d IDENTICAL/NONE
// OFFSET: 0x00808850/0x14051E3A0
static int32_t Script_GetCharacterSelectFacing(lua_State* L) {
    lua_pushnumber(L, CCharacterSelection::m_charFacing * 57.29578f);
    return 1;
}

// OFFSET: 0x00808880/0x14051E3D0
static int32_t Script_SetCharacterSelectFacing(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetCharacterSelectFacing(degrees)");
        return 0;
    }

    CharacterSelectionDisplay::SetCharacterFacing(static_cast<float>(lua_tonumber(L, 1) * 0.017453292f));
    return 1;
}

// PROVEN: 900363a8 IDENTICAL/NONE
// OFFSET: 0x00808E00/0x14051EA40
static int32_t Script_GetNumCharacters(lua_State* L) {
    lua_pushnumber(L, static_cast<int32_t>(CCharacterSelection::m_characterList.Count()));
    return 1;
}

// OFFSET: 0x00808E40/0x14051EA60
static int32_t Script_GetCharacterInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCharacterInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 10;
    }

    CharacterSelectionDisplay* display = &CCharacterSelection::m_characterList.m_data[index];

    lua_pushstring(L, display->m_characterInfo.m_name);

    const char* raceName = CGUnit_C::GetDisplayRaceNameFromRecord(g_chrRacesDB.GetRecord(display->m_characterInfo.m_raceID), display->m_characterInfo.m_sexID, NULL);
    lua_pushstring(L, raceName ? raceName : "");

    const char* className = CGUnit_C::GetDisplayClassNameFromRecord(g_chrClassesDB.GetRecord(display->m_characterInfo.m_classID), display->m_characterInfo.m_sexID, NULL);
    lua_pushstring(L, className ? className : "");

    lua_pushnumber(L, display->m_characterInfo.m_experienceLevel);

    AreaTableRec* areaRec = g_areaTableDB.GetRecord(display->m_characterInfo.m_zoneID);

    if (areaRec) {
        lua_pushstring(L, areaRec->m_areaName);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, g_scriptSexIndex[display->m_characterInfo.m_sexID]);
    lua_pushboolean(L, display->m_characterInfo.m_flags & 0x2000);
    lua_pushboolean(L, display->m_characterInfo.m_customizeFlags & 0x1);
    lua_pushboolean(L, display->m_characterInfo.m_customizeFlags & 0x100000);
    lua_pushboolean(L, display->m_characterInfo.m_customizeFlags & 0x10000);

    int32_t mapFlag = 0;
    MapRec* mapRec = g_mapDB.GetRecord(display->m_characterInfo.m_mapID);

    if (mapRec && (mapRec->m_flags & 0x40000)) {
        mapFlag = 1;
    }

    lua_pushboolean(L, mapFlag);
    return 11;
}

// OFFSET: 0x008090B0/0x14051ED30
static int32_t Script_RenameCharacter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: RenameCharacter(index, name)");
        return 0;
    }

    if (!lua_isstring(L, 2)) {
        return 0;
    }

    const char* name = lua_tolstring(L, 2, NULL);

    if (!name || !*name) {
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        return 0;
    }

    CharacterSelectionDisplay* display = GetCharacterInfo(index);

    if (!display || !(display->m_characterInfo.m_flags & 0x4000)) {
        return 0;
    }

    if (!SStrCmpUTF8I(name, display->m_characterInfo.m_name, STORM_MAX_STR)) {
        FrameScript_SignalEvent(GLUE_EVENT_FORCE_RENAME_CHARACTER, "%s", "CHAR_CREATE_NAME_IN_USE");
        return 0;
    }

    if (CGlueMgr::RenameCharacter(GetCharacterInfo(index)->m_characterInfo.m_guid, name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008091D0/0x14051EEA0
static int32_t Script_DeclineCharacter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: DeclineCharacter(index, name1, name2, name3, name4, name5)");
        return 0;
    }

    const char* names[5];

    for (int32_t i = 0; i < 5; i++) {
        names[i] = lua_tolstring(L, i + 2, NULL);

        if (!names[i] || !*names[i]) {
            return 0;
        }
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        return 0;
    }

    CharacterSelectionDisplay* display = &CCharacterSelection::m_characterList.m_data[index];

    if (!display || (display->m_characterInfo.m_flags & 0x2000000)) {
        return 0;
    }

    if (CGlueMgr::DeclineCharacter(display->m_characterInfo.m_guid, names)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008092C0
static int32_t Script_GetSelectBackgroundModel(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetSelectBackgroundModel(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    const char* modelName = NULL;

    if (index >= 0 && index < static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        CharacterSelectionDisplay* display = &CCharacterSelection::m_characterList.m_data[index];

        if (display) {
            if (display->m_characterInfo.m_classID == 6) {
                ChrClassesRec* classRec = g_chrClassesDB.GetRecord(6);

                if (classRec) {
                    modelName = classRec->m_filename;
                }
            } else {
                ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(display->m_characterInfo.m_raceID);

                if (raceRec) {
                    modelName = raceRec->m_clientFileString;
                }
            }
        }
    }

    if (modelName && SFile::IsStreamingMode()) {
        char path[260] = { 0 };
        CharSelectGetBGModelPathFromRaceStr(modelName, path);

        if (!World::GetScene()->PrefetchModel(path, 5)) {
            modelName = NULL;
        }
    }

    if (!modelName) {
        modelName = "CharacterSelect";
    }

    lua_pushstring(L, modelName);
    return 1;
}

// OFFSET: 0x00809400/0x14051F220
static int32_t Script_SaveCharacterOrder(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: SaveCharacterOrder(table)");
        return 0;
    }

    int32_t count = CCharacterSelection::m_characterList.Count();
    int32_t selectionIndex = CCharacterSelection::m_selectionIndex;
    uint32_t* order = new uint32_t[count];
    int32_t i = 0;

    while (i < count) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, -2);

        if (!lua_isnumber(L, -1)) {
            break;
        }

        int32_t index = static_cast<int32_t>(lua_tonumber(L, -1)) - 1;
        lua_settop(L, -2);

        if (index < 0 || index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count()) || !&CCharacterSelection::m_characterList.m_data[index]) {
            break;
        }

        order[i] = index;

        if (index == selectionIndex) {
            g_lastCharacterIndexVar->Set(i, true, false, false, true);
        }

        i++;
    }

    if (i >= count) {
        RealmConnection* connection = ClientServices::Connection();

        if (connection) {
            connection->ReorderCharacters(order, count);
        }
    }

    delete[] order;
    return 0;
}

// PROVEN: 5658550a IDENTICAL/NONE
// OFFSET: 0x00809A90/0x14051F9F0
static int32_t Script_SetCharSelectBackground(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCharSelectBackground(\"filename\")");
        return 0;
    }

    CCharacterSelection::SetBackgroundModel(lua_tolstring(L, 1, NULL));
    return 0;
}

// PROVEN: 9b1f6dca IDENTICAL/NONE
// OFFSET: 0x00809AE0/0x14051FA40
static int32_t Script_DeleteCharacter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: DeleteCharacter(index)");
        return 0;
    }

    CCharacterSelection::DeleteCharacter(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// PROVEN: 211f25f7 IDENTICAL/NONE
// OFFSET: 0x0080A1E0/0x140520400
static int32_t Script_SelectCharacter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectCharacter(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < -1 || index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        index = -1;
    }

    CCharacterSelection::SelectCharacter(index);
    return 0;
}

// PROVEN: 9c190d76 IDENTICAL/NONE
// OFFSET: 0x0080A950/0x140520C70
static int32_t Script_GetCharacterListUpdate(lua_State* L) {
    CCharacterSelection::OnGetCharacterList();
    return 0;
}

// STATIC: 0x00D07950
static FrameScript_Method s_scriptFunctions[] = {
    { "SetCharSelectModelFrame", &Script_SetCharSelectModelFrame },
    { "SetCharSelectBackground", &Script_SetCharSelectBackground },
    { "GetCharacterListUpdate", &Script_GetCharacterListUpdate },
    { "GetNumCharacters", &Script_GetNumCharacters },
    { "GetCharacterInfo", &Script_GetCharacterInfo },
    { "SelectCharacter", &Script_SelectCharacter },
    { "DeleteCharacter", &Script_DeleteCharacter },
    { "RenameCharacter", &Script_RenameCharacter },
    { "DeclineCharacter", &Script_DeclineCharacter },
    { "UpdateSelectionCustomizationScene", &Script_UpdateSelectionCustomizationScene },
    { "GetCharacterSelectFacing", &Script_GetCharacterSelectFacing },
    { "SetCharacterSelectFacing", &Script_SetCharacterSelectFacing },
    { "GetSelectBackgroundModel", &Script_GetSelectBackgroundModel },
    { "SaveCharacterOrder", &Script_SaveCharacterOrder },
};

// PROVEN: e65f2480 IDENTICAL/NONE
// OFFSET: 0x00807F50
void CharSelectRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// PROVEN: b22ea559 IDENTICAL/NONE
// OFFSET: 0x00807F80/0x14051D930
void CharSelectUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
