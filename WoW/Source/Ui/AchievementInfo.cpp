#include "AchievementInfo.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <ctime>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/DB/Rec/Achievement_CategoryRec.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Ui/ChatFrame.h"

// STATIC: 0x00D12828
TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_criteria;

// STATIC: 0x00D12834
TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_recentCriteria;

// STATIC: 0x00D12840
TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_completedAchievements;

// STATIC: 0x00D1284C
TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_inspectCriteria;

// STATIC: 0x00D12858
TSList<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>> s_inspectRecentCriteria;

// STATIC: 0x00D12864
TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_inspectAchievements;

// STATIC: 0x00D12870
TSList<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>> s_trackedAchievements;

// STATIC: 0x00D1287C
TSList<COMPLETED_QUESTS, TSGetLink<COMPLETED_QUESTS>> s_completedQuests;

// STATIC: 0x00F43AE0
static int32_t s_unkC1F450;

// STATIC: 0x00F43AE4
static int32_t s_focusedAchievement;

// STATIC: 0x00F43AF8
TSHashTable<ACHIEVEMENT, HASHKEY_NONE> s_achievements;

// STATIC: 0x00F43B24
TSHashTable<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE> s_guildAchievementMembers;

// STATIC: 0x00F43AE8
int32_t CGAchievementInfo::m_showAchievementUI;

// STATIC: 0x00F43AF0
WOWGUID CGAchievementInfo::m_inspect;

// OFFSET: 0x0094E0D0
int32_t Packet_SMSG_QUERY_QUESTS_COMPLETED_RESPONSE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t count;
    msg->Get(count);
    COMPLETED_QUESTS* node = s_completedQuests.Head();

    while (node) {
        node = s_completedQuests.DeleteNode(node);
    }

    s_completedQuests.Clear();

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        COMPLETED_QUESTS* quest = s_completedQuests.NewNode(2, 0, 0);
        msg->Get(reinterpret_cast<uint32_t&>(quest->m_questId));
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_QUERY_COMPLETE, NULL);
    return 1;
}

// OFFSET: 0x00947810
static void GuildAchievementMemberNameCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_RECEIVED_ACHIEVEMENT_MEMBER_LIST, "%d", param);
    }
}

// OFFSET: 0x00949040
static bool OnGuildCriteriaDeleted(ClientGuildCriteriaDeleted* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return true;
    }

    if (player->GetGuildGUID() != *reinterpret_cast<WOWGUID*>(msg->unk_0010)) {
        return true;
    }

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head(); !s_criteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_criteria.RawNext(node);

        if (node->m_id == static_cast<int32_t>(msg->unk_0018)) {
            s_criteria.UnlinkNode(node);
            FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, NULL);
            node->~CLIENT_ACHIEVEMENT_CRITERIA();
            SMemFree(node, "delete", -1, 0);
            return true;
        }

        node = next;
    }

    return true;
}

// OFFSET: 0x0094B450
static bool OnGuildCriteriaData(ClientGuildCriteriaData* msg) {
    for (uint32_t i = 0; i < msg->unk_0010.Count(); i++) {
        JamGuildCriteriaProgress& progress = msg->unk_0010[i];
        WowTime date;
        WowTime::WowDecodeTime(progress.unk_000C, &date);
        CGAchievementInfo::UpdateCriteria(progress.unk_0000, *reinterpret_cast<uint64_t*>(progress.unk_0010), progress.unk_0020, date, *reinterpret_cast<WOWGUID*>(progress.unk_0018), progress.unk_0008, progress.unk_0004);
        Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(progress.unk_0000);

        if (!criteria) {
            continue;
        }

        if (criteria->m_achievementID == s_focusedAchievement) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, NULL);
        }

        if (criteria->m_timerTime) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d%d%d%d", criteria->m_achievementID, criteria->m_id, ::time(NULL) - progress.unk_0008, criteria->m_timerTime);
        } else {
            for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
                CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);
                AchievementRec* achievement;

                if (criteria->m_achievementID == node->m_id || ((achievement = g_achievementDB.GetRecord(node->m_id)) && achievement->m_sharesCriteria == criteria->m_achievementID)) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", node->m_id);
                }

                node = next;
            }
        }
    }

    return true;
}

// OFFSET: 0x0094B6B0
static bool OnGuildAchievementDeleted(ClientGuildAchievementDeleted* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return true;
    }

    if (player->GetGuildGUID() != *reinterpret_cast<WOWGUID*>(msg->unk_0038)) {
        return true;
    }

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); !s_completedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_completedAchievements.RawNext(node);

        if (node->m_id == static_cast<int32_t>(msg->unk_0030)) {
            s_completedAchievements.UnlinkNode(node);
            FrameScript_SignalEvent(SCRIPT_EVENT_ACHIEVEMENT_EARNED, "%d", msg->unk_0030);
            node->~CLIENT_ACHIEVEMENTS();
            SMemFree(node, "delete", -1, 0);
            return true;
        }

        node = next;
    }

    return true;
}

// OFFSET: 0x0094B7D0
static bool OnGuildAchievementData(ClientGuildAchievementData* msg) {
    int32_t numAchievements = msg->unk_0010.Count();

    for (int32_t i = 0; i < numAchievements; i++) {
        CGAchievementInfo::UpdateAchievement(msg->unk_0010[i].unk_0000, msg->unk_0010[i].unk_0004);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_RECEIVED_ACHIEVEMENT_LIST, NULL);
    return true;
}

