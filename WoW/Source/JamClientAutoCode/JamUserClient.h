#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMUSERCLIENT_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMUSERCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"

struct JamUserClientReorderEntry {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ uint8_t m_newIndex;
    /* 0009 */ uint8_t unk_0009[7];

    JamUserClientReorderEntry();
    JamUserClientReorderEntry(const JamUserClientReorderEntry& source);
};

class UserClientCancelQueuedSpell : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientCancelQueuedSpell();
    ~UserClientCancelQueuedSpell();
};

class UserClientEnumCharacters : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientEnumCharacters();
    ~UserClientEnumCharacters();
};

class UserClientPlayerLogin : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_guid[8];

    void Put(CDataStore* msg) override;

    UserClientPlayerLogin(uint64_t guid);
    ~UserClientPlayerLogin();
};

class UserClientWorldPortResponse : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientWorldPortResponse();
    ~UserClientWorldPortResponse();
};

class UserClientVoidStorageQuery : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_npc[8];

    void Put(CDataStore* msg) override;

    UserClientVoidStorageQuery();
    ~UserClientVoidStorageQuery();
};

class UserClientVoidStorageUnlock : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_npc[8];

    void Put(CDataStore* msg) override;

    UserClientVoidStorageUnlock();
    ~UserClientVoidStorageUnlock();
};

class UserClientVoidSwapItem : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_npc[8];
    /* 0018 */ uint8_t m_item[8];
    /* 0020 */ uint32_t m_slot;

    void Put(CDataStore* msg) override;

    UserClientVoidSwapItem();
    ~UserClientVoidSwapItem();
};

class UserClientVoidStorageTransfer : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<uint64_t> m_deposits;
    /* 0020 */ TSGrowableArray<uint64_t> m_withdrawals;
    /* 0030 */ uint8_t m_npc[8];

    void Put(CDataStore* msg) override;

    UserClientVoidStorageTransfer();
    ~UserClientVoidStorageTransfer();
};

struct JamTransmogrifyItem {
    /* 0000 */ uint8_t m_itemGuid[8];
    /* 0008 */ int32_t m_itemID;
    /* 000C */ uint32_t m_slot;

    JamTransmogrifyItem();
};

class UserClientSaveCufProfiles : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<JamCliCUFProfile> m_profiles;

    void Put(CDataStore* msg) override;

    UserClientSaveCufProfiles();
    ~UserClientSaveCufProfiles();
};

class UserClientTransmogrifyItems : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<JamTransmogrifyItem> m_items;
    /* 0020 */ uint8_t m_npc[8];

    void Put(CDataStore* msg) override;

    UserClientTransmogrifyItems();
    ~UserClientTransmogrifyItems();
};

class UserClientReforgeItem : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_flags;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t m_forgeMaster[8];
    /* 0020 */ uint32_t m_slot;
    /* 0024 */ uint32_t m_bag;
    /* 0028 */ int32_t m_reforgeId;

    void Put(CDataStore* msg) override;

    UserClientReforgeItem();
    ~UserClientReforgeItem();
};

class UserClientUnlearnSpecialization : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_index;

    void Put(CDataStore* msg) override;

    UserClientUnlearnSpecialization();
    ~UserClientUnlearnSpecialization();
};

class UserClientReturnToGraveyard : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientReturnToGraveyard();
    ~UserClientReturnToGraveyard();
};

class UserClientQueryBattlefieldState : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientQueryBattlefieldState();
    ~UserClientQueryBattlefieldState();
};

class UserClientRequestCemeteryList : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientRequestCemeteryList();
    ~UserClientRequestCemeteryList();
};

class UserClientSetPreferredCemetery : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_cemeteryID;

    void Put(CDataStore* msg) override;

    UserClientSetPreferredCemetery();
    ~UserClientSetPreferredCemetery();
};

class UserClientReorderCharacters : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<JamUserClientReorderEntry> m_entries;

    void Put(CDataStore* msg) override;

    UserClientReorderCharacters();
    ~UserClientReorderCharacters();
};

