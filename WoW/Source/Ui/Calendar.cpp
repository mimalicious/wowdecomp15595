#include "Calendar.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/HolidayNamesRec.h"
#include "WoW/Source/DB/Rec/HolidaysRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "WoW/Source/Ui/CalendarEvent.h"
#include "WoW/Common/WowTime.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/DB/Rec/HolidayDescriptionsRec.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Object/Player.h"
#include "Engine/Source/Event/FrameTime.h"
#include <malloc.h>

// STATIC: 0x00D1579C/0x140BCF020
static TSList<CalendarPendingInvite, TSGetLink<CalendarPendingInvite> > s_pendingInvites;

// STATIC: 0x00D157A8/0x140BCF090
CalendarEventData s_eventClipboard;

// STATIC: 0x00D15784/0x140BCEFF0
static TSList<CALENDARHOLIDAYNODE, TSGetLink<CALENDARHOLIDAYNODE> > s_holidays;

// STATIC: 0x00D15790/0x140BCF008
static TSList<CALENDARRAIDRESETNODE, TSGetLink<CALENDARRAIDRESETNODE> > s_raidResets;

// STATIC: 0x00F4CDC0
static TSGrowableArray<CalendarDayEvent*> s_days[3][31];

// STATIC: 0x00F4D390/0x140E41C68
TSGrowableArray<CalendarDayEvent*> s_guildEvents;

// STATIC: 0x00F4D3A0/0x140E41C80
static CalendarEventTables s_eventTables;

// STATIC: 0x00F4CD94/0x140BCF040
TSHashTable<DYNAMICHOLIDAYHASH, HASHKEY_NONE> s_dynamicHolidays;

// STATIC: 0x00F4D520
TSFixedArray<int32_t> s_eventTextures[3];

// STATIC: 0x00F4CD7C/0x140E41388
CGCalendarEvent* CGCalendar::m_curEvent;

// STATIC: 0x00F4CD80/0x140E41390
uint8_t CGCalendar::m_initialize;

// STATIC: 0x00F4CD81/0x140E41391
uint8_t CGCalendar::m_openedCalendar;

// STATIC: 0x00F4CD82
uint8_t CGCalendar::m_calendarReceived;

// STATIC: 0x00F4CD78/0x140E41380
CalendarDayEvent* CGCalendar::m_contextEventInfo;

// STATIC: 0x00F4CD8C/0x140E4139C
int32_t CGCalendar::m_lastEventTime;

// STATIC: 0x00F4CD84/0x140E41394
int32_t CGCalendar::m_numInitialPending;

// STATIC: 0x00F4CD83/0x140E41393
uint8_t CGCalendar::m_actionPending;

// STATIC: 0x00D153F0/0x140BCE920
static const char* s_fullDateMonthNames[12] = { "FULLDATE_MONTH_JANUARY", "FULLDATE_MONTH_FEBRUARY", "FULLDATE_MONTH_MARCH", "FULLDATE_MONTH_APRIL", "FULLDATE_MONTH_MAY", "FULLDATE_MONTH_JUNE", "FULLDATE_MONTH_JULY", "FULLDATE_MONTH_AUGUST", "FULLDATE_MONTH_SEPTEMBER", "FULLDATE_MONTH_OCTOBER", "FULLDATE_MONTH_NOVEMBER", "FULLDATE_MONTH_DECEMBER" };

// STATIC: 0x00D15398/0x140BCE840
int32_t CGCalendar::m_absMonth = -1;

// STATIC: 0x00D1539C/0x140BCE844
int32_t CGCalendar::m_absYear = 4;

// STATIC: 0x00F4CD90/0x140E413A0
int32_t CGCalendar::m_absMonthDays;

// STATIC: 0x00F4D548/0x140E41F10
CalendarEventSelection CGCalendar::m_selectedEvent;

// STATIC: 0x00F4D560/0x140E41F28
CalendarEventSelection CGCalendar::m_contextEvent;

// STATIC: 0x00F4CD88/0x140E41398
int32_t CGCalendar::m_lastInviteTime;

// STATIC: 0x00D153A0
static CVar** s_eventFilters[8] = { NULL, NULL, &s_cvCalendarShowLockouts, &s_cvCalendarShowResets, NULL, &s_cvCalendarShowWeeklyHolidays, &s_cvCalendarShowDarkmoon, &s_cvCalendarShowBattlegrounds };

// OFFSET: 0x009AB9F0
CalendarDayEvent::~CalendarDayEvent() {
    if (this->m_alarmCallback) {
        CGameTime::GameTimeUnregisterCallback(this->m_alarmCallback);
        this->m_alarmCallback = NULL;
    }
}

// OFFSET: 0x009B79D0/0x140741E70
void CGCalendar::InitializeGame() {
    CGCalendar::m_curEvent = NULL;
    CGCalendar::m_initialize = 1;
    CGCalendar::m_openedCalendar = 0;
    CGCalendar::m_calendarReceived = 0;
    CGCalendar::m_numInitialPending = 0;

    ClientServices::SetMessageHandler(SMSG_CALENDAR_SEND_CALENDAR, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_SEND_EVENT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_STATUS, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_FILTER_GUILD, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_ARENA_TEAM, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_ADDED, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_ALERT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_REMOVED_ALERT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_UPDATED_ALERT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_SEND_NUM_PENDING, PH_CALENDAR_GROUP, NULL);
    ClientServices::SetMessageHandler(SMSG_CALENDAR_CLEAR_PENDING_ACTION, PH_CALENDAR_GROUP, NULL);
}

// OFFSET: 0x009AC2F0/0x140733550
const char* GetHolidayName(int32_t holidayId) {
    HolidaysRec* holiday = g_holidaysDB.GetRecord(holidayId);

    if (!holiday) {
        return NULL;
    }

    HolidayNamesRec* name = g_holidayNamesDB.GetRecord(holiday->m_holidayNameID);
    return name ? static_cast<const char*>(name->m_name) : NULL;
}

// OFFSET: 0x009AC860/0x140733C20
size_t CGCalendar::GetDateTimeText(char* buffer, uint32_t size, uint32_t time) {
    WowTime date;
    WowTime::WowDecodeTime(time, &date);
    char dateText[256] = { 0 };

    if (date.m_year >= 0 && date.m_month >= 0 && date.m_monthDay >= 0 && date.m_weekDay >= 0) {
        SStrPrintf(dateText, sizeof(dateText), FrameScript_GetText("FULLDATE", -1, GENDER_NOT_APPLICABLE), FrameScript_GetText(s_weekdayNames[date.m_weekDay], -1, GENDER_NOT_APPLICABLE), FrameScript_GetText(s_fullDateMonthNames[date.m_month], -1, GENDER_NOT_APPLICABLE), date.m_monthDay + 1, date.m_year + 2000, date.m_month + 1);
    }

    char timeText[64] = { 0 };
    int32_t hour = date.m_hour;

    if (hour >= 0) {
        int32_t minute = date.m_minute;

        if (minute >= 0) {
            if (CVar::Lookup("timeMgrUseMilitaryTime")->m_intValue) {
                SStrPrintf(timeText, sizeof(timeText), FrameScript_GetText("TIME_TWENTYFOURHOURS", -1, GENDER_NOT_APPLICABLE), hour, minute);
            } else {
                const char* token = "TIME_TWELVEHOURAM";

                if (!hour) {
                    hour = 12;
                } else if (hour == 12) {
                    token = "TIME_TWELVEHOURPM";
                } else if (hour > 12) {
                    hour -= 12;
                    token = "TIME_TWELVEHOURPM";
                }

                SStrPrintf(timeText, sizeof(timeText), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), hour, minute);
            }
        }
    }

    return SStrPrintf(buffer, size, FrameScript_GetText("FULLDATE_AND_TIME", -1, GENDER_NOT_APPLICABLE), dateText, timeText);
}

// OFFSET: 0x009ABB30/0x140732CF0
static int32_t QSortTextures(const void* a, const void* b) {
    LfgDungeonsRec* recA = g_lfgDungeonsDB.GetRecord(*static_cast<const int32_t*>(a));
    LfgDungeonsRec* recB = g_lfgDungeonsDB.GetRecord(*static_cast<const int32_t*>(b));
    int32_t result = recB->m_expansionLevel - recA->m_expansionLevel;

    if (!result) {
        return SStrSortUTF8I(recA->m_name, recB->m_name, STORM_MAX_STR);
    }

    return result;
}

// OFFSET: 0x009AC7D0/0x140733B00
void CGCalendar::CalendarError(const char* error, uintptr_t arg) {
    char buffer[3000];

    if (arg) {
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText(error, -1, GENDER_NOT_APPLICABLE), arg);
    } else {
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText(error, -1, GENDER_NOT_APPLICABLE));
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_ERROR, "%s", buffer);
    CGCalendar::m_actionPending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 0);
}

// OFFSET: 0x009ABD10/0x140732F40
void CGCalendar::QueryPendingInvites() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_GET_NUM_PENDING));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009B1260/0x140739840
void CGCalendar::EnterWorld() {
    CGCalendar::m_actionPending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 0);

    if (!CGCalendar::m_initialize) {
        return;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    memset(&s_eventClipboard, 0, sizeof(s_eventClipboard));
    CGCalendar::QueryPendingInvites();

    uint32_t counts[3] = { 0, 0, 0 };
    int32_t numRecords = g_lfgDungeonsDB.m_numRecords;

    for (int32_t i = 0; i < numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        if (rec->m_faction >= 0) {
            FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);
            int32_t group = (faction->m_factionGroup & 0x4) ? 0 : ((faction->m_factionGroup & 0x2) ? 1 : -1);

            if (group != rec->m_faction) {
                continue;
            }
        }

        if (rec->m_typeID == 1) {
            if (rec->IsHeroic()) {
                counts[2]++;
            } else {
                counts[0]++;
            }
        } else if (rec->m_typeID == 2) {
            counts[1]++;
        }
    }

    for (uint32_t i = 0; i < 3; i++) {
        s_eventTextures[i].SetCount(counts[i]);
    }

    counts[0] = 0;
    counts[1] = 0;
    counts[2] = 0;

    for (int32_t i = 0; i < numRecords; i++) {
        LfgDungeonsRec* rec = g_lfgDungeonsDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        if (rec->m_faction >= 0) {
            FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);
            int32_t group = (faction->m_factionGroup & 0x4) ? 0 : ((faction->m_factionGroup & 0x2) ? 1 : -1);

            if (group != rec->m_faction) {
                continue;
            }
        }

        if (rec->m_typeID == 1) {
            if (rec->IsHeroic()) {
                s_eventTextures[2].m_data[counts[2]++] = rec->m_id;
            } else {
                s_eventTextures[0].m_data[counts[0]++] = rec->m_id;
            }
        } else if (rec->m_typeID == 2) {
            s_eventTextures[1].m_data[counts[1]++] = rec->m_id;
        }
    }

    qsort(s_eventTextures[0].m_data, counts[0], sizeof(int32_t), QSortTextures);
    qsort(s_eventTextures[1].m_data, counts[1], sizeof(int32_t), QSortTextures);
    qsort(s_eventTextures[2].m_data, counts[2], sizeof(int32_t), QSortTextures);
    CGCalendar::m_initialize = 0;
}

// OFFSET: 0x009B0AB0/0x140738ED0
uint32_t CGCalendar::CountPendingInvites() {
    uint32_t count = 0;
    CalendarPendingInvite* next;

    for (CalendarPendingInvite* invite = s_pendingInvites.Head(); !s_pendingInvites.IsEnd(invite); invite = next) {
        WOWGUID guid = invite->m_inviterGuid;
        next = invite->RawNext();

        if (guid && !FriendList::s_friendList->IsIgnored(guid, false)) {
            count++;
        }
    }

    uint32_t pending = CGCalendar::m_numInitialPending;

    if (count > pending) {
        return count;
    }

    return pending;
}

// OFFSET: 0x009B7790/0x140741C60
int32_t PH_CALENDAR_GROUP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t clearPending = 1;

    switch (msgId) {
        case SMSG_CALENDAR_SEND_CALENDAR:
            PH_SMSG_CALENDAR_SEND_CALENDAR(msg);
            break;

        case SMSG_CALENDAR_SEND_EVENT:
            CGCalendar::ProcessGetEvent(msg);
            break;

        case SMSG_CALENDAR_FILTER_GUILD:
        case SMSG_CALENDAR_ARENA_TEAM:
            CGCalendar::ProcessInitialInviteList(msg);
            break;

        case SMSG_CALENDAR_EVENT_INVITE:
            CGCalendar::ProcessInvite(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_INVITE_REMOVED:
            CGCalendar::ProcessInviteRemoved(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_STATUS:
            CGCalendar::ProcessInviteStatus(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_RAID_LOCKOUT_ADDED:
            CGCalendar::ProcessLockoutAdded(msg);
            break;

        case SMSG_CALENDAR_RAID_LOCKOUT_REMOVED:
            CGCalendar::ProcessLockoutRemoved(msg);
            break;

        case SMSG_CALENDAR_EVENT_INVITE_ALERT:
            CGCalendar::ProcessInviteAlert(msg);
            return 1;

        case SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT:
            CGCalendar::ProcessInviteRemovedAlert(msg);
            return 1;

        case SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT:
            CGCalendar::ProcessInviteStatusAlert(msg);
            return 1;

        case SMSG_CALENDAR_EVENT_REMOVED_ALERT:
            CGCalendar::ProcessEventRemovedAlert(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_UPDATED_ALERT:
            CGCalendar::ProcessEventUpdatedAlert(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT:
            CGCalendar::ProcessInviteModeratorStatusAlert(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_SEND_NUM_PENDING:
            CGCalendar::UpdatePendingInvites(msg);
            break;

        case SMSG_CALENDAR_EVENT_INVITE_NOTES:
            CGCalendar::ProcessInviteNotes(msg, &clearPending);

            if (!clearPending) {
                return 1;
            }

            break;

        case SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT:
            CGCalendar::ProcessInviteNotesAlert(msg);
            return 1;

        case SMSG_CALENDAR_RAID_LOCKOUT_UPDATED:
            CGCalendar::ProcessLockoutUpdated(msg);
            break;
    }

    CGCalendar::m_actionPending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 0);
    return 1;
}

// OFFSET: 0x009B5630/0x14073F010
CalendarEventTables::CalendarEventTables() {
    this->m_counts[0] = 0;
    this->m_counts[1] = 0;
    this->m_counts[2] = 0;
    this->m_counts[3] = 0;
    this->m_counts[4] = 0;
    this->m_counts[5] = 0;
    this->m_counts[6] = 0;
    this->m_counts[7] = 0;
}

// OFFSET: 0x009B3030/0x14073BD60
void CalendarEventTables::Clear() {
    for (uint32_t i = 0; i < 8; i++) {
        this->m_counts[i] = 0;
        this->m_tables[i].InternalClear(0);
    }
}

// OFFSET: 0x009B4210/0x14073D350
void CGCalendar::ShutdownGame() {
    if (CGCalendar::m_curEvent) {
        delete CGCalendar::m_curEvent;
    }

    memset(&s_eventClipboard, 0, sizeof(s_eventClipboard));
    s_eventTables.Clear();
    s_holidays.Clear();
    s_raidResets.Clear();
    s_pendingInvites.Clear();
    s_dynamicHolidays.InternalClear(0);

    for (uint32_t month = 0; month < 3; month++) {
        for (uint32_t day = 0; day < 31; day++) {
            s_days[month][day].m_count = 0;
        }
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (s_eventTextures[i].m_count) {
            s_eventTextures[i].Clear();
        }
    }

    ClientServices::ClearMessageHandler(SMSG_CALENDAR_SEND_CALENDAR);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_SEND_EVENT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_STATUS);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_FILTER_GUILD);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_ARENA_TEAM);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_ADDED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_REMOVED_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_UPDATED_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_EVENT_MODERATOR_STATUS_ALERT);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_SEND_NUM_PENDING);
    ClientServices::ClearMessageHandler(SMSG_CALENDAR_CLEAR_PENDING_ACTION);
}

