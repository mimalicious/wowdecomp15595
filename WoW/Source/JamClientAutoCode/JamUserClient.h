#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMUSERCLIENT_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMUSERCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
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

#endif
