#ifndef WOW_SOURCE_UI_LFGUILDINFO_H
#define WOW_SOURCE_UI_LFGUILDINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"

class CGLFGuildInfo {
    public:
    static bool s_browsePending;
    static bool s_listed;
    static uint64_t s_selectedGuild;
    static uint64_t s_selectedApplicant;
    static int32_t s_recruitsLastUpdate;
    static int32_t s_numApplicationsRemaining;
    static JamLFGuildSettings s_settings;
    static TSGrowableArray<LFGuildBrowseData> s_browseResults;
    static TSGrowableArray<LFGuildRecruitData> s_recruits;
    static TSGrowableArray<LFGuildApplicationData> s_applications;

    static void ShutdownGame();
    static void InitializeGame();
    static void ClearRecruitmentSettings(bool signal);
    static void SendPost();
    static void SendBrowse();
    static void GetLookingForGuildSettings(uint32_t& interests, uint32_t& availability, uint32_t& classRoles);
    static bool ClientLFGuildPostHandler(ClientLfGuildPostUpdated* msg);
    static bool ClientLFGuildBrowseHandler(ClientLfGuildBrowseUpdated* msg);
    static bool ClientLFGuildRecruitsHandler(ClientLfGuildRecruitListUpdated* msg);
    static bool ClientLFGuildCommandResultHandler(ClientLfGuildCommandResult* msg);
    static bool ClientLFGuildMembershipListHandler(ClientLfGuildMembershipListUpdated* msg);
    static bool ClientLFGuildRecruitListChangedHandler(ClientLfGuildApplicantListUpdated* msg);
    static bool ClientLFGuildMembershipListChangedHandler(ClientLfGuildApplicationsListChanged* msg);
};

#endif