// OFFSET: 0x009AAB70/0x140731D70
void CGCalendar::UpdatePendingInvites(CDataStore* msg) {
    int32_t previous = CGCalendar::m_numInitialPending;
    msg->Get(reinterpret_cast<uint32_t&>(CGCalendar::m_numInitialPending));

    if (CGCalendar::m_numInitialPending != previous) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
    }
}

// OFFSET: 0x009AAAA0/0x1407367F0
int32_t CGCalendar::GetDaysInMonth(int32_t month, int32_t year, int32_t* days, int32_t* weekDay) {
    WowTime date;
    date.m_month = month;
    date.m_monthDay = 0;
    date.m_year = year;
    date.AddDays(0, false);
    *weekDay = date.m_weekDay;
    date.m_month = month + 1;

    if (month == 12) {
        year++;
        date.m_month = 0;
    }

    date.m_year = year;
    date.AddDays(-1, false);
    *days = date.m_monthDay + 1;
    return date.m_monthDay + 1;
}

// OFFSET: 0x009ABD80
int32_t CGCalendar::GetMonth(int32_t offset, int32_t* month, int32_t* year, int32_t* days, int32_t* weekDay) {
    if (!CGCalendar::m_absMonthDays) {
        *month = g_clientGameTime.m_month;
        *year = g_clientGameTime.m_year;
    } else {
        *month = CGCalendar::m_absMonth;
        *year = CGCalendar::m_absYear;
    }

    if (offset) {
        *year += offset / 12;
        *month += offset % 12;

        if (*month >= 12) {
            ++*year;
            *month -= 12;
        } else if (*month < 0) {
            --*year;
            *month += 12;
        }
    }

    int32_t clampedYear = *year;

    if (clampedYear >= 30) {
        clampedYear = 30;
    }

    *year = clampedYear;

    if (clampedYear <= 4) {
        *year = 4;
        int32_t clampedMonth = *month;

        if (clampedMonth <= 10) {
            clampedMonth = 10;
        }

        *month = clampedMonth;
    }

    return CGCalendar::GetDaysInMonth(*month, *year, days, weekDay);
}

// OFFSET: 0x009ABE30
int32_t CGCalendar::ClampAbsMonthDay(int32_t month, int32_t year, int32_t* outMonth, int32_t* outYear, int32_t* days, int32_t* weekDay) {
    int32_t minYear = g_clientGameTime.m_year - 5;
    int32_t maxYear = minYear + 10;

    if (year < minYear) {
        *outYear = minYear;
    } else if (year > maxYear) {
        *outYear = maxYear;
    } else {
        *outYear = year;
    }

    int32_t clampedYear = *outYear;

    if (clampedYear >= 30) {
        clampedYear = 30;
    }

    *outYear = clampedYear;

    if (clampedYear < 4) {
        *outYear = 4;
    }

    *outMonth = abs(month % 12);
    return CGCalendar::GetDaysInMonth(*outMonth, *outYear, days, weekDay);
}

// OFFSET: 0x009ABEC0/0x140732FC0
bool CGCalendar::ComputeEventIndex(const CalendarEventSelection* selection, int32_t* monthOffset, int32_t* day, int32_t* index) {
    *monthOffset = 0;
    *day = 0;
    *index = 0;

    if (!selection->m_eventID && !selection->m_holidayID && !selection->m_startTime) {
        return false;
    }

    WowTime date;
    WowTime::WowDecodeTime(selection->m_startTime, &date);
    uint32_t month = 12 * (date.m_year - CGCalendar::m_absYear) - CGCalendar::m_absMonth + date.m_month + 1;

    if (month <= 2) {
        uint32_t count = s_days[month][date.m_monthDay].Count();

        for (uint32_t i = 0; i < count; i++) {
            CalendarDayEvent* event = s_days[month][date.m_monthDay].m_data[i];

            if (!event) {
                continue;
            }

            bool match = false;

            if (selection->m_eventID) {
                match = selection->m_eventID == event->m_eventID;
            }

            if (selection->m_holidayID) {
                match = selection->m_holidayID == event->m_holidayID;

                if (!match) {
                    continue;
                }

                if (selection->m_type & 0x80) {
                    match = selection->m_endTime == event->m_endTime;
                }
            }

            if (!match) {
                continue;
            }

            *monthOffset = month - 1;
            *day = date.m_monthDay;
            *index = i;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00A32630/0x1407DC8C0
bool GameCalendarGetMinDate(WowTime* date) {
    date->m_month = 10;
    date->m_monthDay = 23;
    date->m_year = 4;
    date->AddDays(0, false);
    return date->SetHourAndMinutes(0u, 0u);
}

// OFFSET: 0x00A32670/0x1407DC900
bool GameCalendarGetMaxDate(WowTime* date) {
    date->m_month = 0;
    date->m_monthDay = 0;
    date->m_year = 31;
    date->AddDays(-1, false);
    return date->SetHourAndMinutes(23u, 59u);
}

// OFFSET: 0x00A326A0/0x1407DC940
bool GameCalendarGetMaxCreateDate(WowTime* date, const WowTime* now) {
    int32_t month = now->m_month + 13;
    date->m_month = month % 12;
    date->m_year = month / 12 + now->m_year;
    date->m_monthDay = 0;
    date->AddDays(-1, false);
    int32_t year = date->m_year;

    if (year >= 31) {
        year = 31;
    }

    date->m_year = year;
    return date->SetHourAndMinutes(23u, 59u);
}

// OFFSET: 0x009AA9B0
CalendarDayEvent::CalendarDayEvent() {
    this->m_eventID = 0;
    this->m_startTime = 0;
    this->m_alarmTime = 0;
    this->m_endTime = 0;
    this->m_holidayID = 0;
    this->m_type = 0;
    this->m_flags = 0;
    this->m_eventType = -1;
    this->m_textureID = 0;
    this->m_inviteStatus = 0;
    this->m_inviteID = 0;
    this->m_creatorGUID = WOWGUID();
    this->m_guildGUID = WOWGUID();
    this->m_alarmCallback = NULL;
    this->m_sequenceIndex = 0;
    this->m_sequenceLength = 0;
}

// OFFSET: 0x009AAA30
int32_t CalendarDayEvent::GetSequenceType() const {
    if (this->m_sequenceLength <= 1) {
        return 4;
    }

    if (!this->m_sequenceIndex) {
        return 1;
    }

    return (this->m_sequenceIndex != this->m_sequenceLength - 1) + 2;
}

// OFFSET: 0x009B15B0/0x140739C30
static bool ComputeEventOccurrence(const WowTime* now, const WowTime* rangeStart, const WowTime* rangeEnd, WowTime* outEnd, HolidaysRec* holiday, int32_t index, bool* weekly, int32_t interval, WowTime* outStart) {
    *weekly = false;
    DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(holiday->m_id, HASHKEY_NONE());
    int32_t* durations;
    int32_t* dates;
    int32_t looping;

    if (dynamic) {
        durations = dynamic->m_duration;
        dates = dynamic->m_date;
        looping = dynamic->m_looping;
    } else {
        looping = holiday->m_looping;
        durations = holiday->m_duration;
        dates = holiday->m_date;
    }

    if (index > 0 && !durations[index]) {
        return false;
    }

    int32_t duration = durations[index];

    if (!duration) {
        duration = 24;
    }

    int32_t durationMinutes = 60 * duration;

    for (uint32_t i = 0; i < 26; i++) {
        if (!dates[i]) {
            continue;
        }

        WowTime::WowDecodeTime(dates[i], outStart);
        const WowTime* reference = now;

        if (holiday->m_flags & 0x1) {
            outStart->m_tzHours = g_clientGameTime.m_tzHours;
            outStart->ConvertToLocal(*outStart);
        }

        int32_t days = 0;

        if (outStart->m_weekDay >= 0 && outStart->m_month < 0 && outStart->m_monthDay < 0 && outStart->m_year < 0) {
            reference = rangeStart;
            *weekly = true;
            int32_t diff = outStart->m_weekDay - rangeStart->m_weekDay;
            days = diff + (diff < 0 ? 7 : 0);
        }

        if (outStart->m_minute < 0) {
            outStart->m_minute = reference->m_minute;
        }

        if (outStart->m_hour < 0) {
            outStart->m_hour = reference->m_hour;
        }

        if (outStart->m_monthDay < 0) {
            outStart->m_monthDay = reference->m_monthDay;
        }

        if (outStart->m_month < 0) {
            outStart->m_month = reference->m_month;
        }

        if (outStart->m_year < 0) {
            int32_t year = CGCalendar::m_absYear;

            if (CGCalendar::m_absYear < 0) {
                year = g_clientGameTime.m_year;
                CGCalendar::m_absYear = year;

                if (year < 0) {
                    year = 4;
                    CGCalendar::m_absYear = 4;
                }
            }

            outStart->m_year = year;

            if (!CGCalendar::m_absMonth && outStart->m_month == 11) {
                outStart->m_year--;
            }
        }

        if (days) {
            outStart->AddDays(days, false);
        }

        for (int32_t j = 0; j < index; j++) {
            int32_t previous = durations[j];

            if (!previous) {
                previous = 24;
            }

            outStart->AddHolidayDuration(60 * previous);
        }

        bool after = *rangeEnd < *outStart;

        if (looping) {
            if (after) {
                return false;
            }

            while (true) {
                *outEnd = *outStart;
                outEnd->AddHolidayDuration(durationMinutes);

                if (!(*rangeStart >= *outEnd)) {
                    break;
                }

                outStart->AddHolidayDuration(interval);

                if (*rangeEnd < *outStart) {
                    return false;
                }
            }

            goto found;
        }

        if (!after) {
            *outEnd = *outStart;
            outEnd->AddHolidayDuration(durationMinutes);

            if (!(*outEnd < *rangeStart)) {
                goto found;
            }
        }
    }

    return false;

found:
    if (!outStart->m_hour && !outStart->m_minute) {
        outStart->AddMinutes(1);
    }

    if (!outEnd->m_hour && !outEnd->m_minute) {
        outEnd->AddMinutes(-1);
    }

    return true;
}

// OFFSET: 0x009B1820/0x140739FD0
static int QSortEvents(const void* a, const void* b) {
    const CalendarDayEvent* left = *static_cast<CalendarDayEvent* const*>(a);
    const CalendarDayEvent* right = *static_cast<CalendarDayEvent* const*>(b);

    if ((left->m_flags & 0x1) && !(right->m_flags & 0x1)) {
        return -1;
    }

    if (!(left->m_flags & 0x1) && (right->m_flags & 0x1)) {
        return 1;
    }

    if ((left->m_type & 0x543) && !(right->m_type & 0x543)) {
        return -1;
    }

    if (!(left->m_type & 0x543) && (right->m_type & 0x543)) {
        return 1;
    }

    uint32_t leftHoliday = left->m_type & 0x8;

    if (leftHoliday && !(right->m_type & 0x8)) {
        return -1;
    }

    if (!leftHoliday && (right->m_type & 0x8)) {
        return 1;
    }

    int32_t rightPriority = -1;
    int32_t leftPriority = -1;
    HolidaysRec* leftRec = NULL;

    if (leftHoliday) {
        DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(static_cast<uint32_t>(left->m_eventID), HASHKEY_NONE());

        if (dynamic) {
            leftPriority = dynamic->m_priority;
        } else {
            leftRec = g_holidaysDB.GetRecord(static_cast<int32_t>(left->m_eventID));

            if (leftRec) {
                leftPriority = leftRec->m_priority;
            }
        }
    }

    HolidaysRec* rightRec = NULL;

    if (right->m_type & 0x8) {
        DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(static_cast<uint32_t>(right->m_eventID), HASHKEY_NONE());

        if (dynamic) {
            rightPriority = dynamic->m_priority;
        } else {
            rightRec = g_holidaysDB.GetRecord(static_cast<int32_t>(right->m_eventID));

            if (rightRec) {
                rightPriority = rightRec->m_priority;
            }
        }
    }

    int32_t result = rightPriority - leftPriority;

    if (result) {
        return result;
    }

    result = left->GetSequenceType() - right->GetSequenceType();

    if (result) {
        return result;
    }

    result = left->m_startTime - right->m_startTime;

    if (result) {
        return result;
    }

    const char* leftName = left->m_title;
    const char* rightName = right->m_title;

    if (leftRec) {
        HolidayNamesRec* name = g_holidayNamesDB.GetRecord(leftRec->m_holidayNameID);

        if (name) {
            leftName = name->m_name;
        }
    } else if (left->m_type & 0x280) {
        MapRec* map = g_mapDB.GetRecord(left->m_holidayID);

        if (map) {
            leftName = map->m_mapName;
        }
    }

    if (rightRec) {
        HolidayNamesRec* name = g_holidayNamesDB.GetRecord(rightRec->m_holidayNameID);

        if (name) {
            rightName = name->m_name;
        }
    } else if (right->m_type & 0x280) {
        MapRec* map = g_mapDB.GetRecord(right->m_holidayID);

        if (map) {
            rightName = map->m_mapName;
        }
    }

    return SStrCmpUTF8I(leftName, rightName, STORM_MAX_STR);
}

// OFFSET: 0x009AABB0/0x140731DB0
static int QSortGuildEvents(const void* a, const void* b) {
    const CalendarDayEvent* left = *static_cast<CalendarDayEvent* const*>(a);
    const CalendarDayEvent* right = *static_cast<CalendarDayEvent* const*>(b);
    WowTime leftTime;
    WowTime rightTime;
    WowTime::WowDecodeTime(left->m_startTime, &leftTime);
    WowTime::WowDecodeTime(right->m_startTime, &rightTime);
    return leftTime <= rightTime ? -1 : 1;
}

// OFFSET: 0x009B2250/0x14073ACB0
void CGCalendar::UpdateGuildEvents() {
    s_guildEvents.m_count = 0;
    uint32_t count = 0;

    for (CALENDAREVENTHASH* entry = s_eventTables.m_tables[1].m_fulllist.Head(); entry; entry = s_eventTables.m_tables[1].Next(entry)) {
        uint32_t numEvents = entry->m_events.Count();

        for (uint32_t i = 0; i < numEvents; i++) {
            WowTime start;
            WowTime::WowDecodeTime(entry->m_events.m_data[i]->m_startTime, &start);
            WowTime limit = g_clientGameTime;
            limit.m_tzHours = g_clientGameTime.m_tzHours;
            limit.AddDays(28, false);

            if (start >= g_clientGameTime && start <= limit) {
                s_guildEvents.SetCount(count + 1);
                s_guildEvents.m_data[count++] = entry->m_events.m_data[i];
            }
        }
    }

    if (s_guildEvents.Count()) {
        qsort(s_guildEvents.m_data, s_guildEvents.Count(), sizeof(CalendarDayEvent*), QSortGuildEvents);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_GUILD_EVENTS, NULL);
}

// OFFSET: 0x009B3180/0x14073BF10
void CGCalendar::UpdateDisplayList(bool signal, bool updateGuildEvents) {
    if (!CGCalendar::m_absMonthDays) {
        int32_t year = g_clientGameTime.m_year;
        int32_t month = g_clientGameTime.m_month;
        CGCalendar::m_absMonth = month;
        CGCalendar::m_absYear = year;

        if (year < 30) {
            if (year <= 4) {
                year = 4;
                CGCalendar::m_absYear = 4;

                if (g_clientGameTime.m_month <= 10) {
                    month = 10;
                    CGCalendar::m_absMonth = 10;
                }
            }
        } else {
            year = 30;
            CGCalendar::m_absYear = 30;
        }

        int32_t weekDay;
        CGCalendar::GetDaysInMonth(month, year, &CGCalendar::m_absMonthDays, &weekDay);
        CGCalendar::CalculateEvents(true);
        return;
    }

    int32_t month = (CGCalendar::m_absMonth + 11) % 12;
    int32_t year = month == 11 ? CGCalendar::m_absYear - 1 : CGCalendar::m_absYear;

    for (uint32_t m = 0; m < 3; m++) {
        for (uint32_t d = 0; d < 31; d++) {
            TSGrowableArray<CalendarDayEvent*>& list = s_days[m][d];
            uint32_t count = 0;
            list.m_count = 0;

            WowTime date;
            date.m_month = month;
            date.m_monthDay = d;
            date.m_year = year;
            uint32_t hash;
            WowTime::WowEncodeTime(hash, &date);

            for (uint32_t t = 0; t < 8; t++) {
                CVar** filter = s_eventFilters[t];

                if (filter && !(*filter && (*filter)->m_intValue)) {
                    continue;
                }

                CALENDAREVENTHASH* entry = s_eventTables.m_tables[t].Ptr(hash, HASHKEY_NONE());

                if (!entry) {
                    continue;
                }

                uint32_t numEvents = entry->m_events.Count();
                list.SetCount(count + numEvents);

                for (uint32_t k = 0; k < numEvents; k++) {
                    CalendarDayEvent* event = entry->m_events.m_data[k];

                    if (!FriendList::s_friendList->IsIgnored(event->m_creatorGUID, false)) {
                        list.m_data[count++] = event;
                    }
                }
            }

            list.SetCount(count);
        }

        for (uint32_t d = 0; d < 31; d++) {
            qsort(s_days[m][d].m_data, s_days[m][d].Count(), sizeof(CalendarDayEvent*), QSortEvents);
        }

        month = (month + 1) % 12;

        if (!month && ++year >= 31) {
            break;
        }
    }

    if (signal) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);

        if (updateGuildEvents) {
            CGCalendar::UpdateGuildEvents();
        }
    }
}

// OFFSET: 0x009B1A40/0x14073A2F0
void CGCalendar::SetFilter(CVar* var, int32_t value) {
    if (!var || !CGCalendar::m_absMonthDays) {
        return;
    }

    int32_t month = (CGCalendar::m_absMonth + 11) % 12;
    int32_t year = month == 11 ? CGCalendar::m_absYear - 1 : CGCalendar::m_absYear;

    for (uint32_t m = 0; m < 3; m++) {
        for (uint32_t d = 0; d < 31; d++) {
            TSGrowableArray<CalendarDayEvent*>& list = s_days[m][d];
            uint32_t count = 0;
            list.m_count = 0;

            WowTime date;
            date.m_month = month;
            date.m_monthDay = d;
            date.m_year = year;
            uint32_t hash;
            WowTime::WowEncodeTime(hash, &date);

            for (uint32_t t = 0; t < 8; t++) {
                CVar** filter = s_eventFilters[t];

                if (filter && *filter) {
                    bool off = *filter == var ? value == 0 : (*filter)->m_intValue == 0;

                    if (off) {
                        continue;
                    }
                }

                CALENDAREVENTHASH* entry = s_eventTables.m_tables[t].Ptr(hash, HASHKEY_NONE());

                if (!entry) {
                    continue;
                }

                uint32_t numEvents = entry->m_events.Count();
                list.SetCount(count + numEvents);

                for (uint32_t k = 0; k < numEvents; k++) {
                    CalendarDayEvent* event = entry->m_events.m_data[k];

                    if (!FriendList::s_friendList->IsIgnored(event->m_creatorGUID, false)) {
                        list.m_data[count++] = event;
                    }
                }
            }

            list.SetCount(count);
        }

        for (uint32_t d = 0; d < 31; d++) {
            qsort(s_days[m][d].m_data, s_days[m][d].Count(), sizeof(CalendarDayEvent*), QSortEvents);
        }

        month = (month + 1) % 12;

        if (!month && ++year >= 31) {
            break;
        }
    }
}

// OFFSET: 0x009B37E0/0x14073C680
void CGCalendar::InitHolidays() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    int32_t factionMask = 0;
    int32_t faction = player->GetPVPFactionIndex();

    if (faction == 1) {
        factionMask = 1;
    } else if (faction == 0) {
        factionMask = 2;
    }

    if (!(factionMask & 3)) {
        return;
    }

    int8_t region = static_cast<int8_t>(s_region);

    for (int32_t i = 0; i < g_holidaysDB.m_numRecords; i++) {
        HolidaysRec* holiday = g_holidaysDB.GetRecordByIndex(i);
        DYNAMICHOLIDAYHASH* dynamic = s_dynamicHolidays.Ptr(holiday->m_id, HASHKEY_NONE());
        int32_t regionMask = dynamic ? dynamic->m_region : holiday->m_region;

        if (regionMask && !((1 << (region - 1)) & regionMask)) {
            continue;
        }

        int32_t looping;
        int32_t filterType;
        int32_t* flags;
        int32_t* durations;

        if (dynamic) {
            looping = dynamic->m_looping;
            flags = dynamic->m_calendarFlags;
            durations = dynamic->m_duration;
            filterType = dynamic->m_calendarFilterType;
        } else {
            looping = holiday->m_looping;
            filterType = holiday->m_calendarFilterType;
            flags = holiday->m_calendarFlags;
            durations = holiday->m_duration;
        }

        for (int32_t j = 0; j < 10; j++) {
            if (!(factionMask & flags[j])) {
                continue;
            }

            CALENDARHOLIDAYNODE* node = s_holidays.NewNode(2, 0, 0);

            if (!node) {
                continue;
            }

            node->m_holiday = holiday;
            node->m_index = j;

            if (looping) {
                int32_t total = durations[j];

                if (!total) {
                    total = 24;
                }

                node->m_duration = total;

                for (int32_t k = 0; k < 10; k++) {
                    if (k != j) {
                        node->m_duration += durations[k];
                    }

                    if (!k && !durations[0]) {
                        node->m_duration += 24;
                    }
                }

                node->m_duration *= 60;
            }

            if (filterType == 0) {
                node->m_type = 5;
            } else if (filterType == 1) {
                node->m_type = 6;
            } else if (filterType == 2) {
                node->m_type = 7;
            }
        }
    }
}

