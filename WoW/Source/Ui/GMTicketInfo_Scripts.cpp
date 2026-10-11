#include "GMTicketInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GMSurveyInfo.h"

// STATIC: 0x00D169B8
static StaticConstant s_staticConstants[7] = {
    { "STATIC_CONSTANTS", 0 },
    { "Loot", 1 },
    { "AuctionHouse", 2 },
    { "Mail", 3 },
    { "Chat", 4 },
    { "Movement", 5 },
    { "Spell", 6 },
};

// OFFSET: 0x009E58E0/0x14077DCC0
static int32_t Script_RegisterStaticConstants(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: RegisterStaticConstants(table)");
        return 0;
    }

    for (uint32_t i = 0; i < sizeof(s_staticConstants) / sizeof(s_staticConstants[0]); i++) {
        lua_pushstring(L, s_staticConstants[i].m_name);
        lua_pushnumber(L, s_staticConstants[i].m_value);
        lua_settable(L, 1);
    }

    return 0;
}

// OFFSET: 0x009E6900/0x14077F030
static int32_t Script_GetGMStatus(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_GET_SYSTEM_STATUS));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x009E67B0/0x14077EEA0
static int32_t Script_GetGMTicket(lua_State* L) {
    SendGetTicketOpcode();
    return 0;
}

// OFFSET: 0x009E5950/0x14077DD60
static int32_t Script_GMSurveyAnswerSubmit(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3)) {
        luaL_error(L, "Usage: GMSurveyAnswerSubmit(question, rank, comment)");
        return 0;
    }

    uint32_t question = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    int32_t rank = static_cast<uint32_t>(lua_tonumber(L, 2));
    const char* comment = lua_tolstring(L, 3, NULL);

    if (question >= 15) {
        luaL_error(L, "GMSurveyAnswerSubmit: Questions limited from %d to %d", 1, 15);
        return 0;
    }

    CGGMSurveyInfo::SetAnswer(question, static_cast<uint8_t>(rank), comment);
    return 0;
}

// PROVEN: 7919c813 IDENTICAL/NONE
// OFFSET: 0x009E5A40/0x14077DE50
static int32_t Script_GMSurveyCommentSubmit(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GMSurveyCommentSubmit(comment)");
        return 0;
    }

    CGGMSurveyInfo::SetComment(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x009E5E20/0x14077E370
static int32_t Script_GMSurveyQuestion(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GMSurveyGetQuestion(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));
    const char* question;

    if (index >= 1 && static_cast<uint32_t>(index) <= 15 && (question = CGGMSurveyInfo::GetQuestion(index))) {
        lua_pushstring(L, question);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009E5EA0/0x14077E3F0
static int32_t Script_GMSurveyAnswer(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GMSurveyGetAnswer(questionIndex, answerIndex)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t answerIndex = static_cast<int32_t>(lua_tonumber(L, 2));
    const char* answer;

    if (index >= 1 && static_cast<uint32_t>(index) <= 15 && (answer = CGGMSurveyInfo::GetAnswer(index, answerIndex))) {
        lua_pushstring(L, answer);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009E5F40/0x14077E4A0
static int32_t Script_GMSurveyNumAnswers(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GMSurveyGetNumAnswers(questionIndex)");
        return 0;
    }

    int32_t count = 0;
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1));

    if (index >= 1 && static_cast<uint32_t>(index) <= 15) {
        count = CGGMSurveyInfo::GetNumAnswers(index);
    }

    lua_pushnumber(L, count);
    return 1;
}

// PROVEN: 97492f0b IDENTICAL/NONE
// OFFSET: 0x009E6760/0x14077EE50
static int32_t Script_GMReportLag(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GMReportLag(number)");
        return 0;
    }

    CGGMReportLag::Submit(static_cast<int32_t>(lua_tonumber(L, 1)));
    return 0;
}

// OFFSET: 0x009E67C0/0x14077EEB0
static int32_t Script_NewGMTicket(lua_State* L) {
    if (!lua_isstring(L, 1) || lua_type(L, 2) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: NewGMTicket(ticketDescription, needResponse)");
        return 0;
    }

    if (CGGameUI::CanPerformAction(17)) {
        const char* text = lua_tolstring(L, 1, NULL);
        int32_t needResponse = lua_toboolean(L, 2);
        CGGMTicketInfo::CreateTicket(text, needResponse, 0);
    }

    return 0;
}

// PROVEN: d831d114 IDENTICAL/NONE
// OFFSET: 0x009E6830/0x14077EF40
static int32_t Script_UpdateGMTicket(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: UpdateGMTicket(ticketDescription)");
        return 0;
    }

    if (CGGameUI::CanPerformAction(17)) {
        SendGMTicketUpdateText(lua_tolstring(L, 1, NULL));
    }

    return 0;
}

// OFFSET: 0x009E6880/0x14077EFA0
static int32_t Script_DeleteGMTicket(lua_State* L) {
    SendGMTicketDeleteTicket();
    return 0;
}

