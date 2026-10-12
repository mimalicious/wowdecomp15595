#include "RealmConnection.h"
#include <cstdlib>
#include <cstring>
#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SHA1.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamClientConnection.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/UIUtil/AddOns.h"
#include "WoW/Source/WowServices/WDataStore.h"

// STATIC: 0x00CC8B38/0x140B68E30
TSList<RealmConnection::REALMCONNECTIONNODE> RealmConnection::s_allRealmConnections;

// STATIC: 0x00DD9FB8/0x140CC0828
SCritSect RealmConnection::s_allRealmConnectionsCrit;

// INLINED
RealmConnection::REALMCONNECTIONNODE::REALMCONNECTIONNODE() {
    this->m_connection = NULL;
}

// OFFSET: 0x006246B0/0x1402B5DF0
int32_t RealmConnection::HandleConnect() {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "Connected", NULL, NULL);
    }

    this->m_authenticated = 0;
    this->m_realmResponse->HandleConnect(this);

    return NetClient::HandleConnect();
}

// OFFSET: 0x006246F0/0x1402B5E50
int32_t RealmConnection::HandleDisconnect() {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "Disconnected", NULL, NULL);
    }

    this->m_realmResponse->HandleDisconnect(this);

    return NetClient::HandleDisconnect();
}

// OFFSET: 0x00624730/0x1402B5EB0
int32_t RealmConnection::HandleCantConnect() {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "Can't Connect", NULL, NULL);
    }

    this->m_realmResponse->HandleCantConnect(this);

    return NetClient::HandleCantConnect();
}

// OFFSET: 0x00624770/0x1402B5F10
bool RealmConnection::HandleWaitQueueUpdate(ClientWaitQueueUpdate* msg) {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "SMSG_WAIT_QUEUE_UPDATE", NULL, NULL);
    }

    if (!this->m_authenticated) {
        return false;
    }

    this->m_queuePosition = msg->m_waitCount;
    this->m_freeCharacterMigration = msg->m_freeCharacterMigration;
    this->m_realmResponse->HandleAuthResponse(this, 27);

    return true;
}

// OFFSET: 0x006247D0/0x1402B5FA0
int32_t RealmConnection::HandleAddonInfo(uint32_t msgId, uint32_t time, CDataStore* msg) {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "SMSG_ADDON_INFO", NULL, NULL);
    }

    SetAddOnInfoRequest(msg);
    this->m_realmResponse->AddOnListReceived(this);

    return 1;
}

// OFFSET: 0x00624820/0x1402B6010
int32_t RealmConnection::HandleClientCacheVersion(uint32_t msgId, uint32_t time, CDataStore* msg) {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "SMSG_CLIENTCACHE_VERSION", NULL, NULL);
    }

    uint32_t version;
    msg->Get(version);
    SetClientCacheVersion(version);

    return 1;
}

// OFFSET: 0x00624870/0x1402B6070
static int32_t CompareCharacters(const void* a, const void* b) {
    const JamClientCharacterListEntry* first = *static_cast<const JamClientCharacterListEntry* const*>(a);
    const JamClientCharacterListEntry* second = *static_cast<const JamClientCharacterListEntry* const*>(b);

    uint32_t firstOrder = first->m_listOrder ? first->m_listOrder : 0xFFFFFFFF;
    uint32_t secondOrder = second->m_listOrder ? second->m_listOrder : 0xFFFFFFFF;

    if (firstOrder != secondOrder) {
        return firstOrder < secondOrder ? -1 : 1;
    }

    uint64_t firstGUID = *reinterpret_cast<const uint64_t*>(first->m_guid);
    uint64_t secondGUID = *reinterpret_cast<const uint64_t*>(second->m_guid);

    if (firstGUID > secondGUID) {
        return 1;
    }

    if (firstGUID < secondGUID) {
        return -1;
    }

    return 0;
}

// OFFSET: 0x006248E0/0x1402B60C0
uint32_t RealmConnection::GetTimeRemaining() {
    uint32_t remaining = this->m_timeRemaining;

    if (!remaining || remaining > 0x418937) {
        return 0xFFFFFFFF;
    }

    uint32_t end = this->m_timeRemainingStart + 1000 * remaining;
    uint32_t now = OsGetAsyncTimeMs();

    if (now - this->m_timeRemainingStart >= end - this->m_timeRemainingStart) {
        return 0;
    }

    uint32_t left = end - now;

    if (left > 0x7FFFFFFF) {
        return 0xFFFFFFFF;
    }

    return left / 1000;
}

