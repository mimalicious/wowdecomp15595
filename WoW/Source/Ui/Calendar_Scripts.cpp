#include "Calendar.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "WoW/Source/Game/GameTime.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/CalendarEvent.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/HolidaysRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"

// STATIC: 0x00D153C0
static const char* s_monthNames[12] = { "MONTH_JANUARY", "MONTH_FEBRUARY", "MONTH_MARCH", "MONTH_APRIL", "MONTH_MAY", "MONTH_JUNE", "MONTH_JULY", "MONTH_AUGUST", "MONTH_SEPTEMBER", "MONTH_OCTOBER", "MONTH_NOVEMBER", "MONTH_DECEMBER" };

// STATIC: 0x00D15420
const char* s_weekdayNames[7] = { "WEEKDAY_SUNDAY", "WEEKDAY_MONDAY", "WEEKDAY_TUESDAY", "WEEKDAY_WEDNESDAY", "WEEKDAY_THURSDAY", "WEEKDAY_FRIDAY", "WEEKDAY_SATURDAY" };

// STATIC: 0x00D1543C
static const uint32_t s_eventTypeDisplayOrder[6] = { 0, 1, 5, 2, 3, 4 };

// STATIC: 0x00C04FB4
static const int32_t s_expansionMaxLevels[4] = { 60, 70, 80, 85 };

// OFFSET: 0x009AB8E0/0x140732B50
static int32_t Script_CalendarIsActionPending(lua_State* L) {
    lua_pushboolean(L, CGCalendar::m_actionPending);
    return 1;
}

// PROVEN: 2e225121 IDENTICAL/NONE
// OFFSET: 0x009AAE60/0x140732020
static int32_t Script_CalendarGetMonthNames(lua_State* L) {
    lua_checkstack(L, 12);

    for (uint32_t i = 0; i < 12; i++) {
        lua_pushstring(L, FrameScript_GetText(s_monthNames[i], -1, GENDER_NOT_APPLICABLE));
    }

    return 12;
}

// OFFSET: 0x009AAEA0/0x140732090
static int32_t Script_CalendarGetWeekdayNames(lua_State* L) {
    lua_checkstack(L, 7);

    for (uint32_t i = 0; i < 7; i++) {
        lua_pushstring(L, FrameScript_GetText(s_weekdayNames[i], -1, GENDER_NOT_APPLICABLE));
    }

    return 7;
}

// PROVEN: 71769826 IDENTICAL/NONE
// OFFSET: 0x009AAEE0/0x140732100
static int32_t Script_CalendarGetDate(lua_State* L) {
    lua_pushnumber(L, g_clientGameTime.m_weekDay + 1);
    lua_pushnumber(L, g_clientGameTime.m_month + 1);
    lua_pushnumber(L, g_clientGameTime.m_monthDay + 1);
    lua_pushnumber(L, g_clientGameTime.m_year + 2000);
    return 4;
}

// OFFSET: 0x009B1060/0x140739520
static int32_t Script_CalendarGetNumPendingInvites(lua_State* L) {
    lua_pushnumber(L, CGCalendar::CountPendingInvites());
    return 1;
}

// OFFSET: 0x009AAF60/0x140732180
static int32_t Script_CalendarGetMinDate(lua_State* L) {
    WowTime date;
    GameCalendarGetMinDate(&date);
    lua_pushnumber(L, date.m_weekDay + 1);
    lua_pushnumber(L, date.m_month + 1);
    lua_pushnumber(L, date.m_monthDay + 1);
    lua_pushnumber(L, date.m_year + 2000);
    return 4;
}

// OFFSET: 0x009AB1D0/0x140732410
static int32_t Script_CalendarCloseEvent(lua_State* L) {
    CGCalendar::m_selectedEvent.m_eventID = 0;
    CGCalendar::m_selectedEvent.m_startTime = 0;
    CGCalendar::m_selectedEvent.m_endTime = 0;
    CGCalendar::m_selectedEvent.m_holidayID = 0;
    CGCalendar::m_selectedEvent.m_type = 0;
    return 0;
}

// OFFSET: 0x009AB6C0/0x1407328C0
static int32_t Script_CalendarEventGetTypes(lua_State* L) {
    lua_checkstack(L, 6);

    for (uint32_t i = 0; i < 6; i++) {
        lua_pushstring(L, FrameScript_GetText(GetEventTypeString(i), -1, GENDER_NOT_APPLICABLE));
    }

    return 6;
}

// OFFSET: 0x009AB710
static int32_t Script_CalendarEventGetTypesDisplayOrdered(lua_State* L) {
    lua_checkstack(L, 12);

    for (uint32_t i = 0; i < 6; i++) {
        lua_pushstring(L, FrameScript_GetText(GetEventTypeString(s_eventTypeDisplayOrder[i]), -1, GENDER_NOT_APPLICABLE));
        lua_pushinteger(L, s_eventTypeDisplayOrder[i] + 1);
    }

    return 12;
}

// PROVEN: 4e40dacc IDENTICAL/NONE
// OFFSET: 0x009AB770/0x1407329A0
static int32_t Script_CalendarEventGetRepeatOptions(lua_State* L) {
    lua_checkstack(L, 4);

    for (uint32_t i = 0; i < 4; i++) {
        lua_pushstring(L, FrameScript_GetText(GetEventRepeatString(i), -1, GENDER_NOT_APPLICABLE));
    }

    return 4;
}

// OFFSET: 0x009AC9E0/0x140733E60
static int32_t Script_CalendarGetMonth(lua_State* L) {
    int32_t offset = 0;

    if (lua_isnumber(L, 1)) {
        offset = static_cast<int32_t>(lua_tonumber(L, 1));
    }

    int32_t month = 0;
    int32_t year = 0;
    int32_t days = 0;
    int32_t weekDay = 0;
    CGCalendar::GetMonth(offset, &month, &year, &days, &weekDay);
    lua_pushnumber(L, month + 1);
    lua_pushnumber(L, year + 2000);
    lua_pushnumber(L, static_cast<uint32_t>(days));
    lua_pushnumber(L, weekDay + 1);
    return 4;
}

// OFFSET: 0x009AD0A0
static int32_t Script_CalendarGetEventIndex(lua_State* L) {
    int32_t monthOffset;
    int32_t day;
    int32_t index;

    if (CGCalendar::ComputeEventIndex(&CGCalendar::m_selectedEvent, &monthOffset, &day, &index)) {
        lua_pushinteger(L, monthOffset);
        lua_pushinteger(L, day + 1);
        lua_pushinteger(L, index + 1);
    } else {
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
    }

    return 3;
}

// OFFSET: 0x009AD7B0
static int32_t Script_CalendarContextGetEventIndex(lua_State* L) {
    int32_t monthOffset;
    int32_t day;
    int32_t index;

    if (CGCalendar::ComputeEventIndex(&CGCalendar::m_contextEvent, &monthOffset, &day, &index)) {
        lua_pushinteger(L, monthOffset);
        lua_pushinteger(L, day + 1);
        lua_pushinteger(L, index + 1);
    } else {
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
    }

    return 3;
}

// OFFSET: 0x009AFF90/0x1407381E0
static int32_t Script_CalendarDefaultGuildFilter(lua_State* L) {
    if (ClntObjMgrGetActivePlayerObj()) {
        double level = s_expansionMaxLevels[ClientServices::Connection()->m_accountExpansion];
        lua_pushnumber(L, level);
        lua_pushnumber(L, level);
        lua_pushnumber(L, CGGuildInfo::m_numUsedRanks);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 3;
}

// OFFSET: 0x009B7BD0/0x1407421B0
static int32_t Script_CalendarSetAbsMonth(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarSetAbsMonth(month[, year])");
        return 0;
    }

    int32_t month = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t year = CGCalendar::m_absYear;

    if (CGCalendar::m_absYear < 0) {
        year = g_clientGameTime.m_year;
        CGCalendar::m_absYear = year;

        if (year < 0) {
            year = 4;
            CGCalendar::m_absYear = 4;
        }
    }

    if (lua_isnumber(L, 2)) {
        year = static_cast<int32_t>(lua_tonumber(L, 2)) - 2000;
    }

    int32_t weekDay;
    CGCalendar::ClampAbsMonthDay(month, year, &CGCalendar::m_absMonth, &CGCalendar::m_absYear, &CGCalendar::m_absMonthDays, &weekDay);
    CGCalendar::CalculateEvents(true);
    return 0;
}