// OFFSET: 0x0094B850
static bool OnAllAchievementData(ClientAllAchievementData* msg) {
    CGAchievementInfo::m_showAchievementUI = 1;
    int32_t now = static_cast<int32_t>(::time(NULL));

    int32_t numAchievements = msg->unk_0010.Count();

    for (int32_t i = 0; i < numAchievements; i++) {
        CGAchievementInfo::UpdateAchievement(msg->unk_0010[i].unk_0000, msg->unk_0010[i].unk_0004);
    }

    int32_t numCriteria = msg->unk_0020.Count();

    for (int32_t i = 0; i < numCriteria; i++) {
        JamCriteriaProgress& progress = msg->unk_0020[i];
        CGAchievementInfo::UpdateCriteria(progress.unk_0000, *reinterpret_cast<uint64_t*>(progress.unk_0008), progress.unk_0018, progress.unk_001C, *reinterpret_cast<WOWGUID*>(progress.unk_0010), now - progress.unk_003C, now - progress.unk_0040);
        Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(progress.unk_0000);

        if (criteria && criteria->m_timerTime) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d%d%d%d", criteria->m_achievementID, criteria->m_id, progress.unk_003C, criteria->m_timerTime);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_RECEIVED_ACHIEVEMENT_LIST, NULL);
    return true;
}

// OFFSET: 0x0094B9A0
static bool OnRespondInspectAchievements(ClientRespondInspectAchievements* msg) {
    int32_t now = static_cast<int32_t>(::time(NULL));
    WOWGUID guid = *reinterpret_cast<WOWGUID*>(msg->unk_0030);

    int32_t numAchievements = msg->unk_0010.Count();

    for (int32_t i = 0; i < numAchievements; i++) {
        CGAchievementInfo::UpdateInspectAchievement(guid, msg->unk_0010[i].unk_0000, msg->unk_0010[i].unk_0004);
    }

    int32_t numCriteria = msg->unk_0020.Count();

    for (int32_t i = 0; i < numCriteria; i++) {
        JamCriteriaProgress& progress = msg->unk_0020[i];
        CGAchievementInfo::UpdateInspectCriteria(guid, progress.unk_0000, *reinterpret_cast<uint64_t*>(progress.unk_0008), *reinterpret_cast<WOWGUID*>(progress.unk_0010), progress.unk_0018, progress.unk_001C, now - progress.unk_003C, now - progress.unk_0040);
    }

    char buffer[20];
    FrameScript_SignalEvent(SCRIPT_EVENT_INSPECT_ACHIEVEMENT_READY, "%s", GUIDToString(guid, buffer));
    return true;
}

// OFFSET: 0x0094BAD0
void CGAchievementInfo::ClearGuildInfo() {
    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);
        AchievementRec* achievement = g_achievementDB.GetRecord(node->m_id);

        if (achievement && (achievement->m_flags & 0x4000)) {
            s_trackedAchievements.UnlinkNode(node);
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", node->m_id);
            node->~CLIENT_ACHIEVEMENTS();
            SMemFree(node, "delete", -1, 0);
        }

        node = next;
    }

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); !s_completedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_completedAchievements.RawNext(node);
        AchievementRec* achievement = g_achievementDB.GetRecord(node->m_id);

        if (achievement && (achievement->m_flags & 0x4000)) {
            s_completedAchievements.UnlinkNode(node);
            node->~CLIENT_ACHIEVEMENTS();
            SMemFree(node, "delete", -1, 0);
        }

        node = next;
    }

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head(); !s_criteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_criteria.RawNext(node);
        Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(node->m_id);
        AchievementRec* achievement;

        if (criteria && (achievement = g_achievementDB.GetRecord(criteria->m_achievementID)) && (achievement->m_flags & 0x4000)) {
            s_criteria.UnlinkNode(node);
            node->~CLIENT_ACHIEVEMENT_CRITERIA();
            SMemFree(node, "delete", -1, 0);

            node = next;
        } else {
            node = s_criteria.RawNext(node);
        }
    }

    GUILDACHIEVEMENTMEMBERLIST* list = s_guildAchievementMembers.m_fulllist.Head();

    while (list) {
        GUILDACHIEVEMENTMEMBERLIST* next = s_guildAchievementMembers.Next(list);
        list->m_members.Clear();
        s_guildAchievementMembers.Delete(list);

        list = next;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, NULL);
}