// OFFSET: 0x009B5810/0x14073F110
void CGCalendar::InsertEvent(CalendarDayEvent* event, int32_t table) {
    uint32_t originalEnd = event->m_endTime;
    WowTime start;
    WowTime end;
    WowTime::WowDecodeTime(event->m_startTime, &start);

    if (event->m_endTime) {
        WowTime::WowDecodeTime(event->m_endTime, &end);

        if (end.m_year == -1) {
            end.m_year = 31;
        }
    } else {
        end = start;
    }

    WowTime day = start;
    uint32_t numDays = 0;

    while (day <= end) {
        numDays++;
        day.AddDays(1, false);
    }

    if (numDays && day.m_monthDay == end.m_monthDay) {
        numDays++;
    }

    int32_t month = start.m_month;
    uint32_t sequence = 0;
    bool first = true;
    TSHashTable<CALENDAREVENTHASH, HASHKEY_NONE>* events = &s_eventTables.m_tables[table];

    do {
        WowTime key;
        key.m_month = month;
        key.m_monthDay = start.m_monthDay;
        key.m_year = start.m_year;
        uint32_t hash;
        WowTime::WowEncodeTime(hash, &key);
        CALENDAREVENTHASH* entry = events->Ptr(hash, HASHKEY_NONE());

        if (!entry) {
            s_eventTables.m_counts[table]++;
            entry = events->New(hash, HASHKEY_NONE(), 0, 0);
        }

        if (!first) {
            void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);

            if (mem) {
                memcpy(mem, event, sizeof(CalendarDayEvent));
                event = static_cast<CalendarDayEvent*>(mem);
            } else {
                event = NULL;
            }
        }

        event->m_sequenceIndex = sequence;
        event->m_sequenceLength = numDays;
        sequence++;

        CalendarDayEvent** slot = entry->m_events.New();

        if (slot) {
            *slot = event;
        }

        if (first) {
            memset(&start, 0xFF, 12);
            first = false;
        }

        start.AddDays(1, false);

        if (start > end && start.m_monthDay != end.m_monthDay) {
            break;
        }

        month = start.m_month;
    } while (start.m_month != end.m_month || start.m_monthDay != end.m_monthDay || end.m_hour || end.m_minute);

    if (numDays > 1) {
        event->m_startTime = originalEnd;
    }
}

// OFFSET: 0x009B5BE0/0x14073F6D0
void CGCalendar::CalculateEvents(bool signal) {
    s_eventTables.m_counts[4] = 0;
    s_eventTables.m_tables[4].InternalClear(0);
    s_eventTables.m_counts[5] = 0;
    s_eventTables.m_tables[5].InternalClear(0);
    s_eventTables.m_counts[6] = 0;
    s_eventTables.m_tables[6].InternalClear(0);
    s_eventTables.m_counts[7] = 0;
    s_eventTables.m_tables[7].InternalClear(0);
    s_eventTables.m_counts[3] = 0;
    s_eventTables.m_tables[3].InternalClear(0);

    WowTime start;
    WowTime end;
    WowTime rangeStart;
    WowTime rangeEnd;
    rangeStart.m_year = CGCalendar::m_absYear;
    rangeStart.m_month = CGCalendar::m_absMonth;
    rangeStart.m_monthDay = 0;
    rangeStart.SetHourAndMinutes(0u, 0u);
    rangeStart.AddDays(-6, false);
    rangeEnd.m_month = CGCalendar::m_absMonth;
    rangeEnd.m_monthDay = 0;
    rangeEnd.m_year = CGCalendar::m_absYear;
    rangeEnd.SetHourAndMinutes(0u, 0u);
    rangeEnd.AddDays(42, false);

    if (!s_holidays.Head()) {
        CGCalendar::InitHolidays();
    }

    for (CALENDARHOLIDAYNODE* node = s_holidays.Head(); node; node = s_holidays.Next(node)) {
        bool weekly = false;

        if (ComputeEventOccurrence(&g_clientGameTime, &rangeStart, &rangeEnd, &end, node->m_holiday, node->m_index, &weekly, node->m_duration, &start)) {
            if (weekly) {
                node->m_duration = 10080;
            } else {
                start.AddDays(0, false);
                end.AddDays(0, false);
            }

            WowTime::WowEncodeTime(node->m_startTime, &start);
            WowTime::WowEncodeTime(node->m_endTime, &end);
        } else {
            node->m_startTime = 0;
            node->m_endTime = 0;
        }
    }

    for (CALENDARHOLIDAYNODE* node = s_holidays.Head(); node; node = s_holidays.Next(node)) {
        if (!node->m_startTime) {
            continue;
        }

        HolidaysRec* holiday = g_holidaysDB.GetRecord(node->m_holiday->m_id);

        if (!holiday) {
            continue;
        }

        HolidayNamesRec* name = g_holidayNamesDB.GetRecord(holiday->m_holidayNameID);

        if (!name || !name->m_name) {
            continue;
        }

        if (node->m_duration) {
            WowTime::WowDecodeTime(node->m_startTime, &start);
            WowTime::WowDecodeTime(node->m_endTime, &end);

            if (end.m_year == -1) {
                end.m_year = 31;
            }

            if (!(start < rangeEnd)) {
                continue;
            }

            while (start.m_year < 31) {
                uint32_t startTime;
                uint32_t endTime;
                WowTime::WowEncodeTime(startTime, &start);
                WowTime::WowEncodeTime(endTime, &end);

                void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
                CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : NULL;
                event->m_eventID = node->m_holiday->m_id;
                event->m_title[0] = '\0';
                event->m_startTime = startTime;
                event->m_endTime = endTime;
                event->m_type = 8;
                CGCalendar::InsertEvent(event, node->m_type);
                start.AddHolidayDuration(node->m_duration);
                end.AddHolidayDuration(node->m_duration);

                if (!(start < rangeEnd)) {
                    break;
                }
            }
        } else {
            void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
            CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : NULL;
            event->m_eventID = node->m_holiday->m_id;
            event->m_title[0] = '\0';
            event->m_startTime = node->m_startTime;
            event->m_endTime = node->m_endTime;
            event->m_type = 8;
            CGCalendar::InsertEvent(event, node->m_type);
        }
    }

    WowTime reset;

    for (CALENDARRAIDRESETNODE* node = s_raidResets.Head(); node; node = s_raidResets.Next(node)) {
        if (!node->m_startTime || !node->m_interval) {
            continue;
        }

        WowTime::WowDecodeTime(node->m_startTime, &reset);
        reset.AddHolidayDuration(node->m_interval);

        while (reset < rangeEnd && reset.m_year < 31) {
            if (reset > rangeStart) {
                void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
                CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : NULL;
                event->m_holidayID = node->m_mapID;
                event->m_title[0] = '\0';
                event->m_type = 512;
                WowTime::WowEncodeTime(event->m_startTime, &reset);
                CGCalendar::InsertEvent(event, 3);
            }

            reset.AddHolidayDuration(node->m_interval);
        }
    }

    CGCalendar::UpdateDisplayList(signal, false);
}