// OFFSET: 0x00624950/0x1402B6150
uint32_t RealmConnection::GetFactionChangeRestriction(uint32_t raceID) {
    if (raceID >= 24) {
        return 0;
    }

    return this->m_factionChangeRestrictions[raceID];
}

// OFFSET: 0x00624970
int32_t RealmConnection::HandleCharacterDelete(uint32_t msgId, uint32_t time, CDataStore* msg) {
    uint8_t result;
    msg->Get(result);
    this->HandleCharacterDelete(result);

    return 1;
}

// OFFSET: 0x006249A0/0x1402B6170
int32_t RealmConnection::HandleLogoutResponse(uint32_t msgId, uint32_t time, CDataStore* msg) {
    uint32_t result = 0;
    msg->Get(result);

    uint8_t instant = 0;
    msg->Get(instant);
    this->HandleLogoutResponse(result, instant);

    return 1;
}

// OFFSET: 0x006249F0/0x1402B61D0
void RealmConnection::RequestCharacterEnum() {
    UserClientEnumCharacters msg;
    this->Send(&msg, CONNECTION_ROUTED);
}

// OFFSET: 0x00624A20/0x1402B6210
void RealmConnection::SetSelectedRealm(uint32_t regionID, uint32_t battlegroupID, uint32_t realmID) {
    this->m_regionID = regionID;
    this->m_battlegroupID = battlegroupID;
    this->m_realmID = realmID;
}

// OFFSET: 0x00624A50/0x1402B6230
static int32_t CompareOrders(const void* a, const void* b) {
    uint8_t first = *static_cast<const uint8_t*>(a);
    uint8_t second = *static_cast<const uint8_t*>(b);

    if (second > first) {
        return -1;
    }

    return second < first;
}

// OFFSET: 0x00624A70
bool RealmConnection::ClientWaitQueueUpdateHandler(ClientWaitQueueUpdate* msg) {
    return static_cast<RealmConnection*>(msg->m_netClient)->HandleWaitQueueUpdate(msg);
}

// OFFSET: 0x00624A90/0x1402B6260
bool RealmConnection::ClientWaitQueueFinishHandler(ClientWaitQueueFinish* msg) {
    RealmConnection* connection = static_cast<RealmConnection*>(msg->m_netClient);

    if (connection->m_realmResponse) {
        connection->m_realmResponse->GameServerResult(connection, "SMSG_WAIT_QUEUE_FINISH", NULL, NULL);
    }

    if (!connection->m_authenticated) {
        return false;
    }

    connection->m_realmResponse->HandleAuthResponse(connection, 12);

    return true;
}

// OFFSET: 0x00624D10/0x1402B65E0
int32_t RealmConnection::MessageHandler(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    RealmConnection* connection = static_cast<RealmConnection*>(param);
    int32_t result = 0;
    uint8_t value;

    switch (msgId) {
        case SMSG_LOGOUT_RESPONSE:
            result = connection->HandleLogoutResponse(SMSG_LOGOUT_RESPONSE, time, msg);
            break;

        case SMSG_DELETE_CHAR:
            result = connection->HandleCharacterDelete(SMSG_DELETE_CHAR, time, msg);
            break;

        case SMSG_LOGOUT_COMPLETE:
            connection->HandleLogoutComplete();
            result = 1;
            break;

        case SMSG_CACHE_VERSION:
            result = connection->HandleClientCacheVersion(msgId, time, msg);
            break;

        case SMSG_ADDON_INFO:
            result = connection->HandleAddonInfo(SMSG_ADDON_INFO, time, msg);
            break;

        case SMSG_CHAR_CREATE:
            msg->Get(value);
            connection->HandleCharacterCreate(value);
            result = 1;
            break;

        case SMSG_CHARACTER_LOGIN_FAILED:
            msg->Get(value);
            connection->HandleCharacterLoginFailed(value);
            result = 1;
            break;

        case SMSG_LOGOUT_CANCEL_ACK:
            connection->HandleLogoutAbortAck();
            result = 1;
            break;

        default:
            break;
    }

    if (!msg->IsRead()) {
        char text[64];
        SStrPrintf(text, sizeof(text), "ID:%d Read:%d Size:%d", msgId, msg->m_read, msg->m_size);

        if (connection->m_realmResponse) {
            connection->m_realmResponse->GameServerResult(connection, "RealmConnection::MessageHandler", "Message Under Read!", text);
        }

        msg->Reset();
    }

    return result;
}