// OFFSET: 0x009AB140/0x140732370
static int32_t Script_CalendarGetMaxCreateDate(lua_State* L) {
    WowTime date;
    GameCalendarGetMaxCreateDate(&date, &g_clientGameTime);
    lua_pushnumber(L, date.m_weekDay + 1);
    lua_pushnumber(L, date.m_month + 1);
    lua_pushnumber(L, date.m_monthDay + 1);
    lua_pushnumber(L, date.m_year + 2000);
    return 4;
}

// OFFSET: 0x009AEBB0/0x1407366F0
static int32_t Script_CalendarCanSendInvite(lua_State* L) {
    int32_t now;

    lua_pushboolean(L, !(CGCalendar::m_actionPending || ((!unk_BD09AC || !unk_BD09AC->m_intValue) && (now = FrameTime::GetCurTimeMs(), CGCalendar::m_lastInviteTime) && now - CGCalendar::m_lastInviteTime < 2000)));

    return 1;
}

// OFFSET: 0x009AFDF0/0x140738080
static int32_t Script_CalendarSetMonth(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarSetMonth(offsetMonths)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    int32_t weekDay;
    CGCalendar::GetMonth(offset, &CGCalendar::m_absMonth, &CGCalendar::m_absYear, &CGCalendar::m_absMonthDays, &weekDay);
    CGCalendar::CalculateEvents(true);
    return 0;
}

// OFFSET: 0x009ACBE0/0x1407341C0
static int32_t Script_CalendarGetNumDayEvents(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CalendarGetNumDayEvents([-1,0,1], monthDay)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    lua_pushnumber(L, CGCalendar::GetNumDayEvents(offset, day));
    return 1;
}

// OFFSET: 0x009ACE00/0x1407343E0
static int32_t Script_CalendarGetFirstPendingInvite(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CalendarGetDay([-1,0,1], monthDay)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    lua_pushinteger(L, CGCalendar::GetFirstPendingInviteIndex(offset, day) + 1);
    return 1;
}

// OFFSET: #0x005B78D0
static void NameCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID key = WOWGUID();

    if (g_nameDBCache.GetRecord(guid.m_guid, key, NULL, NULL, false)) {
        Script_SendUniqueSignal(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST);
    }
}

// OFFSET: #0x005B8040
const char* GetSequenceLabel(uint32_t type, uint32_t index, uint32_t length) {
    if (!(type & 0x8) || length <= 1) {
        return "";
    }

    if (!index) {
        return "START";
    }

    if (index == length - 1) {
        return "END";
    }

    return "ONGOING";
}

// PROVEN: 389896e7 IDENTICAL/NONE
// OFFSET: 0x009AAE00
static const char* GetInviteRoleLabel(uint32_t flags) {
    if (flags & 0x4) {
        return "CREATOR";
    }

    if (flags & 0x2) {
        return "MODERATOR";
    }

    return "";
}

// OFFSET: #0x005B80B0
int32_t IsSignUpInvite(const CalendarDayEvent* event) {
    return (event->m_type & 0x400) && (event->m_flags & 0x8);
}

// OFFSET: 0x009AAFF0/0x140732210
static int32_t Script_CalendarGetMaxDate(lua_State* L) {
    WowTime date;
    GameCalendarGetMaxDate(&date);
    lua_pushnumber(L, date.m_weekDay + 1);
    lua_pushnumber(L, date.m_month + 1);
    lua_pushnumber(L, date.m_monthDay + 1);
    lua_pushnumber(L, date.m_year + 2000);
    return 4;
}

// PROVEN: 92e68404 IDENTICAL/NONE
// OFFSET: 0x009AB080/0x1407322A0
static int32_t Script_CalendarGetMinHistoryDate(lua_State* L) {
    WowTime date;
    date.m_weekDay = g_clientGameTime.m_weekDay;
    date.m_month = g_clientGameTime.m_month;
    date.m_monthDay = g_clientGameTime.m_monthDay;
    date.m_year = g_clientGameTime.m_year;
    date.AddDays(-14, false);
    date.SetHourAndMinutes(0u, 0u);
    lua_pushnumber(L, date.m_weekDay + 1);
    lua_pushnumber(L, date.m_month + 1);
    lua_pushnumber(L, date.m_monthDay + 1);
    lua_pushnumber(L, date.m_year + 2000);
    return 4;
}

// OFFSET: 0x009AB200/0x140732430
static int32_t Script_CalendarEventGetNumInvites(lua_State* L) {
    lua_pushnumber(L, CGCalendar::m_curEvent ? CGCalendar::m_curEvent->m_numInvites : 0);

    if (CGCalendar::m_curEvent) {
        lua_pushboolean(L, CGCalendar::m_curEvent->m_pendingNames == 0);
    } else {
        lua_pushboolean(L, 0);
    }

    return 2;
}

// OFFSET: 0x009AB280/0x1407324A0
static int32_t Script_CalendarContextDeselectEvent(lua_State* L) {
    CGCalendar::m_contextEvent.m_eventID = 0;
    CGCalendar::m_contextEvent.m_startTime = 0;
    CGCalendar::m_contextEvent.m_endTime = 0;
    CGCalendar::m_contextEvent.m_holidayID = 0;
    CGCalendar::m_contextEvent.m_type = 0;
    CGCalendar::m_contextEventInfo = NULL;
    return 0;
}

// OFFSET: 0x009AB2B0/0x1407324C0
static int32_t Script_CalendarContextEventClipboard(lua_State* L) {
    lua_pushboolean(L, s_eventClipboard.m_eventID ? true : false);
    return 1;
}

// OFFSET: 0x009AB2E0/0x1407324E0
static int32_t Script_CalendarEventSortInvites(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: CalendarEventSortInvites(\"criteria\", reverse)");
        return 0;
    }

    uint32_t criterion = 3;
    const char* name = lua_tolstring(L, 1, NULL);

    if (!SStrCmpI(name, "name", STORM_MAX_STR)) {
        criterion = 0;
    } else if (!SStrCmpI(name, "level", STORM_MAX_STR)) {
        criterion = 1;
    } else if (!SStrCmpI(name, "class", STORM_MAX_STR)) {
        criterion = 2;
    } else if (!SStrCmpI(name, "status", STORM_MAX_STR)) {
        criterion = 3;
    } else if (!SStrCmpI(name, "party", STORM_MAX_STR)) {
        criterion = 4;
    } else if (!SStrCmpI(name, "notes", STORM_MAX_STR)) {
        criterion = 5;
    }

    int32_t reverse = StringToBOOL(L, 2, 1);

    if (CGCalendar::m_curEvent && CGCalendar::m_curEvent->SortInvites(criterion, reverse != 0)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_INVITE_LIST, NULL);
    }

    return 0;
}