class UserClientGenerateRandomCharacterName : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_raceID;
    /* 0011 */ uint8_t m_sexID;

    void Put(CDataStore* msg) override;

    UserClientGenerateRandomCharacterName(uint8_t raceID, uint8_t sexID);
    ~UserClientGenerateRandomCharacterName();
};

struct DBQuery {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ int32_t m_id;
    /* 000C */ uint8_t unk_000C[4];

    DBQuery();
};

class UserClientDBQueryBulk : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<DBQuery> m_queries;
    /* 0020 */ uint32_t m_tableHash;

    void Put(CDataStore* msg) override;

    UserClientDBQueryBulk();
    ~UserClientDBQueryBulk();
};

class UserClientPartyInvite : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_proposedRoles;
    /* 0014 */ char m_targetName[305];
    /* 0145 */ uint8_t unk_0145[3];
    /* 0148 */ uint8_t m_targetGUID[8];
    /* 0150 */ char m_targetRealm[256];
    /* 0250 */ uint32_t m_targetCfgRealmID;

    void Put(CDataStore* msg) override;

    UserClientPartyInvite();
    ~UserClientPartyInvite();
};

class UserClientPartyInviteResponse : public JamClientMessage {
    public:
    /* 0010 */ bool m_accept;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ bool m_hasRolesDesired;
    /* 0018 */ uint32_t m_rolesDesired;

    void Put(CDataStore* msg) override;

    UserClientPartyInviteResponse();
    ~UserClientPartyInviteResponse();
};

class UserClientSetEveryoneIsAssistant : public JamClientMessage {
    public:
    /* 0010 */ bool m_everyoneIsAssistant;

    void Put(CDataStore* msg) override;

    UserClientSetEveryoneIsAssistant();
    ~UserClientSetEveryoneIsAssistant();
};

class UserClientClearRaidMarker : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_markerId;

    void Put(CDataStore* msg) override;

    UserClientClearRaidMarker();
    ~UserClientClearRaidMarker();
};

class UserClientRequestPartyJoinUpdates : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientRequestPartyJoinUpdates();
    ~UserClientRequestPartyJoinUpdates();
};

class UserLFGuildAddRecruit : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_classRoles;
    /* 0014 */ char m_comment[1024];
    /* 0414 */ char m_name[48];
    /* 0444 */ uint8_t unk_0444[4];
    /* 0448 */ uint8_t m_guildGUID[8];
    /* 0450 */ uint32_t m_interests;
    /* 0454 */ uint32_t m_availability;

    void Put(CDataStore* msg) override;

    UserLFGuildAddRecruit();
    ~UserLFGuildAddRecruit();
};

class PlayerRequestResearchHistory : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    PlayerRequestResearchHistory();
    ~PlayerRequestResearchHistory();
};

class UserClientSetRole : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_targetGUID[8];
    /* 0018 */ uint32_t m_role;

    void Put(CDataStore* msg) override;

    UserClientSetRole();
    ~UserClientSetRole();
};

class UserClientInitiateRolePoll : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    UserClientInitiateRolePoll();
    ~UserClientInitiateRolePoll();
};

class UserClientRequestBattlefieldStatus : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientRequestBattlefieldStatus();
    ~UserClientRequestBattlefieldStatus();
};

class UserClientGuildAccept : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientGuildAccept();
    ~UserClientGuildAccept();
};

class UserClientLfgSetRoles : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    UserClientLfgSetRoles();
    ~UserClientLfgSetRoles();
};

class UserClientLfgLfrJoin : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    UserClientLfgLfrJoin();
    ~UserClientLfgLfrJoin();
};

class UserClientArenaTeamCreate : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ char unk_0024[96];
    /* 0084 */ uint32_t unk_0084;

    void Put(CDataStore* msg) override;

    UserClientArenaTeamCreate();
    ~UserClientArenaTeamCreate();
};

class UserClientLfgLfrLeave : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    UserClientLfgLfrLeave();
    ~UserClientLfgLfrLeave();
};

class UserClientGuildInvite : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];

    void Put(CDataStore* msg) override;

    UserClientGuildInvite();
    ~UserClientGuildInvite();
};

class UserClientLfgSetComment : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];

    void Put(CDataStore* msg) override;

    UserClientLfgSetComment();
    ~UserClientLfgSetComment();
};