// OFFSET: 0x00624E70/0x1402B6790
void RealmConnection::HandleAuthChallenge(AuthenticationChallenge* challenge) {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "SMSG_AUTH_CHALLENGE", NULL, NULL);
    }

    this->m_authenticated = 0;

    CRndSeed seed;
    seed.SetSeed(0);
    seed.SetSeed(OsGetAsyncTimeMs());

    UserRouterClientAuthSession msg;
    msg.m_build = 15595;
    msg.m_unk0018 = 1;
    msg.m_loginServerID = this->m_loginData.m_loginServerID;
    msg.m_loginServerType = this->m_loginData.m_loginServerType;
    msg.m_localChallenge = CRandom::uint32_(seed);
    uint32_t localChallenge = msg.m_localChallenge;
    SStrCopy(msg.m_account, this->m_loginData.m_account, 1280);
    msg.m_regionID = this->m_regionID;
    msg.m_battlegroupID = this->m_battlegroupID;
    msg.m_realmID = this->m_realmID;
    msg.m_dosResponse = challenge->m_proof;

    uint32_t zero = 0;
    msg.m_useIPv6 = g_useIPv6Var->m_intValue != 0;

    SHA1_CONTEXT context;
    SHA1_Init(&context);
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>(this->m_loginData.m_account), strlen(this->m_loginData.m_account));
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>(&zero), sizeof(zero));
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>(&localChallenge), sizeof(localChallenge));
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>(challenge), sizeof(challenge->m_challenge));
    SHA1_Update(&context, this->m_loginData.m_sessionKey, sizeof(this->m_loginData.m_sessionKey));
    SHA1_Final(msg.m_digest, &context);

    GetAddOnInfoRequest(&msg.m_addonInfo);
    msg.m_addonInfo.Finalize();
    this->Send(&msg, CONNECTION_ROUTED);

    if (!g_disableServerNagleVar->m_intValue) {
        UserRouterClientEnableNagle nagle;
        this->Send(&nagle, CONNECTION_ROUTED);
    }
}

// OFFSET: 0x00625070/0x1402B6A10
bool RealmConnection::HandleAuthResponse(ClientAuthResponse* msg) {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "SMSG_AUTH_RESPONSE", NULL, NULL);
    }

    uint8_t result = msg->m_result;

    if (result == 12) {
        if (msg->m_hasSuccessInfo) {
            this->m_authenticated = 1;
        } else {
            result = 13;
        }
    } else if (result == 27) {
        result = 13;
    }

    if (msg->m_hasSuccessInfo) {
        this->m_billingTimeRemaining = msg->m_billingTimeRemaining;
        this->m_billingFlags = msg->m_billingFlags;
        this->m_billingTimeRested = msg->m_billingTimeRested;
        this->m_accountExpansion = msg->m_accountExpansion;
        this->m_activeExpansion = msg->m_activeExpansion;
        this->m_timeRemaining = msg->m_timeRemaining;
        this->m_timeRemainingStart = OsGetAsyncTimeMs();
    }

    uint8_t status = result;

    if (this->m_authenticated && msg->m_hasWaitInfo) {
        this->m_queuePosition = msg->m_waitCount;
        this->m_freeCharacterMigration = msg->m_freeCharacterMigration;
        status = 27;
    }

    this->m_realmResponse->HandleAuthResponse(this, status);

    return true;
}

// OFFSET: 0x00625150/0x1402B6B40
void RealmConnection::RequestCharacterLogin(WOWGUID guid, int32_t a2) {
    CDataStore unused;
    UserClientPlayerLogin msg(guid.m_guid);
    this->Send(&msg, CONNECTION_ROUTED);
}

// OFFSET: 0x006251D0
bool RealmConnection::ClientAuthResponseHandler(ClientAuthResponse* msg) {
    return static_cast<RealmConnection*>(msg->m_netClient)->HandleAuthResponse(msg);
}