// OFFSET: 0x0094D730
static bool OnGuildAchievementEarned(ClientGuildAchievementEarned* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID() == *reinterpret_cast<WOWGUID*>(msg->unk_0018)) {
        CGAchievementInfo::UpdateAchievement(msg->unk_0010, msg->unk_0020);
        AchievementRec* achievement = g_achievementDB.GetRecord(msg->unk_0010);

        if (achievement) {
            if (!(achievement->m_flags & 0x4)) {
                CEffect* effect = CEffect::Create();
                WOWGUID guid = player->m_obj->m_guid;
                effect->AddSpecialEffect(11, guid, CGObject_C::EffectOnceHandler);
                effect->m_flags |= 0x20;
                effect->Release();
            }

            CGAchievementInfo::RemoveTrackedAchievement(achievement->m_id);
            FrameScript_SignalEvent(SCRIPT_EVENT_ACHIEVEMENT_EARNED, "%d", achievement->m_id);
            WOWGUID requester = player->m_obj->m_guid;
            GuildStats_C* guild = g_guildInfoCache.GetRecord2(reinterpret_cast<WOWGUID*>(msg->unk_0018)->m_guid, requester, NULL, NULL, false);

            if (guild) {
                char text[1024];
                SStrPrintf(text, sizeof(text), FrameScript_GetText("GUILD_ACHIEVEMENT_BROADCAST", -1, GENDER_NOT_APPLICABLE), guild->m_name);
                ChatMessageParams params;
                params.m_senderName = guild->m_name;
                params.m_senderGuid.m_guid = 0;
                params.m_targetGuid.m_guid = 0;
                params.m_achievementID = achievement->m_id;
                CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(49), &params);
            }
        }
    }

    return true;
}

// OFFSET: 0x0094EE20
static bool OnGuildAchievementMembers(ClientGuildAchievementMembers* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->GetGuildGUID() == *reinterpret_cast<WOWGUID*>(msg->unk_0018)) {
        int32_t id = msg->unk_0010;
        GUILDACHIEVEMENTMEMBERLIST* list = s_guildAchievementMembers.Ptr(id, HASHKEY_NONE());

        if (!list) {
            list = s_guildAchievementMembers.New(id, HASHKEY_NONE(), 0, 0);
        }

        bool pending = false;
        list->m_members.Clear();

        for (uint32_t i = 0; i < msg->unk_0020.Count(); i++) {
            GuildAchievementMember* member = list->m_members.New();
            member->m_guid = *reinterpret_cast<WOWGUID*>(msg->unk_0020[i].unk_0000);
            WOWGUID requester = player->m_obj->m_guid;

            if (!g_nameDBCache.GetRecord2(member->m_guid.m_guid, requester, GuildAchievementMemberNameCallback, reinterpret_cast<void*>(msg->unk_0010), true)) {
                pending = true;
            }
        }

        if (!pending) {
            FrameScript_SignalEvent(SCRIPT_EVENT_RECEIVED_ACHIEVEMENT_MEMBER_LIST, "%d", msg->unk_0010);
        }
    }

    return true;
}

// OFFSET: 0x0094F140
void CGAchievementInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_ACHIEVEMENT_EARNED, Packet_SMSG_ACHIEVEMENT_EARNED, NULL);
    ClientServices::SetMessageHandler(SMSG_CRITERIA_UPDATE, Packet_SMSG_CRITERIA_UPDATE, NULL);
    ClientServices::SetMessageHandler(SMSG_CRITERIA_DELETED, Packet_SMSG_CRITERIA_DELETED, NULL);
    ClientServices::SetMessageHandler(SMSG_ACHIEVEMENT_DELETED, Packet_SMSG_ACHIEVEMENT_DELETED, NULL);
    ClientServices::SetMessageHandler(SMSG_QUERY_QUESTS_COMPLETED_RESPONSE, Packet_SMSG_QUERY_QUESTS_COMPLETED_RESPONSE, NULL);
    ClientRespondInspectAchievements::s_handler = OnRespondInspectAchievements;
    ClientAllAchievementData::s_handler = OnAllAchievementData;
    ClientGuildCriteriaData::s_handler = OnGuildCriteriaData;
    ClientGuildCriteriaDeleted::s_handler = OnGuildCriteriaDeleted;
    ClientGuildAchievementEarned::s_handler = OnGuildAchievementEarned;
    ClientGuildAchievementDeleted::s_handler = OnGuildAchievementDeleted;
    ClientGuildAchievementMembers::s_handler = OnGuildAchievementMembers;
    ClientGuildAchievementData::s_handler = OnGuildAchievementData;
    s_unkC1F450 = 1;
    s_focusedAchievement = 0;
    CGAchievementInfo::m_showAchievementUI = 0;
    s_criteria.Clear();
    s_recentCriteria.Clear();
    s_completedAchievements.Clear();
    CGAchievementInfo::m_inspect.m_guid = 0;
    s_inspectCriteria.Clear();
    s_inspectRecentCriteria.Clear();
    s_inspectAchievements.Clear();
    s_trackedAchievements.Clear();
    s_completedQuests.Clear();

    for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
        AchievementRec* rec = g_achievementDB.GetRecordByIndex(i);
        int32_t id = rec->m_id;

        ACHIEVEMENT* achievement = s_achievements.New(id, HASHKEY_NONE(), 0, 0);
        achievement->m_rec = rec;
    }

    for (int32_t i = 0; i < g_achievement_CriteriaDB.m_numRecords; i++) {
        Achievement_CriteriaRec* rec = g_achievement_CriteriaDB.GetRecordByIndex(i);

        if (!rec || !g_achievementDB.GetRecord(rec->m_achievementID) || static_cast<uint32_t>(rec->m_type) > 150) {
            continue;
        }

        ACHIEVEMENT* achievement = s_achievements.Ptr(rec->m_achievementID, HASHKEY_NONE());

        if (achievement) {
            achievement->m_criteria.NewNode(2, 0, 0)->m_criteria = rec;
        }
    }
}

