#include "WoW/Source/Ui/LFGuildInfo.h"
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/JamClientAutoCode/JamGlobal.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F763F0/0x140E6C450
bool CGLFGuildInfo::s_browsePending;

// STATIC: 0x00F763F1/0x140E6C451
bool CGLFGuildInfo::s_listed;

// STATIC: 0x00F763F8/0x140E6C458
uint64_t CGLFGuildInfo::s_selectedGuild;

// STATIC: 0x00F76400/0x140E6C460
uint64_t CGLFGuildInfo::s_selectedApplicant;

// STATIC: 0x00F76408/0x140E6C468
int32_t CGLFGuildInfo::s_recruitsLastUpdate;

// STATIC: 0x00F7640C/0x140E6C470
int32_t CGLFGuildInfo::s_numApplicationsRemaining;

// STATIC: 0x00F76410
JamLFGuildSettings CGLFGuildInfo::s_settings;

// STATIC: 0x00F76824/0x140E6C898
TSGrowableArray<LFGuildBrowseData> CGLFGuildInfo::s_browseResults;

// STATIC: 0x00F76834/0x140E6C8B4
TSGrowableArray<LFGuildRecruitData> CGLFGuildInfo::s_recruits;

// STATIC: 0x00F76844/0x140E6C8CC
TSGrowableArray<LFGuildApplicationData> CGLFGuildInfo::s_applications;

// PROVEN: 85b3fec8 IDENTICAL/IDENTICAL
// OFFSET: 0x009EB9B0/0x1407849C0
bool CGLFGuildInfo::ClientLFGuildRecruitListChangedHandler(ClientLfGuildApplicantListUpdated* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_RECRUIT_LIST_CHANGED, NULL);
    return true;
}

// PROVEN: ac7bff11 IDENTICAL/IDENTICAL
// OFFSET: 0x009EB9D0/0x1407849E0
bool CGLFGuildInfo::ClientLFGuildMembershipListChangedHandler(ClientLfGuildApplicationsListChanged* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_MEMBERSHIP_LIST_CHANGED, NULL);
    return true;
}

// PROVEN: 41bbb9be IDENTICAL/IDENTICAL
// OFFSET: 0x009EBC10/0x140784C60
void CGLFGuildInfo::ShutdownGame() {
    ClientLfGuildPostUpdated::s_handler = NULL;
    ClientLfGuildBrowseUpdated::s_handler = NULL;
    ClientLfGuildRecruitListUpdated::s_handler = NULL;
    ClientLfGuildCommandResult::s_handler = NULL;
    ClientLfGuildMembershipListUpdated::s_handler = NULL;
    ClientLfGuildApplicantListUpdated::s_handler = NULL;
    ClientLfGuildApplicationsListChanged::s_handler = NULL;
}

// OFFSET: 0x009EBC40/0x140784CA0
bool CGLFGuildInfo::ClientLFGuildPostHandler(ClientLfGuildPostUpdated* msg) {
    const JamLFGuildPost* post = msg->m_hasPost ? &msg->m_post : NULL;

    if (!post) {
        memset(&CGLFGuildInfo::s_settings, 0, sizeof(CGLFGuildInfo::s_settings));
        CGLFGuildInfo::s_listed = false;
        FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_POST_UPDATED, NULL);
        return true;
    }

    CGLFGuildInfo::s_settings = post->m_settings;
    CGLFGuildInfo::s_listed = post->m_listed;
    FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_POST_UPDATED, NULL);
    return true;
}

// PROVEN: ba13be1b IDENTICAL/IDENTICAL
// OFFSET: 0x009EBD20/0x140784D70
bool CGLFGuildInfo::ClientLFGuildCommandResultHandler(ClientLfGuildCommandResult* msg) {
    if (!msg->m_result) {
        CGGameUI::DisplayError(ERR_GUILD_INTERNAL);
    }

    return true;
}

// PROVEN: 6de3a50e IDENTICAL/DIFFERENT
// OFFSET: 0x009EBD40/0x140784D90
void CGLFGuildInfo::SendPost() {
    GlobalLFGuildSetGuildPost msg;
    msg.m_interests = CGLFGuildInfo::s_settings.m_interests;
    msg.m_availability = CGLFGuildInfo::s_settings.m_availability;
    msg.m_classRoles = CGLFGuildInfo::s_settings.m_classRoles;
    msg.m_level = CGLFGuildInfo::s_settings.m_level;
    msg.m_listed = CGLFGuildInfo::s_listed;
    SStrCopy(msg.m_comment, CGLFGuildInfo::s_settings.m_comment, sizeof(msg.m_comment));
    ClientServices::Send(&msg);
}

// PROVEN: ea20f7b6 IDENTICAL/NONE
// OFFSET: 0x009EBDD0
void CGLFGuildInfo::GetLookingForGuildSettings(uint32_t& interests, uint32_t& availability, uint32_t& classRoles) {
    CVar* var = CVar::Lookup("lfGuildSettings");

    if (var) {
        int32_t value = var->m_intValue;
        interests = value & 0x1F;
        availability = (value >> 5) & 3;
        classRoles = (value >> 7) & 7;
    } else {
        interests = 1;
        availability = 1;
        classRoles = 4;
    }
}