// OFFSET: 0x00625350/0x1402B6CA0
void RealmConnection::PollNet(const void* data, void* param) {
    RealmConnection::s_allRealmConnectionsCrit.Enter();

    RealmConnection::REALMCONNECTIONNODE* node = RealmConnection::s_allRealmConnections.Head();

    while (!TSList<RealmConnection::REALMCONNECTIONNODE, TSGetLink<RealmConnection::REALMCONNECTIONNODE> >::IsEnd(node)) {
        RealmConnection::REALMCONNECTIONNODE* next = RealmConnection::s_allRealmConnections.RawNext(node);
        node->m_connection->PollEventQueue();
        node = next;
    }

    RealmConnection::s_allRealmConnectionsCrit.Leave();
}

// OFFSET: 0x006253A0/0x1402B6D00
bool RealmConnection::HandleCharEnum(ClientEnumCharactersResult* msg) {
    if (this->m_realmResponse) {
        this->m_realmResponse->GameServerResult(this, "SMSG_CHAR_ENUM", NULL, NULL);
    }

    this->m_characterList.Clear();
    memset(this->m_factionChangeRestrictions, 0, sizeof(this->m_factionChangeRestrictions));

    bool failed = false;
    uint32_t count;

    if (!msg->m_success || (count = msg->m_characters.Count()) > 200) {
        failed = true;
        this->m_characterList.Clear();
        memset(this->m_factionChangeRestrictions, 0, sizeof(this->m_factionChangeRestrictions));
    } else {
        this->m_characterList.SetCount(count);

        TSFixedArray<const JamClientCharacterListEntry*> sorted;

        if (count) {
            sorted.ReallocData(count);
            sorted.m_count = count;
        }

        for (uint32_t i = 0; i < count; i++) {
            sorted.m_data[i] = &msg->m_characters.m_data[i];
        }

        qsort(sorted.m_data, count, sizeof(const JamClientCharacterListEntry*), CompareCharacters);

        for (uint32_t i = 0; i < count; i++) {
            const JamClientCharacterListEntry* entry = sorted.m_data[i];
            CHARACTER_INFO& info = this->m_characterList.m_data[i];

            info.m_guid.m_guid = *reinterpret_cast<const uint64_t*>(entry->m_guid);
            SStrCopy(info.m_name, entry->m_name, sizeof(info.m_name));
            info.m_listOrder = entry->m_listOrder;
            info.m_raceID = entry->m_raceID;
            info.m_classID = entry->m_classID;
            info.m_sexID = entry->m_sexID;
            info.m_skinColorID = entry->m_skinColorID;
            info.m_faceID = entry->m_faceID;
            info.m_hairStyleID = entry->m_hairStyleID;
            info.m_hairColorID = entry->m_hairColorID;
            info.m_facialHairStyleID = entry->m_facialHairStyleID;
            info.m_experienceLevel = entry->m_experienceLevel;
            info.m_zoneID = entry->m_zoneID;
            info.m_mapID = entry->m_mapID;
            info.m_position.x = entry->m_position.x;
            info.m_position.y = entry->m_position.y;
            info.m_position.z = entry->m_position.z;
            info.m_guildGUID.m_guid = *reinterpret_cast<const uint64_t*>(entry->m_guildGUID);
            info.m_flags = entry->m_flags;
            info.m_customizeFlags = entry->m_customizeFlags;
            info.m_firstLogin = entry->m_firstLogin != 0;
            info.m_petDisplayInfoID = entry->m_petDisplayInfoID;
            info.m_petExperienceLevel = entry->m_petExperienceLevel;
            info.m_petCreatureFamilyID = entry->m_petCreatureFamilyID;

            for (uint32_t j = 0; j < 23; j++) {
                info.m_items[j].m_displayID = entry->m_items[j].m_displayID;
                info.m_items[j].m_inventoryType = entry->m_items[j].m_inventoryType;
                info.m_items[j].m_visualID = entry->m_items[j].m_auraVisualID;
            }
        }

        for (uint32_t i = 0; i < msg->m_factionChangeRestrictions.Count(); i++) {
            const JamClientRestrictedFactionChangeRule& rule = msg->m_factionChangeRestrictions.m_data[i];

            if (rule.m_raceID < 24) {
                this->m_factionChangeRestrictions[rule.m_raceID] = rule.m_mask;
            }
        }
    }

    this->m_realmResponse->CharacterListReceived(this, this->m_characterList, !failed);

    return true;
}

// OFFSET: 0x006256B0
bool RealmConnection::ClientEnumCharactersResultHandler(ClientEnumCharactersResult* msg) {
    return static_cast<RealmConnection*>(msg->m_netClient)->HandleCharEnum(msg);
}