// OFFSET: 0x00A32710/0x1407DC9C0
bool CGCalendar::ValidateDate(const WowTime* date, int32_t* error) {
    WowTime check = *date;
    check.m_tzHours = date->m_tzHours;

    WowTime limit;
    limit.m_month = 10;
    limit.m_monthDay = 23;
    limit.m_year = 4;
    limit.AddDays(0, false);
    limit.SetHourAndMinutes(0u, 0u);
    check.SetHourAndMinutes(limit.m_hour, limit.m_minute);

    if (check < limit) {
        if (error) {
            *error = 16;
        }

        return false;
    }

    limit.m_month = 0;
    limit.m_monthDay = 0;
    limit.m_year = 31;
    limit.AddDays(-1, false);
    limit.SetHourAndMinutes(23u, 59u);
    check.SetHourAndMinutes(limit.m_hour, limit.m_minute);

    if (check > limit) {
        if (error) {
            *error = 16;
        }

        return false;
    }

    return true;
}

// OFFSET: 0x009AC060/0x140733170
bool CGCalendar::GetDayEvent(uint32_t offset, uint32_t day) {
    if (offset + 1 <= 2) {
        if (day >= 31) {
            return false;
        }

        WowTime date;
        int32_t month = 0;
        int32_t year = 0;
        int32_t days;
        int32_t weekDay;
        CGCalendar::GetMonth(offset, &month, &year, &days, &weekDay);
        date.SetDate(month, day, year);
        return CGCalendar::ValidateDate(&date, NULL);
    }

    return false;
}

// OFFSET: 0x009AC0E0
uint32_t CGCalendar::GetNumDayEvents(uint32_t offset, uint32_t day) {
    if (!CGCalendar::GetDayEvent(offset, day)) {
        return 0;
    }

    uint32_t count = s_days[offset + 1][day].Count();
    uint32_t result = 0;

    if (count > result) {
        result = count;
    }

    return result;
}

// OFFSET: 0x009AC170/0x140733340
int32_t CGCalendar::GetFirstPendingInviteIndex(uint32_t offset, uint32_t day) {
    if (!CGCalendar::GetDayEvent(offset, day)) {
        return -1;
    }

    uint32_t count = s_days[offset + 1][day].Count();

    if (!count) {
        return -1;
    }

    for (uint32_t i = 0; i < count; i++) {
        CalendarDayEvent* event = s_days[offset + 1][day].m_data[i];

        if (event && (event->m_flags & 0x1)) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x009ABBA0/0x140732D90
static void __stdcall FireEventAlarm(CGameTime* time, void* param) {
    CalendarDayEvent* event = static_cast<CalendarDayEvent*>(param);
    WowTime alarm;
    WowTime::WowDecodeTime(event->m_alarmTime, &alarm);
    alarm.m_tzHours = 0;

    if (*time == alarm && (event->m_inviteStatus == 3 || event->m_inviteStatus == 1)) {
        WowTime start;
        WowTime::WowDecodeTime(event->m_startTime, &start);
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_EVENT_ALARM, "%s%d%d", static_cast<const char*>(event->m_title), start.m_hour, start.m_minute);
        CGameTime::GameTimeUnregisterCallback(event->m_alarmCallback);
        event->m_alarmCallback = NULL;
    }
}

// OFFSET: 0x009ABC40/0x140732E40
void CalendarDayEvent::ScheduleAlarm() {
    WowTime alarm;
    WowTime::WowDecodeTime(this->m_startTime, &alarm);
    alarm.m_tzHours = 0;
    alarm.AddMinutes(-15);
    WowTime::WowEncodeTime(this->m_alarmTime, &alarm);
    this->m_alarmCallback = g_clientGameTime.GameTimeRegisterCallback(&alarm, FireEventAlarm, this);
}

// OFFSET: 0x009AAB10/0x140731D20
static uint32_t EncodeDate(int32_t month, int32_t monthDay, int32_t year) {
    WowTime date;
    date.m_month = month;
    date.m_monthDay = monthDay;
    date.m_year = year;
    uint32_t value;
    WowTime::WowEncodeTime(value, &date);
    return value;
}

// OFFSET: 0x00A327F0/0x1407DCAE0
bool CGCalendar::ValidateCreateDate(const WowTime* date, const WowTime* now, int32_t* error) {
    if (!CGCalendar::ValidateDate(date, error)) {
        return false;
    }

    WowTime check = *date;
    check.m_tzHours = date->m_tzHours;
    WowTime limit = *now;
    limit.m_tzHours = now->m_tzHours;
    check.SetHourAndMinutes(limit.m_hour, limit.m_minute);

    if (check < limit) {
        if (error) {
            *error = 20;
        }

        return false;
    }

    GameCalendarGetMaxCreateDate(&limit, now);
    check.SetHourAndMinutes(limit.m_hour, limit.m_minute);

    if (check > limit) {
        if (error) {
            *error = 16;
        }

        return false;
    }

    return true;
}

// OFFSET: 0x009AAB50
void CGCalendar::ProcessInitialInviteList(CDataStore* msg) {
    CGCalendar::m_curEvent->ProcessInitialInviteList(msg);
}

// OFFSET: 0x009AAC20/0x140731E20
bool CGCalendar::CanEditCurrentEvent() {
    if (CGCalendar::m_curEvent) {
        bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;

        if (guildAnnouncement) {
            if (CGGuildInfo::CanEditGuildEvent()) {
                return true;
            }
        } else if (CGCalendar::m_curEvent->IsModerator()) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009AF410/0x140737330
bool CGCalendar::CanEditEvent(CalendarDayEvent* event) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && event) {
        if (event->m_type & 0x440) {
            WOWGUID guild = player->GetGuildGUID();

            if (guild && guild == event->m_guildGUID && ((event->m_flags & 0x6) || CGGuildInfo::CanEditGuildEvent())) {
                return true;
            }
        } else if (event->m_flags & 0x6) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009AF4A0/0x1407373C0
bool CGCalendar::CanRemoveEvent(CalendarDayEvent* event) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && event) {
        if (event->m_type & 0x440) {
            WOWGUID guild = player->GetGuildGUID();

            if (guild && guild == event->m_guildGUID && ((event->m_flags & 0x4) || CGGuildInfo::CanRemoveGuildEvent())) {
                return true;
            }
        } else if (event->m_flags & 0x6) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009AAC60/0x140731E70
void CGCalendar::SetInviteStatus(uint32_t index, uint32_t status) {
    if (!CGCalendar::CanEditCurrentEvent()) {
        return;
    }

    if (CGCalendar::m_curEvent->m_new || !CGCalendar::m_actionPending) {
        CGCalendar::m_curEvent->SetInviteStatus(index, status);
    }
}

// OFFSET: 0x009AACC0/0x140731EF0
void CGCalendar::SetModeratorStatus(uint32_t index, uint32_t status) {
    if (!CGCalendar::CanEditCurrentEvent()) {
        return;
    }

    if (CGCalendar::m_curEvent->m_new || !CGCalendar::m_actionPending) {
        CGCalendar::m_curEvent->SetModeratorStatus(index, status);
    }
}

// OFFSET: 0x009AAD20
void CGCalendar::SetActionPending(uint8_t pending) {
    CGCalendar::m_actionPending = pending;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", pending);
}

// OFFSET: 0x009AAD50/0x140731F70
const char* CGCalendar::GetCalendarTypeString(uint32_t flags) {
    if (!(flags & 0x1)) {
        if (flags & 0x40) {
            return "GUILD_ANNOUNCEMENT";
        }

        if (flags & 0x400) {
            return "GUILD_EVENT";
        }

        if (flags & 0x4) {
            return "SYSTEM";
        }

        if (flags & 0x8) {
            return "HOLIDAY";
        }

        if (flags & 0x80) {
            return "RAID_LOCKOUT";
        }

        if (flags & 0x200) {
            return "RAID_RESET";
        }

        if (!(flags & 0x2)) {
            if (!(flags & 0x100)) {
                return "";
            }
        }
    }

    return "PLAYER";
}

// OFFSET: 0x009AC000/0x140733100
uint32_t CGCalendar::GetDateKey(uint32_t time) {
    WowTime date;
    WowTime::WowDecodeTime(time, &date);
    return EncodeDate(date.m_month, date.m_monthDay, date.m_year);
}

// OFFSET: 0x009AC120/0x1407332C0
CalendarDayEvent* CGCalendar::GetDayEvent(uint32_t offset, uint32_t day, uint32_t index) {
    if (CGCalendar::GetDayEvent(offset, day) && index < s_days[offset + 1][day].Count()) {
        return s_days[offset + 1][day].m_data[index];
    }

    return NULL;
}

// OFFSET: 0x009AC1D0/0x1407333C0
void CGCalendar::GetEvent(uint32_t offset, uint32_t day, uint32_t index) {
    if (CGCalendar::m_actionPending || offset + 1 > 2 || day >= 31 || index >= s_days[offset + 1][day].Count()) {
        return;
    }

    CalendarDayEvent* event = CGCalendar::GetDayEvent(offset, day, index);

    if (event && (event->m_type & 0x543)) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_GET_EVENT));
        msg.Put(event->m_eventID);
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGCalendar::SetActionPending(1);
        return;
    }

    CGCalendar::m_selectedEvent.m_eventID = event->m_eventID;
    CGCalendar::m_selectedEvent.m_startTime = event->m_startTime;
    CGCalendar::m_selectedEvent.m_endTime = event->m_endTime;
    CGCalendar::m_selectedEvent.m_holidayID = event->m_holidayID;
    CGCalendar::m_selectedEvent.m_type = event->m_type;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_OPEN_EVENT, "%s", CGCalendar::GetCalendarTypeString(event->m_type));
}

// OFFSET: 0x009AC350/0x1407335D0
const char* GetHolidayDescription(int32_t holidayId) {
    HolidaysRec* holiday = g_holidaysDB.GetRecord(holidayId);

    if (!holiday) {
        return NULL;
    }

    HolidayDescriptionsRec* description = g_holidayDescriptionsDB.GetRecord(holiday->m_holidayDescriptionID);
    return description ? static_cast<const char*>(description->m_description) : NULL;
}

// OFFSET: 0x009AC3B0/0x140733650
bool CGCalendar::SetEventDate(uint32_t month, uint32_t monthDay, uint32_t year) {
    bool canEdit = false;

    if (CGCalendar::m_curEvent) {
        bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;
        canEdit = guildAnnouncement ? CGGuildInfo::CanEditGuildEvent() : CGCalendar::m_curEvent->IsModerator();
    }

    if (!canEdit) {
        return false;
    }

    return CGCalendar::m_curEvent->SetDate(month, monthDay, year);
}

// OFFSET: 0x009AC400/0x1407336D0
bool CGCalendar::SetEventTime(uint32_t hour, uint32_t minute) {
    bool canEdit = false;

    if (CGCalendar::m_curEvent) {
        bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;
        canEdit = guildAnnouncement ? CGGuildInfo::CanEditGuildEvent() : CGCalendar::m_curEvent->IsModerator();
    }

    if (!canEdit) {
        return false;
    }

    return CGCalendar::m_curEvent->SetTime(hour, minute);
}

// OFFSET: 0x009AC450/0x140733740
bool CGCalendar::SetEventLockoutDate(uint32_t month, uint32_t monthDay, uint32_t year) {
    bool canEdit = false;

    if (CGCalendar::m_curEvent) {
        bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;
        canEdit = guildAnnouncement ? CGGuildInfo::CanEditGuildEvent() : CGCalendar::m_curEvent->IsModerator();
    }

    if (!canEdit) {
        return false;
    }

    return CGCalendar::m_curEvent->SetLockoutDate(month, monthDay, year);
}

// OFFSET: 0x009AC4A0/0x1407337C0
bool CGCalendar::SetEventLockoutTime(uint32_t hour, uint32_t minute) {
    bool canEdit = false;

    if (CGCalendar::m_curEvent) {
        bool guildAnnouncement = (CGCalendar::m_curEvent->m_flags >> 6) & 1;
        canEdit = guildAnnouncement ? CGGuildInfo::CanEditGuildEvent() : CGCalendar::m_curEvent->IsModerator();
    }

    if (!canEdit) {
        return false;
    }

    return CGCalendar::m_curEvent->SetLockoutTime(hour, minute);
}

// OFFSET: 0x009AC4F0/0x140733830
void CGCalendar::SetEventTextureID(uint32_t index) {
    if (!CGCalendar::CanEditCurrentEvent()) {
        return;
    }

    uint32_t list;

    if (CGCalendar::m_curEvent->m_type == 1) {
        list = 0;
    } else if (CGCalendar::m_curEvent->m_type == 0) {
        list = 1;
    } else if (CGCalendar::m_curEvent->m_type == 5) {
        list = 2;
    } else {
        return;
    }

    if (index < s_eventTextures[list].Count()) {
        CGCalendar::m_curEvent->SetTextureID(s_eventTextures[list].m_data[index]);
    }
}