class UserClientBattlefieldLeave : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientBattlefieldLeave();
    ~UserClientBattlefieldLeave();
};

class UserClientBattlemasterJoinArena : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010;

    void Put(CDataStore* msg) override;

    UserClientBattlemasterJoinArena(uint8_t a1);
    ~UserClientBattlemasterJoinArena();
};

class UserClientBattlefieldList : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;

    void Put(CDataStore* msg) override;

    UserClientBattlefieldList(uint32_t a1);
    ~UserClientBattlefieldList();
};

class UserClientBattlemasterJoin : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ bool unk_0014;
    /* 0015 */ uint8_t unk_0015[3];
    /* 0018 */ uint8_t unk_0018[8];

    void Put(CDataStore* msg) override;

    UserClientBattlemasterJoin();
    ~UserClientBattlemasterJoin();
};

class UserClientGuildDisband : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientGuildDisband();
    ~UserClientGuildDisband();
};

class UserClientGuildLeave : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientGuildLeave();
    ~UserClientGuildLeave();
};

class UserClientGuildLeader : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[48];

    void Put(CDataStore* msg) override;

    UserClientGuildLeader(const char* a1);
    ~UserClientGuildLeader();
};

class UserClientGuildRemove : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];

    void Put(CDataStore* msg) override;

    UserClientGuildRemove(uint64_t a1);
    ~UserClientGuildRemove();
};

class UserClientGuildPromote : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];

    void Put(CDataStore* msg) override;

    UserClientGuildPromote(uint64_t a1);
    ~UserClientGuildPromote();
};

class UserClientGuildDemote : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];

    void Put(CDataStore* msg) override;

    UserClientGuildDemote(uint64_t a1);
    ~UserClientGuildDemote();
};

class UserClientGuildMotd : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[516];

    void Put(CDataStore* msg) override;

    UserClientGuildMotd(const char* a1);
    ~UserClientGuildMotd();
};

class UserClientChatChannelVoiceOn : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelVoiceOn(const char* a1);
    ~UserClientChatChannelVoiceOn();
};

class UserClientChatChannelKick : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelKick(const char* a1, const char* a2);
    ~UserClientChatChannelKick();
};

class UserClientChatChannelOwner : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelOwner(const char* a1);
    ~UserClientChatChannelOwner();
};

class UserClientChatChannelBan : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelBan(const char* a1, const char* a2);
    ~UserClientChatChannelBan();
};

class UserClientChatChannelUnsilenceAll : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelUnsilenceAll(const char* a1, const char* a2);
    ~UserClientChatChannelUnsilenceAll();
};

class UserClientChatChannelSilenceAll : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[48];
    /* 0040 */ char unk_0040[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelSilenceAll(const char* a1, const char* a2);
    ~UserClientChatChannelSilenceAll();
};

class UserClientChatChannelDisplayList : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelDisplayList(const char* a1);
    ~UserClientChatChannelDisplayList();
};

class UserClientChatChannelList : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelList(const char* a1);
    ~UserClientChatChannelList();
};

class UserClientChatChannelUnban : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[48];
    /* 0040 */ char unk_0040[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelUnban(const char* a1, const char* a2);
    ~UserClientChatChannelUnban();
};

class UserClientChatChannelInvite : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[48];
    /* 0040 */ char unk_0040[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelInvite(const char* a1, const char* a2);
    ~UserClientChatChannelInvite();
};

class UserClientChatChannelVoiceOff : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelVoiceOff(const char* a1);
    ~UserClientChatChannelVoiceOff();
};

class UserClientChatChannelPassword : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[48];
    /* 0040 */ char unk_0040[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelPassword(const char* a1, const char* a2);
    ~UserClientChatChannelPassword();
};

class UserClientChatChannelUnmute : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[48];
    /* 0040 */ char unk_0040[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelUnmute(const char* a1, const char* a2);
    ~UserClientChatChannelUnmute();
};

class UserClientChatChannelUnsilenceVoice : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[48];
    /* 0040 */ char unk_0040[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelUnsilenceVoice(const char* a1, const char* a2);
    ~UserClientChatChannelUnsilenceVoice();
};