// OFFSET: 0x006256D0/0x1402B70F0
RealmConnection::RealmConnection(RealmResponse* realmResponse) : m_realmResponse(realmResponse), m_authenticated(0), m_queuePosition(0), m_freeCharacterMigration(0), m_billingTimeRemaining(0), m_billingTimeRested(0), m_billingFlags(0), m_accountExpansion(0), m_activeExpansion(0), m_timeRemaining(0), m_timeRemainingStart(0), m_regionID(0), m_battlegroupID(0), m_realmID(0) {
    this->Initialize();

    ClientAuthResponse::s_handler = JamHandlerEncode(RealmConnection::ClientAuthResponseHandler, g_jamHandlerKey_CFF274);
    ClientWaitQueueUpdate::s_handler = JamHandlerEncode(RealmConnection::ClientWaitQueueUpdateHandler, g_jamHandlerKey_CFD04C);
    ClientWaitQueueFinish::s_handler = JamHandlerEncode(RealmConnection::ClientWaitQueueFinishHandler, g_jamHandlerKey_CFDD28);

    this->SetMessageHandler(SMSG_ADDON_INFO, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_CHAR_CREATE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_CHARACTER_LOGIN_FAILED, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_LOGOUT_COMPLETE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_LOGOUT_CANCEL_ACK, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_LOGOUT_RESPONSE, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_DELETE_CHAR, RealmConnection::MessageHandler, this);
    this->SetMessageHandler(SMSG_CACHE_VERSION, RealmConnection::MessageHandler, this);

    ClientEnumCharactersResult::s_handler = JamHandlerEncode(RealmConnection::ClientEnumCharactersResultHandler, g_jamHandlerKey_CFE57C);

    RealmConnection::s_allRealmConnectionsCrit.Enter();
    RealmConnection::REALMCONNECTIONNODE* node = RealmConnection::s_allRealmConnections.NewNode(2, 0, 8);
    node->m_connection = this;
    RealmConnection::s_allRealmConnectionsCrit.Leave();
}

// OFFSET: 0x006258B0/0x1402B7330
bool RealmConnection::ReorderCharacters(const uint32_t* order, uint32_t count) {
    bool unordered = false;

    if (count != this->m_characterList.Count()) {
        return false;
    }

    if (count < 2) {
        return true;
    }

    CHARACTER_REORDER* entries = static_cast<CHARACTER_REORDER*>(alloca(sizeof(CHARACTER_REORDER) * count));
    memset(entries, 0, sizeof(CHARACTER_REORDER) * count);

    WowConnection::IDBITS used;
    used.SetCount(count);

    if (used.m_count) {
        memset(used.m_bits.m_data, 0, used.m_bits.m_count * sizeof(uint32_t));
    }

    bool reordered = false;

    for (uint32_t i = 0; i < count; i++) {
        uint32_t index = order[i];

        if (index >= this->m_characterList.Count()) {
            return false;
        }

        if (index != i) {
            reordered = true;
        }

        uint32_t mask = 1 << (index & 0x1F);
        uint32_t& word = used.m_bits.m_data[index >> 5];

        if (word & mask) {
            return false;
        }

        word |= mask;

        const CHARACTER_INFO& character = this->m_characterList.m_data[index];
        entries[i].m_guid = character.m_guid.m_guid;
        entries[i].m_oldOrder = character.m_listOrder;
        entries[i].m_newOrder = character.m_listOrder;

        if (!character.m_listOrder) {
            unordered = true;
        }
    }

    if (!reordered) {
        return true;
    }

    uint8_t* orders = static_cast<uint8_t*>(alloca(count));

    for (uint32_t i = 0; i < count; i++) {
        orders[i] = entries[i].m_oldOrder;
    }

    qsort(orders, count, sizeof(uint8_t), CompareOrders);

    bool renumber = false;

    for (uint32_t i = 1; i < count; i++) {
        if (orders[i] == orders[i - 1]) {
            renumber = true;
            break;
        }
    }

    if (!renumber) {
        renumber = true;

        if (!unordered) {
            uint32_t previous = 0;

            for (uint32_t i = 0; i < count; i++) {
                uint32_t next = 0xFFFFFFFF;

                for (uint32_t j = i + 1; j < count; j++) {
                    if (entries[j].m_oldOrder < next) {
                        next = entries[j].m_oldOrder;
                    }
                }

                uint32_t value = entries[i].m_oldOrder;

                if (value <= previous || value >= next) {
                    value = next <= 0xFF ? previous + ((next - previous) >> 1) : previous + 10;

                    if (value <= previous || value >= next) {
                        break;
                    }
                }

                if (value - 1 > 0xFE) {
                    break;
                }

                entries[i].m_newOrder = value;

                if (i + 1 >= count) {
                    uint8_t last = 0;
                    renumber = false;

                    for (uint32_t j = 0; j < count; j++) {
                        if (entries[j].m_newOrder <= last) {
                            renumber = true;
                            break;
                        }

                        last = entries[j].m_newOrder;
                    }

                    break;
                }

                previous = value;
            }
        }
    }

    if (renumber) {
        for (uint32_t i = 0; i < count; i++) {
            entries[i].m_newOrder = (count > 25 ? 1 : 10) * (i + 1);
        }
    }

    UserClientReorderCharacters msg;

    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].m_newOrder != entries[i].m_oldOrder) {
            JamUserClientReorderEntry* entry = msg.m_entries.New();
            memcpy(entry->m_guid, &entries[i].m_guid, sizeof(entry->m_guid));
            entry->m_newIndex = entries[i].m_newOrder;
        }
    }

    if (msg.m_entries.Count()) {
        this->Send(&msg, CONNECTION_ROUTED);
    }

    return true;
}