// OFFSET: 0x00947B10
bool CGAchievementInfo::IsCriteriaStat(int32_t criteriaId) {
    Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(criteriaId);

    if (criteria) {
        AchievementRec* achievement = g_achievementDB.GetRecord(criteria->m_achievementID);

        if (achievement) {
            Achievement_CategoryRec* category = g_achievement_CategoryDB.GetRecord(achievement->m_category);

            if (category) {
                if (category->m_id != 1) {
                    category = g_achievement_CategoryDB.GetRecord(category->m_parent);

                    while (true) {
                        if (!category) {
                            return false;
                        }

                        if (category->m_id == 1) {
                            break;
                        }

                        category = g_achievement_CategoryDB.GetRecord(category->m_parent);
                    }
                }

                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x0094A730
void CGAchievementInfo::UpdateCriteria(int32_t id, uint64_t counter, uint32_t flags, WowTime date, WOWGUID player, uint32_t timer1, uint32_t timer2) {
    CLIENT_ACHIEVEMENT_CRITERIA* criteria = NULL;

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head(); !s_criteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_criteria.RawNext(node);

        if (node->m_id == id) {
            criteria = node;
            break;
        }

        node = next;
    }

    if (!criteria) {
        criteria = s_criteria.NewNode(0, 0, 0);
        CLIENT_ACHIEVEMENT_CRITERIA* position = s_criteria.Head();

        while (position && position->m_date > date) {
            position = s_criteria.Next(position);
        }

        s_criteria.LinkNode(criteria, 2, position);
    }

    criteria->m_counter = counter;
    criteria->m_id = id;
    criteria->m_flags = flags;
    criteria->m_date = date;
    criteria->m_player = player;
    criteria->m_timer1 = timer1;
    criteria->m_timer2 = timer2;

    if (flags & 1) {
        s_criteria.UnlinkNode(criteria);
        criteria->~CLIENT_ACHIEVEMENT_CRITERIA();
        SMemFree(criteria, "delete", -1, 0);
        return;
    }

    if (!CGAchievementInfo::IsCriteriaStat(id)) {
        return;
    }

    CLIENT_ACHIEVEMENT_CRITERIA* recent = s_recentCriteria.NewNode(0, 0, 0);
    recent->m_id = criteria->m_id;
    recent->m_counter = criteria->m_counter;
    recent->m_flags = criteria->m_flags;
    recent->m_date = criteria->m_date;
    recent->m_player = criteria->m_player;
    recent->m_timer1 = criteria->m_timer1;

    Achievement_CriteriaRec* rec = g_achievement_CriteriaDB.GetRecord(recent->m_id);
    CLIENT_ACHIEVEMENT_CRITERIA* node = s_recentCriteria.Head();
    s_recentCriteria.LinkNode(recent, 2, node);

    for (int32_t count = 0; node; ) {
        Achievement_CriteriaRec* nodeRec = g_achievement_CriteriaDB.GetRecord(node->m_id);

        if (nodeRec) {
            if (node->m_id == recent->m_id || nodeRec->m_achievementID == rec->m_achievementID) {
                s_recentCriteria.UnlinkNode(node);
                node->~CLIENT_ACHIEVEMENT_CRITERIA();
                SMemFree(node, "delete", -1, 0);
                node = NULL;
            } else {
                node = s_recentCriteria.Next(node);
            }
        }

        if (++count >= 5) {
            if (node) {
                s_recentCriteria.UnlinkNode(node);
                node->~CLIENT_ACHIEVEMENT_CRITERIA();
                SMemFree(node, "delete", -1, 0);
            }

            return;
        }
    }
}

// OFFSET: 0x0094AAF0
void CGAchievementInfo::UpdateAchievement(int32_t id, WowTime date) {
    CLIENT_ACHIEVEMENTS* achievement = CGAchievementInfo::GetAchievement(id);
    AchievementRec* rec = g_achievementDB.GetRecord(id);

    if (!rec) {
        return;
    }

    if (!achievement) {
        achievement = s_completedAchievements.NewNode(0, 0, 0);
        CLIENT_ACHIEVEMENTS* position = s_completedAchievements.Head();
        bool found = false;

        while (position && position->m_date >= date && !found) {
            if (position->m_date == date) {
                while (position && position->m_date >= date && !found) {
                    AchievementRec* other = g_achievementDB.GetRecord(position->m_id);

                    if (other && rec->m_uiOrder > other->m_uiOrder) {
                        found = true;
                    } else {
                        position = s_completedAchievements.Next(position);
                    }
                }
            } else {
                position = s_completedAchievements.Next(position);
            }
        }

        s_completedAchievements.LinkNode(achievement, 2, position);
    }

    achievement->m_id = id;
    achievement->m_date = date;
}

// OFFSET: 0x0094D480
int32_t AddTrackedAchievement(int32_t id) {
    AchievementRec* rec = g_achievementDB.GetRecord(id);

    if (!rec || CGAchievementInfo::GetAchievement(rec->m_id)) {
        return 0;
    }

    int32_t count = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);

        count++;

        if (node->m_id == id) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", id);
            return 1;
        }

        if (count == 10) {
            return 0;
        }

        node = next;
    }

    CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.NewNode(2, 0, 0);
    node->m_id = id;
    FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", id);
    return 1;
}

