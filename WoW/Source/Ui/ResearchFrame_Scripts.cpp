#include "WoW/Source/Ui/ResearchFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/DungeonMapRec.h"
#include "WoW/Source/DB/Rec/ResearchBranchRec.h"
#include "WoW/Source/DB/Rec/ResearchFieldRec.h"
#include "WoW/Source/DB/Rec/ResearchProjectRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// PROVEN: 356c83ef IDENTICAL/DIFFERENT
// OFFSET: 0x009F45B0/0x140790270
static int32_t Script_GetArchaeologyInfo(lua_State* L) {
    if (CGResearchFrame::m_fieldRec) {
        lua_pushstring(L, CGResearchFrame::m_fieldRec->m_name);
    } else {
        lua_pushstring(L, "UNKNOWN");
    }

    return 1;
}

// PROVEN: 69b9ac64 IDENTICAL/IDENTICAL
// OFFSET: 0x009F45F0/0x14078FEB0
static int32_t Script_ItemAddedToArtifact(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsItemAddedToProject(index)");
        return 0;
    }

    lua_pushboolean(L, static_cast<int32_t>(lua_tonumber(L, 1)) - 1 < CGResearchFrame::m_itemCount);

    return 1;
}

// PROVEN: efd0c9bb IDENTICAL/IDENTICAL
// OFFSET: 0x009F4650/0x14078FF10
static int32_t Script_RemoveItemFromArtifact(lua_State* L) {
    if (CGResearchFrame::m_itemCount > 0) {
        CGResearchFrame::m_itemCount--;
        FrameScript_SignalEvent(SCRIPT_EVENT_ARTIFACT_UPDATE, NULL);
    }

    return 1;
}

// PROVEN: e75cd075 IDENTICAL/IDENTICAL
// OFFSET: 0x009F4680/0x14078FF40
static int32_t Script_IsArtifactCompletionHistoryAvailable(lua_State* L) {
    if (CGResearchFrame::m_historyLoaded) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 08444dd9 IDENTICAL/IDENTICAL
// OFFSET: 0x009F46C0/0x14078FF80
static int32_t Script_RequestArtifactCompletionHistory(lua_State* L) {
    if (!CGResearchFrame::m_historyRequested) {
        PlayerRequestResearchHistory msg;
        ClientServices::Send(&msg);
    }

    return 0;
}

// PROVEN: 44606076 IDENTICAL/IDENTICAL
// OFFSET: 0x009F4700/0x14078FFC0
static int32_t Script_CloseResearch(lua_State* L) {
    CGResearchFrame::m_skillLine = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_ARCHAEOLOGY_CLOSED, NULL);

    return 0;
}

// PROVEN: 2fbff7ee IDENTICAL/DIFFERENT
// OFFSET: 0x009F48E0/0x1407902D0
static int32_t PushArtifactInfo(const ResearchProjectRec* project, lua_State* L) {
    if (project) {
        lua_pushstring(L, project->m_name);
        lua_pushstring(L, project->m_description);
        lua_pushnumber(L, project->m_rarity);
        const SpellRec* spell = g_spellDB.GetRecord(project->m_spellID);

        if (spell) {
            const SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);

            if (icon) {
                lua_pushstring(L, icon->m_textureFilename);
            } else {
                lua_pushstring(L, "");
            }

            lua_pushstring(L, spell->m_description);
        } else {
            lua_pushstring(L, "");
            lua_pushstring(L, "");
        }

        lua_pushnumber(L, project->m_numSockets);
        lua_pushstring(L, project->m_texture);

        return 7;
    }

    return 0;
}

// PROVEN: 27b388ae IDENTICAL/DIFFERENT
// OFFSET: 0x009F49C0/0x140790460
static int32_t Script_GetSelectedArtifactInfo(lua_State* L) {
    return PushArtifactInfo(CGResearchFrame::m_projectRec, L);
}

// OFFSET: 0x009F4F30/0x140790A50
static int32_t Script_GetNumArchaeologyRaces(lua_State* L) {
    lua_pushnumber(L, CGResearchFrame::m_branches.Count());

    return 1;
}

// OFFSET: 0x009F4F70/0x140790A80
static int32_t Script_GetArchaeologyRaceInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetBranchInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    const ResearchBranchRec* branch;

    if (index < 0 || index >= static_cast<int32_t>(CGResearchFrame::m_branches.Count()) || (branch = CGResearchFrame::m_branches[index]) == NULL) {
        lua_pushstring(L, "UNKNOWN");
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 5;
    }

    lua_pushstring(L, branch->m_name);
    lua_pushstring(L, branch->m_texture);
    lua_pushnumber(L, branch->m_itemID);
    PlayerCurrency* currency = CGCurrencyTypes::GetCurrency(branch->m_currencyID);
    lua_pushnumber(L, !currency ? 0.0 : currency->m_quantity);
    const ResearchProjectRec* project = CGResearchFrame::GetActiveProject(branch->m_id);
    lua_pushnumber(L, project ? project->m_requiredWeight : 0.0);

    return 5;
}

