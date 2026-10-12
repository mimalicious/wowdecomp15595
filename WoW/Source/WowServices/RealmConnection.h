#ifndef WOW_SOURCE_WOWSERVICES_REALMCONNECTION_H
#define WOW_SOURCE_WOWSERVICES_REALMCONNECTION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/WowServices/Types.h"

class CHARACTER_INFO;
class ClientAuthResponse;
class ClientEnumCharactersResult;
class ClientWaitQueueFinish;
class ClientWaitQueueUpdate;
class RealmConnection;
class RealmResponse;
struct REALM_INFO;

class CHARACTER_INFO {
    public:
    struct ITEM {
        /* 0000 */ uint32_t m_displayID;
        /* 0004 */ uint32_t m_inventoryType;
        /* 0008 */ uint32_t m_visualID;
    };

    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ char m_name [48];
    /* 0038 */ uint8_t m_listOrder;
    /* 003C */ uint32_t m_mapID;
    /* 0040 */ uint32_t m_zoneID;
    /* 0044 */ uint8_t unk_0044[4];
    /* 0048 */ WOWGUID m_guildGUID;
    /* 0050 */ C3Vector m_position;
    /* 005C */ CHARACTER_INFO::ITEM m_items [23];
    /* 0170 */ uint32_t m_petDisplayInfoID;
    /* 0174 */ uint32_t m_petExperienceLevel;
    /* 0178 */ uint32_t m_petCreatureFamilyID;
    /* 017C */ uint32_t m_flags;
    /* 0180 */ uint32_t m_customizeFlags;
    /* 0184 */ uint8_t m_raceID;
    /* 0185 */ uint8_t m_classID;
    /* 0186 */ uint8_t m_sexID;
    /* 0187 */ uint8_t m_skinColorID;
    /* 0188 */ uint8_t m_faceID;
    /* 0189 */ uint8_t m_hairStyleID;
    /* 018A */ uint8_t m_hairColorID;
    /* 018B */ uint8_t m_facialHairStyleID;
    /* 018C */ uint8_t m_experienceLevel;
    /* 018D */ uint8_t m_firstLogin;
    /* 018E */ uint8_t unk_018E[2];
};

struct CHARACTER_REORDER {
    /* 0000 */ uint64_t m_guid;
    /* 0008 */ uint8_t m_oldOrder;
    /* 0009 */ uint8_t m_newOrder;
};

class RealmConnection : public NetClient {
    public:
    struct REALMCONNECTIONNODE : public TSLinkedNode<RealmConnection::REALMCONNECTIONNODE> {
        /* 0008 */ RealmConnection* m_connection;

        REALMCONNECTIONNODE();
    };

    static TSList<RealmConnection::REALMCONNECTIONNODE> s_allRealmConnections;
    static SCritSect s_allRealmConnectionsCrit;