class UserClientChatChannelUnmoderator : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelUnmoderator(const char* a1, const char* a2);
    ~UserClientChatChannelUnmoderator();
};

class UserClientChatChannelSilenceVoice : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelSilenceVoice(const char* a1, const char* a2);
    ~UserClientChatChannelSilenceVoice();
};

class UserClientChatChannelSetOwner : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelSetOwner(const char* a1, const char* a2);
    ~UserClientChatChannelSetOwner();
};

class UserClientChatChannelMute : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelMute(const char* a1, const char* a2);
    ~UserClientChatChannelMute();
};

class UserClientChatChannelModerator : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];
    /* 0090 */ char unk_0090[48];

    void Put(CDataStore* msg) override;

    UserClientChatChannelModerator(const char* a1, const char* a2);
    ~UserClientChatChannelModerator();
};

class UserClientChatChannelAnnouncements : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[128];

    void Put(CDataStore* msg) override;

    UserClientChatChannelAnnouncements(const char* a1);
    ~UserClientChatChannelAnnouncements();
};

class UserClientChatJoinChannel : public JamClientMessage {
    public:
    /* 0010 */ bool unk_0010;
    /* 0011 */ bool unk_0011;
    /* 0012 */ char m_channelName[128];
    /* 0094 */ uint32_t m_channelID;
    /* 0098 */ char m_password[128];

    void Put(CDataStore* msg) override;

    UserClientChatJoinChannel(uint32_t channelID, bool a2, bool a3, const char* channelName, const char* password);
    ~UserClientChatJoinChannel();
};

class UserClientMessageChatAfk : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatAfk(const char* a1);
    ~UserClientMessageChatAfk();
};

class UserClientMessageChatAddonRaid : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[17];
    /* 0021 */ char unk_0021[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatAddonRaid(const char* a1, const char* a2);
    ~UserClientMessageChatAddonRaid();
};

class UserClientMessageChatRaidWarning : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ char unk_0014[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatRaidWarning(uint32_t a1, const char* a2);
    ~UserClientMessageChatRaidWarning();
};

class UserClientMessageChatSay : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ uint32_t unk_0110;

    void Put(CDataStore* msg) override;

    UserClientMessageChatSay(uint32_t a1, const char* a2);
    ~UserClientMessageChatSay();
};

class UserClientMessageChatAddonOfficer : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ char unk_0110[17];

    void Put(CDataStore* msg) override;

    UserClientMessageChatAddonOfficer(const char* a1, const char* a2);
    ~UserClientMessageChatAddonOfficer();
};

class UserClientMessageChatAddonParty : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[17];
    /* 0021 */ char unk_0021[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatAddonParty(const char* a1, const char* a2);
    ~UserClientMessageChatAddonParty();
};

class UserClientMessageChatEmote : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatEmote(const char* a1);
    ~UserClientMessageChatEmote();
};

class UserClientMessageChatYell : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ uint32_t unk_0110;

    void Put(CDataStore* msg) override;

    UserClientMessageChatYell(uint32_t a1, const char* a2);
    ~UserClientMessageChatYell();
};

class UserClientMessageChatAddonWhisper : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ char unk_0110[17];
    /* 0121 */ char unk_0121[305];

    void Put(CDataStore* msg) override;

    UserClientMessageChatAddonWhisper(const char* a1, const char* a2, const char* a3);
    ~UserClientMessageChatAddonWhisper();
};

class UserClientMessageChatAddonGuild : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[17];
    /* 0021 */ char unk_0021[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatAddonGuild(const char* a1, const char* a2);
    ~UserClientMessageChatAddonGuild();
};

class UserClientMessageChatParty : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ uint32_t unk_0110;

    void Put(CDataStore* msg) override;

    UserClientMessageChatParty(uint32_t a1, const char* a2);
    ~UserClientMessageChatParty();
};

class UserClientMessageChatBattleground : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ char unk_0014[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatBattleground(uint32_t a1, const char* a2);
    ~UserClientMessageChatBattleground();
};