// OFFSET: 0x00949150
int32_t Packet_SMSG_ACHIEVEMENT_DELETED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t id;
    msg->Get(reinterpret_cast<uint32_t&>(id));

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); !s_completedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_completedAchievements.RawNext(node);

        if (node->m_id == id) {
            s_completedAchievements.UnlinkNode(node);
            FrameScript_SignalEvent(SCRIPT_EVENT_ACHIEVEMENT_EARNED, "%d", id);
            node->~CLIENT_ACHIEVEMENTS();
            SMemFree(node, "delete", -1, 0);
            return 1;
        }

        node = next;
    }

    return 1;
}

// OFFSET: 0x00949230
int32_t Packet_SMSG_CRITERIA_DELETED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t id;
    msg->Get(reinterpret_cast<uint32_t&>(id));

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head(); !s_criteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_criteria.RawNext(node);

        if (node->m_id == id) {
            s_criteria.UnlinkNode(node);
            FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, NULL);
            node->~CLIENT_ACHIEVEMENT_CRITERIA();
            SMemFree(node, "delete", -1, 0);
            return 1;
        }

        node = next;
    }

    return 1;
}

// OFFSET: 0x0094D8D0
void CGAchievementInfo::LoadTrackedAchievements() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("trackedAchievements");
    list.m_pos = 2;
    list.m_state = 0;

    for (uint32_t id = list.GetNextValue(); id; id = list.GetNextValue()) {
        AddTrackedAchievement(id);
    }

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}

// OFFSET: 0x0094DC00
void CGAchievementInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_ACHIEVEMENT_EARNED);
    ClientServices::ClearMessageHandler(SMSG_CRITERIA_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_ACHIEVEMENT_DELETED);
    ClientServices::ClearMessageHandler(SMSG_CRITERIA_DELETED);
    ClientServices::ClearMessageHandler(SMSG_QUERY_QUESTS_COMPLETED_RESPONSE);
    ClientRespondInspectAchievements::s_handler = NULL;
    ClientAllAchievementData::s_handler = NULL;
    ClientGuildCriteriaData::s_handler = NULL;
    ClientGuildCriteriaDeleted::s_handler = NULL;
    ClientGuildAchievementEarned::s_handler = NULL;
    ClientGuildAchievementDeleted::s_handler = NULL;
    ClientGuildAchievementMembers::s_handler = NULL;
    ClientGuildAchievementData::s_handler = NULL;
    s_unkC1F450 = 0;
    s_focusedAchievement = 0;

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_criteria.Head(); !s_criteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_criteria.RawNext(node);
        s_criteria.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_recentCriteria.Head(); !s_recentCriteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_recentCriteria.RawNext(node);
        s_recentCriteria.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head(); !s_completedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_completedAchievements.RawNext(node);
        s_completedAchievements.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectCriteria.Head(); !s_inspectCriteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_inspectCriteria.RawNext(node);
        s_inspectCriteria.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectRecentCriteria.Head(); !s_inspectRecentCriteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_inspectRecentCriteria.RawNext(node);
        s_inspectRecentCriteria.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(node);
        s_inspectAchievements.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);
        s_trackedAchievements.DeleteNode(node);
        node = next;
    }

    for (COMPLETED_QUESTS* node = s_completedQuests.Head(); !s_completedQuests.IsEnd(node); ) {
        COMPLETED_QUESTS* next = s_completedQuests.RawNext(node);
        s_completedQuests.DeleteNode(node);
        node = next;
    }

    ACHIEVEMENT* achievement = s_achievements.m_fulllist.Head();

    while (achievement) {
        ACHIEVEMENT* next = s_achievements.Next(achievement);
        achievement->m_criteria.Clear();
        s_achievements.Delete(achievement);

        achievement = next;
    }

    GUILDACHIEVEMENTMEMBERLIST* list = s_guildAchievementMembers.m_fulllist.Head();

    while (list) {
        GUILDACHIEVEMENTMEMBERLIST* next = s_guildAchievementMembers.Next(list);
        list->m_members.Clear();
        s_guildAchievementMembers.Delete(list);

        list = next;
    }
}

// OFFSET: 0x009493A0
void CGAchievementInfo::SaveTrackedAchievements() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("trackedAchievements");
    list.m_pos = 2;
    list.m_state = 0;

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);

        list.AddNextValue(node->m_id);

        node = next;
    }

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}

// OFFSET: 0x0094D5C0
void CGAchievementInfo::RemoveTrackedAchievement(int32_t id) {
    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);

        if (id == node->m_id) {
            s_trackedAchievements.DeleteNode(node);
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", id);
            return;
        }

        node = next;
    }
}