    static bool ClientAuthResponseHandler(ClientAuthResponse* msg);
    static bool ClientEnumCharactersResultHandler(ClientEnumCharactersResult* msg);
    static bool ClientWaitQueueFinishHandler(ClientWaitQueueFinish* msg);
    static bool ClientWaitQueueUpdateHandler(ClientWaitQueueUpdate* msg);
    static int32_t MessageHandler(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void PollNet(const void* data, void* param);

    /* 46C8 */ RealmResponse* m_realmResponse;
    /* 46CC */ TSFixedArray<CHARACTER_INFO> m_characterList;
    /* 46D8 */ uint8_t m_authenticated;
    /* 46DC */ uint32_t m_queuePosition;
    /* 46E0 */ uint32_t m_freeCharacterMigration;
    /* 46E4 */ uint32_t m_billingTimeRemaining;
    /* 46E8 */ uint32_t m_billingTimeRested;
    /* 46EC */ uint8_t m_billingFlags;
    /* 46ED */ uint8_t m_accountExpansion;
    /* 46EE */ uint8_t m_activeExpansion;
    /* 46F0 */ uint32_t m_timeRemaining;
    /* 46F4 */ uint32_t m_timeRemainingStart;
    /* 46F8 */ uint32_t m_regionID;
    /* 46FC */ uint32_t m_battlegroupID;
    /* 4700 */ uint32_t m_realmID;
    /* 4704 */ uint32_t m_factionChangeRestrictions [24];

    ~RealmConnection();
    void HandleAuthChallenge(AuthenticationChallenge* challenge) override;
    int32_t HandleConnect() override;
    int32_t HandleDisconnect() override;
    int32_t HandleCantConnect() override;
    virtual void HandleCharacterCreate(uint8_t result) = 0;
    virtual void HandleCharacterDelete(uint8_t result) = 0;
    virtual void HandleCharacterLoginFailed(uint8_t result) = 0;
    virtual void HandleLogoutComplete() = 0;
    virtual void HandleLogoutResponse(int32_t result, int32_t instant) = 0;
    virtual void HandleLogoutAbortAck() = 0;

    RealmConnection(RealmResponse* realmResponse);
    uint32_t GetFactionChangeRestriction(uint32_t raceID);
    uint32_t GetTimeRemaining();
    int32_t HandleAddonInfo(uint32_t msgId, uint32_t time, CDataStore* msg);
    bool HandleAuthResponse(ClientAuthResponse* msg);
    bool HandleCharEnum(ClientEnumCharactersResult* msg);
    int32_t HandleCharacterDelete(uint32_t msgId, uint32_t time, CDataStore* msg);
    int32_t HandleClientCacheVersion(uint32_t msgId, uint32_t time, CDataStore* msg);
    int32_t HandleLogoutResponse(uint32_t msgId, uint32_t time, CDataStore* msg);
    bool HandleWaitQueueUpdate(ClientWaitQueueUpdate* msg);
    bool ReorderCharacters(const uint32_t* order, uint32_t count);
    void RequestCharacterEnum();
    void RequestCharacterLogin(WOWGUID guid, int32_t a2);
    void SetSelectedRealm(uint32_t regionID, uint32_t battlegroupID, uint32_t realmID);
};

class RealmResponse {
    public:
    virtual void LoginServerResult(LOGIN_STATE state, LOGIN_RESULT result, const char* message1, const char* message2, const char* message3) = 0;
    virtual void RealmListReceived(TSFixedArray<REALM_INFO>& realms) = 0;
    virtual void JoinRealmResult(uint32_t realmID, int32_t result, int32_t a3, int32_t a4, int32_t a5);
    virtual void HandleAuthResponse(RealmConnection* connection, uint8_t result) = 0;
    virtual void AddOnListReceived(RealmConnection* connection) = 0;
    virtual void CharacterListReceived(RealmConnection* connection, const TSFixedArray<CHARACTER_INFO>& characters, int32_t success) = 0;
    virtual void GameServerResult(RealmConnection* connection, const char* message1, const char* message2, const char* message3) = 0;
    virtual void MessageReceived(RealmConnection* connection, uint32_t message) = 0;
    virtual void OnChatReceived(RealmConnection* connection, WOWGUID guid, const char* text, uint8_t type) = 0;
    virtual void OnChatPlayerNotFound(RealmConnection* connection, const char* name);
    virtual void OnAccountOnline(RealmConnection* connection, int32_t a2, WOWGUID guid) = 0;
    virtual void OnCharacterRestored(RealmConnection* connection, bool success, uint32_t a3, uint32_t a4, uint32_t a5, uint64_t a6, uint64_t a7, uint32_t a8, const char* a9, uint32_t a10, CDataStore* msg, uint64_t a12, uint64_t a13);
    virtual void OnChangeArenaTeamRating(RealmConnection* connection, bool success, int32_t a3, int32_t a4);
    virtual void OnAFKMonitorInfoResponse(RealmConnection* connection, WOWGUID guid, int32_t a3, int32_t a4, int32_t a5, int32_t a6) = 0;
    virtual void OnServerInfo(RealmConnection* connection, int32_t a2, int32_t a3, char* a4);
    virtual void HandleConnect(RealmConnection* connection) = 0;
    virtual void HandleDisconnect(RealmConnection* connection) = 0;
    virtual void HandleCantConnect(RealmConnection* connection) = 0;
    virtual void HandleCharacterLoginFailed(RealmConnection* connection, uint8_t result);
    virtual void HandleReceivingObjects(RealmConnection* connection);
};

#endif
