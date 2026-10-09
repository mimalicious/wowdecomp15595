#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENT_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "WoW/Source/WowServices/WDataStore.h"

struct JamClientCharacterListItem {
    /* 0000 */ uint32_t m_displayID;
    /* 0004 */ uint32_t m_auraVisualID;
    /* 0008 */ uint8_t m_inventoryType;
    /* 0009 */ uint8_t unk_0009[3];
};

struct JamClientCharacterListEntry {
    /* 0000 */ uint8_t m_guid[8];
    /* 0008 */ char m_name[48];
    /* 0038 */ uint8_t m_listOrder;
    /* 0039 */ uint8_t m_raceID;
    /* 003A */ uint8_t m_classID;
    /* 003B */ uint8_t m_sexID;
    /* 003C */ uint8_t m_skinColorID;
    /* 003D */ uint8_t m_faceID;
    /* 003E */ uint8_t m_hairStyleID;
    /* 003F */ uint8_t m_hairColorID;
    /* 0040 */ uint8_t m_facialHairStyleID;
    /* 0041 */ uint8_t m_experienceLevel;
    /* 0042 */ uint8_t unk_0042[2];
    /* 0044 */ uint32_t m_zoneID;
    /* 0048 */ uint32_t m_mapID;
    /* 004C */ C3Vector m_position;
    /* 0058 */ uint8_t m_guildGUID[8];
    /* 0060 */ uint32_t m_flags;
    /* 0064 */ uint32_t m_customizeFlags;
    /* 0068 */ bool m_firstLogin;
    /* 0069 */ uint8_t unk_0069[3];
    /* 006C */ uint32_t m_petDisplayInfoID;
    /* 0070 */ uint32_t m_petExperienceLevel;
    /* 0074 */ uint32_t m_petCreatureFamilyID;
    /* 0078 */ JamClientCharacterListItem m_items[23];
    /* 018C */ uint8_t unk_018C[4];
};

struct JamClientRestrictedFactionChangeRule {
    /* 0000 */ uint32_t m_mask;
    /* 0004 */ uint8_t m_raceID;
    /* 0005 */ uint8_t unk_0005[3];
};

class ClientAuthResponse : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAuthResponse* msg);

    /* 0010 */ bool m_hasSuccessInfo;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ uint32_t m_billingTimeRemaining;
    /* 0018 */ uint32_t m_billingTimeRested;
    /* 001C */ uint32_t m_timeRemaining;
    /* 0020 */ uint8_t m_billingFlags;
    /* 0021 */ uint8_t m_accountExpansion;
    /* 0022 */ uint8_t m_activeExpansion;
    /* 0023 */ uint8_t unk_0023;
    /* 0024 */ bool m_hasWaitInfo;
    /* 0025 */ uint8_t unk_0025[3];
    /* 0028 */ uint32_t m_waitCount;
    /* 002C */ bool m_freeCharacterMigration;
    /* 002D */ uint8_t unk_002D[3];
    /* 0030 */ uint8_t m_result;

    ClientAuthResponse(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAuthResponse();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientWaitQueueUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWaitQueueUpdate* msg);

    /* 0010 */ uint32_t m_waitCount;
    /* 0014 */ bool m_freeCharacterMigration;

    ClientWaitQueueUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWaitQueueUpdate();
    bool Handle();
};

class ClientWaitQueueFinish : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientWaitQueueFinish* msg);

    ClientWaitQueueFinish(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientWaitQueueFinish();
    bool Handle();
};

class ClientEnumCharactersResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientEnumCharactersResult* msg);

    /* 0010 */ bool m_success;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ TSGrowableArray<JamClientCharacterListEntry> m_characters;
    /* 0024 */ TSGrowableArray<JamClientRestrictedFactionChangeRule> m_factionChangeRestrictions;

    ClientEnumCharactersResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientEnumCharactersResult();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientGenerateRandomCharacterNameResult : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientGenerateRandomCharacterNameResult* msg);

    /* 0010 */ bool m_success;
    /* 0011 */ char m_name[48];

    ClientGenerateRandomCharacterNameResult(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientGenerateRandomCharacterNameResult();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

struct HotfixNotify {
    /* 0000 */ uint32_t m_tableHash;
    /* 0004 */ int32_t m_recordID;
    /* 0008 */ uint32_t m_timestamp;
};

class ClientDBReply : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientDBReply* msg);

    /* 0010 */ uint32_t m_tableHash;
    /* 0014 */ int32_t m_recordID;
    /* 0018 */ uint32_t m_timestamp;
    /* 001C */ WDataStore m_data;

    ClientDBReply(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientDBReply();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientHotfixNotify : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientHotfixNotify* msg);

    /* 0010 */ TSGrowableArray<HotfixNotify> m_hotfixes;

    ClientHotfixNotify(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientHotfixNotify();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientHotfixNotifyBlob : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientHotfixNotifyBlob* msg);

    /* 0010 */ uint32_t m_tableHash;
    /* 0014 */ int32_t m_recordID;
    /* 0018 */ uint32_t m_timestamp;

    ClientHotfixNotifyBlob(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientHotfixNotifyBlob();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

#endif