// OFFSET: 0x0094ACB0
void CGAchievementInfo::UpdateInspectAchievement(WOWGUID guid, int32_t id, WowTime date) {
    if (CGAchievementInfo::m_inspect != guid) {
        return;
    }

    CLIENT_ACHIEVEMENTS* achievement = NULL;

    for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(node);

        if (node->m_id == id) {
            achievement = node;
            break;
        }

        node = next;
    }

    AchievementRec* rec = g_achievementDB.GetRecord(id);

    if (!achievement) {
        achievement = s_inspectAchievements.NewNode(0, 0, 0);
        CLIENT_ACHIEVEMENTS* position = s_inspectAchievements.Head();
        bool found = false;

        while (position && position->m_date >= date && !found) {
            if (position->m_date == date) {
                while (position && position->m_date >= date && !found) {
                    AchievementRec* other = g_achievementDB.GetRecord(position->m_id);

                    if (rec && other && rec->m_uiOrder > other->m_uiOrder) {
                        found = true;
                    } else {
                        position = s_inspectAchievements.Next(position);
                    }
                }
            } else {
                position = s_inspectAchievements.Next(position);
            }
        }

        s_inspectAchievements.LinkNode(achievement, 2, position);
    }

    achievement->m_id = id;
    achievement->m_date = date;
}

// OFFSET: 0x0094AEC0
void CGAchievementInfo::UpdateInspectCriteria(WOWGUID guid, int32_t id, uint64_t counter, WOWGUID player, uint32_t flags, WowTime date, uint32_t timer1, uint32_t timer2) {
    if (CGAchievementInfo::m_inspect != guid) {
        return;
    }

    CLIENT_ACHIEVEMENT_CRITERIA* criteria = NULL;

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectCriteria.Head(); !s_inspectCriteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_inspectCriteria.RawNext(node);

        if (node->m_id == id) {
            criteria = node;
            break;
        }

        node = next;
    }

    if (!criteria) {
        criteria = s_inspectCriteria.NewNode(0, 0, 0);
        CLIENT_ACHIEVEMENT_CRITERIA* position = s_inspectCriteria.Head();

        while (position && position->m_date > date) {
            position = s_inspectCriteria.Next(position);
        }

        s_inspectCriteria.LinkNode(criteria, 2, position);
    }

    criteria->m_counter = counter;
    criteria->m_flags = flags;
    criteria->m_id = id;
    criteria->m_date = date;
    criteria->m_player = player;
    criteria->m_timer1 = timer1;
    criteria->m_timer2 = timer2;

    if (!CGAchievementInfo::IsCriteriaStat(id)) {
        return;
    }

    CLIENT_ACHIEVEMENT_CRITERIA* recent = s_inspectRecentCriteria.NewNode(0, 0, 0);
    recent->m_id = criteria->m_id;
    recent->m_counter = criteria->m_counter;
    recent->m_flags = criteria->m_flags;
    recent->m_date = criteria->m_date;
    recent->m_player = criteria->m_player;
    recent->m_timer1 = criteria->m_timer1;
    recent->m_timer2 = criteria->m_timer2;

    CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectRecentCriteria.Head();
    s_inspectRecentCriteria.LinkNode(recent, 2, node);

    for (int32_t count = 0; node; ) {
        if (node->m_id == recent->m_id) {
            s_inspectRecentCriteria.UnlinkNode(node);
            node->~CLIENT_ACHIEVEMENT_CRITERIA();
            SMemFree(node, "delete", -1, 0);
            node = NULL;
        } else {
            node = s_inspectRecentCriteria.Next(node);
        }

        if (++count >= 5) {
            if (node) {
                s_inspectRecentCriteria.UnlinkNode(node);
                node->~CLIENT_ACHIEVEMENT_CRITERIA();
                SMemFree(node, "delete", -1, 0);
            }

            return;
        }
    }
}

// OFFSET: 0x0094D620
int32_t Packet_SMSG_ACHIEVEMENT_EARNED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WowTime date;
    SmartGUID player;
    int32_t id;
    uint32_t silent;
    *msg >> player;
    msg->Get(reinterpret_cast<uint32_t&>(id));
    *msg >> date;
    msg->Get(silent);

    if (ClntObjMgrGetActivePlayer() == player) {
        CGAchievementInfo::UpdateAchievement(id, date);
    }

    AchievementRec* achievement = g_achievementDB.GetRecord(id);

    if (achievement && !silent) {
        if (!(achievement->m_flags & 0x4)) {
            CEffect* effect = CEffect::Create();
            WOWGUID guid = player;
            effect->AddSpecialEffect(11, guid, CGObject_C::EffectOnceHandler);
            effect->m_flags |= 0x20;
            effect->Release();
        }

        if (ClntObjMgrGetActivePlayer() == player) {
            CGAchievementInfo::RemoveTrackedAchievement(achievement->m_id);
            FrameScript_SignalEvent(SCRIPT_EVENT_ACHIEVEMENT_EARNED, "%d", achievement->m_id);
        }
    }

    return 1;
}

