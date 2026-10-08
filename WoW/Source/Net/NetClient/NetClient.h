#ifndef WOW_SOURCE_NET_NETCLIENT_NETCLIENT_H
#define WOW_SOURCE_NET_NETCLIENT_NETCLIENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/RSA.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/Net/NetClient/Types.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/WowServices/WowConnection.h"

class CGObject_C;
class CHashKeyGUID;
class ClientConnection;
struct ClntObjMgr;

struct ClntObjMgr {
    /* 0000 */ TSHashTable<CGObject_C, CHashKeyGUID> m_objects;
    /* 002C */ TSHashTable<CGObject_C, CHashKeyGUID> m_lazyCleanupObjects;
    /* 0058 */ TSList<CGObject_C> m_lazyCleanupFifo [8];
    /* 00B8 */ TSList<CGObject_C> m_visibleObjects;
    /* 00C8 */ WOWGUID m_activePlayer;
    /* 00D0 */ PLAYER_TYPE m_type;
    /* 00D4 */ uint32_t m_mapID;
    /* 00D8 */ ClientConnection* m_net;
    /* 00DC */ uint8_t unk_00DC[4];
};

void ClntObjMgrPop();
void ClntObjMgrPush(ClntObjMgr* mgr);

class AuthenticationChallenge {
    public:
    /* 0000 */ uint32_t m_challenge;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint64_t m_proof;
};

struct LoginData {
    /* 0000 */ char m_account [1280];
    /* 0500 */ int32_t m_loginServerID;
    /* 0504 */ uint8_t m_sessionKey [40];
    /* 052C */ int32_t m_loginServerType;
};

enum CONNECTION_ID {
    CONNECTION_USER = 0,
    CONNECTION_INSTANCE = 1,
    CONNECTION_ROUTED = 2,
};

enum DISCONNECT_REASON {
    DISCONNECT_REASON_NONE = 0,
    DISCONNECT_REASON_PROTOCOL_VIOLATION = 3,
    DISCONNECT_REASON_AUTH_FAILED = 4,
    DISCONNECT_REASON_AUTH_UNEXPECTED = 7,
    DISCONNECT_REASON_REPLACED = 11,
    DISCONNECT_REASON_RECONNECT = 12,
    DISCONNECT_REASON_SHUTDOWN = 13,
    DISCONNECT_REASON_DECOMPRESS_FAILED = 17,
};

typedef int32_t (* MESSAGE_HANDLER_interface )(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);

typedef MESSAGE_HANDLER_interface MESSAGE_HANDLER;

class JamClientMessage;
class NetClient;
class ClientConnectionAuthChallenge;
class ClientConnectionConnectTo;
class ClientConnectionDropNewConnection;
class ClientConnectionPong;
class ClientConnectionResetCompressionContext;
class ClientConnectionResumeComms;
class ClientConnectionSuspendComms;
struct z_stream_s;

struct NETEVENTQUEUENODE : public TSLinkedNode<NETEVENTQUEUENODE> {
    /* 0008 */ uint32_t m_eventId;
    /* 000C */ uint32_t m_timeReceived;
    /* 0010 */ void* m_data;
    /* 0014 */ uint32_t m_dataSize;

    ~NETEVENTQUEUENODE();
};

class NETEVENTQUEUE {
    public:
    /* 0000 */ NetClient* m_client;
    /* 0004 */ SCritSect m_critSect;
    /* 002C */ TSList<NETEVENTQUEUENODE> m_eventQueue;

    NETEVENTQUEUE(NetClient* client);
    ~NETEVENTQUEUE();
    void AddEvent(EVENTID eventId, void* connection, NetClient* client, const void* data, uint32_t size);
    void Clear();
    void Poll();
};

struct NETCONNECTIONSTATS {
    /* 0000 */ uint32_t m_pingSequence;
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint64_t m_pingSent;
    /* 0010 */ uint32_t m_latency[16];
    /* 0050 */ uint32_t m_latencyStart;
    /* 0054 */ uint32_t m_latencyEnd;
};

class NetClient : public WowConnectionResponse {
    public:
    static uint32_t s_clientCount;

    enum CONNECTIONS {
        CONNECTIONS_USER = 0,
        CONNECTIONS_INSTANCE = 1,
        CONNECTIONS_NEW_USER = 2,
        CONNECTIONS_NEW_INSTANCE = 3,
    };

    struct HELDMESSAGE : public TSLinkedNode<NetClient::HELDMESSAGE> {
        /* 0008 */ CONNECTION_ID m_connection;
        /* 000C */ CDataStore m_msg;
    };