class UserClientMessageChatChannel : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[305];
    /* 0144 */ uint32_t unk_0144;
    /* 0148 */ char unk_0148[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatChannel(uint32_t a1, const char* a2, const char* a3);
    ~UserClientMessageChatChannel();
};

class UserClientMessageChatAddonBattleground : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[17];
    /* 0021 */ char unk_0021[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatAddonBattleground(const char* a1, const char* a2);
    ~UserClientMessageChatAddonBattleground();
};

class UserClientMessageChatDnd : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatDnd(const char* a1);
    ~UserClientMessageChatDnd();
};

class UserClientMessageChatWhisper : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ uint32_t unk_0110;
    /* 0114 */ char unk_0114[305];

    void Put(CDataStore* msg) override;

    UserClientMessageChatWhisper(uint32_t a1, const char* a2, const char* a3);
    ~UserClientMessageChatWhisper();
};

class UserClientMessageChatGuild : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ char unk_0014[256];

    void Put(CDataStore* msg) override;

    UserClientMessageChatGuild(uint32_t a1, const char* a2);
    ~UserClientMessageChatGuild();
};

class UserClientMessageChatRaid : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ uint32_t unk_0110;

    void Put(CDataStore* msg) override;

    UserClientMessageChatRaid(uint32_t a1, const char* a2);
    ~UserClientMessageChatRaid();
};

class UserClientMessageChatOfficer : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[256];
    /* 0110 */ uint32_t unk_0110;

    void Put(CDataStore* msg) override;

    UserClientMessageChatOfficer(uint32_t a1, const char* a2);
    ~UserClientMessageChatOfficer();
};

class UserClientRequestRatedBgInfo : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientRequestRatedBgInfo();
    ~UserClientRequestRatedBgInfo();
};

class UserClientGetPVPOptionsEnabled : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientGetPVPOptionsEnabled();
    ~UserClientGetPVPOptionsEnabled();
};

class UserClientLfgLockInfoRequest : public JamClientMessage {
    public:
    /* 0010 */ bool unk_0010;

    void Put(CDataStore* msg) override;

    UserClientLfgLockInfoRequest();
    ~UserClientLfgLockInfoRequest();
};

class UserClientLfgSetBootVote : public JamClientMessage {
    public:
    /* 0010 */ bool unk_0010;

    void Put(CDataStore* msg) override;

    UserClientLfgSetBootVote();
    ~UserClientLfgSetBootVote();
};

class UserClientStartWarGame : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ uint8_t unk_0018[8];

    void Put(CDataStore* msg) override;

    UserClientStartWarGame();
    ~UserClientStartWarGame();
};

class UserClientLfgTeleport : public JamClientMessage {
    public:
    /* 0010 */ bool unk_0010;

    void Put(CDataStore* msg) override;

    UserClientLfgTeleport();
    ~UserClientLfgTeleport();
};

class UserClientCommentatorGetPartyInfo : public JamClientMessage {
    public:
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ char unk_0014[256];

    void Put(CDataStore* msg) override;

    UserClientCommentatorGetPartyInfo();
    ~UserClientCommentatorGetPartyInfo();
};

class UserClientCommentatorStartWargame : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ char unk_0018[48];
    /* 0048 */ char unk_0048[256];

    void Put(CDataStore* msg) override;

    UserClientCommentatorStartWargame();
    ~UserClientCommentatorStartWargame();
};

class UserClientRequestRatedInfo : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010;

    void Put(CDataStore* msg) override;

    UserClientRequestRatedInfo();
    ~UserClientRequestRatedInfo();
};

class UserClientSendSorRequestViaAddress : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[1280];
    /* 0510 */ char unk_0510[256];

    void Put(CDataStore* msg) override;

    UserClientSendSorRequestViaAddress();
    ~UserClientSendSorRequestViaAddress();
};

class UserClientSendSorRequestViaBnetAccountId : public JamClientMessage {
    public:
    /* 0010 */ char unk_0010[1020];
    /* 040C */ uint32_t unk_040C;

    void Put(CDataStore* msg) override;

    UserClientSendSorRequestViaBnetAccountId();
    ~UserClientSendSorRequestViaBnetAccountId();
};