// INLINED
void CGCalendar::RemoveEvent() {
    if (CGCalendar::m_actionPending || !CGCalendar::m_curEvent || CGCalendar::m_curEvent->m_new) {
        return;
    }

    if (CGCalendar::m_curEvent->RemoveInviteByGUID(ClntObjMgrGetActivePlayer())) {
        CGCalendar::SetActionPending(1);
    }
}

// OFFSET: 0x009AC570
int32_t CGCalendar::GetTextureIndex(uint32_t type, int32_t textureID) {
    int32_t result = -1;
    uint32_t list;

    if (type == 1) {
        list = 0;
    } else if (type == 0) {
        list = 1;
    } else if (type == 5) {
        list = 2;
    } else {
        return -1;
    }

    for (uint32_t i = 0; i < s_eventTextures[list].Count(); i++) {
        if (textureID == s_eventTextures[list].m_data[i]) {
            result = i;
            break;
        }
    }

    return result;
}

// OFFSET: 0x009AC5D0/0x1407338C0
EventInviteInfo* CGCalendar::GetCurrentEventInvite(WOWGUID guid) {
    if (!CGCalendar::m_curEvent) {
        return NULL;
    }

    return CGCalendar::m_curEvent->GetInvite(guid);
}

// OFFSET: 0x009AC5F0/0x140733910
void CGCalendar::RemoveInvite(uint32_t index) {
    if (CGCalendar::m_actionPending || !CGCalendar::m_curEvent || !CGCalendar::m_curEvent->RemoveInvite(index)) {
        return;
    }

    if (CGCalendar::m_curEvent->m_new) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_INVITE_LIST, NULL);
    } else {
        CGCalendar::SetActionPending(1);
    }
}

// OFFSET: 0x009AC640/0x140733980
void CGCalendar::SelectContextEvent(uint32_t offset, uint32_t day, uint32_t index) {
    if (!CGCalendar::GetDayEvent(offset, day) || index >= s_days[offset + 1][day].Count()) {
        return;
    }

    CalendarDayEvent* event = s_days[offset + 1][day].m_data[index];

    if (!event) {
        return;
    }

    CGCalendar::m_contextEvent.m_eventID = event->m_eventID;
    CGCalendar::m_contextEvent.m_startTime = event->m_startTime;
    CGCalendar::m_contextEvent.m_endTime = event->m_endTime;
    CGCalendar::m_contextEvent.m_holidayID = event->m_holidayID;
    CGCalendar::m_contextEvent.m_type = event->m_type;
    CGCalendar::m_contextEventInfo = event;
}

// OFFSET: 0x009AC6D0/0x1407339E0
void CGCalendar::ContextMenuInviteRSVP(CalendarDayEvent* event, uint32_t status) {
    if (!event || CGCalendar::m_actionPending) {
        return;
    }

    if ((event->m_type & 0x400) && (event->m_flags & 0x8)) {
        return;
    }

    if (status != 1 && status != 2 && status != 8) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_EVENT_RSVP));
    msg.Put(event->m_eventID);
    msg.Put(event->m_inviteID);
    msg.Put(status);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGCalendar::m_actionPending = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 1);
}

// OFFSET: 0x009ABCA0/0x140732EB0
void CGCalendar::GetCalendar() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_GET_CALENDAR));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGCalendar::m_openedCalendar = 1;
}

// OFFSET: 0x009AECB0/0x140736920
void CGCalendar::ProcessInviteNotes(CDataStore* msg, uint8_t* clearPending) {
    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        msg->m_read = msg->m_size;
        return;
    }

    SmartGUID guid;
    *msg >> guid;
    uint64_t eventID;
    msg->Get(eventID);
    char notes[128];
    msg->GetString(notes, STORM_MAX_STR);
    uint8_t clear;
    msg->Get(clear);

    if (!clear) {
        *clearPending = 0;
    }

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        CGCalendar::m_curEvent->UpdateInviteNotes(guid, notes);
    }
}

// OFFSET: 0x009AED60/0x140736A00
void CGCalendar::ProcessInviteNotesAlert(CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        msg->m_read = msg->m_size;
        return;
    }

    uint64_t eventID;
    msg->Get(eventID);
    char notes[128];
    msg->GetString(notes, STORM_MAX_STR);

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        CGCalendar::m_curEvent->UpdateInviteNotes(player->m_obj->m_guid, notes);
    }
}

// OFFSET: 0x009AEDF0/0x140736AB0
void CGCalendar::AddEvent() {
    if (CGCalendar::m_actionPending) {
        return;
    }

    if (!CGCalendar::m_curEvent) {
        CGCalendar::CalendarError("CALENDAR_ERROR_INTERNAL", 0);
        return;
    }

    int32_t now;

    if ((unk_BD09AC && unk_BD09AC->m_intValue) || (now = FrameTime::GetCurTimeMs(), !CGCalendar::m_lastEventTime) || now - CGCalendar::m_lastEventTime >= 5000) {
        if (CGCalendar::ValidateCreateDate(&CGCalendar::m_curEvent->m_date, &g_clientGameTime, NULL) && CGCalendar::m_curEvent->AddEvent()) {
            CGCalendar::m_lastEventTime = FrameTime::GetCurTimeMs();
            CGCalendar::SetActionPending(1);
        }
    } else {
        CGCalendar::CalendarError("CALENDAR_ERROR_EVENT_THROTTLED", 0);
    }
}

// OFFSET: 0x009AEEF0/0x140736BB0
void CGCalendar::CreatePlayerEvent() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (CGCalendar::m_curEvent) {
        delete CGCalendar::m_curEvent;
    }

    void* mem = SMemAlloc(sizeof(CGCalendarEvent), __FILE__, __LINE__, 0);
    CGCalendar::m_curEvent = mem ? new (mem) CGCalendarEvent(player->m_obj->m_guid, player->m_unit->m_unitFieldLevel, 0) : NULL;
    CGCalendar::m_selectedEvent.m_eventID = 0;
    CGCalendar::m_selectedEvent.m_startTime = 0;
    CGCalendar::m_selectedEvent.m_endTime = 0;
    CGCalendar::m_selectedEvent.m_holidayID = 0;
    CGCalendar::m_selectedEvent.m_type = 0;

    if (CGCalendar::m_curEvent) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_INVITE_LIST, "%b", 1);
    }
}

// OFFSET: 0x009AEFC0/0x140736CA0
void CGCalendar::CreateGuildAnnouncement() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    if (!player->GetGuildGUID()) {
        CGCalendar::CalendarError("ERR_GUILD_PLAYER_NOT_IN_GUILD", 0);
        return;
    }

    if (CGCalendar::m_curEvent) {
        delete CGCalendar::m_curEvent;
    }

    void* mem = SMemAlloc(sizeof(CGCalendarEvent), __FILE__, __LINE__, 0);
    CGCalendar::m_curEvent = mem ? new (mem) CGCalendarEvent(WOWGUID(), 0, 1) : NULL;
    CGCalendar::m_selectedEvent.m_eventID = 0;
    CGCalendar::m_selectedEvent.m_startTime = 0;
    CGCalendar::m_selectedEvent.m_endTime = 0;
    CGCalendar::m_selectedEvent.m_holidayID = 0;
    CGCalendar::m_selectedEvent.m_type = 0;
}

// OFFSET: 0x009AF0E0/0x140736DF0
void CGCalendar::CreateGuildEvent() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (!player->GetGuildGUID()) {
        CGCalendar::CalendarError("ERR_GUILD_PLAYER_NOT_IN_GUILD", 0);
        return;
    }

    if (CGCalendar::m_curEvent) {
        delete CGCalendar::m_curEvent;
    }

    void* mem = SMemAlloc(sizeof(CGCalendarEvent), __FILE__, __LINE__, 0);
    CGCalendar::m_curEvent = mem ? new (mem) CGCalendarEvent(player->m_obj->m_guid, player->m_unit->m_unitFieldLevel, 2) : NULL;
    CGCalendar::m_selectedEvent.m_eventID = 0;
    CGCalendar::m_selectedEvent.m_startTime = 0;
    CGCalendar::m_selectedEvent.m_endTime = 0;
    CGCalendar::m_selectedEvent.m_holidayID = 0;
    CGCalendar::m_selectedEvent.m_type = 0;
}

// OFFSET: 0x009AF210/0x140736F50
void CGCalendar::GuildFilter(uint32_t minLevel, uint32_t maxLevel, uint32_t minRank) {
    if (CGCalendar::m_actionPending) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (!player->GetGuildGUID()) {
        CGCalendar::CalendarError("ERR_GUILD_PLAYER_NOT_IN_GUILD", 0);
        return;
    }

    if (!CGCalendar::m_curEvent || !CGCalendar::m_curEvent->m_new || (CGCalendar::m_curEvent->m_flags & 0x440)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_GUILD_FILTER));
    msg.Put(minLevel);
    msg.Put(maxLevel);
    msg.Put(minRank);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGCalendar::SetActionPending(1);
}

// OFFSET: 0x009AF2F0/0x140737120
void CGCalendar::ArenaTeam(uint32_t index) {
    if (CGCalendar::m_actionPending) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || index >= 3) {
        return;
    }

    uint32_t* info = &player->m_player->m_playerFieldArenaTeamInfo11[index * 7];

    if (!info || !*info || !CGCalendar::m_curEvent || !CGCalendar::m_curEvent->m_new || (CGCalendar::m_curEvent->m_flags & 0x440)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_ARENA_TEAM));
    msg.Put(*info);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGCalendar::SetActionPending(1);
}

// OFFSET: 0x009AF3C0/0x140737270
void CGCalendar::UpdateEvent() {
    if (CGCalendar::m_actionPending || !CGCalendar::m_curEvent || CGCalendar::m_curEvent->m_new) {
        return;
    }

    if (CGCalendar::CanEditCurrentEvent()) {
        if (CGCalendar::m_curEvent->Update()) {
            CGCalendar::SetActionPending(1);
        }
    } else {
        CGCalendar::CalendarError("CALENDAR_ERROR_PERMISSIONS", 0);
    }
}

// OFFSET: 0x009AF530/0x140737460
void CGCalendar::Invite(const char* name) {
    if (CGCalendar::m_actionPending || !CGCalendar::m_curEvent || !CGCalendar::m_curEvent->IsModerator()) {
        return;
    }

    int32_t now;

    if ((unk_BD09AC && unk_BD09AC->m_intValue) || (now = FrameTime::GetCurTimeMs(), !CGCalendar::m_lastInviteTime) || now - CGCalendar::m_lastInviteTime >= 2000) {
        if (CGCalendar::m_curEvent->Invite(name)) {
            CGCalendar::m_lastInviteTime = FrameTime::GetCurTimeMs();
            CGCalendar::SetActionPending(1);
        }
    } else {
        CGCalendar::CalendarError("CALENDAR_ERROR_INVITE_THROTTLED", 0);
    }
}

// OFFSET: 0x009AF5B0/0x140737550
void CGCalendar::InviteRSVP(uint32_t status) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || CGCalendar::m_actionPending || !CGCalendar::m_curEvent) {
        return;
    }

    bool sent;

    if ((CGCalendar::m_curEvent->m_flags & 0x400) && CGCalendar::m_curEvent->m_guildID == player->GetGuildGUID().m_guid) {
        if (status != 8) {
            return;
        }

        sent = CGCalendar::m_curEvent->SignUp(1);
    } else {
        sent = CGCalendar::m_curEvent->InviteRSVP(status);
    }

    if (sent) {
        CGCalendar::SetActionPending(1);
    }
}

// OFFSET: 0x009AF650/0x140737600
void CGCalendar::EventSignUp() {
    if (CGCalendar::m_actionPending || !CGCalendar::m_curEvent || !(static_cast<uint8_t>(CGCalendar::m_curEvent->m_flags >> 10) & 1)) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if ((!player || !CGCalendar::GetCurrentEventInvite(player->m_obj->m_guid)) && CGCalendar::m_curEvent->SignUp(0)) {
        CGCalendar::SetActionPending(1);
    }
}

// OFFSET: 0x009AF6C0/0x140737690
void CGCalendar::ContextMenuInviteRemove(CalendarDayEvent* event) {
    if (!event || CGCalendar::m_actionPending) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    SmartGUID guid;
    guid.m_guid = player->m_obj->m_guid.m_guid;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_EVENT_REMOVE_INVITE));
    msg << guid;
    msg.Put(event->m_inviteID);
    msg.Put(static_cast<uint64_t>(0));
    msg.Put(event->m_eventID);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGCalendar::m_actionPending = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 1);
}

// OFFSET: 0x009AF7D0/0x1407377C0
void CGCalendar::ContextMenuEventSignUp(CalendarDayEvent* event, uint8_t tentative) {
    if (!event || CGCalendar::m_actionPending || ((event->m_type & 0x400) && event->m_inviteStatus != 7)) {
        return;
    }

    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_EVENT_SIGN_UP));
    msg.Put(event->m_eventID);
    msg.Put(tentative);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGCalendar::m_actionPending = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 1);
}

// OFFSET: 0x009AF8C0/0x1407378F0
void CGCalendar::ContextMenuEventRemove(CalendarDayEvent* event) {
    if (!event || CGCalendar::m_actionPending) {
        return;
    }

    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_REMOVE_EVENT));
    msg.Put(event->m_eventID);
    msg.Put(event->m_inviteID);
    msg.Put(event->m_type);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGCalendar::m_actionPending = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 1);
}

// OFFSET: 0x009AF9B0/0x140737A10
void CGCalendar::ContextMenuEventCopy(CalendarDayEvent* event) {
    if (!event) {
        return;
    }

    if (CGCalendar::CanEditEvent(event)) {
        s_eventClipboard = *event;
    } else {
        memset(&s_eventClipboard, 0, sizeof(s_eventClipboard));
        CGCalendar::CalendarError("CALENDAR_ERROR_PERMISSIONS", 0);
    }
}