    /* 0004 */ LoginData m_loginData;
    /* 0534 */ SCritSect m_critSect;
    /* 055C */ NETSTATE m_netState;
    /* 0560 */ MESSAGE_HANDLER m_handlers [2048];
    /* 2560 */ void* m_handlerParams [2048];
    /* 4560 */ NETEVENTQUEUE* m_netEventQueue;
    /* 4564 */ ATOMIC32 m_refCount;
    /* 4568 */ bool m_deleteMe;
    /* 4569 */ uint8_t unk_4569[7];
    /* 4570 */ NETCONNECTIONSTATS m_connectionStatistics [2];
    /* 4620 */ uint64_t m_connectKeys [2];
    /* 4630 */ uint32_t m_bytesSent;
    /* 4634 */ uint32_t m_bytesReceived;
    /* 4638 */ uint32_t m_connectedTimestamp;
    /* 463C */ ClntObjMgr* m_objMgr;
    /* 4640 */ WowConnection* m_connections [4];
    /* 4650 */ z_stream_s* m_inflateStreams [4];
    /* 4660 */ TSList<NetClient::HELDMESSAGE> m_heldMessages [2];
    /* 4678 */ uint32_t m_connectSerials [2];
    /* 4680 */ bool m_suspended [2];
    /* 4682 */ bool m_handshakeDone [4];
    /* 4686 */ SCritSect m_killLock;
    /* 46AE */ uint8_t unk_46AE[2];
    /* 46B0 */ TSGrowableArray<WowConnection*> m_killedConnections;
    /* 46C0 */ Crypt::RSA m_rsa;

    static bool ClientConnectionAuthChallengeHandler(ClientConnectionAuthChallenge* msg);
    static bool ClientConnectionDropNewConnectionHandler(ClientConnectionDropNewConnection* msg);
    static bool ClientConnectionPongHandler(ClientConnectionPong* msg);
    static bool ClientConnectionResetCompressionContextHandler(ClientConnectionResetCompressionContext* msg);
    static bool ClientConnectionResumeCommsHandler(ClientConnectionResumeComms* msg);
    static bool ClientConnectionSuspendCommsHandler(ClientConnectionSuspendComms* msg);
    static int32_t GetMessageTarget(CDataStore* msg, CONNECTION_ID connection);
    static void SendDisconnectReason(WowConnection* connection, DISCONNECT_REASON reason);
    static void SendOnConnection(JamClientMessage* msg, WowConnection* connection);

    ~NetClient();
    void WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) override;
    void WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) override;
    void WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) override;
    virtual int32_t Initialize();
    virtual void Destroy();
    virtual int32_t DelayedDelete();
    virtual int32_t HandleData(uint32_t timeReceived, void* data, int32_t size);
    virtual void HandleAuthChallenge(AuthenticationChallenge* challenge) = 0;
    virtual int32_t HandleConnect();
    virtual int32_t HandleDisconnect();
    virtual int32_t HandleCantConnect();
    virtual void ValidateMessageId(uint32_t msgId);

    NetClient();
    void CleanupConnections();
    void ConnectInternal(const SockAddr& address, uint16_t port);
    bool DecompressAndAddToQueue(WowConnection* connection, uint16_t msgId, const void* data, int32_t size);
    void DestroyInflateStream(int32_t index);
    void Disconnect(DISCONNECT_REASON reason);
    void DropExtraConnections(DISCONNECT_REASON reason);
    void EnableEncryption(WowConnection* connection, const uint8_t* seed, uint8_t seedLength);
    int32_t GetLocalAddress(int32_t connection);
    void GetNetBandwidth(float& bytesIn, float& bytesOut);
    void GetNetLatency(CONNECTION_ID connection, uint32_t& latency);
    void GetNetStats(CONNECTION_ID connection, float& bytesIn, float& bytesOut, uint32_t& latency);
    void GetNetStats(uint32_t time, CONNECTION_ID connection, float& bytesIn, float& bytesOut, uint32_t& latency);
    uint8_t* GetSessionKey();
    void HandleIdle();
    void HandleMessage(WowConnection* connection, uint32_t timeStamp, CDataStore* msg);
    void HandleMultiplePackets(WowConnection* connection, uint32_t timeStamp, CDataStore* msg);
    bool IsConnectionReady(WowConnection* connection);
    bool JAMClientConnectionDispatch(WowConnection* connection, uint32_t timeStamp, uint16_t msgId, CDataStore* msg);
    bool JAMClientDispatch(WowConnection* connection, uint32_t timeStamp, uint16_t msgId, CDataStore* msg);
    void KillConnection(WowConnection* connection);
    void Ping(CONNECTION_ID connection);
    void PollEventQueue();
    int32_t ProcessMessage(uint32_t timeReceived, CDataStore* msg, int32_t a4);
    void ResetInflateStream(int32_t index);
    void ResetHandler(NETMESSAGE msgId);
    void Send(CDataStore* msg, CONNECTION_ID connection);
    void Send(JamClientMessage* msg, CONNECTION_ID connection);
    void SendQueuedMessages(NetClient::CONNECTIONS connection);
    void SetLoginData(LoginData* loginData);
    void SetMessageHandler(NETMESSAGE msgId, MESSAGE_HANDLER handler, void* param);
};

struct NETCLIENTNODE : public TSLinkedNode<NETCLIENTNODE> {
    /* 0008 */ NetClient* m_client;
};

#include "WoW/Source/Net/NetClient/NetClient.inl"

#endif