// PROVEN: 033f2571 IDENTICAL/IDENTICAL
// OFFSET: 0x009F50A0/0x140790C10
static int32_t Script_SocketItemToArtifact(lua_State* L) {
    lua_pushboolean(L, CGResearchFrame::ApplyItemToProject());

    return 1;
}

// PROVEN: 314f246b IDENTICAL/DIFFERENT
// OFFSET: 0x009F50C0/0x140790C40
static int32_t Script_GetActiveArtifactByRace(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetActiveProjectByBranch(branchIndex, index)");
        return 0;
    }

    int32_t branchID = -1;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 0 && index < static_cast<int32_t>(CGResearchFrame::m_branches.Count())) {
        const ResearchBranchRec* branch = CGResearchFrame::m_branches[index];

        if (branch) {
            branchID = branch->m_id;
        }
    }

    return PushArtifactInfo(CGResearchFrame::GetActiveProject(branchID), L);
}

// OFFSET: 0x009F59C0/0x1407919B0
static int32_t Script_GetNumArtifactsByRace(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetNumProjects(branchID)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 0 || index >= static_cast<int32_t>(CGResearchFrame::m_branches.Count())) {
        return 0;
    }

    const ResearchBranchRec* branch = CGResearchFrame::m_branches[index];

    if (!branch) {
        return 0;
    }

    HASHKEY_NONE key;
    CGResearchFrame::BRANCH* node = CGResearchFrame::m_projectsByBranch.Ptr(branch->m_id, key);
    lua_pushnumber(L, node ? node->m_numProjects : 0);

    return 1;
}

// PROVEN: 18d9991b IDENTICAL/DIFFERENT
// OFFSET: 0x009F5A80/0x140791A50
static int32_t Script_GetArtifactInfoByRace(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetProjectInfo(branchID, index)");
        return 0;
    }

    int32_t branchID = -1;
    const ResearchBranchRec* branch = CGResearchFrame::GetBranch(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

    if (branch) {
        branchID = branch->m_id;
    }

    CGResearchFrame::PROJECT* project = CGResearchFrame::GetProject(branchID, static_cast<int32_t>(lua_tonumber(L, 2)) - 1);

    if (project) {
        int32_t count = PushArtifactInfo(project->m_project, L);
        lua_pushnumber(L, project->m_firstCompleted);
        lua_pushnumber(L, project->m_completionCount);

        return count + 2;
    }

    return 0;
}

// OFFSET: 0x009F5B40/0x140791B70
static int32_t Script_SetSelectedArtifact(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetProject(branchIndex, index) or SetProject(branchIndex)");
        return 0;
    }

    int32_t branchID = -1;
    int32_t branchIndex = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (branchIndex >= 0 && branchIndex < static_cast<int32_t>(CGResearchFrame::m_branches.Count())) {
        const ResearchBranchRec* branch = CGResearchFrame::m_branches[branchIndex];

        if (branch) {
            branchID = branch->m_id;
        }
    }

    int32_t index;

    if (lua_isnumber(L, 2) && (index = static_cast<int32_t>(lua_tonumber(L, 2))) != 0) {
        if (CGResearchFrame::GetProject(branchID, index - 1)) {
            const ResearchProjectRec* project = CGResearchFrame::GetProject(branchID, index - 1)->m_project;

            if (project) {
                CGResearchFrame::SelectProject(project->m_id);
            }
        }
    } else {
        const ResearchProjectRec* project = CGResearchFrame::GetActiveProject(branchID);

        if (project) {
            CGResearchFrame::SelectProject(project->m_id);
        }
    }

    return 0;
}

// OFFSET: 0x009F5C30/0x140791C80
static int32_t Script_ArchaeologyMapUpdateAll(lua_State* L) {
    const WorldMapAreaRec* area = g_worldMapAreaDB.GetRecord(CGWorldMap::GetCurrentAreaID());
    const DungeonMapRec* dungeonMap = g_dungeonMapDB.GetRecord(CGWorldMap::GetCurrentDungeonMap());
    int32_t count = 0;

    if ((area && (CGWorldMap::m_currentZone != -1 || CGWorldMap::m_currentContinent == -2)) || dungeonMap) {
        count = CGResearchFrame::UpdateArchaeologyDigSites();
    }

    lua_pushnumber(L, count);

    return 1;
}