// OFFSET: 0x009AB3F0/0x140732610
static int32_t Script_CalendarEventGetInviteSortCriterion(lua_State* L) {
    InviteSortCriterion* criterion = !CGCalendar::m_curEvent ? NULL : &CGCalendar::m_curEvent->m_sortCriteria[CGCalendar::m_curEvent->m_sortType];
    uint8_t reverse = 0;
    const char* name = "";

    if (criterion) {
        switch (criterion->m_criterion) {
            case 0:
                name = "name";
                break;

            case 1:
                name = "level";
                break;

            case 2:
                name = "class";
                break;

            case 3:
                name = "status";
                break;

            case 4:
                name = "party";
                break;

            case 5:
                name = "notes";
                break;
        }

        reverse = criterion->m_reverse;
    }

    lua_pushstring(L, name);
    lua_pushboolean(L, reverse);
    return 2;
}

// OFFSET: 0x009AB490/0x1407326E0
static int32_t Script_CalendarEventSetStatus(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CalendarEventSetStatus(index, status)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t status = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    CGCalendar::SetInviteStatus(index, status);
    return 0;
}

// OFFSET: 0x009AB530/0x140732760
static int32_t Script_CalendarEventSetModerator(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventSetModerator(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGCalendar::SetModeratorStatus(index, 1);
    return 0;
}

// OFFSET: 0x009AB5A0/0x1407327C0
static int32_t Script_CalendarEventClearModerator(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventClearModerator(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGCalendar::SetModeratorStatus(index, 0);
    return 0;
}

// PROVEN: 61dec677 IDENTICAL/NONE
// OFFSET: 0x009AB610/0x140732810
static int32_t Script_CalendarEventIsModerator(lua_State* L) {
    lua_pushboolean(L, CGCalendar::m_curEvent ? CGCalendar::m_curEvent->IsModerator() : false);
    return 1;
}

// OFFSET: 0x009AB640/0x140732850
static int32_t Script_CalendarEventCanModerate(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventCanModerate(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    lua_pushboolean(L, CGCalendar::m_curEvent ? CGCalendar::m_curEvent->CanModerate(index) : false);
    return 1;
}

// PROVEN: 09fa7e18 IDENTICAL/NONE
// OFFSET: 0x009AB7C0/0x140732A00
static int32_t Script_CalendarEventHaveSettingsChanged(lua_State* L) {
    if (CGCalendar::m_curEvent) {
        lua_pushboolean(L, CGCalendar::m_curEvent->m_changed);
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: 0x009AB800/0x140732A40
static int32_t Script_CalendarEventCanEdit(lua_State* L) {
    lua_pushboolean(L, CGCalendar::CanEditCurrentEvent());
    return 1;
}

// PROVEN: e6efb57c IDENTICAL/NONE
// OFFSET: 0x009AB850/0x140732AA0
static int32_t Script_CalendarEventGetCalendarType(lua_State* L) {
    if (CGCalendar::m_curEvent) {
        lua_pushstring(L, CGCalendar::GetCalendarTypeString(CGCalendar::m_curEvent->m_flags));
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009AB890/0x140732AF0
static int32_t Script_CalendarEventSelectInvite(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventSelectInvite(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 1) - 1;

    if (CGCalendar::m_curEvent) {
        CGCalendar::m_curEvent->SelectInvite(index);
    }

    return 0;
}

// OFFSET: 0x009AEC20/0x140736760
static int32_t Script_CalendarCanAddEvent(lua_State* L) {
    int32_t now;

    lua_pushboolean(L, !(CGCalendar::m_actionPending || ((!unk_BD09AC || !unk_BD09AC->m_intValue) && (now = FrameTime::GetCurTimeMs(), CGCalendar::m_lastEventTime) && now - CGCalendar::m_lastEventTime < 5000)));

    return 1;
}

// OFFSET: 0x009ACAB0/0x140733FF0
static int32_t Script_CalendarGetAbsMonth(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarGetAbsMonth(month[, year])");
        return 0;
    }

    int32_t month = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    int32_t year = CGCalendar::m_absYear;

    if (CGCalendar::m_absYear < 0) {
        year = g_clientGameTime.m_year;
        CGCalendar::m_absYear = year;

        if (year < 0) {
            year = 4;
            CGCalendar::m_absYear = 4;
        }
    }

    if (lua_isnumber(L, 2)) {
        year = static_cast<int32_t>(lua_tonumber(L, 2)) - 2000;
    }

    int32_t outMonth = 0;
    int32_t outYear = 0;
    int32_t days = 0;
    int32_t weekDay = 0;
    CGCalendar::ClampAbsMonthDay(month, year, &outMonth, &outYear, &days, &weekDay);
    lua_pushnumber(L, ++outMonth);
    outYear += 2000;
    lua_pushnumber(L, outYear);
    lua_pushnumber(L, static_cast<uint32_t>(days));
    lua_pushnumber(L, weekDay + 1);
    return 4;
}

// OFFSET: 0x009ACC90/0x140734290
static int32_t Script_CalendarGetDayEventSequenceInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarGetDayEventSequenceInfo([-1.0.1], monthDay, index)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    CalendarDayEvent* event = CGCalendar::GetDayEvent(offset, day, index);

    if (event) {
        lua_pushnumber(L, event->m_sequenceIndex);
        lua_pushnumber(L, event->m_sequenceLength);
        lua_pushstring(L, GetSequenceLabel(event->m_type, event->m_sequenceIndex, event->m_sequenceLength));
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
    }

    return 3;
}

// OFFSET: 0x009ACE90
static int32_t Script_CalendarGetNumGuildEvents(lua_State* L) {
    lua_pushinteger(L, s_guildEvents.Count());
    return 1;
}

// OFFSET: 0x009ACEB0
static int32_t Script_CalendarGetGuildEventSelectionInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarGetGuildEventSelectionInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CalendarDayEvent* event = index < s_guildEvents.Count() ? s_guildEvents.m_data[index] : NULL;
    int32_t monthOffset = 0;
    int32_t day = 0;
    int32_t eventIndex = 0;

    if (event) {
        CalendarEventSelection selection;
        selection.m_eventID = event->m_eventID;
        selection.m_startTime = event->m_startTime;
        selection.m_endTime = event->m_endTime;
        selection.m_holidayID = event->m_holidayID;
        selection.m_type = event->m_type;

        if (CGCalendar::ComputeEventIndex(&selection, &monthOffset, &day, &eventIndex)) {
            day++;
            eventIndex++;
        }
    }

    lua_pushnumber(L, monthOffset);
    lua_pushnumber(L, day);
    lua_pushnumber(L, eventIndex);
    return 3;
}

// OFFSET: 0x009ACFE0/0x1407345D0
static int32_t Script_CalendarOpenEvent(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarOpenEvent([-1,0,1], monthDay, index)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    CGCalendar::GetEvent(offset, day, index);
    return 0;
}

// OFFSET: 0x009AD120/0x140734810
static int32_t Script_CalendarGetRaidInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarGetRaidInfo([-1,0,1], monthDay, index)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    CalendarDayEvent* event = CGCalendar::GetDayEvent(offset, day, index);

    if (!event) {
        lua_pushnil(L);
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        return 7;
    }

    MapRec* map = g_mapDB.GetRecord(event->m_holidayID);
    lua_pushstring(L, map ? map->m_mapName : "");
    lua_pushstring(L, CGCalendar::GetCalendarTypeString(event->m_type));
    lua_pushnumber(L, static_cast<int32_t>(event->m_eventID));
    WowTime date;
    WowTime::WowDecodeTime(event->m_startTime, &date);
    lua_pushnumber(L, date.m_hour);
    lua_pushnumber(L, date.m_minute);
    lua_pushnumber(L, static_cast<int32_t>(event->m_endTime + 1));
    MapDifficultyRec* difficulty = GetMapDifficultyRec(event->m_holidayID, event->m_endTime, NULL);

    if (!difficulty) {
        lua_pushstring(L, "");
        return 7;
    }

    lua_pushstring(L, FrameScript_GetText(difficulty->m_difficultystring, -1, GENDER_NOT_APPLICABLE));
    return 7;
}

// OFFSET: 0x009AD340/0x140734A90
static int32_t Script_CalendarEventGetInvite(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventGetInvite(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGCalendarEvent* event = CGCalendar::m_curEvent;
    EventInviteInfo* invite;

    if (!event || index >= event->m_numInvites || !(invite = event->m_invites.m_data[index])) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushboolean(L, 0);
        lua_pushinteger(L, 0);
        lua_pushnil(L);
        return 9;
    }

    WOWGUID key = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(invite->m_guid.m_guid, key, NULL, NULL, false);
    lua_pushstring(L, name ? name->m_name : "");
    lua_pushnumber(L, invite->m_level);
    ChrClassesRec* classRec = name ? g_chrClassesDB.GetRecord(name->m_class) : NULL;

    if (classRec) {
        lua_pushstring(L, classRec->m_name);
        lua_pushstring(L, classRec->m_filename);
    } else {
        lua_pushstring(L, "");
        lua_pushstring(L, "");
    }

    if (event) {
        lua_pushinteger(L, invite->m_status + 1);
    } else {
        lua_pushinteger(L, 0);
    }

    if (invite->m_moderatorStatus == 1) {
        lua_pushstring(L, "MODERATOR");
    } else if (invite->m_moderatorStatus == 2) {
        lua_pushstring(L, "CREATOR");
    } else {
        lua_pushstring(L, "");
    }

    lua_pushboolean(L, ClntObjMgrGetActivePlayer() == invite->m_guid);
    lua_pushinteger(L, invite->m_inviteType + 1);
    lua_pushstring(L, invite->m_notes);
    return 9;
}

// PROVEN: 7150bd92 IDENTICAL/NONE
// OFFSET: 0x009AD540/0x140734D10
static int32_t Script_CalendarEventGetInviteResponseTime(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventGetInviteResponseTime(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    EventInviteInfo* invite;

    if (CGCalendar::m_curEvent && index < CGCalendar::m_curEvent->m_numInvites && (invite = CGCalendar::m_curEvent->m_invites.m_data[index]) && invite->m_responseTime) {
        WowTime date;
        WowTime::WowDecodeTime(invite->m_responseTime, &date);
        lua_pushnumber(L, date.m_weekDay + 1);
        lua_pushnumber(L, date.m_month + 1);
        lua_pushnumber(L, date.m_monthDay + 1);
        lua_pushnumber(L, date.m_year + 2000);
        lua_pushnumber(L, date.m_hour);
        lua_pushnumber(L, date.m_minute);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 6;
}

// OFFSET: 0x009AD6B0/0x140734EB0
static int32_t Script_CalendarRemoveEvent(lua_State* L) {
    CGCalendar::RemoveEvent();
    return 0;
}

// OFFSET: 0x009AD6F0/0x140734F10
static int32_t Script_CalendarContextSelectEvent(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarContextSelectEvent([-1.0.1], monthDay, index)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    CGCalendar::SelectContextEvent(offset, day, index);
    return 0;
}

// OFFSET: 0x009AD830/0x140735150
static int32_t Script_CalendarContextInviteIsPending(lua_State* L) {
    uint8_t pending = 0;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        CalendarDayEvent* event = CGCalendar::GetDayEvent(offset, day, index);

        if (event && (event->m_flags & 0x1)) {
            pending = 1;
        }
    } else if (!CGCalendar::m_contextEventInfo) {
        luaL_error(L, "Usage: CalendarContextInviteIsPending([-1.0.1], monthDay, index)");
    }

    lua_pushboolean(L, pending);
    return 1;
}

// OFFSET: 0x009AD920/0x140735240
static int32_t Script_CalendarContextInviteModeratorStatus(lua_State* L) {
    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        CalendarDayEvent* event = CGCalendar::GetDayEvent(offset, day, index);

        if (event) {
            const char* status = NULL;

            if (event->m_flags & 0x4) {
                status = "CREATOR";
            } else if (event->m_flags & 0x2) {
                status = "MODERATOR";
            }

            if (status) {
                lua_pushstring(L, status);
                return 1;
            }
        }
    } else if (!CGCalendar::m_contextEventInfo) {
        luaL_error(L, "Usage: CalendarContextInviteModeratorStatus([-1.0.1], monthDay, index)");
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009ADA40/0x140735350
static int32_t Script_CalendarContextInviteStatus(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextInviteStatus([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (event) {
        lua_pushinteger(L, event->m_inviteStatus + 1);
    } else {
        lua_pushinteger(L, 0);
    }

    return 1;
}

// OFFSET: 0x009ADB40/0x140735440
static int32_t Script_CalendarContextInviteType(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextInviteType([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (event) {
        lua_pushinteger(L, IsSignUpInvite(event) + 1);
    } else {
        lua_pushinteger(L, 0);
    }

    return 1;
}

// OFFSET: 0x009ADC70/0x140735550
static int32_t Script_CalendarContextInviteAvailable(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);

        if (!event) {
            return 0;
        }
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextInviteAvailable([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (!CGCalendar::m_actionPending && (!(event->m_type & 0x400) || !(event->m_flags & 0x8))) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_EVENT_RSVP));
        msg.Put(event->m_eventID);
        msg.Put(event->m_inviteID);
        msg.Put(static_cast<uint32_t>(1));
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGCalendar::m_actionPending = 1;
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 1);
    }

    return 0;
}

// OFFSET: 0x009ADE20/0x140735710
static int32_t Script_CalendarContextInviteDecline(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);

        if (!event) {
            return 0;
        }
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextInviteDecline([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (!CGCalendar::m_actionPending && (!(event->m_type & 0x400) || !(event->m_flags & 0x8))) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_EVENT_RSVP));
        msg.Put(event->m_eventID);
        msg.Put(event->m_inviteID);
        msg.Put(static_cast<uint32_t>(2));
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGCalendar::m_actionPending = 1;
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 1);
    }

    return 0;
}

// OFFSET: 0x009B08B0/0x140738B30
static int32_t Script_CalendarContextEventCanEdit(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextEventCanEdit([-1.0.1], monthDay, index)");
        }
    }

    lua_pushboolean(L, CGCalendar::CanEditEvent(event));
    return 1;
}

// OFFSET: 0x009B0990
static int32_t Script_CalendarContextEventCanRemove(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: Script_CalendarContextEventCanRemove([-1.0.1], monthDay, index)");
        }
    }

    lua_pushboolean(L, CGCalendar::CanRemoveEvent(event));
    return 1;
}