// OFFSET: [0x00625D00/0x1402B77C0, 0x004CF280/0x1402B79C0]
RealmConnection::~RealmConnection() {
    RealmConnection::s_allRealmConnectionsCrit.Enter();

    for (RealmConnection::REALMCONNECTIONNODE* node = RealmConnection::s_allRealmConnections.Head(); node; node = RealmConnection::s_allRealmConnections.Next(node)) {
        if (node->m_connection == this) {
            RealmConnection::s_allRealmConnections.DeleteNode(node);
            break;
        }
    }

    ClientAuthResponse::s_handler = NULL;
    ClientWaitQueueUpdate::s_handler = NULL;
    ClientWaitQueueFinish::s_handler = NULL;

    this->ResetHandler(SMSG_ADDON_INFO);
    this->ResetHandler(SMSG_CHAR_CREATE);
    this->ResetHandler(SMSG_CHARACTER_LOGIN_FAILED);
    this->ResetHandler(SMSG_LOGOUT_COMPLETE);
    this->ResetHandler(SMSG_LOGOUT_CANCEL_ACK);
    this->ResetHandler(SMSG_LOGOUT_RESPONSE);
    this->ResetHandler(SMSG_DELETE_CHAR);
    this->ResetHandler(SMSG_CACHE_VERSION);

    ClientEnumCharactersResult::s_handler = NULL;

    this->Destroy();

    RealmConnection::s_allRealmConnectionsCrit.Leave();

    this->m_characterList.Clear();

    RealmConnection::s_allRealmConnectionsCrit.Enter();
    this->m_realmResponse = NULL;
    RealmConnection::s_allRealmConnectionsCrit.Leave();
}

// INLINED
void RealmResponse::JoinRealmResult(uint32_t realmID, int32_t result, int32_t a3, int32_t a4, int32_t a5) {
}

// OFFSET: 0x00841AE0/0x140211B90
void RealmResponse::OnChatPlayerNotFound(RealmConnection* connection, const char* name) {
}

// OFFSET: 0x004CF270/0x1407D7E80
void RealmResponse::OnCharacterRestored(RealmConnection* connection, bool success, uint32_t a3, uint32_t a4, uint32_t a5, uint64_t a6, uint64_t a7, uint32_t a8, const char* a9, uint32_t a10, CDataStore* msg, uint64_t a12, uint64_t a13) {
}

// OFFSET: 0x004CF260/0x1407D7E80
void RealmResponse::OnChangeArenaTeamRating(RealmConnection* connection, bool success, int32_t a3, int32_t a4) {
}

// OFFSET: 0x004CF260/0x1407D7E80
void RealmResponse::OnServerInfo(RealmConnection* connection, int32_t a2, int32_t a3, char* a4) {
}

// OFFSET: 0x00841AE0/0x140211B90
void RealmResponse::HandleCharacterLoginFailed(RealmConnection* connection, uint8_t result) {
}

// OFFSET: 0x00847950/0x1407D7E80
void RealmResponse::HandleReceivingObjects(RealmConnection* connection) {
}