// OFFSET: 0x009F5D10/0x140791D80
static int32_t Script_ArcheologyGetVisibleBlobID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ArcheologyGetVisibleBlobID(index)");
        return 0;
    }

    lua_pushinteger(L, CGResearchFrame::GetArchaeologyBlobID(static_cast<int32_t>(lua_tonumber(L, 1)) - 1));

    return 1;
}

// OFFSET: 0x009F5D60/0x140791DE0
static int32_t Script_ArchaeologyGetIconInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ArchaeologyGetIconInfo(index)");
        return 0;
    }

    int32_t blobID = CGResearchFrame::GetArchaeologyBlobID(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    HASHKEY_NONE key;
    CGResearchFrame::BLOBPOIHASH* node = CGResearchFrame::m_blobHash.Ptr(blobID, key);

    if (!node) {
        return 0;
    }

    int32_t first = node->m_data.unk_0014;
    int32_t second = node->m_data.unk_0020;
    lua_pushinteger(L, second);
    lua_pushinteger(L, second);
    lua_pushinteger(L, first);

    return 3;
}

// PROVEN: 435bf4fe IDENTICAL/IDENTICAL
// OFFSET: 0x009F6950/0x140792DB0
static int32_t Script_CanItemBeSocketedToArtifact(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CanItemBeAppliedToProject(itemID)");
        return 0;
    }

    if (CGResearchFrame::CanItemBeApplied(static_cast<int32_t>(lua_tonumber(L, 1)))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 67b38805 IDENTICAL/IDENTICAL
// OFFSET: 0x009F69C0/0x140792E20
static int32_t Script_GetArtifactProgress(lua_State* L) {
    int32_t currency;
    int32_t items = 0;
    int32_t required = 0;
    CGResearchFrame::GetProgress(currency, items, required);
    lua_pushnumber(L, currency);
    lua_pushnumber(L, items);
    lua_pushnumber(L, required);

    return 3;
}

// PROVEN: 062f2f32 IDENTICAL/IDENTICAL
// OFFSET: 0x009F6A20/0x140792E90
static int32_t Script_CanSolveArtifact(lua_State* L) {
    int32_t currency;
    int32_t items;
    int32_t required;
    CGResearchFrame::GetProgress(currency, items, required);

    if (items + currency >= required) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: c8b9f975 IDENTICAL/IDENTICAL
// OFFSET: 0x009F6A80/0x140792EF0
static int32_t Script_SolveArtifact(lua_State* L) {
    CGResearchFrame::Solve();

    return 0;
}

// STATIC: 0x00D16F60/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseResearch", &Script_CloseResearch },
    { "GetArchaeologyInfo", &Script_GetArchaeologyInfo },
    { "GetNumArchaeologyRaces", &Script_GetNumArchaeologyRaces },
    { "GetArchaeologyRaceInfo", &Script_GetArchaeologyRaceInfo },
    { "GetNumArtifactsByRace", &Script_GetNumArtifactsByRace },
    { "GetArtifactInfoByRace", &Script_GetArtifactInfoByRace },
    { "SetSelectedArtifact", &Script_SetSelectedArtifact },
    { "GetSelectedArtifactInfo", &Script_GetSelectedArtifactInfo },
    { "CanItemBeSocketedToArtifact", &Script_CanItemBeSocketedToArtifact },
    { "ItemAddedToArtifact", &Script_ItemAddedToArtifact },
    { "SocketItemToArtifact", &Script_SocketItemToArtifact },
    { "RemoveItemFromArtifact", &Script_RemoveItemFromArtifact },
    { "GetArtifactProgress", &Script_GetArtifactProgress },
    { "CanSolveArtifact", &Script_CanSolveArtifact },
    { "SolveArtifact", &Script_SolveArtifact },
    { "GetActiveArtifactByRace", &Script_GetActiveArtifactByRace },
    { "IsArtifactCompletionHistoryAvailable", &Script_IsArtifactCompletionHistoryAvailable },
    { "RequestArtifactCompletionHistory", &Script_RequestArtifactCompletionHistory },
    { "ArchaeologyMapUpdateAll", &Script_ArchaeologyMapUpdateAll },
    { "ArcheologyGetVisibleBlobID", &Script_ArcheologyGetVisibleBlobID },
    { "ArchaeologyGetIconInfo", &Script_ArchaeologyGetIconInfo },
};

// PROVEN: 3a50fd58 IDENTICAL/UNPROVEN
// OFFSET: 0x009F4720/0x14078FFF0
void ResearchRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 74b68029 IDENTICAL/UNPROVEN
// OFFSET: 0x009F4750/0x140790040
void ResearchUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