// OFFSET: 0x009ADFD0/0x1407358D0
static int32_t Script_CalendarContextEventGetCalendarType(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextEventGetCalendarType([-1.0.1], monthDay, index)");
        }
    }

    if (event) {
        lua_pushstring(L, CGCalendar::GetCalendarTypeString(event->m_type));
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 31788cfe IDENTICAL/NONE
// OFFSET: 0x009AE0D0/0x1407359C0
static int32_t Script_CalendarEventRemoveInvite(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventRemoveInvite(index)");
        return 0;
    }

    CGCalendar::RemoveInvite(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x009AE140/0x140735A10
static int32_t Script_CalendarEventGetStatusOptions(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventGetStatusOptions(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGCalendarEvent* event = CGCalendar::m_curEvent;

    if (!event) {
        return 0;
    }

    if (!event->IsModerator()) {
        CGCalendar::CalendarError("CALENDAR_ERROR_PERMISSIONS", 0);
        return 0;
    }

    int32_t count = 0;

    for (uint32_t i = 0; i < 9; i++) {
        if (event->CanSetInviteStatus(index, i)) {
            lua_pushinteger(L, i + 1);
            lua_pushstring(L, FrameScript_GetText(GetEventStatusString(i), -1, GENDER_NOT_APPLICABLE));
            count += 2;
        }
    }

    return count;
}

// OFFSET: 0x009AE220/0x140735B70
static int32_t Script_CalendarEventSetTitle(lua_State* L) {
    const char* title = NULL;

    if (lua_isstring(L, 1)) {
        title = lua_tolstring(L, 1, NULL);
    }

    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->SetTitle(title);
    return 0;
}

// OFFSET: 0x009AE290/0x140735C00
static int32_t Script_CalendarEventSetDescription(lua_State* L) {
    const char* description = NULL;

    if (lua_isstring(L, 1)) {
        description = lua_tolstring(L, 1, NULL);
    }

    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->SetDescription(description);
    return 0;
}

// OFFSET: 0x009AE300/0x140735C90
static int32_t Script_CalendarEventSetType(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    uint32_t type = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->SetType(type);
    return 0;
}

// OFFSET: 0x009AE390/0x140735D10
static int32_t Script_CalendarEventSetRepeatOption(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    uint32_t repeatOption = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->SetRepeatOption(repeatOption);
    return 0;
}

// OFFSET: 0x009AE420/0x140735D90
static int32_t Script_CalendarEventSetSize(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    uint32_t size = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->SetSize(size);
    return 0;
}

// PROVEN: d92a0d07 IDENTICAL/NONE
// OFFSET: 0x009AE4B0/0x140735E10
static int32_t Script_CalendarEventSetDate(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarEventSetDate(month, day, year)");
        return 0;
    }

    uint32_t month = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t monthDay = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t year = static_cast<uint32_t>(lua_tonumber(L, 3)) - 2000;

    if (!CGCalendar::SetEventDate(month, monthDay, year)) {
        CGCalendar::CalendarError("CALENDAR_ERROR_INVALID_DATE", 0);
    }

    return 0;
}

// PROVEN: 8ad2f7d9 IDENTICAL/NONE
// OFFSET: 0x009AE5B0/0x140735F40
static int32_t Script_CalendarEventSetTime(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CalendarEventSetTime(hour, minute)");
        return 0;
    }

    uint32_t hour = static_cast<uint32_t>(lua_tonumber(L, 1));
    uint32_t minute = static_cast<uint32_t>(lua_tonumber(L, 2));

    if (!CGCalendar::SetEventTime(hour, minute)) {
        CGCalendar::CalendarError("CALENDAR_ERROR_INVALID_TIME", 0);
    }

    return 0;
}

// PROVEN: ce7dcfd0 IDENTICAL/NONE
// OFFSET: 0x009AE660/0x140736030
static int32_t Script_CalendarEventSetLockoutDate(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarEventSetLockoutDate(month, day, year)");
        return 0;
    }

    uint32_t month = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t monthDay = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t year = static_cast<uint32_t>(lua_tonumber(L, 3)) - 2000;

    if (!CGCalendar::SetEventLockoutDate(month, monthDay, year)) {
        CGCalendar::CalendarError("CALENDAR_ERROR_INVALID_DATE", 0);
    }

    return 0;
}