// OFFSET: 0x009AFA50/0x140737AB0
void CGCalendar::ContextMenuEventPaste(int32_t offset, uint32_t day) {
    if (CGCalendar::m_actionPending || !s_eventClipboard.m_eventID) {
        return;
    }

    int32_t month = 0;
    int32_t year = 0;
    int32_t days = 0;
    int32_t now;

    if ((unk_BD09AC && unk_BD09AC->m_intValue) || (now = FrameTime::GetCurTimeMs(), !CGCalendar::m_lastEventTime) || now - CGCalendar::m_lastEventTime >= 5000) {
        int32_t weekDay;
        CGCalendar::GetMonth(offset, &month, &year, &days, &weekDay);

        if (day >= static_cast<uint32_t>(days)) {
            return;
        }

        WowTime date;
        WowTime::WowDecodeTime(s_eventClipboard.m_startTime, &date);
        date.m_tzHours = 0;
        date.m_month = month;
        date.m_monthDay = day;
        date.m_year = year;
        date.AddDays(0, false);

        if (!CGCalendar::ValidateCreateDate(&date, &g_clientGameTime, NULL)) {
            return;
        }

        uint32_t time;
        WowTime::WowEncodeTime(time, &date);
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_COPY_EVENT));
        msg.Put(s_eventClipboard.m_eventID);
        msg.Put(s_eventClipboard.m_inviteID);
        msg.Put(time);
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGCalendar::m_lastEventTime = FrameTime::GetCurTimeMs();
        CGCalendar::SetActionPending(1);
    } else {
        CGCalendar::CalendarError("CALENDAR_ERROR_EVENT_THROTTLED", 0);
    }
}

// OFFSET: 0x009AFBC0/0x140737DB0
bool CGCalendar::CanComplainAboutEvent(CalendarDayEvent* event) {
    if (!event) {
        return false;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    return player && CGChat::m_complaintsSystemStatus && !(event->m_type & 0x6CC) && event->m_creatorGUID.m_guid != player->m_obj->m_guid.m_guid && !FriendList::s_friendList->IsFriend(event->m_creatorGUID);
}

// OFFSET: 0x009AFC40/0x140737E30
void CGCalendar::Complain(CalendarDayEvent* event) {
    if (CGCalendar::m_actionPending || !CGCalendar::CanComplainAboutEvent(event)) {
        return;
    }

    {
        CDataStore msg;

        if (FriendList::s_friendList->IsComplained(event->m_creatorGUID)) {
            CGCalendar::CalendarError("COMPLAINT_ADDED", 0);
            SmartGUID guid;
            guid.m_guid = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->m_obj->m_guid.m_guid;
            msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_EVENT_REMOVE_INVITE));
            msg << guid;
            msg.Put(event->m_inviteID);
            msg.Put(static_cast<uint64_t>(0));
            msg.Put(event->m_eventID);
        } else {
            FriendList::s_friendList->AddComplained(event->m_creatorGUID);
            msg.Put(static_cast<uint32_t>(CMSG_CALENDAR_COMPLAIN));
            msg.Put(event->m_creatorGUID.m_guid);
            msg.Put(event->m_eventID);
            msg.Put(event->m_inviteID);
        }

        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    CGCalendar::m_actionPending = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_ACTION_PENDING, "%b", 1);
}

// OFFSET: 0x009B0B00
bool CGCalendar::HasPendingInvite(uint64_t eventID) {
    CalendarPendingInvite* next;

    for (CalendarPendingInvite* invite = s_pendingInvites.Head(); !s_pendingInvites.IsEnd(invite); invite = next) {
        next = invite->RawNext();

        if (eventID == invite->m_eventID) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009B4D30/0x14073E1E0
CALENDAREVENTHASH::~CALENDAREVENTHASH() {
    uint32_t count = this->m_events.Count();

    for (uint32_t i = 0; i < count; i++) {
        delete this->m_events.m_data[i];
    }
}

// OFFSET: 0x009B23F0/0x14073AEE0
void CGCalendar::InvalidatePlayer(WOWGUID guid) {
    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return;
    }

    for (CALENDAREVENTHASH* entry = s_eventTables.m_tables[0].m_fulllist.Head(); entry; entry = s_eventTables.m_tables[0].Next(entry)) {
        for (uint32_t i = 0; i < entry->m_events.Count(); i++) {
            if (entry->m_events.m_data[i]->m_creatorGUID == guid) {
                CGCalendar::GetCalendar();
                return;
            }
        }
    }
}

// OFFSET: 0x009B30B0/0x14073BE20
void CGCalendar::AddPendingInvite(CalendarDayEvent* event) {
    CalendarPendingInvite* invite = s_pendingInvites.NewNode(2, 0, 0);

    if (!invite) {
        return;
    }

    event->m_flags |= 0x1;
    invite->m_eventID = event->m_eventID;
    invite->m_inviteID = event->m_inviteID;
    invite->m_inviterGuid = event->m_creatorGUID;
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
}

// OFFSET: 0x009B10F0/0x140739620
bool CalendarEventTables::Delete(int32_t table, uint32_t date, uint64_t eventID) {
    CALENDAREVENTHASH* entry = this->m_tables[table].Ptr(date, HASHKEY_NONE());

    if (entry) {
        uint32_t count = entry->m_events.Count();

        if (count) {
            uint32_t index;

            for (index = 0; index < count; index++) {
                if (entry->m_events.m_data[index]->m_eventID == eventID) {
                    delete entry->m_events.m_data[index];
                    break;
                }
            }

            if (index < count) {
                count--;

                for (; index < count; index++) {
                    entry->m_events.m_data[index] = entry->m_events.m_data[index + 1];
                }

                entry->m_events.SetCount(count);
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x009B11F0/0x1407397B0
CalendarDayEvent* CalendarEventTables::GetEvent(int32_t table, uint32_t date, uint64_t eventID) {
    CalendarDayEvent* event = NULL;
    CALENDAREVENTHASH* entry = this->m_tables[table].Ptr(date, HASHKEY_NONE());

    if (entry) {
        uint32_t count = entry->m_events.Count();

        if (count) {
            for (uint32_t i = 0; i < count; i++) {
                if (entry->m_events.m_data[i]->m_eventID == eventID) {
                    event = entry->m_events.m_data[i];
                    break;
                }
            }
        }
    }

    return event;
}

// OFFSET: 0x009B4440/0x14073D660
bool CGCalendar::DeletePendingInvite(uint64_t eventID) {
    CalendarPendingInvite* next;

    for (CalendarPendingInvite* invite = s_pendingInvites.Head(); !s_pendingInvites.IsEnd(invite); invite = next) {
        next = invite->RawNext();

        if (eventID == invite->m_eventID) {
            s_pendingInvites.DeleteNode(invite);
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009B34D0/0x14073C320
void CGCalendar::ProcessGetEvent(CDataStore* msg) {
    if (!static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        return;
    }

    if (CGCalendar::m_curEvent) {
        delete CGCalendar::m_curEvent;
    }

    void* mem = SMemAlloc(sizeof(CGCalendarEvent), __FILE__, __LINE__, 0);
    CGCalendar::m_curEvent = mem ? new (mem) CGCalendarEvent() : NULL;

    if (!CGCalendar::m_curEvent) {
        return;
    }

    uint8_t sendType;
    msg->Get(sendType);
    int32_t status = -1;

    if (!CGCalendar::m_curEvent->ProcessGetEvent(msg, &status)) {
        return;
    }

    uint64_t eventID = CGCalendar::m_curEvent->m_eventID;

    if (!status && CGCalendar::m_curEvent->m_date > g_clientGameTime && !CGCalendar::HasPendingInvite(eventID)) {
        CalendarPendingInvite* invite = s_pendingInvites.NewNode(2, 0, 0);

        if (invite) {
            invite->m_eventID = eventID;
            CalendarDayEvent* event = s_eventTables.GetEvent((CGCalendar::m_curEvent->m_flags & 0x440) != 0, EncodeDate(CGCalendar::m_curEvent->m_date.m_month, CGCalendar::m_curEvent->m_date.m_monthDay, CGCalendar::m_curEvent->m_date.m_year), eventID);

            if (event) {
                event->m_flags |= 0x1;
                invite->m_inviterGuid = event->m_creatorGUID;
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
        }
    }

    if (sendType == 1 || sendType == 2) {
        if (CGCalendar::m_curEvent->m_pendingNames) {
            CGCalendar::m_curEvent->SetSignalType(sendType == 1 ? 2 : 3);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_NEW_EVENT, "%b", sendType == 2);
        }

        return;
    }

    CGCalendar::m_selectedEvent.m_startTime = 0;
    CGCalendar::m_selectedEvent.m_endTime = 0;
    CGCalendar::m_selectedEvent.m_holidayID = 0;
    CGCalendar::m_selectedEvent.m_type = 0;
    CGCalendar::m_selectedEvent.m_eventID = eventID;
    WowTime::WowEncodeTime(CGCalendar::m_selectedEvent.m_startTime, &CGCalendar::m_curEvent->m_date);

    if (CGCalendar::m_curEvent->m_pendingNames) {
        CGCalendar::m_curEvent->SetSignalType(4);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_OPEN_EVENT, "%s", CGCalendar::GetCalendarTypeString(CGCalendar::m_curEvent->m_flags));
    }
}

// OFFSET: 0x009B1D20/0x14073A710
void CGCalendar::ProcessInvite(CDataStore* msg, uint8_t* clearPending) {
    uint32_t responseTime = 0;
    SmartGUID guid;
    *msg >> guid;
    uint64_t eventID;
    msg->Get(eventID);
    uint64_t inviteID;
    msg->Get(inviteID);
    uint8_t level;
    msg->Get(level);
    uint8_t status;
    msg->Get(status);
    uint8_t inviteType;
    msg->Get(inviteType);

    if (inviteType == 1) {
        msg->Get(responseTime);
    }

    uint8_t clear;
    msg->Get(clear);

    if (!clear) {
        *clearPending = 0;
    }

    if (inviteType == 1) {
        if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
            CalendarDayEvent* event = s_eventTables.GetEvent((CGCalendar::m_curEvent->m_flags & 0x440) != 0, EncodeDate(CGCalendar::m_curEvent->m_date.m_month, CGCalendar::m_curEvent->m_date.m_monthDay, CGCalendar::m_curEvent->m_date.m_year), eventID);

            if (event) {
                event->m_inviteID = inviteID;
                event->m_inviteStatus = status;
                FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
            }
        } else {
            for (CALENDAREVENTHASH* entry = s_eventTables.m_tables[1].m_fulllist.Head(); entry; entry = s_eventTables.m_tables[1].Next(entry)) {
                for (uint32_t i = 0; i < entry->m_events.Count(); i++) {
                    if (entry->m_events.m_data[i]->m_eventID == eventID) {
                        entry->m_events.m_data[i]->m_inviteID = inviteID;
                        entry->m_events.m_data[i]->m_inviteStatus = status;
                        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
                        return;
                    }
                }
            }
        }
    }

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        CGCalendar::m_curEvent->AddInvite(inviteID, guid, level, status, inviteType, responseTime);
    }
}

// OFFSET: 0x009B1F40/0x14073A950
void CGCalendar::ProcessInviteRemoved(CDataStore* msg, uint8_t* clearPending) {
    SmartGUID guid;
    *msg >> guid;
    uint64_t eventID;
    msg->Get(eventID);
    uint32_t flags;
    msg->Get(flags);
    uint8_t clear;
    msg->Get(clear);

    if (!clear) {
        *clearPending = 0;
    }

    if (flags & 0x400) {
        if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
            CalendarDayEvent* event = s_eventTables.GetEvent((CGCalendar::m_curEvent->m_flags & 0x440) != 0, EncodeDate(CGCalendar::m_curEvent->m_date.m_month, CGCalendar::m_curEvent->m_date.m_monthDay, CGCalendar::m_curEvent->m_date.m_year), eventID);

            if (event) {
                event->m_inviteID = 0;
                event->m_inviteStatus = 7;
                FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
            }
        } else {
            for (CALENDAREVENTHASH* entry = s_eventTables.m_tables[1].m_fulllist.Head(); entry; entry = s_eventTables.m_tables[1].Next(entry)) {
                for (uint32_t i = 0; i < entry->m_events.Count(); i++) {
                    if (entry->m_events.m_data[i]->m_eventID == eventID) {
                        entry->m_events.m_data[i]->m_inviteID = 0;
                        entry->m_events.m_data[i]->m_inviteStatus = 7;
                        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
                        return;
                    }
                }
            }
        }
    }

    if (CGCalendar::m_curEvent && CGCalendar::m_curEvent->m_eventID == eventID) {
        CGCalendar::m_curEvent->InviteRemoved(guid);
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_INVITE_LIST, NULL);
    }
}

// OFFSET: 0x009B4490/0x14073D710
void CGCalendar::ProcessInviteStatus(CDataStore* msg, uint8_t* clearPending) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        msg->m_read = msg->m_size;
        return;
    }

    SmartGUID guid;
    *msg >> guid;
    uint64_t eventID;
    msg->Get(eventID);
    uint32_t date;
    msg->Get(date);
    uint32_t flags;
    msg->Get(flags);
    uint8_t status;
    msg->Get(status);
    uint8_t clear;
    msg->Get(clear);
    uint32_t responseTime;
    msg->Get(responseTime);

    if (!clear) {
        *clearPending = 0;
    }

    CalendarDayEvent* event = s_eventTables.GetEvent((flags & 0x440) != 0, CGCalendar::GetDateKey(date), eventID);

    if (!event) {
        return;
    }

    if (guid == player->m_obj->m_guid) {
        event->m_inviteStatus = status;

        if (status) {
            if (CGCalendar::DeletePendingInvite(eventID)) {
                event->m_flags &= ~0x1;
                FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
            }
        } else {
            CGCalendar::AddPendingInvite(event);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
    }

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        CGCalendar::m_curEvent->UpdateInviteStatus(guid, status, responseTime);
    }
}

// OFFSET: 0x009B4820/0x14073DB30
void CGCalendar::ProcessInviteStatusAlert(CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        msg->m_read = msg->m_size;
        return;
    }

    uint64_t eventID;
    msg->Get(eventID);
    uint32_t date;
    msg->Get(date);
    uint32_t flags;
    msg->Get(flags);
    uint8_t status;
    msg->Get(status);

    CalendarDayEvent* event = s_eventTables.GetEvent((flags & 0x440) != 0, CGCalendar::GetDateKey(date), eventID);

    if (!event) {
        if (!CGCalendar::m_calendarReceived) {
            CGCalendar::QueryPendingInvites();
        }

        return;
    }

    event->m_inviteStatus = status;

    if (!(event->m_type & 0x10)) {
        if (status) {
            if (CGCalendar::DeletePendingInvite(eventID)) {
                event->m_flags &= ~0x1;
                FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
            }
        } else if (s_pendingInvites.NewNode(2, 0, 0)) {
            CGCalendar::AddPendingInvite(event);
        }
    }

    if (CGCalendar::m_curEvent && CGCalendar::m_curEvent->m_eventID == eventID) {
        CGCalendar::m_curEvent->UpdateInviteStatus(player->m_obj->m_guid, status, 0);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
}

// OFFSET: 0x009B2110/0x14073AB00
void CGCalendar::SetInviteFlag(uint64_t eventID, uint32_t flag, bool set) {
    CalendarDayEvent* event = NULL;

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        uint32_t flags = CGCalendar::m_curEvent->m_flags;
        WowTime date;
        date.m_month = CGCalendar::m_curEvent->m_date.m_month;
        date.m_monthDay = CGCalendar::m_curEvent->m_date.m_monthDay;
        date.m_year = CGCalendar::m_curEvent->m_date.m_year;
        uint32_t key;
        WowTime::WowEncodeTime(key, &date);
        event = s_eventTables.GetEvent((flags & 0x440) != 0, key, eventID);
    } else {
        for (CALENDAREVENTHASH* entry = s_eventTables.m_tables[0].m_fulllist.Head(); entry && !event; entry = s_eventTables.m_tables[0].Next(entry)) {
            for (uint32_t i = 0; i < entry->m_events.Count(); i++) {
                if (entry->m_events.m_data[i]->m_eventID == eventID) {
                    event = entry->m_events.m_data[i];
                    break;
                }
            }
        }
    }

    if (!event) {
        return;
    }

    if (set) {
        event->m_flags |= flag;
    } else {
        event->m_flags &= ~flag;
    }
}