// OFFSET: 0x0094B2C0
int32_t Packet_SMSG_CRITERIA_UPDATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGAchievementInfo::m_showAchievementUI = 1;
    WowTime date;
    int32_t id;
    SmartGUID counter;
    SmartGUID player;
    uint32_t flags;
    uint32_t timer1;
    uint32_t timer2;
    msg->Get(reinterpret_cast<uint32_t&>(id));
    *msg >> counter;
    *msg >> player;
    msg->Get(flags);
    *msg >> date;
    msg->Get(timer1);
    msg->Get(timer2);
    uint32_t elapsed1 = ::time(NULL) - timer1;
    uint32_t elapsed2 = ::time(NULL) - timer2;
    CGAchievementInfo::UpdateCriteria(id, counter.m_guid, flags, date, player, elapsed1, elapsed2);
    FrameScript_SignalEvent(SCRIPT_EVENT_CRITERIA_UPDATE, NULL);
    Achievement_CriteriaRec* criteria = g_achievement_CriteriaDB.GetRecord(id);

    if (!criteria) {
        return 1;
    }

    if (criteria->m_timerTime) {
        FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d%d%d%d", criteria->m_achievementID, criteria->m_id, timer1, criteria->m_timerTime);
        return 1;
    }

    for (CLIENT_ACHIEVEMENTS* node = s_trackedAchievements.Head(); !s_trackedAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_trackedAchievements.RawNext(node);

        AchievementRec* achievement;

        if (criteria->m_achievementID == node->m_id || ((achievement = g_achievementDB.GetRecord(node->m_id)) && achievement->m_sharesCriteria == criteria->m_achievementID)) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRACKED_ACHIEVEMENT_UPDATE, "%d", node->m_id);
        }

        node = next;
    }

    return 1;
}

// OFFSET: 0x009479F0
void CGAchievementInfo::SetFocusedAchievement(int32_t id) {
    if (id == s_focusedAchievement) {
        return;
    }

    AchievementRec* achievement = g_achievementDB.GetRecord(id);

    if (achievement && (achievement->m_flags & 0x4000)) {
        s_focusedAchievement = id;
        UserClientOpcode3235 request;
        request.unk_0010 = id;
        ClientServices::Send(&request);
    }
}

// OFFSET: 0x00947A60
bool CGAchievementInfo::IsRealAchievement(int32_t id, bool guild) {
    AchievementRec* achievement = g_achievementDB.GetRecord(id);

    if (achievement && guild == ((achievement->m_flags & 0x4000) != 0)) {
        Achievement_CategoryRec* category = g_achievement_CategoryDB.GetRecord(achievement->m_category);

        if (category && category->m_id != 1) {
            for (category = g_achievement_CategoryDB.GetRecord(category->m_parent); category; category = g_achievement_CategoryDB.GetRecord(category->m_parent)) {
                if (category->m_id == 1) {
                    return false;
                }
            }

            return true;
        }
    }

    return false;
}

// OFFSET: 0x00949340
bool CGAchievementInfo::IsSupersededByCompleted(int32_t id) {
    for (int32_t i = 0; i < g_achievementDB.m_numRecords; i++) {
        AchievementRec* achievement = g_achievementDB.GetRecordByIndex(i);

        if (achievement && achievement->m_supercedes == id && CGAchievementInfo::GetAchievement(achievement->m_id)) {
            return true;
        }
    }

    return false;
}