// PROVEN: c5c18a26 IDENTICAL/NONE
// OFFSET: 0x009AE760/0x140736160
static int32_t Script_CalendarEventSetLockoutTime(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CalendarEvenSettLockoutTime(hour, minute)");
        return 0;
    }

    uint32_t hour = static_cast<uint32_t>(lua_tonumber(L, 1));
    uint32_t minute = static_cast<uint32_t>(lua_tonumber(L, 2));

    if (!CGCalendar::SetEventLockoutTime(hour, minute)) {
        CGCalendar::CalendarError("CALENDAR_ERROR_INVALID_TIME", 0);
    }

    return 0;
}

// PROVEN: 59e3b087 IDENTICAL/NONE
// OFFSET: 0x009AE810/0x140736250
static int32_t Script_CalendarEventSetTextureID(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventSetTextureID(textureIndex)");
        return 0;
    }

    CGCalendar::SetEventTextureID(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x009AE880/0x1407362A0
static int32_t Script_CalendarEventSetLocked(lua_State* L) {
    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->SetFlag(0x10);
    return 0;
}

// OFFSET: 0x009AE8C0/0x1407362F0
static int32_t Script_CalendarEventClearLocked(lua_State* L) {
    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->ClearFlag(0x10);
    return 0;
}

// OFFSET: 0x009AE900/0x140736340
static int32_t Script_CalendarEventSetAutoApprove(lua_State* L) {
    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->SetFlag(0x20);
    return 0;
}

// OFFSET: 0x009AE940/0x140736390
static int32_t Script_CalendarEventClearAutoApprove(lua_State* L) {
    if (!CGCalendar::m_curEvent) {
        return 0;
    }

    bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

    if (guildAnnouncement) {
        if (!CGGuildInfo::CanEditGuildEvent()) {
            return 0;
        }
    } else if (!CGCalendar::m_curEvent->IsModerator()) {
        return 0;
    }

    CGCalendar::m_curEvent->ClearFlag(0x20);
    return 0;
}

// OFFSET: 0x009AE980/0x1407363E0
static int32_t Script_CalendarEventGetTextures(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarEventGetTextures(eventType)");
        return 0;
    }

    double type = lua_tonumber(L, 1);
    uint32_t list;

    if (type == 2) {
        list = 0;
    } else if (type == 1) {
        list = 1;
    } else if (type == 6) {
        list = 2;
    } else {
        return 0;
    }

    TSFixedArray<int32_t>* textures = &s_eventTextures[list];
    uint32_t count = textures->Count();

    if (!count) {
        return 0;
    }

    lua_checkstack(L, 4 * count);

    for (uint32_t i = 0; i < count; i++) {
        LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(textures->m_data[i]);

        if (dungeon) {
            if (dungeon->IsHeroic()) {
                char name[512];
                SStrPrintf(name, sizeof(name), FrameScript_GetText("HEROIC_PREFIX", -1, GENDER_NOT_APPLICABLE), dungeon->m_name);
                lua_pushstring(L, name);
            } else {
                lua_pushstring(L, dungeon->m_name);
            }

            lua_pushstring(L, dungeon->m_textureFilename);
            lua_pushinteger(L, dungeon->m_expansionLevel);
            MapDifficultyRec* difficulty;

            if (type == 1 && (difficulty = GetMapDifficultyRec(dungeon->m_mapID, dungeon->m_difficulty, NULL))) {
                lua_pushstring(L, FrameScript_GetText(difficulty->m_difficultystring, -1, GENDER_NOT_APPLICABLE));
            } else {
                lua_pushstring(L, "");
            }
        } else {
            lua_pushstring(L, "");
            lua_pushstring(L, "");
            lua_pushinteger(L, -1);
            lua_pushstring(L, "");
        }
    }

    return 4 * count;
}

// PROVEN: 5bd04f09 IDENTICAL/NONE
// OFFSET: 0x009AEB70/0x1407366A0
static int32_t Script_CalendarEventGetSelectedInvite(lua_State* L) {
    if (CGCalendar::m_curEvent) {
        lua_pushinteger(L, CGCalendar::m_curEvent->GetSelectedInviteIndex() + 1);
    } else {
        lua_pushinteger(L, 0);
    }

    return 1;
}

// OFFSET: 0x009AFE60/0x1407380D0
static int32_t Script_CalendarAddEvent(lua_State* L) {
    if ((unk_BD09AC && unk_BD09AC->m_intValue) || CGGameUI::CanPerformAction(19)) {
        CGCalendar::AddEvent();
    }

    return 0;
}

// OFFSET: 0x009AFE90/0x140738100
static int32_t Script_CalendarNewEvent(lua_State* L) {
    CGCalendar::CreatePlayerEvent();
    return 0;
}