// INLINED
DYNAMICHOLIDAYHASH* CalendarGetHolidayInfo(int32_t holidayId) {
    return s_dynamicHolidays.Ptr(holidayId, HASHKEY_NONE());
}

// OFFSET: 0x009B3AD0/0x14073CA20
void CGCalendar::ProcessLockoutRemoved(CDataStore* msg) {
    int32_t mapID = -1;
    int32_t difficulty = 0;
    int32_t resetTime = 0;
    uint64_t instanceID = 0;
    msg->Get(reinterpret_cast<uint32_t&>(mapID));

    if (mapID < 0) {
        for (CALENDAREVENTHASH* entry = s_eventTables.m_tables[2].m_fulllist.Head(); entry; entry = s_eventTables.m_tables[2].Next(entry)) {
            uint32_t count = entry->m_events.Count();

            if (!count) {
                continue;
            }

            CalendarDayEvent** keep = static_cast<CalendarDayEvent**>(alloca(count * sizeof(CalendarDayEvent*)));
            uint32_t kept = 0;

            for (uint32_t i = 0; i < count; i++) {
                CalendarDayEvent* event = entry->m_events.m_data[i];

                if (event->m_type & 0x80) {
                    delete event;
                } else {
                    keep[kept++] = event;
                }
            }

            if (kept < count) {
                for (uint32_t i = 0; i < kept; i++) {
                    entry->m_events.m_data[i] = keep[i];
                }

                entry->m_events.SetCount(kept);
            }
        }
    } else {
        msg->Get(reinterpret_cast<uint32_t&>(difficulty));
        msg->Get(reinterpret_cast<uint32_t&>(resetTime));
        msg->Get(instanceID);
        WowTime reset = g_clientGameTime;
        reset.m_tzHours = g_clientGameTime.m_tzHours;
        reset.AddMinutes(resetTime / 60);
        WowTime key;
        key.m_month = reset.m_month;
        key.m_monthDay = reset.m_monthDay;
        key.m_year = reset.m_year;
        uint32_t hash;
        WowTime::WowEncodeTime(hash, &key);
        CALENDAREVENTHASH* entry = s_eventTables.m_tables[2].Ptr(hash, HASHKEY_NONE());

        if (entry) {
            uint32_t count = entry->m_events.Count();

            if (count) {
                CalendarDayEvent** keep = static_cast<CalendarDayEvent**>(alloca(count * sizeof(CalendarDayEvent*)));
                uint32_t kept = 0;

                for (uint32_t i = 0; i < count; i++) {
                    CalendarDayEvent* event = entry->m_events.m_data[i];

                    if ((event->m_type & 0x80) && event->m_holidayID == static_cast<uint32_t>(mapID) && (difficulty == -1 || event->m_endTime == static_cast<uint32_t>(difficulty)) && (instanceID == ~0ull || event->m_eventID == instanceID)) {
                        delete event;
                    } else {
                        keep[kept++] = event;
                    }
                }

                if (kept < count) {
                    for (uint32_t i = 0; i < kept; i++) {
                        entry->m_events.m_data[i] = keep[i];
                    }

                    entry->m_events.SetCount(kept);
                }
            }
        }
    }

    CGCalendar::UpdateDisplayList(true, false);
}

// OFFSET: 0x009B4600/0x14073D8A0
void CGCalendar::ProcessInviteRemovedAlert(CDataStore* msg) {
    uint64_t eventID;
    msg->Get(eventID);
    uint32_t date;
    msg->Get(date);
    uint32_t flags;
    msg->Get(flags);
    uint8_t status;
    msg->Get(status);

    bool remove = true;
    uint32_t key = CGCalendar::GetDateKey(date);
    CalendarDayEvent* event = s_eventTables.GetEvent((flags & 0x440) != 0, key, eventID);

    if (event && (event->m_type & 0x400) && (event->m_flags & 0x8)) {
        event->m_inviteStatus = 7;

        if (CGCalendar::m_curEvent && CGCalendar::m_curEvent->m_eventID == eventID) {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                CGCalendar::m_curEvent->InviteRemoved(player->m_obj->m_guid);
                FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_INVITE_LIST, NULL);
            }
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
        remove = false;
    } else {
        if (CGCalendar::m_selectedEvent.m_eventID == eventID) {
            CGCalendar::m_selectedEvent.m_eventID = 0;
            CGCalendar::m_selectedEvent.m_startTime = 0;
            CGCalendar::m_selectedEvent.m_endTime = 0;
            CGCalendar::m_selectedEvent.m_holidayID = 0;
            CGCalendar::m_selectedEvent.m_type = 0;
        }

        if (CGCalendar::m_curEvent && CGCalendar::m_curEvent->m_eventID == eventID) {
            delete CGCalendar::m_curEvent;
            CGCalendar::m_curEvent = NULL;
            FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_CLOSE_EVENT, NULL);
        }

        if (CGCalendar::m_contextEvent.m_eventID == eventID) {
            CGCalendar::m_contextEvent.m_eventID = 0;
            CGCalendar::m_contextEvent.m_startTime = 0;
            CGCalendar::m_contextEvent.m_endTime = 0;
            CGCalendar::m_contextEvent.m_holidayID = 0;
            CGCalendar::m_contextEvent.m_type = 0;
            CGCalendar::m_contextEventInfo = NULL;
        }
    }

    if (status && status != 9) {
        CGMailInfo::ReceivedMail(0.0f);
    }

    CGCalendar::DeletePendingInvite(eventID);
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);

    if (remove && s_eventTables.Delete((flags & 0x440) != 0, key, eventID)) {
        CGCalendar::UpdateDisplayList(true, false);
    }
}

// OFFSET: 0x009B4980/0x14073DD40
void CGCalendar::ProcessEventRemovedAlert(CDataStore* msg, uint8_t* clearPending) {
    uint8_t clear;
    msg->Get(clear);

    if (!clear) {
        *clearPending = 0;
    }

    uint64_t eventID;
    msg->Get(eventID);
    uint32_t date;
    msg->Get(date);

    if (CGCalendar::m_selectedEvent.m_eventID == eventID) {
        CGCalendar::m_selectedEvent.m_eventID = 0;
        CGCalendar::m_selectedEvent.m_startTime = 0;
        CGCalendar::m_selectedEvent.m_endTime = 0;
        CGCalendar::m_selectedEvent.m_holidayID = 0;
        CGCalendar::m_selectedEvent.m_type = 0;
    }

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        delete CGCalendar::m_curEvent;
        CGCalendar::m_curEvent = NULL;
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_CLOSE_EVENT, NULL);
    }

    if (CGCalendar::m_contextEvent.m_eventID == eventID) {
        CGCalendar::m_contextEvent.m_eventID = 0;
        CGCalendar::m_contextEvent.m_startTime = 0;
        CGCalendar::m_contextEvent.m_endTime = 0;
        CGCalendar::m_contextEvent.m_holidayID = 0;
        CGCalendar::m_contextEvent.m_type = 0;
        CGCalendar::m_contextEventInfo = NULL;
    }

    if (s_eventClipboard.m_eventID == eventID) {
        memset(&s_eventClipboard, 0, sizeof(s_eventClipboard));
    }

    uint32_t key = CGCalendar::GetDateKey(date);
    int32_t table = 0;

    while (!s_eventTables.Delete(table, key, eventID)) {
        if (++table > 1) {
            return;
        }
    }

    CGCalendar::DeletePendingInvite(eventID);
    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
    CGCalendar::UpdateDisplayList(true, table == 1);
}

// OFFSET: 0x009B3DD0/0x14073CE80
void CGCalendar::ProcessInviteModeratorStatusAlert(CDataStore* msg, uint8_t* clearPending) {
    SmartGUID guid;
    *msg >> guid;
    uint64_t eventID;
    msg->Get(eventID);
    uint8_t status;
    msg->Get(status);
    uint8_t clear;
    msg->Get(clear);

    if (!clear) {
        *clearPending = 0;
    }

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        CGCalendar::m_curEvent->UpdateInviteModeratorStatus(guid, status);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_obj->m_guid == guid) {
        CGCalendar::SetInviteFlag(eventID, 0x2, status == 1);
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT_LIST, NULL);
    }
}

// OFFSET: 0x009B3EA0/0x14073CF60
void CGCalendar::GuildChanged() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    if (player->GetGuildGUID()) {
        CGCalendar::GetCalendar();
        return;
    }

    s_eventTables.m_counts[1] = 0;
    s_eventTables.m_tables[1].InternalClear(0);

    if (s_eventClipboard.m_type & 0x440) {
        memset(&s_eventClipboard, 0, sizeof(s_eventClipboard));
    }

    CGCalendar::UpdateDisplayList(true, false);
}

// OFFSET: 0x009B5B70/0x14073F5D0
void CGCalendar::InsertLockoutEvent(int32_t mapID, int32_t difficulty, uint64_t instanceID, uint32_t time) {
    void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
    CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : NULL;
    event->m_holidayID = mapID;
    event->m_endTime = difficulty;
    event->m_eventID = instanceID;
    event->m_title[0] = '\0';
    event->m_startTime = time;
    event->m_type = 0x80;
    CGCalendar::InsertEvent(event, 2);
}