// OFFSET: #0x005B0EA0
uint64_t CGAchievementInfo::BuildOutputString(char* buffer, int32_t size, uint64_t sum, uint64_t highest, int32_t highestId, uint64_t count, uint64_t value, CLIENT_ACHIEVEMENT_CRITERIA* criteria, int32_t flags, int32_t criteriaFlags, bool completed, const AchievementRec* achievement, const Achievement_CriteriaRec* criteriaRec, int32_t required) {
    float perDay = 0.0f;
    CVar* colorblindMode = CVar::Lookup("colorblindMode");
    uint64_t result;

    if (flags & 0x8) {
        result = sum;

        if (criteria && (flags & 0x40)) {
            uint32_t days = static_cast<uint32_t>(::time(NULL) - criteria->m_timer2) / 86400;

            if (days) {
                perDay = static_cast<float>(sum) / static_cast<float>(days);
                result = static_cast<int64_t>(perDay);
            }
        }

        if (criteriaFlags & 0x20) {
            if (colorblindMode && colorblindMode->m_intValue) {
                GetCurrencyShortText(static_cast<int32_t>(result), buffer, size, " ");
            } else {
                GetCurrencyTextureString(static_cast<int32_t>(result), buffer, size, 0);
            }
        } else if (perDay != 0.0f) {
            SStrPrintf(buffer, size, "%0.2f", perDay);
        } else if (result) {
            SStrPrintf(buffer, size, "%I64d", result);
        } else if (flags & 0x1) {
            SStrPrintf(buffer, size, "--");
        } else {
            SStrPrintf(buffer, size, "0");
        }
    } else if (flags & 0x10) {
        Achievement_CriteriaRec* best = g_achievement_CriteriaDB.GetRecord(highestId);
        result = highest;

        if (best) {
            if (flags & 0x1) {
                SStrPrintf(buffer, size, "%I64d (%s)", highest, static_cast<const char*>(best->m_description));
            } else {
                SStrPrintf(buffer, size, "%I64d", highest);
            }
        } else if (result) {
            SStrPrintf(buffer, size, "%I64d", result);
        } else if (flags & 0x1) {
            SStrPrintf(buffer, size, "--");
        } else {
            SStrPrintf(buffer, size, "0");
        }
    } else if (flags & 0x20) {
        result = count;

        if (result) {
            SStrPrintf(buffer, size, "%I64d", result);
        } else if (flags & 0x1) {
            SStrPrintf(buffer, size, "--");
        } else {
            SStrPrintf(buffer, size, "0");
        }
    } else if (value) {
        result = value;

        if (criteriaRec && !(flags & 0x1) && result >= static_cast<uint64_t>(static_cast<int64_t>(criteriaRec->m_quantity))) {
            result = static_cast<int64_t>(criteriaRec->m_quantity);
        }

        if (flags & 0x40) {
            uint32_t days = static_cast<uint32_t>(::time(NULL) - criteria->m_timer2) / 86400;

            if (days) {
                perDay = static_cast<float>(result) / static_cast<float>(days);
                result = static_cast<int64_t>(perDay);
            }
        }

        Achievement_CriteriaRec* rec = g_achievement_CriteriaDB.GetRecord(criteria->m_id);

        if (rec && (rec->m_flags & 0x10)) {
            WowTime time;
            DBDATESTRUCT date;
            WowTime::WowDecodeTimeToDbDate(static_cast<uint32_t>(result), &date);
            SStrPrintf(buffer, size, "%02d/%02d/%d", date.m_month, date.m_day, static_cast<int16_t>(date.m_year));
        } else if (rec && (rec->m_flags & 0x20)) {
            if (colorblindMode && colorblindMode->m_intValue) {
                GetCurrencyShortText(static_cast<int32_t>(result), buffer, size, " ");
            } else {
                GetCurrencyTextureString(static_cast<int32_t>(result), buffer, size, 0);
            }
        } else if (!(flags & 0x1)) {
            SStrPrintf(buffer, size, "%I64d", result);
        } else if (perDay != 0.0f) {
            SStrPrintf(buffer, size, "%0.2f", perDay);
        } else if (result) {
            SStrPrintf(buffer, size, "%I64d", result);
        } else {
            SStrPrintf(buffer, size, "--");
        }
    } else if (completed && !achievement->m_minimumCriteria) {
        result = static_cast<int64_t>(criteriaRec->m_quantity);

        if (!result) {
            if (flags & 0x1) {
                SStrPrintf(buffer, size, "--");
            } else {
                SStrPrintf(buffer, size, "0");
            }
        } else if (criteriaFlags & 0x20) {
            if (colorblindMode && colorblindMode->m_intValue) {
                GetCurrencyShortText(static_cast<int32_t>(result), buffer, size, " ");
            } else {
                GetCurrencyTextureString(static_cast<int32_t>(result), buffer, size, 0);
            }
        } else {
            SStrPrintf(buffer, size, "%I64d", result);
        }
    } else {
        if ((criteriaFlags & 0x20) && (criteriaFlags & 0x1)) {
            if (colorblindMode && colorblindMode->m_intValue) {
                GetCurrencyShortText(0, buffer, size, " ");
            } else {
                GetCurrencyTextureString(0, buffer, size, 0);
            }
        } else if (flags & 0x1) {
            SStrPrintf(buffer, size, "--");
        } else {
            SStrPrintf(buffer, size, "%I64d", static_cast<int64_t>(0));
        }

        result = 0;
    }

    if ((criteriaFlags & 0x1) && criteriaRec) {
        char total[128];

        if (criteriaFlags & 0x20) {
            if (colorblindMode && colorblindMode->m_intValue) {
                GetCurrencyShortText(required ? required : criteriaRec->m_quantity, total, sizeof(total), " ");
            } else {
                GetCurrencyTextureString(required ? required : criteriaRec->m_quantity, total, sizeof(total), 0);
            }
        } else {
            SStrPrintf(total, sizeof(total), "%d", required ? required : criteriaRec->m_quantity);
        }

        SStrPrintf(buffer, size, "%s / %s", buffer, total);
    }

    return result;
}

// OFFSET: 0x0094B1C0
void CGAchievementInfo::ReleaseInspect() {
    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectCriteria.Head(); !s_inspectCriteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_inspectCriteria.RawNext(node);
        s_inspectCriteria.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENT_CRITERIA* node = s_inspectRecentCriteria.Head(); !s_inspectRecentCriteria.IsEnd(node); ) {
        CLIENT_ACHIEVEMENT_CRITERIA* next = s_inspectRecentCriteria.RawNext(node);
        s_inspectRecentCriteria.DeleteNode(node);
        node = next;
    }

    for (CLIENT_ACHIEVEMENTS* node = s_inspectAchievements.Head(); !s_inspectAchievements.IsEnd(node); ) {
        CLIENT_ACHIEVEMENTS* next = s_inspectAchievements.RawNext(node);
        s_inspectAchievements.DeleteNode(node);
        node = next;
    }
}

// PROVEN: 011d99d0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGAchievementInfo::LeaveWorld() {
}

// OFFSET: 0x00949300/0x1406B6D80
CLIENT_ACHIEVEMENTS* CGAchievementInfo::GetAchievement(int32_t id) {
    CLIENT_ACHIEVEMENTS* result = NULL;
    CLIENT_ACHIEVEMENTS* node = s_completedAchievements.Head();

    while (!s_completedAchievements.IsEnd(node)) {
        CLIENT_ACHIEVEMENTS* entry = node;
        node = s_completedAchievements.RawNext(node);

        if (entry->m_id == id) {
            result = entry;
            break;
        }
    }

    return result;
}