// OFFSET: 0x009E6890/0x14077EFB0
static int32_t Script_GMResponseNeedMoreHelp(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GMResponseNeedMoreHelp(newTicketDescription)");
        return 0;
    }

    if (CGGameUI::CanPerformAction(17)) {
        const char* text = lua_tolstring(L, 1, NULL);
        CGGMTicketInfo::CreateTicket(text, 1, 1);
    }

    return 0;
}

// OFFSET: 0x009E68F0/0x14077F020
static int32_t Script_GMResponseResolve(lua_State* L) {
    SendGMResponseResolve();
    return 0;
}

// OFFSET: 0x009E6970/0x14077F0B0
static int32_t Script_GMSurveySubmit(lua_State* L) {
    CGGMSurveyInfo::Submit();
    return 0;
}

// OFFSET: 0x009E6980/0x14077F0C0
static int32_t Script_GMSubmitSuggestion(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GMSubmitSuggestion(\"suggestion\")");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(26) || !CGGMTicketInfo::s_quickTicketSystemStatus) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !CGGMTicketInfo::s_quickTicketThrottle.IncrementAndCheck()) {
        return 0;
    }

    UserClientSupportTicketSubmitSuggestion msg;
    msg.m_header.m_mapID = CGGameUI::m_iCurrentMapID;
    msg.m_header.m_position = player->GetPosition();
    msg.m_header.m_facing = player->GetFacing();
    SStrCopy(msg.m_note, lua_tolstring(L, 1, NULL), sizeof(msg.m_note));
    ClientServices::Send(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_QUICK_TICKET_THROTTLE_CHANGED, NULL);
    CGGameUI::DisplayError(ERR_SUGGESTION_SUBMITTED_SUCCESSFULLY);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x009E6AC0/0x14077F240
static int32_t Script_GMSubmitBug(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GMSubmitBug(\"bugInfo\")");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(26) || !CGGMTicketInfo::s_quickTicketSystemStatus) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !CGGMTicketInfo::s_quickTicketThrottle.IncrementAndCheck()) {
        return 0;
    }

    UserClientSupportTicketSubmitBug msg;
    msg.m_header.m_mapID = CGGameUI::m_iCurrentMapID;
    msg.m_header.m_position = player->GetPosition();
    msg.m_header.m_facing = player->GetFacing();
    SStrCopy(msg.m_note, lua_tolstring(L, 1, NULL), sizeof(msg.m_note));
    ClientServices::Send(&msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_QUICK_TICKET_THROTTLE_CHANGED, NULL);
    CGGameUI::DisplayError(ERR_BUG_SUBMITTED_SUCCESSFULLY);
    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x009E5A90/0x14077DEA0
static int32_t Script_GMQuickTicketSystemEnabled(lua_State* L) {
    lua_pushboolean(L, CGGMTicketInfo::s_quickTicketSystemStatus);
    return 1;
}

// OFFSET: 0x009E5AB0/0x14077DEC0
static int32_t Script_GMItemRestorationButtonEnabled(lua_State* L) {
    lua_pushboolean(L, CGGMTicketInfo::s_itemRestorationButtonStatus);
    return 1;
}

// OFFSET: 0x009E5AD0/0x14077DEE0
static int32_t Script_GMQuickTicketSystemThrottled(lua_State* L) {
    lua_pushboolean(L, !CGGMTicketInfo::s_quickTicketThrottle.Check());
    return 1;
}

// STATIC: 0x00D169F8
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetGMTicket", &Script_GetGMTicket },
    { "NewGMTicket", &Script_NewGMTicket },
    { "UpdateGMTicket", &Script_UpdateGMTicket },
    { "DeleteGMTicket", &Script_DeleteGMTicket },
    { "GMResponseNeedMoreHelp", &Script_GMResponseNeedMoreHelp },
    { "GMResponseResolve", &Script_GMResponseResolve },
    { "GetGMStatus", &Script_GetGMStatus },
    { "GMSurveyQuestion", &Script_GMSurveyQuestion },
    { "GMSurveyNumAnswers", &Script_GMSurveyNumAnswers },
    { "GMSurveyAnswer", &Script_GMSurveyAnswer },
    { "GMSurveyAnswerSubmit", &Script_GMSurveyAnswerSubmit },
    { "GMSurveyCommentSubmit", &Script_GMSurveyCommentSubmit },
    { "GMSurveySubmit", &Script_GMSurveySubmit },
    { "GMReportLag", &Script_GMReportLag },
    { "RegisterStaticConstants", &Script_RegisterStaticConstants },
    { "GMSubmitSuggestion", &Script_GMSubmitSuggestion },
    { "GMSubmitBug", &Script_GMSubmitBug },
    { "GMQuickTicketSystemEnabled", &Script_GMQuickTicketSystemEnabled },
    { "GMItemRestorationButtonEnabled", &Script_GMItemRestorationButtonEnabled },
    { "GMQuickTicketSystemThrottled", &Script_GMQuickTicketSystemThrottled },
};

// OFFSET: 0x009E5B00
void GMTicketInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009E5B30
void GMTicketInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
