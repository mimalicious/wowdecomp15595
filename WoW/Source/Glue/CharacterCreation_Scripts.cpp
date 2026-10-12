#include "CharacterCreation.h"
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/FactionGroupRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/CharacterSelection.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// INLINED
static const ChrRacesRec* GetSelectedRaceRec() {
    int32_t raceID = CCharacterCreation::m_raceIndex < CCharacterCreation::m_races.Count() ? CCharacterCreation::m_races.m_data[CCharacterCreation::m_raceIndex] : 0;
    return g_chrRacesDB.GetRecord(raceID);
}

// OFFSET: 0x0080C1A0/0x140522810
static int32_t Script_IsRaceClassRestricted(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: Script_IsRaceClassRestricted(raceID, classID)");
        return 0;
    }

    uint32_t raceID = static_cast<uint32_t>(lua_tonumber(L, 1));
    uint32_t classID = static_cast<uint32_t>(lua_tonumber(L, 2));
    int32_t restricted = 0;
    RealmConnection* connection;

    if (classID < 32 && (connection = ClientServices::Connection()) != NULL && (connection->GetFactionChangeRestriction(raceID) & (1 << classID))) {
        restricted = 1;
    }

    lua_pushnumber(L, restricted);
    return 1;
}

// OFFSET: 0x0080C280/0x1405228F0
static int32_t Script_UpdateCustomizationScene(lua_State* L) {
    if (CCharacterCreation::m_character) {
        CCharacterCreation::m_character->RenderPrep(0);
    }

    if (CCharacterCreation::m_alteredCharacter) {
        CCharacterCreation::m_alteredCharacter->RenderPrep(0);
    }

    return 0;
}

// OFFSET: 0x0080C2B0/0x140522930
static int32_t Script_GenerateRandomName(lua_State* L) {
    char name[16];
    size_t length;

    do {
        CCharacterCreation::m_nameGen.GenerateName(name, 14);
        length = strlen(name);
    } while (length && (ClientServices::CharacterValidateName(name) != 89 || length < 4 || length > 10));

    lua_pushstring(L, name);
    return 1;
}

// OFFSET: 0x0080C320/0x1405229B0
static int32_t Script_PaidChange_GetPreviousRaceIndex(lua_State* L) {
    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x0080C340/0x1405229D0
static int32_t Script_HasAlteredForm(lua_State* L) {
    lua_pushboolean(L, CCharacterCreation::m_alteredCharacter != NULL);
    return 1;
}

// OFFSET: 0x0080C370/0x1405229F0
static int32_t Script_IsViewingAlteredForm(lua_State* L) {
    lua_pushboolean(L, CCharacterCreation::m_alteredCharacter ? CCharacterCreation::m_alteredFormAtFront : 0);
    return 1;
}

// OFFSET: 0x0080D350/0x140523BE0
static int32_t Script_SetCharCustomizeFrame(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCharCustomizeFrame(\"frameName\")");
        return 0;
    }

    CSimpleModelFFX* frame = static_cast<CSimpleModelFFX*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 1, NULL), CSimpleModel::GetObjectType()));

    if (frame) {
        CCharacterCreation::m_charCustomizeFrame = frame;
    }

    return 0;
}