// PROVEN: d9666a65 IDENTICAL/NONE
// OFFSET: 0x009AFEA0/0x140738110
static int32_t Script_CalendarMassInviteGuild(lua_State* L) {
    if (!lua_isnumber(L, 1) && !lua_isnumber(L, 2) && !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarMassInviteGuild(minLevel, maxLevel, minRank)");
        return 0;
    }

    uint32_t minLevel = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t maxLevel = static_cast<int32_t>(lua_tonumber(L, 2));
    uint32_t minRank = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    CGCalendar::GuildFilter(minLevel, maxLevel, minRank);
    return 0;
}

// OFFSET: 0x009AFF70/0x1407381C0
static int32_t Script_CalendarNewGuildAnnouncement(lua_State* L) {
    CGCalendar::CreateGuildAnnouncement();
    return 0;
}

// OFFSET: 0x009AFF80/0x1407381D0
static int32_t Script_CalendarNewGuildEvent(lua_State* L) {
    CGCalendar::CreateGuildEvent();
    return 0;
}

// PROVEN: 4e55717f IDENTICAL/NONE
// OFFSET: 0x009B0050/0x140738290
static int32_t Script_CalendarMassInviteArenaTeam(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarNewArenaTeamEvent(index)");
        return 0;
    }

    CGCalendar::ArenaTeam(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x009B00C0/0x1407382E0
static int32_t Script_CalendarUpdateEvent(lua_State* L) {
    if ((unk_BD09AC && unk_BD09AC->m_intValue) || CGGameUI::CanPerformAction(19)) {
        CGCalendar::UpdateEvent();
    }

    return 0;
}

// PROVEN: 5e6b659f IDENTICAL/NONE
// OFFSET: 0x009B00F0/0x140738310
static int32_t Script_CalendarEventInvite(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);

    if (name) {
        CGCalendar::Invite(name);
    }

    return 0;
}