// OFFSET: 0x009ECCC0/0x140785CB0
void CGLFGuildInfo::SendBrowse() {
    if (CGLFGuildInfo::s_browsePending) {
        return;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGLFGuildInfo::s_browsePending = true;
    uint32_t classRoles;
    uint32_t availability;
    uint32_t interests;
    CGLFGuildInfo::GetLookingForGuildSettings(interests, availability, classRoles);
    GlobalLFGuildBrowse msg;
    msg.m_classRoles = classRoles;
    msg.m_interests = interests;
    msg.m_availability = availability;
    msg.m_characterLevel = player->m_unit->m_unitFieldLevel;
    ClientServices::Send(&msg);
}

// OFFSET: 0x009ED0B0/0x140786190
void CGLFGuildInfo::ClearRecruitmentSettings(bool signal) {
    memset(&CGLFGuildInfo::s_settings, 0, sizeof(CGLFGuildInfo::s_settings));
    CGLFGuildInfo::s_settings.m_interests = 1;
    CGLFGuildInfo::s_settings.m_availability = 1;
    CGLFGuildInfo::s_settings.m_classRoles = 4;
    CGLFGuildInfo::s_settings.m_level = 1;
    CGLFGuildInfo::s_browseResults.Clear();
    CGLFGuildInfo::s_recruits.Clear();
    CGLFGuildInfo::s_applications.Clear();
    CGLFGuildInfo::s_numApplicationsRemaining = 0;
    CGLFGuildInfo::s_recruitsLastUpdate = 0;
    CGLFGuildInfo::s_selectedApplicant = 0;
    CGLFGuildInfo::s_selectedGuild = 0;
    CGLFGuildInfo::s_browsePending = false;
    CGLFGuildInfo::s_listed = false;

    if (signal) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_POST_UPDATED, NULL);
    }
}

// OFFSET: 0x009ED190
bool CGLFGuildInfo::ClientLFGuildBrowseHandler(ClientLfGuildBrowseUpdated* msg) {
    CGLFGuildInfo::s_browseResults.Clear();
    CGLFGuildInfo::s_browseResults.SetCount(msg->m_guilds.m_count);
    CGLFGuildInfo::s_browsePending = false;

    for (uint32_t i = 0; i < msg->m_guilds.m_count; i++) {
        CGLFGuildInfo::s_browseResults.m_data[i] = msg->m_guilds.m_data[i];
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_BROWSE_UPDATED, NULL);
    return true;
}

// OFFSET: 0x009ED210
bool CGLFGuildInfo::ClientLFGuildRecruitsHandler(ClientLfGuildRecruitListUpdated* msg) {
    if (msg->m_updateTime) {
        CGLFGuildInfo::s_recruitsLastUpdate = msg->m_updateTime;
        CGLFGuildInfo::s_recruits.Clear();
        CGLFGuildInfo::s_recruits.SetCount(msg->m_recruits.m_count);

        for (uint32_t i = 0; i < msg->m_recruits.m_count; i++) {
            CGLFGuildInfo::s_recruits.m_data[i] = msg->m_recruits.m_data[i];
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_RECRUITS_UPDATED, NULL);
    return true;
}

// OFFSET: 0x009ED2A0
bool CGLFGuildInfo::ClientLFGuildMembershipListHandler(ClientLfGuildMembershipListUpdated* msg) {
    CGLFGuildInfo::s_applications.Clear();
    CGLFGuildInfo::s_applications.SetCount(msg->m_applications.m_count);

    for (uint32_t i = 0; i < msg->m_applications.m_count; i++) {
        CGLFGuildInfo::s_applications.m_data[i] = msg->m_applications.m_data[i];
    }

    CGLFGuildInfo::s_numApplicationsRemaining = msg->m_numRemaining;
    FrameScript_SignalEvent(SCRIPT_EVENT_LF_GUILD_MEMBERSHIP_LIST_UPDATED, "%d", CGLFGuildInfo::s_numApplicationsRemaining);
    return true;
}

// OFFSET: 0x009ED330/0x140786660
void CGLFGuildInfo::InitializeGame() {
    CGLFGuildInfo::ClearRecruitmentSettings(false);
    ClientLfGuildPostUpdated::s_handler = JamHandlerEncode(&CGLFGuildInfo::ClientLFGuildPostHandler, g_jamHandlerKey_CFD344);
    ClientLfGuildBrowseUpdated::s_handler = JamHandlerEncode(&CGLFGuildInfo::ClientLFGuildBrowseHandler, g_jamHandlerKey_CFF49C);
    ClientLfGuildRecruitListUpdated::s_handler = JamHandlerEncode(&CGLFGuildInfo::ClientLFGuildRecruitsHandler, g_jamHandlerKey_CFEDB4);
    ClientLfGuildCommandResult::s_handler = JamHandlerEncode(&CGLFGuildInfo::ClientLFGuildCommandResultHandler, g_jamHandlerKey_CFE94C);
    ClientLfGuildMembershipListUpdated::s_handler = JamHandlerEncode(&CGLFGuildInfo::ClientLFGuildMembershipListHandler, g_jamHandlerKey_CFE774);
    ClientLfGuildApplicantListUpdated::s_handler = JamHandlerEncode(&CGLFGuildInfo::ClientLFGuildRecruitListChangedHandler, g_jamHandlerKey_CFF2D0);
    ClientLfGuildApplicationsListChanged::s_handler = JamHandlerEncode(&CGLFGuildInfo::ClientLFGuildMembershipListChangedHandler, g_jamHandlerKey_CFD344);
}