// OFFSET: 0x0080D3C0/0x140523C70
static int32_t Script_SetCharCustomizeBackground(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCharCustomizeBackground(\"filename\")");
        return 0;
    }

    CCharacterCreation::SetCharCustomizeModel(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x0080D410/0x140523CC0
static int32_t Script_GetNameForRace(lua_State* L) {
    int32_t raceID = CCharacterCreation::m_character ? CCharacterCreation::m_character->m_data.m_preferences.m_raceID : 0;
    const ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(raceID);
    int32_t sexID = CCharacterCreation::m_character ? CCharacterCreation::m_character->m_data.m_preferences.m_sexID : 0;
    const char* name = CGUnit_C::GetDisplayRaceNameFromRecord(raceRec, sexID, NULL);

    if (raceRec && name) {
        lua_pushstring(L, name);
        lua_pushstring(L, raceRec->m_clientFileString);
        return 2;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x0080D4A0/0x140523D90
static int32_t Script_GetFactionForRace(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetFactionForRace(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t raceID = index < CCharacterCreation::m_races.Count() ? CCharacterCreation::m_races.m_data[index] : 0;
    const ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(raceID);
    const FactionTemplateRec* templateRec = raceRec ? g_factionTemplateDB.GetRecord(raceRec->m_factionID) : NULL;

    if (templateRec) {
        for (uint32_t i = 0; i < static_cast<uint32_t>(g_factionGroupDB.m_numRecords); i++) {
            const FactionGroupRec* groupRec = g_factionGroupDB.GetRecordByIndex(i);

            if (groupRec && groupRec->m_maskID && ((1 << groupRec->m_maskID) & templateRec->m_factionGroup)) {
                lua_pushstring(L, groupRec->m_name);
                lua_pushstring(L, groupRec->m_internalName);
                return 2;
            }
        }
    }

    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
}

// OFFSET: 0x0080D5D0/0x140523F10
static int32_t Script_GetAvailableClasses(lua_State* L) {
    uint32_t count = g_chrClassesDB.m_numRecords;

    for (uint32_t i = 0; i < count; i++) {
        const ChrClassesRec* classRec = g_chrClassesDB.GetRecordByIndex(i);
        const char* name = CGUnit_C::GetDisplayClassNameFromRecord(classRec, CCharacterCreation::GetSelectedSex(), NULL);
        int32_t available = ClientServices::Connection()->m_accountExpansion >= classRec->m_requiredExpansion;

        if (name) {
            lua_pushstring(L, name);
            lua_pushstring(L, classRec->m_filename);
            lua_pushnumber(L, available);
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }

    return 3 * count;
}

// OFFSET: 0x0080D6A0/0x140524010
static int32_t Script_GetHairCustomization(lua_State* L) {
    const ChrRacesRec* raceRec = GetSelectedRaceRec();

    if (raceRec) {
        lua_pushstring(L, raceRec->m_hairCustomization);
        return 1;
    }

    lua_pushstring(L, "NORMAL");
    return 1;
}

// OFFSET: 0x0080D710/0x1405240C0
static int32_t Script_GetFacialHairCustomization(lua_State* L) {
    const ChrRacesRec* raceRec = GetSelectedRaceRec();

    if (raceRec) {
        int32_t sexID = CCharacterCreation::m_character ? CCharacterCreation::m_character->m_data.m_preferences.m_sexID : 0;
        lua_pushstring(L, raceRec->m_facialHairCustomization[sexID]);
        return 1;
    }

    lua_pushstring(L, "NORMAL");
    return 1;
}

// OFFSET: 0x0080D7B0/0x140524180
static int32_t Script_GetSelectedRace(lua_State* L) {
    lua_pushnumber(L, CCharacterCreation::m_raceIndex + 1);
    return 1;
}

// OFFSET: 0x0080D7F0/0x1405241B0
static int32_t Script_GetSelectedSex(lua_State* L) {
    int32_t sexID = CCharacterCreation::m_character ? CCharacterCreation::m_character->m_data.m_preferences.m_sexID : 0;
    lua_pushnumber(L, g_scriptSexIndex[sexID]);
    return 1;
}

// OFFSET: 0x0080D830/0x1405241F0
static int32_t Script_GetSelectedClass(lua_State* L) {
    int32_t classID = CCharacterCreation::m_selectedClassID;
    const ChrClassesRec* classRec = g_chrClassesDB.GetRecord(classID);

    if (!classRec) {
        return 0;
    }

    int32_t sexID = CCharacterCreation::m_character ? CCharacterCreation::m_character->m_data.m_preferences.m_sexID : 0;
    lua_pushstring(L, CGUnit_C::GetDisplayClassNameFromRecord(classRec, sexID, NULL));
    lua_pushstring(L, classRec->m_filename);

    int32_t index = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_chrClassesDB.m_numRecords); i++) {
        if (g_chrClassesDB.GetRecordByIndex(i)->m_id == classID) {
            index = i;
        }
    }

    lua_pushnumber(L, index + 1);

    uint8_t roles = CGLookingForGroup::GetClassRoles(CCharacterCreation::m_selectedClassID);
    lua_pushboolean(L, roles & 0x2);
    lua_pushboolean(L, roles & 0x4);
    lua_pushboolean(L, roles & 0x8);
    return 6;
}

// OFFSET: 0x0080D930/0x140524340
static int32_t Script_GetCharacterCreateFacing(lua_State* L) {
    lua_pushnumber(L, CCharacterCreation::m_charFacing * 57.29578f);
    return 1;
}

// OFFSET: 0x0080D960/0x140524370
static int32_t Script_SetCharacterCreateFacing(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetCharacterCreateFacing(degrees)");
        return 0;
    }

    CCharacterCreation::SetCharacterFacing(static_cast<float>(lua_tonumber(L, 1)) * 0.017453292f);
    return 1;
}

// OFFSET: 0x0080D9B0/0x1405243D0
static int32_t Script_RequestRandomName(lua_State* L) {
    CGlueMgr::RequestRandomCharacterName(CCharacterCreation::m_character->m_data.m_preferences.m_raceID, CCharacterCreation::m_character->m_data.m_preferences.m_sexID);
    return 0;
}

// OFFSET: 0x0080D9D0/0x1405243F0
static int32_t Script_CreateCharacter(lua_State* L) {
    if (lua_isstring(L, 1)) {
        lua_tolstring(L, 1, NULL);
    }

    CCharacterCreation::CreateCharacter(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x0080DA10/0x140524440
static int32_t Script_PaidChange_GetCurrentRaceIndex(lua_State* L) {
    lua_pushnumber(L, CCharacterCreation::GetExistingRaceIndex() + 1);
    return 1;
}

// OFFSET: 0x0080DA40/0x1405244B0
static int32_t Script_PaidChange_GetCurrentClassIndex(lua_State* L) {
    lua_pushnumber(L, CCharacterCreation::GetExistingClassIndex() + 1);
    return 1;
}

// OFFSET: 0x0080DA70/0x140524530
static int32_t Script_IsRaceClassValid(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: IsRaceClassValid(raceIndex, classIndex)");
        return 0;
    }

    uint32_t raceIndex = lua_tointeger(L, 1) - 1;
    int32_t classIndex = lua_tointeger(L, 2) - 1;

    if (classIndex < 0 || classIndex >= g_chrClassesDB.m_numRecords) {
        return 0;
    }

    const ChrClassesRec* classRec = g_chrClassesDB.GetRecordByIndex(classIndex);

    if (!classRec) {
        return 0;
    }

    int32_t raceID = raceIndex < CCharacterCreation::m_races.Count() ? CCharacterCreation::m_races.m_data[raceIndex] : 0;

    if (IsRaceClassValid(raceID, classRec->m_id)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0080DB50/0x140524620
static int32_t Script_GetCreateBackgroundModel(lua_State* L) {
    const char* modelName = NULL;

    if (CCharacterCreation::m_selectedClassID == 6) {
        const ChrClassesRec* classRec = g_chrClassesDB.GetRecord(6);

        if (classRec) {
            modelName = classRec->m_filename;
        }
    } else {
        int32_t raceID = CCharacterCreation::m_character ? CCharacterCreation::m_character->m_data.m_preferences.m_raceID : 0;
        const ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(raceID);

        if (raceRec) {
            modelName = raceRec->m_clientFileString;
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

// OFFSET: 0x0080DC40/0x140524770
static int32_t Script_SetViewingAlteredForm(lua_State* L) {
    if (lua_type(L, 1) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: SetViewingAlteredForm(bool)");
        return 0;
    }

    CCharacterCreation::SetViewingAlteredForm(lua_toboolean(L, 1));
    return 0;
}

// OFFSET: 0x0080DC80/0x1405247C0
static int32_t Script_SetPortraitTexture(lua_State* L) {
    CSimpleTexture* texture;

    if (lua_isstring(L, 1)) {
        texture = static_cast<CSimpleTexture*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 1, NULL), CSimpleTexture::GetObjectType()));

        if (!texture) {
            luaL_error(L, "SetPortraitToTexture(): Couldn't find texture named '%s'", lua_tolstring(L, 1, NULL));
            return 0;
        }
    } else {
        if (lua_type(L, 1) != LUA_TTABLE) {
            luaL_error(L, "SetPortraitToTexture(): Couldn't find texture named '%s'", lua_tolstring(L, 1, NULL));
            return 0;
        }

        lua_rawgeti(L, 1, 0);
        texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            luaL_error(L, "SetPortraitToTexture(): Couldn't find 'this' in texture object");
            return 0;
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            luaL_error(L, "SetPortraitToTexture(): Wrong object type, expected texture");
            return 0;
        }
    }

    if (lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t raceID = lua_tointeger(L, 2);
        int32_t sex = lua_tointeger(L, 3);

        for (uint32_t i = 0; i < 3; i++) {
            if (sex == g_scriptSexIndex[i]) {
                SetPortraitTexture(texture, raceID, i, WOWGUID());
            }
        }
    }

    return 0;
}

// OFFSET: 0x0080E2A0/0x140525060
static int32_t Script_GetAvailableRaces(lua_State* L) {
    uint32_t count = CCharacterCreation::m_races.Count();

    for (uint32_t i = 0; i < count; i++) {
        const ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(CCharacterCreation::m_races.m_data[i]);
        int32_t sexID = CCharacterCreation::m_character ? CCharacterCreation::m_character->m_data.m_preferences.m_sexID : 0;
        lua_pushstring(L, CGUnit_C::GetDisplayRaceNameFromRecord(raceRec, sexID, NULL));

        const ChrRacesRec* fileRec = g_chrRacesDB.GetRecord(CCharacterCreation::m_races.m_data[i]);
        lua_pushstring(L, fileRec ? static_cast<const char*>(fileRec->m_clientFileString) : NULL);

        const ChrRacesRec* expansionRec = g_chrRacesDB.GetRecord(CCharacterCreation::m_races.m_data[i]);
        int32_t available = !expansionRec ? 0 : ClientServices::Connection()->m_accountExpansion >= expansionRec->m_requiredExpansion;
        lua_pushnumber(L, available);
    }

    return 3 * count;
}

// OFFSET: 0x0080E3C0/0x1405251D0
static int32_t Script_GetClassesForRace(lua_State* L) {
    uint32_t count = CCharacterCreation::m_classes.Count();

    for (uint32_t i = 0; i < count; i++) {
        const ChrClassesRec* classRec = i >= CCharacterCreation::m_classes.Count() ? NULL : CCharacterCreation::m_classes.m_data[i];
        const char* name = CGUnit_C::GetDisplayClassNameFromRecord(classRec, CCharacterCreation::GetSelectedSex(), NULL);
        int32_t available = ClientServices::Connection()->m_accountExpansion >= classRec->m_requiredExpansion;

        if (name) {
            lua_pushstring(L, name);
            lua_pushstring(L, classRec->m_filename);
            lua_pushnumber(L, available);
        } else {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnil(L);
        }
    }

    return 3 * count;
}

// OFFSET: 0x0080E480/0x1405252E0
static int32_t Script_PaidChange_GetName(lua_State* L) {
    CharacterSelectionDisplay* display = CCharacterCreation::m_existingCharacterIndex >= 0 && CCharacterCreation::m_existingCharacterIndex < static_cast<int32_t>(CCharacterSelection::m_characterList.Count()) ? &CCharacterSelection::m_characterList.m_data[CCharacterCreation::m_existingCharacterIndex] : NULL;

    if (display) {
        lua_pushstring(L, display->m_characterInfo.m_name);
        return 1;
    }

    lua_pushstring(L, NULL);
    return 1;
}

// OFFSET: 0x0080EA50/0x140525A00
static int32_t Script_SetSelectedClass(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedClass(index)");
        return 0;
    }

    int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    const ChrClassesRec* classRec = index < 0 || index >= g_chrClassesDB.m_numRecords ? NULL : g_chrClassesDB.GetRecordByIndex(index);
    CCharacterCreation::SetSelectedClass(classRec->m_id);
    return 0;
}

// OFFSET: 0x0080EAF0/0x140525A80
static int32_t Script_CycleCharCustomization(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CycleCharCustomization(index, delta)");
        return 0;
    }

    int32_t type = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t delta = static_cast<int32_t>(lua_tonumber(L, 2));
    CCharacterCreation::CycleCharCustomization(static_cast<CHAR_CUSTOMIZATION_TYPE>(type), delta);
    return 0;
}

// OFFSET: 0x0080EB80/0x140525B00
static int32_t Script_RandomizeCharCustomization(lua_State* L) {
    CCharacterCreation::RandomizeCharCustomization();
    return 0;
}

// OFFSET: 0x0080F060/0x140526230
static int32_t Script_SetSelectedSex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedSex(index)");
        return 0;
    }

    int32_t sex = lua_tointeger(L, 1);

    for (uint32_t i = 0; i < 3; i++) {
        if (sex == g_scriptSexIndex[i]) {
            CCharacterCreation::SetSelectedSex(i);
        }
    }

    return 0;
}