// OFFSET: 0x009B0120/0x140738340
static int32_t Script_CalendarContextInviteTentative(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);

        if (!event) {
            return 0;
        }
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: Script_CalendarContextInviteTentative([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if ((event->m_type & 0x400) && (event->m_flags & 0x8)) {
        CGCalendar::ContextMenuEventSignUp(event, 1);
    } else {
        CGCalendar::ContextMenuInviteRSVP(event, 8);
    }

    return 0;
}

// OFFSET: 0x009B0230/0x140738440
static int32_t Script_CalendarContextInviteRemove(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);

        if (!event) {
            return 0;
        }
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextInviteRemove([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (event->m_type & 0x401) {
        CGCalendar::ContextMenuInviteRemove(event);
    }

    return 0;
}

// OFFSET: 0x009B0320/0x140738520
static int32_t Script_CalendarContextEventSignUp(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);

        if (!event) {
            return 0;
        }
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextEventSignUp([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (event->m_type & 0x400) {
        CGCalendar::ContextMenuEventSignUp(event, 0);
    }

    return 0;
}

// OFFSET: 0x009B0410/0x140738600
static int32_t Script_CalendarContextEventRemove(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);

        if (!event) {
            return 0;
        }
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: Script_CalendarContextEventRemove([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (CGCalendar::CanRemoveEvent(event)) {
        CGCalendar::ContextMenuEventRemove(event);
    } else {
        CGCalendar::CalendarError("CALENDAR_ERROR_PERMISSIONS", 0);
    }

    return 0;
}

// OFFSET: 0x009B0520/0x140738770
static int32_t Script_CalendarContextEventCopy(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);

        if (!event) {
            return 0;
        }
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: Script_CalendarContextEventCopy([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    if (CGCalendar::CanEditEvent(event)) {
        CGCalendar::ContextMenuEventCopy(event);
    } else {
        CGCalendar::CalendarError("CALENDAR_ERROR_PERMISSIONS", 0);
    }

    return 0;
}

// OFFSET: 0x009B0630/0x1407388E0
static int32_t Script_CalendarContextEventPaste(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: Script_CalendarContextEventPaste([-1.0.1], monthDay)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    CGCalendar::ContextMenuEventPaste(offset, day);
    return 0;
}

// OFFSET: 0x009B06C0/0x140738960
static int32_t Script_CalendarContextEventCanComplain(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextEventCanComplain([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    lua_pushboolean(L, !CGCalendar::m_actionPending && CGCalendar::CanComplainAboutEvent(event));
    return 1;
}

// OFFSET: 0x009B07D0/0x140738A60
static int32_t Script_CalendarContextEventComplain(lua_State* L) {
    CalendarDayEvent* event;

    if (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
        int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
        uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
        event = CGCalendar::GetDayEvent(offset, day, index);
    } else {
        event = CGCalendar::m_contextEventInfo;

        if (!event) {
            luaL_error(L, "Usage: CalendarContextEventComplain([-1.0.1], monthDay, index)");
            return 0;
        }
    }

    CGCalendar::Complain(event);
    return 0;
}

// OFFSET: 0x009B0A70
static int32_t Script_CalendarEventAvailable(lua_State* L) {
    CGCalendar::InviteRSVP(1);
    return 0;
}

// PROVEN: d8d082fe IDENTICAL/NONE
// OFFSET: 0x009B0A80/0x140738D70
static int32_t Script_CalendarEventTentative(lua_State* L) {
    CGCalendar::InviteRSVP(8);
    return 0;
}

// OFFSET: 0x009B0A90
static int32_t Script_CalendarEventDecline(lua_State* L) {
    CGCalendar::InviteRSVP(2);
    return 0;
}

// OFFSET: 0x009B0AA0/0x140738E30
static int32_t Script_CalendarEventSignUp(lua_State* L) {
    CGCalendar::EventSignUp();
    return 0;
}

// PROVEN: 12a0a0b9 IDENTICAL/NONE
// OFFSET: 0x009AEC90/0x1407367D0
static int32_t Script_OpenCalendar(lua_State* L) {
    if (!CGCalendar::m_openedCalendar) {
        CGCalendar::GetCalendar();
    }

    return 0;
}

// OFFSET: 0x009B0B50/0x140738F40
static int32_t Script_CalendarGetEventInfo(lua_State* L) {
    CGCalendarEvent* event = CGCalendar::m_curEvent;

    if (!event) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushstring(L, "");

        for (uint32_t i = 0; i < 16; i++) {
            lua_pushnumber(L, 0.0);
        }

        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        return 25;
    }

    char title[128];
    SStrCopy(title, event->m_title, STORM_MAX_STR);
    ProfanityFilter(title, false, false);
    lua_pushstring(L, title);
    char description[1024];
    SStrCopy(description, event->m_description, STORM_MAX_STR);
    ProfanityFilter(description, false, false);
    lua_pushstring(L, description);
    WOWGUID key = WOWGUID();
    NameCache* creator = g_nameDBCache.GetRecord(event->m_creatorGUID.m_guid, key, NameCacheCallback, NULL, false);
    lua_pushstring(L, creator ? creator->m_name : "");
    lua_pushnumber(L, static_cast<int32_t>(event->m_type + 1));
    lua_pushnumber(L, static_cast<int32_t>(event->m_repeatOption + 1));
    lua_pushnumber(L, event->m_maxSize);
    lua_pushnumber(L, CGCalendar::GetTextureIndex(event->m_type, event->m_textureID) + 1);
    lua_pushnumber(L, event->m_date.m_weekDay + 1);
    lua_pushnumber(L, event->m_date.m_month + 1);
    lua_pushnumber(L, event->m_date.m_monthDay + 1);
    lua_pushnumber(L, event->m_date.m_year + 2000);
    lua_pushnumber(L, event->m_date.m_hour);
    lua_pushnumber(L, event->m_date.m_minute);
    lua_pushnumber(L, event->m_lockoutDate.m_weekDay + 1);
    lua_pushnumber(L, event->m_lockoutDate.m_month + 1);
    lua_pushnumber(L, event->m_lockoutDate.m_monthDay + 1);
    lua_pushnumber(L, event->m_lockoutDate.m_year + 2000);
    lua_pushnumber(L, event->m_lockoutDate.m_hour);
    lua_pushnumber(L, event->m_lockoutDate.m_minute);

    if (event->m_flags & 0x10) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (event->m_flags & 0x20) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (CGCalendar::HasPendingInvite(event->m_eventID)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    EventInviteInfo* invite = event->GetInvite(ClntObjMgrGetActivePlayer());

    if (invite) {
        lua_pushnumber(L, invite->m_status + 1);
        lua_pushnumber(L, invite->m_inviteType + 1);
    } else if (event->m_flags & 0x400) {
        lua_pushnumber(L, 8.0);
        lua_pushnumber(L, 2.0);
    } else {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    lua_pushstring(L, CGCalendar::GetCalendarTypeString(event->m_flags));
    return 25;
}

// PROVEN: 1df50f6e IDENTICAL/NONE
// OFFSET: 0x009B10A0/0x140739550
static int32_t Script_CalendarEventHasPendingInvite(lua_State* L) {
    if (CGCalendar::m_curEvent) {
        lua_pushboolean(L, CGCalendar::HasPendingInvite(CGCalendar::m_curEvent->m_eventID));
    } else {
        lua_pushboolean(L, 0);
    }

    return 1;
}

// OFFSET: #0x005C0FB0
static void PushDayEventTexture(lua_State* L, CalendarDayEvent* event) {
    const char* texture = "";

    if (event) {
        if (event->m_type & 0x8) {
            DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(static_cast<uint32_t>(event->m_eventID), HASHKEY_NONE());

            if (dynamic) {
                lua_pushstring(L, dynamic->m_textureFileName);
                return;
            }

            HolidaysRec* holiday = g_holidaysDB.GetRecord(static_cast<int32_t>(event->m_eventID));

            if (holiday) {
                lua_pushstring(L, holiday->m_textureFileName);
                return;
            }
        } else if ((event->m_type & 0x543) && event->m_textureID >= 0) {
            LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(event->m_textureID);

            if (dungeon) {
                texture = dungeon->m_textureFilename;
            }
        }
    }

    lua_pushstring(L, texture);
}

// OFFSET: 0x009B2970
static int32_t Script_CalendarGetGuildEventInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CalendarGetGuildEventInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CalendarDayEvent* event;

    if (index < s_guildEvents.Count() && (event = s_guildEvents.m_data[index]) != NULL) {
        char title[128];
        SStrCopy(title, event->m_title, sizeof(title));
        ProfanityFilter(title, false, false);
        WowTime start;
        WowTime::WowDecodeTime(event->m_startTime, &start);
        lua_pushnumber(L, start.m_month + 1);
        lua_pushnumber(L, start.m_monthDay + 1);
        lua_pushnumber(L, start.m_weekDay + 1);
        lua_pushnumber(L, start.m_hour);
        lua_pushnumber(L, start.m_minute);
        lua_pushnumber(L, event->m_eventType + 1);
        lua_pushstring(L, title);
        lua_pushstring(L, CGCalendar::GetCalendarTypeString(event->m_type));
        PushDayEventTexture(L, event);
        return 9;
    }

    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    lua_pushstring(L, "");
    return 9;
}

// OFFSET: 0x009B2550
static int32_t Script_CalendarGetDayEvent(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarGetDayEvent([-1.0.1], monthDay, index)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    CalendarDayEvent* event = CGCalendar::GetDayEvent(offset, day, index);

    if (!event) {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        return 15;
    }

    WowTime start;
    WowTime::WowDecodeTime(event->m_startTime, &start);

    if (event->m_type & 0x8) {
        lua_pushstring(L, GetHolidayName(static_cast<int32_t>(event->m_eventID)));
    } else if (event->m_type & 0x280) {
        MapRec* map = g_mapDB.GetRecord(event->m_holidayID);

        if (map) {
            lua_pushstring(L, map->m_mapName);
        } else {
            lua_pushstring(L, "");
        }
    } else {
        char title[128];
        SStrCopy(title, event->m_title, STORM_MAX_STR);
        ProfanityFilter(title, false, false);
        lua_pushstring(L, title);
    }

    lua_pushnumber(L, start.m_hour);
    lua_pushnumber(L, start.m_minute);
    lua_pushstring(L, CGCalendar::GetCalendarTypeString(event->m_type));
    lua_pushstring(L, GetSequenceLabel(event->m_type, event->m_sequenceIndex, event->m_sequenceLength));
    lua_pushnumber(L, event->m_eventType + 1);
    PushDayEventTexture(L, event);
    lua_pushstring(L, GetInviteRoleLabel(event->m_flags));
    lua_pushnumber(L, event->m_inviteStatus + 1);
    WOWGUID key = WOWGUID();
    NameCache* creator = g_nameDBCache.GetRecord(event->m_creatorGUID.m_guid, key, NameCacheCallback, NULL, false);
    lua_pushstring(L, creator ? creator->m_name : "");
    lua_pushnumber(L, (event->m_type & 0x280) ? static_cast<int32_t>(event->m_endTime + 1) : 0.0);
    lua_pushnumber(L, IsSignUpInvite(event) + 1);
    lua_pushnumber(L, event->m_sequenceIndex + 1);
    lua_pushnumber(L, event->m_sequenceLength);
    MapDifficultyRec* difficulty;

    if (!(event->m_type & 0x280) || !(difficulty = GetMapDifficultyRec(event->m_holidayID, event->m_endTime, NULL))) {
        lua_pushstring(L, "");
        return 15;
    }

    lua_pushstring(L, FrameScript_GetText(difficulty->m_difficultystring, -1, GENDER_NOT_APPLICABLE));
    return 15;
}

// OFFSET: 0x009B2B80/0x14073B820
static int32_t Script_CalendarGetHolidayInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: CalendarGetHolidayInfo([-1,0,1], monthDay, index)");
        return 0;
    }

    int32_t offset = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t day = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    CalendarDayEvent* event = CGCalendar::GetDayEvent(offset, day, index);
    const char* name;
    const char* description;

    if (!event || !(name = GetHolidayName(static_cast<int32_t>(event->m_eventID)), description = GetHolidayDescription(static_cast<int32_t>(event->m_eventID)), name) || !description) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushstring(L, name);
    lua_pushstring(L, description);
    const char* texture = "";
    DYNAMICHOLIDAYHASH* dynamic = CalendarGetHolidayInfo(static_cast<int32_t>(event->m_eventID));

    if (dynamic) {
        texture = dynamic->m_textureFileName;
    } else {
        HolidaysRec* holiday = g_holidaysDB.GetRecord(static_cast<int32_t>(event->m_eventID));

        if (holiday) {
            texture = holiday->m_textureFileName;
        }
    }

    lua_pushstring(L, texture);
    return 3;
}

// STATIC: 0x00D15458
static FrameScript_Method s_ScriptFunctions[] = {
    { "CalendarGetMonthNames", &Script_CalendarGetMonthNames },
    { "CalendarGetWeekdayNames", &Script_CalendarGetWeekdayNames },
    { "CalendarGetDate", &Script_CalendarGetDate },
    { "CalendarGetMinDate", &Script_CalendarGetMinDate },
    { "CalendarGetMaxDate", &Script_CalendarGetMaxDate },
    { "CalendarGetMinHistoryDate", &Script_CalendarGetMinHistoryDate },
    { "CalendarGetMaxCreateDate", &Script_CalendarGetMaxCreateDate },
    { "CalendarGetMonth", &Script_CalendarGetMonth },
    { "CalendarGetAbsMonth", &Script_CalendarGetAbsMonth },
    { "CalendarSetMonth", &Script_CalendarSetMonth },
    { "CalendarSetAbsMonth", &Script_CalendarSetAbsMonth },
    { "CalendarGetNumDayEvents", &Script_CalendarGetNumDayEvents },
    { "CalendarGetDayEvent", &Script_CalendarGetDayEvent },
    { "CalendarGetDayEventSequenceInfo", &Script_CalendarGetDayEventSequenceInfo },
    { "CalendarGetFirstPendingInvite", &Script_CalendarGetFirstPendingInvite },
    { "CalendarGetNumGuildEvents", &Script_CalendarGetNumGuildEvents },
    { "CalendarGetGuildEventInfo", &Script_CalendarGetGuildEventInfo },
    { "CalendarGetGuildEventSelectionInfo", &Script_CalendarGetGuildEventSelectionInfo },
    { "CalendarOpenEvent", &Script_CalendarOpenEvent },
    { "CalendarGetEventIndex", &Script_CalendarGetEventIndex },
    { "CalendarCloseEvent", &Script_CalendarCloseEvent },
    { "CalendarGetEventInfo", &Script_CalendarGetEventInfo },
    { "CalendarGetHolidayInfo", &Script_CalendarGetHolidayInfo },
    { "CalendarGetRaidInfo", &Script_CalendarGetRaidInfo },
    { "CalendarGetNumPendingInvites", &Script_CalendarGetNumPendingInvites },
    { "CalendarEventGetNumInvites", &Script_CalendarEventGetNumInvites },
    { "CalendarEventGetInvite", &Script_CalendarEventGetInvite },
    { "CalendarEventGetInviteResponseTime", &Script_CalendarEventGetInviteResponseTime },
    { "CalendarAddEvent", &Script_CalendarAddEvent },
    { "CalendarNewEvent", &Script_CalendarNewEvent },
    { "CalendarMassInviteGuild", &Script_CalendarMassInviteGuild },
    { "CalendarMassInviteArenaTeam", &Script_CalendarMassInviteArenaTeam },
    { "CalendarNewGuildAnnouncement", &Script_CalendarNewGuildAnnouncement },
    { "CalendarNewGuildEvent", &Script_CalendarNewGuildEvent },
    { "CalendarDefaultGuildFilter", &Script_CalendarDefaultGuildFilter },
    { "CalendarUpdateEvent", &Script_CalendarUpdateEvent },
    { "CalendarRemoveEvent", &Script_CalendarRemoveEvent },
    { "CalendarEventSelectInvite", &Script_CalendarEventSelectInvite },
    { "CalendarEventGetSelectedInvite", &Script_CalendarEventGetSelectedInvite },
    { "CalendarContextSelectEvent", &Script_CalendarContextSelectEvent },
    { "CalendarContextDeselectEvent", &Script_CalendarContextDeselectEvent },
    { "CalendarContextGetEventIndex", &Script_CalendarContextGetEventIndex },
    { "CalendarContextInviteIsPending", &Script_CalendarContextInviteIsPending },
    { "CalendarContextInviteModeratorStatus", &Script_CalendarContextInviteModeratorStatus },
    { "CalendarContextInviteStatus", &Script_CalendarContextInviteStatus },
    { "CalendarContextInviteType", &Script_CalendarContextInviteType },
    { "CalendarContextInviteAvailable", &Script_CalendarContextInviteAvailable },
    { "CalendarContextInviteTentative", &Script_CalendarContextInviteTentative },
    { "CalendarContextInviteDecline", &Script_CalendarContextInviteDecline },
    { "CalendarContextInviteRemove", &Script_CalendarContextInviteRemove },
    { "CalendarContextEventSignUp", &Script_CalendarContextEventSignUp },
    { "CalendarContextEventRemove", &Script_CalendarContextEventRemove },
    { "CalendarContextEventCopy", &Script_CalendarContextEventCopy },
    { "CalendarContextEventPaste", &Script_CalendarContextEventPaste },
    { "CalendarContextEventClipboard", &Script_CalendarContextEventClipboard },
    { "CalendarContextEventCanComplain", &Script_CalendarContextEventCanComplain },
    { "CalendarContextEventComplain", &Script_CalendarContextEventComplain },
    { "CalendarContextEventCanEdit", &Script_CalendarContextEventCanEdit },
    { "CalendarContextEventCanRemove", &Script_CalendarContextEventCanRemove },
    { "CalendarContextEventGetCalendarType", &Script_CalendarContextEventGetCalendarType },
    { "CalendarEventInvite", &Script_CalendarEventInvite },
    { "CalendarEventRemoveInvite", &Script_CalendarEventRemoveInvite },
    { "CalendarEventAvailable", &Script_CalendarEventAvailable },
    { "CalendarEventTentative", &Script_CalendarEventTentative },
    { "CalendarEventDecline", &Script_CalendarEventDecline },
    { "CalendarEventSignUp", &Script_CalendarEventSignUp },
    { "CalendarEventSortInvites", &Script_CalendarEventSortInvites },
    { "CalendarEventGetInviteSortCriterion", &Script_CalendarEventGetInviteSortCriterion },
    { "CalendarEventGetStatusOptions", &Script_CalendarEventGetStatusOptions },
    { "CalendarEventSetStatus", &Script_CalendarEventSetStatus },
    { "CalendarEventSetModerator", &Script_CalendarEventSetModerator },
    { "CalendarEventClearModerator", &Script_CalendarEventClearModerator },
    { "CalendarEventCanModerate", &Script_CalendarEventCanModerate },
    { "CalendarEventIsModerator", &Script_CalendarEventIsModerator },
    { "CalendarEventGetTypes", &Script_CalendarEventGetTypes },
    { "CalendarEventGetTypesDisplayOrdered", &Script_CalendarEventGetTypesDisplayOrdered },
    { "CalendarEventGetRepeatOptions", &Script_CalendarEventGetRepeatOptions },
    { "CalendarEventSetTitle", &Script_CalendarEventSetTitle },
    { "CalendarEventSetDescription", &Script_CalendarEventSetDescription },
    { "CalendarEventSetType", &Script_CalendarEventSetType },
    { "CalendarEventSetRepeatOption", &Script_CalendarEventSetRepeatOption },
    { "CalendarEventSetSize", &Script_CalendarEventSetSize },
    { "CalendarEventSetDate", &Script_CalendarEventSetDate },
    { "CalendarEventSetTime", &Script_CalendarEventSetTime },
    { "CalendarEventSetLockoutDate", &Script_CalendarEventSetLockoutDate },
    { "CalendarEventSetLockoutTime", &Script_CalendarEventSetLockoutTime },
    { "CalendarEventSetTextureID", &Script_CalendarEventSetTextureID },
    { "CalendarEventSetLocked", &Script_CalendarEventSetLocked },
    { "CalendarEventClearLocked", &Script_CalendarEventClearLocked },
    { "CalendarEventSetAutoApprove", &Script_CalendarEventSetAutoApprove },
    { "CalendarEventClearAutoApprove", &Script_CalendarEventClearAutoApprove },
    { "CalendarEventGetTextures", &Script_CalendarEventGetTextures },
    { "CalendarEventHasPendingInvite", &Script_CalendarEventHasPendingInvite },
    { "CalendarEventHaveSettingsChanged", &Script_CalendarEventHaveSettingsChanged },
    { "CalendarEventCanEdit", &Script_CalendarEventCanEdit },
    { "CalendarEventGetCalendarType", &Script_CalendarEventGetCalendarType },
    { "CalendarCanSendInvite", &Script_CalendarCanSendInvite },
    { "CalendarCanAddEvent", &Script_CalendarCanAddEvent },
    { "CalendarIsActionPending", &Script_CalendarIsActionPending },
    { "OpenCalendar", &Script_OpenCalendar },
};

// OFFSET: 0x009AB900
void CalendarRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009AB930
void CalendarUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}