// OFFSET: 0x009B6170/0x14073FE20
void PH_SMSG_CALENDAR_SEND_CALENDAR(CDataStore* msg) {
    CGCalendar::m_numInitialPending = 0;
    s_pendingInvites.Clear();

    uint32_t numInvites;
    msg->Get(numInvites);

    for (uint32_t i = 0; i < numInvites; i++) {
        uint64_t eventID;
        msg->Get(eventID);
        uint64_t inviteID;
        msg->Get(inviteID);
        uint8_t status;
        msg->Get(status);
        uint8_t rank;
        msg->Get(rank);
        uint8_t inviteType;
        msg->Get(inviteType);
        SmartGUID inviter;
        *msg >> inviter;

        CalendarPendingInvite* invite = s_pendingInvites.NewNode(2, 0, 0);

        if (invite) {
            invite->m_eventID = eventID;
            invite->m_inviteID = inviteID;
            invite->m_inviterGuid = inviter;
            invite->m_status = status;
            invite->m_rank = rank;
            invite->m_inviteType = inviteType;
            invite->m_valid = 0;
        }
    }

    s_eventTables.m_counts[0] = 0;
    s_eventTables.m_tables[0].InternalClear(0);
    s_eventTables.m_counts[1] = 0;
    s_eventTables.m_tables[1].InternalClear(0);

    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();
    bool guildEvents = false;
    uint32_t numEvents;
    msg->Get(numEvents);

    for (uint32_t i = 0; i < numEvents; i++) {
        CalendarDayEvent event;
        msg->Get(event.m_eventID);
        msg->GetString(event.m_title, sizeof(event.m_title));
        msg->Get(reinterpret_cast<uint32_t&>(event.m_eventType));
        msg->Get(event.m_startTime);
        event.m_endTime = 0;
        msg->Get(event.m_type);
        msg->Get(reinterpret_cast<uint32_t&>(event.m_textureID));
        msg->Get(event.m_guildGUID.m_guid);
        SmartGUID creator;
        *msg >> creator;
        event.m_flags = (activePlayer && creator == activePlayer) ? 0x4 : 0;
        event.m_inviteID = 0;
        event.m_creatorGUID.m_guid = 0;
        int32_t table;

        if (event.m_type & 0x440) {
            table = 1;
            guildEvents = true;
        } else {
            table = 0;
        }

        if (event.m_type & 0x40) {
            void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
            CalendarDayEvent* copy = mem ? new (mem) CalendarDayEvent() : NULL;
            static_cast<CalendarEventData&>(*copy) = event;
            WowTime alarm;
            WowTime::WowDecodeTime(copy->m_startTime, &alarm);
            alarm.m_tzHours = 0;
            alarm.AddMinutes(-15);
            WowTime::WowEncodeTime(copy->m_alarmTime, &alarm);
            copy->m_alarmCallback = g_clientGameTime.GameTimeRegisterCallback(&alarm, FireEventAlarm, copy);
            copy->m_creatorGUID = creator;
            CGCalendar::InsertEvent(copy, table);
            continue;
        }

        CalendarPendingInvite* invite = s_pendingInvites.Head();

        while (invite && invite->m_eventID != event.m_eventID) {
            invite = s_pendingInvites.Next(invite);
        }

        if (invite) {
            event.m_inviteStatus = invite->m_status;
            event.m_inviteID = invite->m_inviteID;

            if (invite->m_rank) {
                event.m_flags |= 0x2;

                if (invite->m_rank == 2) {
                    event.m_flags |= 0x4;
                }
            }

            if (invite->m_inviteType == 1) {
                event.m_flags |= 0x8;
                event.m_creatorGUID = creator;
            } else {
                event.m_creatorGUID = invite->m_inviterGuid;
            }

            WowTime start;
            WowTime::WowDecodeTime(event.m_startTime, &start);
            start.m_tzHours = 0;

            if (!invite->m_status && start > g_clientGameTime && !(event.m_type & 0x10)) {
                event.m_flags |= 0x1;
                invite->m_valid = 1;
            } else {
                s_pendingInvites.DeleteNode(invite);
                numInvites--;
            }
        } else {
            if (!(event.m_type & 0x400)) {
                continue;
            }

            event.m_flags |= 0x8;
            event.m_creatorGUID = creator;
            event.m_inviteStatus = 7;
        }

        void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
        CalendarDayEvent* copy = mem ? new (mem) CalendarDayEvent() : NULL;
        static_cast<CalendarEventData&>(*copy) = event;
        WowTime alarm;
        WowTime::WowDecodeTime(copy->m_startTime, &alarm);
        alarm.m_tzHours = 0;
        alarm.AddMinutes(-15);
        WowTime::WowEncodeTime(copy->m_alarmTime, &alarm);
        copy->m_alarmCallback = g_clientGameTime.GameTimeRegisterCallback(&alarm, FireEventAlarm, copy);
        CGCalendar::InsertEvent(copy, table);
        WOWGUID none = WOWGUID();
        g_nameDBCache.GetRecord(event.m_creatorGUID.m_guid, none, NULL, NULL, false);
    }

    CalendarPendingInvite* invite = s_pendingInvites.Head();

    while (invite) {
        CalendarPendingInvite* next = invite->Next();

        if (!invite->m_valid || !invite->m_inviterGuid) {
            s_pendingInvites.DeleteNode(invite);
            numInvites--;
        } else if (FriendList::s_friendList->IsIgnored(invite->m_inviterGuid, false)) {
            numInvites--;
        }

        invite = next;
    }

    WowTime serverTime;
    uint32_t serverNow = 0;
    msg->Get(serverNow);
    uint32_t serverDate;
    msg->Get(serverDate);
    WowTime::WowDecodeTime(serverDate, &serverTime);

    s_eventTables.m_counts[2] = 0;
    s_eventTables.m_tables[2].InternalClear(0);

    uint32_t numLockouts;
    msg->Get(numLockouts);

    for (uint32_t i = 0; i < numLockouts; i++) {
        int32_t mapID;
        msg->Get(reinterpret_cast<uint32_t&>(mapID));
        int32_t difficulty;
        msg->Get(reinterpret_cast<uint32_t&>(difficulty));
        int32_t resetTime;
        msg->Get(reinterpret_cast<uint32_t&>(resetTime));
        uint64_t instanceID;
        msg->Get(instanceID);
        WowTime reset = serverTime;
        reset.m_tzHours = serverTime.m_tzHours;
        reset.AddMinutes(resetTime / 60);
        uint32_t time;
        WowTime::WowEncodeTime(time, &reset);
        CGCalendar::InsertLockoutEvent(mapID, difficulty, instanceID, time);
    }

    uint32_t baseTime = 0;
    msg->Get(baseTime);
    s_raidResets.Clear();
    uint32_t numResets;
    msg->Get(numResets);

    for (uint32_t i = 0; i < numResets; i++) {
        int32_t mapID;
        msg->Get(reinterpret_cast<uint32_t&>(mapID));
        uint32_t period;
        msg->Get(period);
        uint32_t offset;
        msg->Get(offset);

        CALENDARRAIDRESETNODE* node = s_raidResets.NewNode(2, 0, 0);

        if (node) {
            WowTime reset = serverTime;
            reset.m_tzHours = serverTime.m_tzHours;
            uint32_t start = baseTime + offset;
            uint32_t periods = static_cast<uint32_t>(static_cast<float>(serverNow - start) / static_cast<float>(period));
            reset.AddMinutes(static_cast<int32_t>(start + periods * period - serverNow) / 60);
            uint32_t time;
            WowTime::WowEncodeTime(time, &reset);
            node->m_mapID = mapID;
            node->m_interval = period / 60;
            node->m_startTime = time;
        }
    }

    uint32_t numHolidays;
    msg->Get(numHolidays);

    if (numHolidays) {
        s_dynamicHolidays.InternalClear(0);
        s_holidays.Clear();

        for (uint32_t i = 0; i < numHolidays; i++) {
            uint32_t id;
            msg->Get(id);
            int32_t region;
            msg->Get(reinterpret_cast<uint32_t&>(region));
            int32_t looping;
            msg->Get(reinterpret_cast<uint32_t&>(looping));
            int32_t priority;
            msg->Get(reinterpret_cast<uint32_t&>(priority));
            int32_t filterType;
            msg->Get(reinterpret_cast<uint32_t&>(filterType));

            int32_t dates[26];

            for (uint32_t j = 0; j < 26; j++) {
                msg->Get(reinterpret_cast<uint32_t&>(dates[j]));
            }

            int32_t durations[10];

            for (uint32_t j = 0; j < 10; j++) {
                msg->Get(reinterpret_cast<uint32_t&>(durations[j]));
            }

            int32_t flags[10];

            for (uint32_t j = 0; j < 10; j++) {
                msg->Get(reinterpret_cast<uint32_t&>(flags[j]));
            }

            char textureFileName[64];
            msg->GetString(textureFileName, sizeof(textureFileName));

            DYNAMICHOLIDAYHASH* holiday = s_dynamicHolidays.New(id, HASHKEY_NONE(), 0, 0);
            holiday->m_region = region;
            holiday->m_looping = looping;
            holiday->m_priority = priority;
            holiday->m_calendarFilterType = filterType;

            for (uint32_t j = 0; j < 26; j++) {
                holiday->m_date[j] = dates[j];
            }

            for (uint32_t j = 0; j < 10; j++) {
                holiday->m_duration[j] = durations[j];
            }

            for (uint32_t j = 0; j < 10; j++) {
                holiday->m_calendarFlags[j] = flags[j];
            }

            SStrCopy(holiday->m_textureFileName, textureFileName, sizeof(holiday->m_textureFileName));
        }
    }

    CGCalendar::CalculateEvents(true);

    if (numInvites) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
    }

    if (guildEvents) {
        CGCalendar::UpdateGuildEvents();
    }

    CGCalendar::m_calendarReceived = 1;
}

// OFFSET: 0x009B6F10/0x140741130
void CGCalendar::ProcessLockoutAdded(CDataStore* msg) {
    WowTime date;
    uint32_t time;
    msg->Get(time);
    WowTime::WowDecodeTime(time, &date);
    int32_t mapID;
    msg->Get(reinterpret_cast<uint32_t&>(mapID));
    int32_t difficulty;
    msg->Get(reinterpret_cast<uint32_t&>(difficulty));
    int32_t resetTime;
    msg->Get(reinterpret_cast<uint32_t&>(resetTime));
    uint64_t instanceID;
    msg->Get(instanceID);
    date.AddMinutes(resetTime / 60);
    WowTime::WowEncodeTime(time, &date);
    CGCalendar::InsertLockoutEvent(mapID, difficulty, instanceID, time);
    CGCalendar::UpdateDisplayList(true, false);
}

// OFFSET: 0x009B6FC0/0x1407411F0
void CGCalendar::ProcessLockoutUpdated(CDataStore* msg) {
    int32_t mapID = -1;
    int32_t difficulty = 0;
    int32_t oldReset = 0;
    int32_t newReset = 0;
    WowTime oldDate;
    uint32_t time;
    msg->Get(time);
    WowTime::WowDecodeTime(time, &oldDate);
    WowTime newDate = oldDate;
    newDate.m_tzHours = oldDate.m_tzHours;
    msg->Get(reinterpret_cast<uint32_t&>(mapID));
    msg->Get(reinterpret_cast<uint32_t&>(difficulty));
    msg->Get(reinterpret_cast<uint32_t&>(oldReset));
    msg->Get(reinterpret_cast<uint32_t&>(newReset));
    oldDate.AddMinutes(oldReset / 60);
    newDate.AddMinutes(newReset / 60);

    WowTime key;
    key.m_month = oldDate.m_month;
    key.m_monthDay = oldDate.m_monthDay;
    key.m_year = oldDate.m_year;
    uint32_t hash;
    WowTime::WowEncodeTime(hash, &key);
    CALENDAREVENTHASH* entry = s_eventTables.m_tables[2].Ptr(hash, HASHKEY_NONE());

    if (entry) {
        uint32_t count = entry->m_events.Count();

        if (count) {
            CalendarDayEvent** keep = static_cast<CalendarDayEvent**>(alloca(count * sizeof(CalendarDayEvent*)));
            uint32_t kept = 0;

            for (uint32_t i = 0; i < count; i++) {
                CalendarDayEvent* event = entry->m_events.m_data[i];

                if ((event->m_type & 0x80) && event->m_holidayID == static_cast<uint32_t>(mapID) && event->m_endTime == static_cast<uint32_t>(difficulty) && EncodeDate(newDate.m_month, newDate.m_monthDay, newDate.m_year) != hash) {
                    WowTime::WowEncodeTime(time, &newDate);
                    event->m_startTime = time;
                    CGCalendar::InsertEvent(event, 2);
                } else {
                    keep[kept++] = event;
                }
            }

            if (kept < count) {
                for (uint32_t i = 0; i < kept; i++) {
                    entry->m_events.m_data[i] = keep[i];
                }

                entry->m_events.SetCount(kept);
            }
        }
    }

    CGCalendar::UpdateDisplayList(true, false);
}

// OFFSET: 0x009B71F0/0x1407414D0
void CGCalendar::ProcessInviteAlert(CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
    CalendarDayEvent* event = mem ? new (mem) CalendarDayEvent() : NULL;
    event->m_flags = 0;
    msg->Get(event->m_eventID);
    msg->GetString(event->m_title, sizeof(event->m_title));
    msg->Get(event->m_startTime);
    event->m_endTime = 0;
    msg->Get(event->m_type);
    msg->Get(reinterpret_cast<uint32_t&>(event->m_eventType));
    msg->Get(reinterpret_cast<uint32_t&>(event->m_textureID));
    msg->Get(event->m_guildGUID.m_guid);
    msg->Get(event->m_inviteID);
    msg->Get(event->m_inviteStatus);
    uint8_t rank;
    msg->Get(rank);
    SmartGUID creator;
    *msg >> creator;
    SmartGUID inviter;
    *msg >> inviter;
    event->ScheduleAlarm();

    bool pending = false;

    if (event->m_inviteStatus == 7 || event->m_inviteStatus == 6) {
        event->m_creatorGUID = creator;
        event->m_flags = 0x8;
    } else {
        event->m_creatorGUID = inviter;

        if (!event->m_inviteStatus && !(event->m_type & 0x40)) {
            pending = true;
            event->m_flags = 0x1;
        }
    }

    if (rank) {
        event->m_flags |= 0x2;
    }

    if (creator == player->m_obj->m_guid || rank == 2) {
        event->m_flags |= 0x4;
    }

    bool table = (event->m_type & 0x440) != 0;
    CGCalendar::InsertEvent(event, table);

    if (pending) {
        CalendarPendingInvite* invite = s_pendingInvites.NewNode(2, 0, 0);

        if (invite) {
            invite->m_eventID = event->m_eventID;
            invite->m_inviterGuid = event->m_creatorGUID;

            if (!FriendList::s_friendList->IsIgnored(event->m_creatorGUID, false)) {
                FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
            }
        }
    }

    CGCalendar::UpdateDisplayList(true, table);
}

// OFFSET: 0x009B74A0/0x140741820
void CGCalendar::ProcessEventUpdatedAlert(CDataStore* msg, uint8_t* clearPending) {
    uint8_t clear;
    msg->Get(clear);

    if (!clear) {
        *clearPending = 0;
    }

    uint64_t eventID;
    msg->Get(eventID);
    uint32_t oldDate;
    msg->Get(oldDate);
    uint32_t read = msg->m_read;
    uint32_t flags;
    msg->Get(flags);
    uint32_t newDate;
    msg->Get(newDate);
    uint8_t eventType;
    msg->Get(eventType);
    uint32_t textureID;
    msg->Get(textureID);
    char title[128];
    msg->GetString(title, sizeof(title));

    int32_t table = (flags & 0x440) != 0;
    uint32_t key = CGCalendar::GetDateKey(oldDate);
    CalendarDayEvent* event = s_eventTables.GetEvent(table, key, eventID);

    if (event) {
        bool locked = (event->m_type & 0x10) != 0;

        if (oldDate != newDate) {
            void* mem = SMemAlloc(sizeof(CalendarDayEvent), __FILE__, __LINE__, 0);
            CalendarDayEvent* moved = mem ? new (mem) CalendarDayEvent() : NULL;
            moved->m_eventID = eventID;
            moved->m_startTime = newDate;
            moved->m_endTime = 0;
            moved->m_flags = event->m_flags;
            moved->m_inviteStatus = event->m_inviteStatus;
            moved->m_inviteID = event->m_inviteID;
            moved->m_creatorGUID = event->m_creatorGUID;
            moved->m_sequenceIndex = event->m_sequenceIndex;
            moved->m_sequenceLength = event->m_sequenceLength;
            moved->ScheduleAlarm();
            moved->m_guildGUID = event->m_guildGUID;
            CGCalendar::InsertEvent(moved, table);
            s_eventTables.Delete(table, key, eventID);

            if (CGCalendar::m_contextEventInfo == event) {
                CGCalendar::m_contextEventInfo = moved;
            }

            event = moved;

            if (table) {
                CGCalendar::UpdateGuildEvents();
            }
        }

        SStrCopy(event->m_title, title, sizeof(event->m_title));
        event->m_type = flags;
        event->m_eventType = eventType;
        event->m_textureID = textureID;

        if (locked != ((flags & 0x10) != 0) && !event->m_inviteStatus) {
            if (flags & 0x10) {
                if (CGCalendar::DeletePendingInvite(eventID)) {
                    event->m_flags &= ~0x1;
                    FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_PENDING_INVITES, NULL);
                }
            } else {
                CGCalendar::AddPendingInvite(event);
            }
        }

        CGCalendar::UpdateDisplayList(true, false);
    } else if (!CGCalendar::m_calendarReceived) {
        CGCalendar::QueryPendingInvites();
    }

    if (CGCalendar::m_curEvent && eventID == CGCalendar::m_curEvent->m_eventID) {
        msg->m_read = read;

        if (CGCalendar::m_curEvent->ProcessUpdateEvent(msg)) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CALENDAR_UPDATE_EVENT, NULL);
        }
    } else {
        msg->m_read = msg->m_size;
    }
}

// OFFSET: 0x00401000/0x1407D7E80
void CGCalendar::LeaveWorld() {
}