// OFFSET: 0x0080F700/0x1405269E0
static int32_t Script_ResetCharCustomize(lua_State* L) {
    CCharacterCreation::ResetCharCustomizeInfo();
    return 0;
}

// OFFSET: 0x0080F710/0x1405269F0
static int32_t Script_SetSelectedRace(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetSelectedRace(index)");
        return 0;
    }

    CCharacterCreation::SetSelectedRace(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x0080F780/0x140526A40
static int32_t Script_UpdateCustomizationBackground(lua_State* L) {
    CCharacterCreation::SetSelectedRace(CCharacterCreation::m_raceIndex);
    return 0;
}

// OFFSET: 0x0080F7A0/0x140526A60
static int32_t Script_CustomizeExistingCharacter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CustomizeExistingCharacter(index)");
        return 0;
    }

    CCharacterCreation::SetToExistingCharacter(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// STATIC: 0x00D07AA0/0x140BD1C10
static FrameScript_Method s_scriptFunctions[] = {
    { "SetCharCustomizeFrame", &Script_SetCharCustomizeFrame },
    { "SetCharCustomizeBackground", &Script_SetCharCustomizeBackground },
    { "ResetCharCustomize", &Script_ResetCharCustomize },
    { "GetNameForRace", &Script_GetNameForRace },
    { "GetFactionForRace", &Script_GetFactionForRace },
    { "GetAvailableRaces", &Script_GetAvailableRaces },
    { "GetAvailableClasses", &Script_GetAvailableClasses },
    { "GetClassesForRace", &Script_GetClassesForRace },
    { "GetHairCustomization", &Script_GetHairCustomization },
    { "GetFacialHairCustomization", &Script_GetFacialHairCustomization },
    { "GetSelectedRace", &Script_GetSelectedRace },
    { "GetSelectedSex", &Script_GetSelectedSex },
    { "GetSelectedClass", &Script_GetSelectedClass },
    { "SetSelectedRace", &Script_SetSelectedRace },
    { "SetSelectedSex", &Script_SetSelectedSex },
    { "SetSelectedClass", &Script_SetSelectedClass },
    { "UpdateCustomizationBackground", &Script_UpdateCustomizationBackground },
    { "UpdateCustomizationScene", &Script_UpdateCustomizationScene },
    { "CycleCharCustomization", &Script_CycleCharCustomization },
    { "RandomizeCharCustomization", &Script_RandomizeCharCustomization },
    { "GetCharacterCreateFacing", &Script_GetCharacterCreateFacing },
    { "SetCharacterCreateFacing", &Script_SetCharacterCreateFacing },
    { "GenerateRandomName", &Script_GenerateRandomName },
    { "RequestRandomName", &Script_RequestRandomName },
    { "CreateCharacter", &Script_CreateCharacter },
    { "CustomizeExistingCharacter", &Script_CustomizeExistingCharacter },
    { "PaidChange_GetPreviousRaceIndex", &Script_PaidChange_GetPreviousRaceIndex },
    { "PaidChange_GetCurrentRaceIndex", &Script_PaidChange_GetCurrentRaceIndex },
    { "PaidChange_GetCurrentClassIndex", &Script_PaidChange_GetCurrentClassIndex },
    { "PaidChange_GetName", &Script_PaidChange_GetName },
    { "IsRaceClassValid", &Script_IsRaceClassValid },
    { "IsRaceClassRestricted", &Script_IsRaceClassRestricted },
    { "GetCreateBackgroundModel", &Script_GetCreateBackgroundModel },
    { "HasAlteredForm", &Script_HasAlteredForm },
    { "IsViewingAlteredForm", &Script_IsViewingAlteredForm },
    { "SetViewingAlteredForm", &Script_SetViewingAlteredForm },
    { "SetPortraitTexture", &Script_SetPortraitTexture },
};

// OFFSET: 0x0080C3A0/0x140522A20
void CharacterCreateRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_scriptFunctions[i].m_name, s_scriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0080C3D0/0x140522A70
void CharacterCreateUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_scriptFunctions) / sizeof(s_scriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_scriptFunctions[i].m_name);
    }
}