class UserClientRequestPvpRewards : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientRequestPvpRewards();
    ~UserClientRequestPvpRewards();
};

class UserClientBattlemasterJoinRated : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientBattlemasterJoinRated();
    ~UserClientBattlemasterJoinRated();
};

class UserClientGuildAchievementMembers : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ uint8_t unk_0018[8];
    /* 0020 */ uint32_t unk_0020;

    void Put(CDataStore* msg) override;

    UserClientGuildAchievementMembers(uint64_t a1, uint64_t a3, uint32_t a5);
    ~UserClientGuildAchievementMembers();
};

class UserClientBFMgrQueueInviteResponse : public JamClientMessage {
    public:
    /* 0010 */ bool unk_0010;
    /* 0011 */ uint8_t unk_0011[7];
    /* 0018 */ uint8_t unk_0018[8];

    void Put(CDataStore* msg) override;

    UserClientBFMgrQueueInviteResponse(uint64_t queueID, bool accept);
    ~UserClientBFMgrQueueInviteResponse();
};

class UserClientBFMgrQueueExitRequest : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];

    void Put(CDataStore* msg) override;

    UserClientBFMgrQueueExitRequest(uint64_t queueID);
    ~UserClientBFMgrQueueExitRequest();
};

class UserClientBFMgrEntryInviteResponse : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];
    /* 0018 */ bool unk_0018;

    void Put(CDataStore* msg) override;

    UserClientBFMgrEntryInviteResponse();
    ~UserClientBFMgrEntryInviteResponse();
};

class UserClientRequestPvpLogData : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientRequestPvpLogData();
    ~UserClientRequestPvpLogData();
};

class UserClientRequestBattlefieldPositions : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientRequestBattlefieldPositions();
    ~UserClientRequestBattlefieldPositions();
};

class UserClientBFMgrQueueRequest : public JamClientMessage {
    public:
    /* 0010 */ uint8_t unk_0010[8];

    void Put(CDataStore* msg) override;

    UserClientBFMgrQueueRequest(uint64_t queueID);
    ~UserClientBFMgrQueueRequest();
};

class UserClientAcceptWargameInvite : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_opposingPartyMemberGUID[8];
    /* 0018 */ uint8_t m_queueID[8];
    /* 0020 */ bool m_accept;

    void Put(CDataStore* msg) override;

    UserClientAcceptWargameInvite();
    ~UserClientAcceptWargameInvite();
};

class UserClientChatReportIgnored : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_ignoredGUID[8];
    /* 0018 */ uint8_t m_reason;
    /* 0019 */ uint8_t unk_0019[7];

    void Put(CDataStore* msg) override;

    UserClientChatReportIgnored();
    ~UserClientChatReportIgnored();
};

class UserClientChatUnregisterAllAddonPrefixes : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserClientChatUnregisterAllAddonPrefixes();
    ~UserClientChatUnregisterAllAddonPrefixes();
};

class UserClientChatRegisterAddonPrefixes : public JamClientMessage {
    public:
    /* 0010 */ TSGrowableArray<JamCliStructAddonPrefix> m_prefixes;

    void Put(CDataStore* msg) override;

    UserClientChatRegisterAddonPrefixes();
    ~UserClientChatRegisterAddonPrefixes();
};

class UserClientSupportTicketSubmitComplaint : public JamClientMessage {
    public:
    /* 0010 */ uint8_t m_targetCharacterGUID[8];
    /* 0018 */ char m_note[1024];
    /* 0418 */ JamCliSupportTicketChatLog m_chatLog;
    /* 0430 */ JamCliSupportTicketHeader m_header;
    /* 0444 */ uint32_t m_complaintType;

    void Put(CDataStore* msg) override;

    UserClientSupportTicketSubmitComplaint();
    ~UserClientSupportTicketSubmitComplaint();
};

class UserClientBattlefieldPort : public JamClientMessage {
    public:
    /* 0010 */ JamClientRideTicket m_ticket;
    /* 0024 */ uint8_t unk_0024[4];
    /* 0028 */ bool m_accept;

    void Put(CDataStore* msg) override;

    UserClientBattlefieldPort();
    ~UserClientBattlefieldPort();
};

#endif
