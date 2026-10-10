#include "NetClient.h"
#include <cstdlib>
#include <cstring>
#include <new>
#if defined(_MSC_VER)
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SHA1.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "ThirdParty/zlib/zlib.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/JamClientAutoCode/JamClientConnection.h"
#include "WoW/Source/JamClientAutoCode/JamClientLegacy.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "WoW/Source/JamClientAutoCode/JamClientMove.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/JamClientAutoCode/JamPlayerMove.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"

// STATIC: 0x00C9A888
static TSList<NETCLIENTNODE> s_clientList;

// STATIC: 0x00D73290
static HPROPCONTEXT s_propContext;

// STATIC: 0x00D73294
static uint32_t s_stats[6];

// STATIC: 0x00D732AC
static uint32_t s_initializeTime;

// STATIC: 0x00D732B0
static uint32_t s_idleStats[4];

// STATIC: 0x00D732C0
uint32_t NetClient::s_clientCount;

// STATIC: 0x00B987A0
static const uint8_t s_connectToModulus[256] = {
    0x91, 0xD5, 0x9B, 0xB7, 0xD4, 0xE1, 0x83, 0xA5, 0x22, 0x2B, 0x5F, 0x38, 0xF4, 0xB8, 0x86, 0xFF,
    0x32, 0x84, 0x38, 0x2D, 0x99, 0x38, 0x8F, 0xBA, 0xF3, 0xC9, 0x22, 0x5D, 0x51, 0x73, 0x1E, 0x28,
    0x87, 0x24, 0x8F, 0xB5, 0xC9, 0xB0, 0x7C, 0x95, 0xD0, 0x6B, 0x5B, 0xF4, 0x94, 0xC5, 0x94, 0x9D,
    0xFA, 0x6F, 0x47, 0x3A, 0xA3, 0x86, 0xC0, 0xA8, 0x37, 0xF3, 0x9B, 0xEF, 0x2F, 0xC1, 0xFB, 0xB3,
    0xF4, 0x1C, 0x2B, 0x0E, 0xD3, 0x6D, 0x88, 0xBB, 0x02, 0xE0, 0x4E, 0x63, 0xFA, 0x76, 0xE3, 0x43,
    0xF9, 0x01, 0xFD, 0x23, 0x5E, 0x6A, 0x0B, 0x14, 0xEC, 0x5E, 0x91, 0x34, 0x0D, 0x0B, 0x4F, 0xA3,
    0x5A, 0x46, 0xC5, 0x5E, 0xDC, 0xB5, 0xCD, 0xC1, 0x47, 0x6B, 0x59, 0xCA, 0xFA, 0xA9, 0xBE, 0x24,
    0x9F, 0xF5, 0x05, 0x6B, 0xBB, 0x67, 0x8B, 0xB7, 0xE4, 0x3A, 0x43, 0x00, 0x5C, 0x1C, 0xB7, 0xCA,
    0x98, 0x90, 0x79, 0x77, 0x6D, 0x05, 0x4F, 0x83, 0xCC, 0xAC, 0x06, 0x2E, 0x50, 0x11, 0x87, 0x23,
    0xD8, 0xA6, 0xF7, 0x6F, 0x7A, 0x59, 0x87, 0xA6, 0xDE, 0x5D, 0xD8, 0xEC, 0x44, 0xBE, 0x45, 0x31,
    0x7F, 0x8A, 0xF0, 0x58, 0x89, 0x53, 0x74, 0xDF, 0xCC, 0xAD, 0x01, 0x24, 0xD8, 0x19, 0x65, 0x1C,
    0x25, 0xD3, 0xE1, 0x6B, 0x8B, 0xDA, 0xFE, 0x1D, 0xA4, 0x2C, 0x8B, 0x25, 0xED, 0x7C, 0xFF, 0x6A,
    0xE0, 0x63, 0xD0, 0x52, 0x20, 0x7E, 0x62, 0x49, 0xD2, 0xB3, 0x6B, 0xCC, 0x91, 0x69, 0xA5, 0x08,
    0x8B, 0x69, 0x65, 0xFF, 0xB9, 0xC9, 0x17, 0x02, 0x5D, 0xD8, 0x8E, 0x1A, 0x63, 0xD9, 0x2A, 0x7F,
    0xDB, 0xE3, 0xF8, 0x76, 0x6D, 0xEA, 0x0E, 0x36, 0x98, 0x78, 0x19, 0xC5, 0x87, 0xBA, 0x6C, 0x20,
    0xB6, 0x08, 0x44, 0x04, 0x4C, 0xA8, 0xD5, 0xB6, 0x9D, 0x4D, 0x00, 0x20, 0x40, 0x00, 0x90, 0x04,
};

// STATIC: 0x00B98798
static const uint8_t s_connectToExponent[4] = {
    0x01, 0x00, 0x01, 0x00,
};

// PROVEN: 8f67d674 IDENTICAL/NONE
// OFFSET: 0x00487370/0x1400A8750
static void InitializePropContext() {
    if (PropGetSelectedContext() != s_propContext) {
        PropSelectContext(s_propContext);
    }
}

// PROVEN: 4ed6cb94 IDENTICAL/NONE
// OFFSET: 0x00487860
static voidpf ZAlloc(voidpf opaque, uInt items, uInt size) {
    return SMemAlloc(items * size, __FILE__, __LINE__, 0);
}

// PROVEN: eebd3447 IDENTICAL/NONE
// OFFSET: 0x00487880
static void ZFree(voidpf opaque, voidpf address) {
    SMemFree(address, __FILE__, __LINE__, 0);
}

// OFFSET: 0x004877A0
static bool __stdcall ComputeProofOfWork(const SHA1_CONTEXT* context, uint32_t difficulty, uint64_t* proof) {
    uint64_t nonce = 0;

    while (true) {
        SHA1_CONTEXT attempt;
        memcpy(&attempt, context, sizeof(attempt));
        SHA1_Update(&attempt, reinterpret_cast<const uint8_t*>(&nonce), sizeof(nonce));

        uint8_t digest[20];
        SHA1_Final(digest, &attempt);

        uint8_t bits = 0;
        int32_t i = 0;

        while (!digest[i]) {
            i++;
            bits += 8;

            if (i >= 20) {
                break;
            }
        }

        if (i < 20) {
            for (uint8_t value = digest[i]; !(value & 1); value >>= 1) {
                bits++;
            }
        }

        if (bits >= difficulty) {
            break;
        }

        if (++nonce == 0) {
            return false;
        }
    }

    *proof = nonce;

    return true;
}

// OFFSET: 0x00487AB0
static bool IsFromInstanceRealm(uint32_t msgId) {
    uint32_t index = (msgId & 3) | (((msgId & 0x30) | (((msgId & 0xF00) | ((msgId >> 1) & 0x3000)) >> 2)) >> 2);

    return (g_jam_ClientLegacy_has_flag_FromInstanceRealm[index >> 3] >> (index & 7)) & 1;
}

// PROVEN: debe5c24 IDENTICAL/NONE
// OFFSET: 0x0048A190/0x1400AC650
NETEVENTQUEUE::NETEVENTQUEUE(NetClient* client) {
    this->m_client = client;
}

// INLINED
NETEVENTQUEUENODE::~NETEVENTQUEUENODE() {
    if (this->m_data) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x0048A240/0x1400AC770
void NETEVENTQUEUE::Poll() {
    this->m_critSect.Enter();

    NetClient* client = this->m_client;
    client->m_refCount.Increment();

    TSList<NETEVENTQUEUENODE> events;
    events.Combine(&this->m_eventQueue, 2, NULL);

    this->m_critSect.Leave();

    bool destroyed = false;

    for (NETEVENTQUEUENODE* node = events.Head(); node; node = events.Next(node)) {
        if (!client->m_deleteMe) {
            switch (node->m_eventId) {
                case EVENT_ID_NET_DATA:
                    client->HandleData(node->m_timeReceived, node->m_data, node->m_dataSize);
                    break;

                case EVENT_ID_NET_CONNECT:
                    client->HandleConnect();
                    break;

                case EVENT_ID_NET_DISCONNECT:
                    client->HandleDisconnect();
                    break;

                case EVENT_ID_NET_CANTCONNECT:
                    client->HandleCantConnect();
                    break;

                case EVENT_ID_NET_DESTROY:
                    client->m_deleteMe = true;
                    destroyed = true;
                    break;

                case EVENT_ID_NET_AUTH_CHALLENGE:
                    client->HandleAuthChallenge(static_cast<AuthenticationChallenge*>(node->m_data));
                    break;

                default:
                    break;
            }
        }

        if (client->m_refCount.Decrement() == 0 && client->m_deleteMe) {
            delete client;
        }
    }

    if (!destroyed) {
        client->HandleIdle();
    }

    if (client->m_refCount.Decrement() == 0 && client->m_deleteMe) {
        delete client;
    }

    events.Clear();
}

// OFFSET: 0x0048A460/0x1400ACAE0
void NETEVENTQUEUE::Clear() {
    this->m_critSect.Enter();
    this->m_eventQueue.Clear();
    this->m_critSect.Leave();
}

// OFFSET: 0x0048A480/0x1400ACB20
void NETEVENTQUEUE::AddEvent(EVENTID eventId, void* connection, NetClient* client, const void* data, uint32_t size) {
    this->m_critSect.Enter();

    NETEVENTQUEUENODE* node = this->m_eventQueue.NewNode(2, 0, 8);
    node->m_eventId = eventId;

    if (size) {
        node->m_data = SMemAlloc(size, "..\\NetInternal.cpp", 96, 0);
        memcpy(node->m_data, data, size);
        node->m_dataSize = size;
    } else {
        node->m_data = NULL;
        node->m_dataSize = 0;
    }

    node->m_timeReceived = OsGetAsyncTimeMsPrecise();
    client->m_refCount.Increment();

    this->m_critSect.Leave();
}

// OFFSET: 0x0048A560/0x1400ACC60
NETEVENTQUEUE::~NETEVENTQUEUE() {
    this->m_critSect.Enter();
    this->m_eventQueue.Clear();
    this->m_critSect.Leave();
}

// OFFSET: 0x00489ED0/0x1400AC350
NetClient::NetClient() {
    this->m_netState = NS_UNINITIALIZED;
    this->m_netEventQueue = NULL;

    for (int32_t i = 0; i < 4; i++) {
        this->m_connections[i] = NULL;
        this->m_inflateStreams[i] = NULL;
    }

    this->m_suspended[0] = false;
    this->m_suspended[1] = true;
    this->m_refCount.m_value = 0;
    this->m_deleteMe = false;

    for (int32_t i = 0; i < 4; i++) {
        this->m_handshakeDone[i] = false;
    }

    for (int32_t i = 0; i < 2; i++) {
        NETCONNECTIONSTATS& stats = this->m_connectionStatistics[i];
        memset(stats.m_latency, 0, sizeof(stats.m_latency));
        stats.m_latencyEnd = 0;
        stats.m_latencyStart = 0;
        stats.m_pingSent = 0;
        stats.m_pingSequence = 0;
    }

    this->m_bytesSent = 0;
    this->m_bytesReceived = 0;
    this->m_connectedTimestamp = 0;
    this->m_objMgr = NULL;

    this->m_rsa.Prepare(s_connectToModulus, sizeof(s_connectToModulus), s_connectToExponent, sizeof(s_connectToExponent));

    ClientConnectionAuthChallenge::s_handler = NetClient::ClientConnectionAuthChallengeHandler;
    ClientConnectionSuspendComms::s_handler = NetClient::ClientConnectionSuspendCommsHandler;
    ClientConnectionResumeComms::s_handler = NetClient::ClientConnectionResumeCommsHandler;
    ClientConnectionDropNewConnection::s_handler = NetClient::ClientConnectionDropNewConnectionHandler;
    ClientConnectionPong::s_handler = NetClient::ClientConnectionPongHandler;
    ClientConnectionResetCompressionContext::s_handler = NetClient::ClientConnectionResetCompressionContextHandler;

    NETCLIENTNODE* node = s_clientList.NewNode(2, 0, 8);
    node->m_client = this;
}

// OFFSET: [0x00489C90/0x1400ABFB0, 0x0048A110/0x1400AC600]
NetClient::~NetClient() {
    this->Destroy();

    for (NETCLIENTNODE* node = s_clientList.Head(); node; node = s_clientList.Next(node)) {
        if (node->m_client == this) {
            s_clientList.DeleteNode(node);
            break;
        }
    }
}

// OFFSET: 0x00487D50/0x1400A99B0
int32_t NetClient::Initialize() {
    if (!NetClient::s_clientCount) {
        s_propContext = PropGetSelectedContext();

        if (!WowConnection::InitOsNet(NULL, InitializePropContext, 1, false)) {
            return 0;
        }
    }

    NetClient::s_clientCount++;

    this->m_netEventQueue = new (SMemAlloc(sizeof(NETEVENTQUEUE), __FILE__, __LINE__, 0)) NETEVENTQUEUE(this);
    memset(this->m_handlers, 0, sizeof(this->m_handlers));
    memset(this->m_handlerParams, 0, sizeof(this->m_handlerParams));
    this->m_connections[CONNECTIONS_USER] = new (SMemAlloc(sizeof(WowConnection), __FILE__, __LINE__, 0)) WowConnection(this, NULL);
    this->ResetInflateStream(CONNECTIONS_USER);
    this->m_netState = NS_INITIALIZED;
    s_initializeTime = OsGetAsyncTimeMs();

    return 1;
}

// OFFSET: 0x00487BF0/0x1400A9840
void NetClient::ResetInflateStream(int32_t index) {
    z_stream* stream = this->m_inflateStreams[index];

    if (stream) {
        this->m_inflateStreams[index] = NULL;
        inflateEnd(stream);
        delete stream;
    }

    stream = static_cast<z_stream*>(SMemAlloc(sizeof(z_stream), __FILE__, __LINE__, 0));
    stream->zalloc = ZAlloc;
    stream->zfree = ZFree;
    stream->opaque = NULL;
    stream->avail_in = 0;
    stream->next_in = NULL;
    inflateInit(stream);
    this->m_inflateStreams[index] = stream;
}

// OFFSET: 0x004878A0
void NetClient::DestroyInflateStream(int32_t index) {
    z_stream* stream = this->m_inflateStreams[index];
    this->m_inflateStreams[index] = NULL;
    inflateEnd(stream);

    if (stream) {
        delete stream;
    }
}

// PROVEN: 79d93fe8 IDENTICAL/NONE
// OFFSET: 0x00487390
void NetClient::SetLoginData(LoginData* loginData) {
    this->m_critSect.Enter();
    memcpy(&this->m_loginData, loginData, sizeof(this->m_loginData));
    this->m_critSect.Leave();
}

// PROVEN: a91c0b00 IDENTICAL/NONE
// OFFSET: 0x004873C0/0x1400A87C0
uint8_t* NetClient::GetSessionKey() {
    return this->m_loginData.m_sessionKey;
}

// OFFSET: 0x004873D0/0x1400A87D0
int32_t NetClient::ProcessMessage(uint32_t timeReceived, CDataStore* msg, int32_t a4) {
    s_stats[4]++;

    uint16_t msgId;
    msg->Get(msgId);

    if ((msgId & 0x92E8) == 0x10A0) {
        return this->JAMClientDispatch(NULL, timeReceived, msgId, msg);
    }

    this->ValidateMessageId(msgId);

    uint32_t index = (msgId & 3) | (((msgId & 0x30) | (((msgId & 0xF00) | ((msgId >> 1) & 0x3000)) >> 2)) >> 2);
    MESSAGE_HANDLER handler;

    if ((msgId & 0x90CC) == 4 && (handler = this->m_handlers[index]) != NULL) {
        uint32_t key = (msgId | (msgId << 16)) ^ 0x62A3A31D;
        handler = reinterpret_cast<MESSAGE_HANDLER>(reinterpret_cast<uintptr_t>(handler) - key);

        return handler(this->m_handlerParams[index], static_cast<NETMESSAGE>(msgId), timeReceived, msg);
    }

    msg->Reset();

    return 0;
}

void NetClient::ValidateMessageId(uint32_t msgId) {
}

// OFFSET: 0x004874A0/0x1400A88D0
void NetClient::WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_critSect.Enter();

    uint32_t index = 0;

    while (connection != this->m_connections[index]) {
        if (++index >= 4) {
            this->m_critSect.Leave();
            return;
        }
    }

    this->m_handshakeDone[index] = false;

    if (index) {
        ConsolePrintf("Got new connection %d", index);
        this->m_critSect.Leave();
        return;
    }

    this->m_connectedTimestamp = timeStamp;
    this->m_bytesReceived = 0;
    this->m_bytesSent = 0;

    for (int32_t i = 0; i < 2; i++) {
        NETCONNECTIONSTATS& stats = this->m_connectionStatistics[i];
        stats.m_latencyStart = 0;
        stats.m_latencyEnd = 0;
        stats.m_pingSent = OsGetAsyncTimeClocks();
    }

    this->m_netEventQueue->AddEvent(EVENT_ID_NET_CONNECT, connection, this, NULL, 0);

    this->m_critSect.Leave();
}

// OFFSET: 0x00487580
void NetClient::GetNetStats(uint32_t time, CONNECTION_ID connection, float& bytesIn, float& bytesOut, uint32_t& latency) {
    this->m_critSect.Enter();

    double elapsed = static_cast<uint32_t>(time - this->m_connectedTimestamp) * 0.001;
    bytesIn = this->m_bytesReceived * 0.0009765625 / elapsed;

    NETCONNECTIONSTATS& stats = this->m_connectionStatistics[connection];
    uint32_t count = 0;
    bytesOut = 0.0009765625 * this->m_bytesSent / elapsed;

    uint32_t index = stats.m_latencyStart;
    uint32_t end = stats.m_latencyEnd;
    uint32_t total = 0;

    while (index != end) {
        if (index >= 16) {
            index = 0;

            if (!end) {
                break;
            }
        }

        total += stats.m_latency[index++];
        count++;
    }

    latency = count ? total / count : 0;

    this->m_critSect.Leave();
}

// OFFSET: 0x00487660
void NetClient::GetNetStats(CONNECTION_ID connection, float& bytesIn, float& bytesOut, uint32_t& latency) {
    this->GetNetStats(OsGetAsyncTimeMsPrecise(), connection, bytesIn, bytesOut, latency);
}

// OFFSET: 0x00487690/0x1400A8B40
void NetClient::GetNetBandwidth(float& bytesIn, float& bytesOut) {
    uint32_t time = OsGetAsyncTimeMsPrecise();

    this->m_critSect.Enter();

    double elapsed = static_cast<uint32_t>(time - this->m_connectedTimestamp) * 0.001;
    bytesIn = this->m_bytesReceived * 0.0009765625 / elapsed;
    bytesOut = 0.0009765625 * this->m_bytesSent / elapsed;

    this->m_critSect.Leave();
}

// OFFSET: 0x00487720/0x1400A8C00
void NetClient::GetNetLatency(CONNECTION_ID connection, uint32_t& latency) {
    this->m_critSect.Enter();

    NETCONNECTIONSTATS& stats = this->m_connectionStatistics[connection];
    uint32_t index = stats.m_latencyStart;
    uint32_t end = stats.m_latencyEnd;
    uint32_t total = 0;
    uint32_t count = 0;

    while (index != end) {
        if (index >= 16) {
            index = 0;

            if (!end) {
                break;
            }
        }

        total += stats.m_latency[index++];
        count++;
    }

    latency = count ? total / count : 0;

    this->m_critSect.Leave();
}

// PROVEN: 3007d186 IDENTICAL/NONE
// OFFSET: 0x00487B00/0x1400A9290
int32_t NetClient::HandleConnect() {
    if (this->m_objMgr) {
        ClntObjMgrPush(this->m_objMgr);
    }

    this->m_netState = NS_CONNECTED;
    ClntObjMgrPop();

    return 1;
}

// OFFSET: 0x00487B30/0x1400A92D0
int32_t NetClient::HandleDisconnect() {
    if (this->m_objMgr) {
        ClntObjMgrPush(this->m_objMgr);
    }

    this->m_netState = NS_INITIALIZED;
    ConsolePrintf("NetClient::HandleDisconnect()");
    CGlueMgr::NetDisconnectHandler(this, NULL);
    ClntObjMgrPop();

    return 1;
}

// PROVEN: 053c8f1e IDENTICAL/NONE
// OFFSET: 0x00487B80/0x1400A9320
int32_t NetClient::HandleCantConnect() {
    if (this->m_objMgr) {
        ClntObjMgrPush(this->m_objMgr);
    }

    this->m_netState = NS_INITIALIZED;
    ClntObjMgrPop();

    return 1;
}

// PROVEN: 4bda54ba IDENTICAL/NONE
// OFFSET: 0x00487BB0
void NetClient::EnableEncryption(WowConnection* connection, const uint8_t* seed, uint8_t seedLength) {
    connection->SetEncryptionKey(this->m_loginData.m_sessionKey, sizeof(this->m_loginData.m_sessionKey), 1, seed, seedLength);
    connection->m_sendEncryptBytes = 4;
    connection->m_recvEncryptBytes = 2;
    connection->SetEncryptionType(WC_ENCRYPT_ARC4);
}

// OFFSET: 0x00487E30
void NetClient::SendOnConnection(JamClientMessage* msg, WowConnection* connection) {
    CDataStore data;
    msg->Put(&data);
    data.Finalize();
    connection->Send(&data, 0);
}

// OFFSET: 0x00487EA0
void NetClient::HandleMultiplePackets(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) {
    while (msg->m_read != msg->m_size) {
        uint16_t msgId = 0;
        msg->Get(msgId);

        uint32_t position = msg->m_read;
        uint16_t size = 0;
        msg->Get(size);
        msg->m_read = position;

        void* data;
        msg->GetDataInSitu(data, size + 2);

        if (!data) {
            break;
        }

        *static_cast<uint16_t*>(data) = msgId;

        CDataStore packet;
        packet.InitExternal(static_cast<uint8_t*>(data), size + 2);
        this->HandleMessage(connection, timeStamp, &packet);
    }
}

// OFFSET: 0x00487FA0/0x1400A91B0
int32_t NetClient::HandleData(uint32_t timeReceived, void* data, int32_t size) {
    if (this->m_objMgr) {
        ClntObjMgrPush(this->m_objMgr);
    }

    s_stats[3] += size + 2;

    if (this->m_netState == NS_CONNECTED) {
        CDataStore msg(static_cast<uint8_t*>(data), size);
        this->ProcessMessage(timeReceived, &msg, 0);
    }

    ClntObjMgrPop();

    return 1;
}

// OFFSET: 0x00488050
void NetClient::Ping(CONNECTION_ID connection) {
    this->m_critSect.Enter();

    WowConnection* target = this->m_connections[connection];

    if (!target || !target->m_encryptionType) {
        this->m_critSect.Leave();
        return;
    }

    NETCONNECTIONSTATS& stats = this->m_connectionStatistics[connection];
    stats.m_pingSent = OsGetAsyncTimeClocks();

    UserRouterClientPing ping;
    ping.m_present |= 0x2;
    ping.m_serial = ++stats.m_pingSequence;

    if (this->m_netState == NS_CONNECTED) {
        ping.m_latency = stats.m_latencyEnd ? stats.m_latency[stats.m_latencyEnd - 1] : stats.m_latency[0];
    } else {
        ping.m_latency = 0;
    }

    ping.m_present |= 0x1;
    NetClient::SendOnConnection(&ping, this->m_connections[connection]);

    this->m_critSect.Leave();
}

// OFFSET: 0x00488120
bool NetClient::ClientConnectionResetCompressionContextHandler(ClientConnectionResetCompressionContext* msg) {
    NetClient* client = msg->m_netClient;
    uint32_t index = 0;

    while (msg->m_connection != client->m_connections[index]) {
        if (++index >= 4) {
            index = 4;
            break;
        }
    }

    client->DestroyInflateStream(index);
    client->ResetInflateStream(index);

    return true;
}

// OFFSET: 0x00488190/0x1400A9560
int32_t NetClient::GetLocalAddress(int32_t connection) {
    int32_t type = SOCKADDR_NONE;

    if (connection >= 2) {
        return SOCKADDR_NONE;
    }

    SockAddr address;

    this->m_critSect.Enter();

    if (this->m_connections[connection]) {
        uint8_t name[32];
        this->m_connections[connection]->GetLocalAddr(reinterpret_cast<sockaddr*>(name));
        address = SockAddrFromSockaddr(reinterpret_cast<sockaddr*>(name));

        if (address.m_type == SOCKADDR_IPV4) {
            type = SOCKADDR_IPV4;
        } else if (address.m_type == SOCKADDR_IPV6) {
            type = SOCKADDR_IPV6;
        }
    }

    this->m_critSect.Leave();

    return type;
}

// OFFSET: 0x00488290
void NetClient::HandleIdle() {
    s_idleStats[0] = s_stats[0];
    s_idleStats[1] = s_stats[3];
    s_idleStats[2] = s_stats[1];
    s_idleStats[3] = s_stats[4];

    for (uint32_t i = 0; i < 2; i++) {
        uint64_t elapsed = OsGetAsyncTimeClocks() - this->m_connectionStatistics[i].m_pingSent;

        if (static_cast<uint32_t>(elapsed / (OsGetAsyncClocksPerSecond() / 1000)) >= 30000) {
            this->Ping(static_cast<CONNECTION_ID>(i));
        }
    }
}

// OFFSET: 0x00488330
void NetClient::SendDisconnectReason(WowConnection* connection, DISCONNECT_REASON reason) {
    UserRouterClientLogDisconnect msg(reason);
    NetClient::SendOnConnection(&msg, connection);
}

// OFFSET: 0x00488370
bool NetClient::IsConnectionReady(WowConnection* connection) {
    uint32_t index = 0;

    while (connection != this->m_connections[index]) {
        if (++index >= 4) {
            return false;
        }
    }

    if (!(index & 2) && !this->m_suspended[index & 1]) {
        return true;
    }

    NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_PROTOCOL_VIOLATION);
    connection->Disconnect();

    return false;
}

// OFFSET: 0x00488880
bool NetClient::ClientConnectionSuspendCommsHandler(ClientConnectionSuspendComms* msg) {
    WowConnection* connection = msg->m_connection;
    NetClient* client = msg->m_netClient;
    uint32_t index = 0;

    while (connection != client->m_connections[index]) {
        if (++index >= 4) {
            return true;
        }
    }

    bool* suspended;

    if ((index & 2) || *(suspended = &client->m_suspended[index & 1])) {
        NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_PROTOCOL_VIOLATION);
        connection->Disconnect();
        return true;
    }

    UserRouterClientSuspendCommsAck ack;
    ack.m_present |= 0x1;
    ack.m_serial = msg->m_serial;
    NetClient::SendOnConnection(&ack, connection);
    *suspended = true;

    return true;
}

// OFFSET: 0x00488940
bool NetClient::ClientConnectionAuthChallengeHandler(ClientConnectionAuthChallenge* msg) {
    WowConnection* connection = msg->m_connection;
    NetClient* client = msg->m_netClient;

    AuthenticationChallenge* challenge = static_cast<AuthenticationChallenge*>(SMemAlloc(sizeof(AuthenticationChallenge), __FILE__, __LINE__, 0));
    challenge->m_challenge = msg->m_challenge;

    SHA1_CONTEXT context;
    SHA1_Init(&context);
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>(client->m_loginData.m_account), strlen(client->m_loginData.m_account));

    for (int32_t i = 0; i < 8; i++) {
        uint32_t value = msg->m_dosChallenge[i];
        SHA1_Update(&context, reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }

    if (!ComputeProofOfWork(&context, msg->m_dosZeroBits, &challenge->m_proof)) {
        NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_AUTH_FAILED);
        connection->Disconnect();
        delete challenge;
        return true;
    }

    uint32_t index = 0;

    while (connection != client->m_connections[index]) {
        if (++index >= 4) {
            delete challenge;
            return true;
        }
    }

    if (!index) {
        client->m_netEventQueue->AddEvent(EVENT_ID_NET_AUTH_CHALLENGE, connection, client, challenge, sizeof(AuthenticationChallenge));
    } else if (index & 2) {
        UserRouterClientAuthContinuedSession session;
        session.m_key = client->m_connectKeys[index & 1];
        session.m_dosResponse = challenge->m_proof;

        SHA1_Init(&context);
        SHA1_Update(&context, reinterpret_cast<const uint8_t*>(client->m_loginData.m_account), strlen(client->m_loginData.m_account));
        SHA1_Update(&context, client->m_loginData.m_sessionKey, sizeof(client->m_loginData.m_sessionKey));
        SHA1_Update(&context, reinterpret_cast<const uint8_t*>(challenge), sizeof(challenge->m_challenge));
        SHA1_Final(session.m_digest, &context);

        NetClient::SendOnConnection(&session, connection);

        connection->SetEncryptionKey(client->m_loginData.m_sessionKey, sizeof(client->m_loginData.m_sessionKey), 1, reinterpret_cast<const uint8_t*>(msg->m_dosChallenge), sizeof(msg->m_dosChallenge));
        connection->m_sendEncryptBytes = 4;
        connection->m_recvEncryptBytes = 2;
        connection->SetEncryptionType(WC_ENCRYPT_ARC4);

        if (!g_disableServerNagleVar->m_intValue) {
            UserRouterClientEnableNagle nagle;
            NetClient::SendOnConnection(&nagle, connection);
        }
    } else {
        NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_AUTH_UNEXPECTED);
        connection->Disconnect();
    }

    delete challenge;

    return true;
}

// OFFSET: 0x00488BD0
void NetClient::KillConnection(WowConnection* connection) {
    this->m_killLock.Enter();
    this->m_killedConnections.New(connection);
    this->m_killLock.Leave();
}

// OFFSET: 0x00488C40/0x1400AB020
void NetClient::DropExtraConnections(DISCONNECT_REASON reason) {
    for (int32_t i = 1; i < 4; i++) {
        WowConnection* connection = this->m_connections[i];

        if (connection) {
            NetClient::SendDisconnectReason(connection, reason);
            this->KillConnection(this->m_connections[i]);
            this->DestroyInflateStream(i);
            this->m_connections[i] = NULL;
        }
    }
}

// OFFSET: 0x00488CD0/0x1400AB0F0
void NetClient::Disconnect(DISCONNECT_REASON reason) {
    this->m_critSect.Enter();

    this->DropExtraConnections(reason);

    WowConnection* connection = this->m_connections[CONNECTIONS_USER];

    if (this->m_netState == NS_CONNECTED) {
        this->m_netState = NS_DISCONNECTING;
        NetClient::SendDisconnectReason(connection, reason);
        this->m_connections[CONNECTIONS_USER]->Disconnect();
        this->m_critSect.Leave();
        return;
    }

    NetClient::SendDisconnectReason(connection, reason);
    this->KillConnection(this->m_connections[CONNECTIONS_USER]);
    this->DestroyInflateStream(CONNECTIONS_USER);
    this->m_netEventQueue->Clear();
    this->m_connections[CONNECTIONS_USER] = new (SMemAlloc(sizeof(WowConnection), __FILE__, __LINE__, 0)) WowConnection(this, NULL);
    this->ResetInflateStream(CONNECTIONS_USER);
    this->m_netState = NS_INITIALIZED;

    this->m_critSect.Leave();
}

// OFFSET: 0x00488E00/0x1400AA9A0
void NetClient::HandleMessage(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) {
    uint32_t size = msg->m_size;
    void* data;
    msg->GetDataInSitu(data, size);

    uint32_t index = 0;

    while (connection != this->m_connections[index]) {
        if (++index >= 4) {
            NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_PROTOCOL_VIOLATION);
            connection->Disconnect();
            return;
        }
    }

    if (!this->m_handshakeDone[index]) {
        this->m_handshakeDone[index] = true;

        if (!strncmp(static_cast<const char*>(data), "WORLD OF WARCRAFT CONNECTION - SERVER TO CLIENT", 47)) {
            CDataStore reply;
            reply.Finalize();
            reply.PutArray(reinterpret_cast<const uint8_t*>("WORLD OF WARCRAFT CONNECTION - CLIENT TO SERVER"), 48);
            connection->Send(&reply, 0);
        } else {
            NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_PROTOCOL_VIOLATION);
            connection->Disconnect();
        }

        return;
    }

    msg->m_read = 0;

    uint16_t msgId;
    msg->Get(msgId);

    if (msgId == 0xD40) {
        this->HandleMultiplePackets(connection, timeStamp, msg);
        return;
    }

    if ((msgId & 0xB3FD) == 0x140) {
        this->JAMClientConnectionDispatch(connection, timeStamp, msgId, msg);
        return;
    }

    if (!this->IsConnectionReady(connection)) {
        return;
    }

    msg->m_read = msg->m_size;

    if ((msgId & 0x90CC) == 4) {
        if (IsFromInstanceRealm(msgId)) {
            uint32_t source = 0;

            while (connection != this->m_connections[source]) {
                if (++source >= 4) {
                    return;
                }
            }

            if (source != CONNECTIONS_INSTANCE) {
                return;
            }
        }
    } else if ((msgId & 0x92E8) != 0x10A0) {
        if (msgId & 0x8000) {
            this->DecompressAndAddToQueue(connection, msgId & 0x7FFF, data, size);
        }

        return;
    }

    this->m_netEventQueue->AddEvent(EVENT_ID_NET_DATA, connection, this, data, size);
}

// OFFSET: 0x00489030/0x1400AB2C0
void NetClient::WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    uint32_t time = OsGetAsyncTimeMsPrecise();

    this->m_critSect.Enter();

    float bytesIn;
    float bytesOut;
    uint32_t latency;
    this->GetNetStats(time, CONNECTION_USER, bytesIn, bytesOut, latency);

    this->m_critSect.Leave();

    this->m_critSect.Enter();

    uint32_t index = 0;

    while (connection != this->m_connections[index]) {
        if (++index >= 4) {
            index = 4;
            break;
        }
    }

    if (this->m_netEventQueue && !index) {
        this->DestroyInflateStream(CONNECTIONS_USER);
        this->ResetInflateStream(CONNECTIONS_USER);
        this->m_netEventQueue->AddEvent(EVENT_ID_NET_DISCONNECT, connection, this, NULL, 0);
    } else if (index == 4) {
        this->m_critSect.Leave();
        return;
    }

    this->m_suspended[index] = index != 0;

    if (index) {
        this->KillConnection(connection);
        this->m_connections[index] = NULL;
    }

    this->m_critSect.Leave();
}

// OFFSET: 0x00489130
bool NetClient::ClientConnectionPongHandler(ClientConnectionPong* msg) {
    NetClient* client = msg->m_netClient;

    if (!client->IsConnectionReady(msg->m_connection)) {
        return true;
    }

    uint32_t index = 0;

    while (msg->m_connection != client->m_connections[index]) {
        if (++index >= 4) {
            return true;
        }
    }

    NETCONNECTIONSTATS& stats = client->m_connectionStatistics[index];

    if (msg->m_serial != stats.m_pingSequence) {
        ConsolePrintf("Received pong with old sequence");
        return true;
    }

    uint64_t elapsed = OsGetAsyncTimeClocks() - stats.m_pingSent;
    stats.m_latency[stats.m_latencyEnd++] = static_cast<uint32_t>(elapsed / (OsGetAsyncClocksPerSecond() / 1000));

    if (stats.m_latencyEnd >= 16) {
        stats.m_latencyEnd = 0;
    }

    if (stats.m_latencyEnd == stats.m_latencyStart) {
        if (++stats.m_latencyStart >= 16) {
            stats.m_latencyStart = 0;
        }
    }

    return true;
}

// OFFSET: 0x00489210
bool NetClient::ClientConnectionDropNewConnectionHandler(ClientConnectionDropNewConnection* msg) {
    NetClient* client = msg->m_netClient;
    uint32_t index = 0;

    while (msg->m_connection != client->m_connections[index]) {
        if (++index >= 4) {
            return true;
        }
    }

    uint32_t pending = index | 2;
    WowConnection* connection = client->m_connections[pending];

    if (connection) {
        NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_REPLACED);
        client->KillConnection(client->m_connections[pending]);
        client->DestroyInflateStream(pending);
        client->m_connections[pending] = NULL;
    }

    return true;
}

// OFFSET: 0x004892A0/0x1400AADA0
bool NetClient::DecompressAndAddToQueue(WowConnection* connection, uint16_t msgId, const void* data, int32_t size) {
    if (size < 6) {
        return false;
    }

    uint32_t uncompressedSize = *reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(data) + 2);

    if (uncompressedSize > 0x7FFFFF) {
        return false;
    }

    uint32_t bufferSize = uncompressedSize + 2;
    void* allocated = NULL;
    uint8_t* buffer;

    if (bufferSize >= 0x2000) {
        allocated = SMemAlloc(bufferSize, __FILE__, __LINE__, 0);
        buffer = static_cast<uint8_t*>(allocated);
    } else {
        buffer = static_cast<uint8_t*>(alloca(bufferSize));
    }

    CDataStore msg;

    uint32_t index = 0;

    while (connection != this->m_connections[index]) {
        if (++index >= 4) {
            index = 4;
            break;
        }
    }

    z_stream* stream = this->m_inflateStreams[index];
    stream->next_in = const_cast<Bytef*>(static_cast<const Bytef*>(data) + 6);
    stream->avail_in = size - 6;
    stream->next_out = buffer + 2;
    stream->avail_out = uncompressedSize;

    if (inflate(stream, Z_SYNC_FLUSH) != Z_OK || uncompressedSize - stream->avail_out != uncompressedSize) {
        NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_DECOMPRESS_FAILED);
        connection->Disconnect();
        return false;
    }

    *reinterpret_cast<uint16_t*>(buffer) = msgId;
    msg.InitExternal(buffer, bufferSize);
    this->HandleMessage(connection, 0, &msg);
    SMemFree(allocated, __FILE__, __LINE__, 0);

    return true;
}

// PROVEN: c39d7ebf IDENTICAL/NONE
// OFFSET: 0x00489420/0x1400AB4F0
void NetClient::CleanupConnections() {
    TSGrowableArray<WowConnection*> connections;

    this->m_killLock.Enter();
    connections = this->m_killedConnections;
    this->m_killedConnections.Clear();
    this->m_killLock.Leave();

    for (uint32_t i = 0; i < connections.Count(); i++) {
        connections[i]->SetResponse(NULL, false);
        connections[i]->Disconnect();
        connections[i]->Release();
    }
}

// OFFSET: 0x00489560/0x1400AB6C0
int32_t NetClient::DelayedDelete() {
    if (!this->m_netState) {
        return 0;
    }

    this->Disconnect(DISCONNECT_REASON_SHUTDOWN);
    this->m_netEventQueue->AddEvent(EVENT_ID_NET_DESTROY, NULL, this, NULL, 0);

    return 1;
}

// INLINED
int32_t NetClient::GetMessageTarget(CDataStore* msg, CONNECTION_ID connection) {
    if (connection != CONNECTION_ROUTED) {
        return connection;
    }

    uint32_t position = msg->m_read;
    uint32_t msgId = 0;
    msg->Get(msgId);
    msg->m_read = position;

    if ((msgId & 0x90CC) == 4) {
        uint32_t index = (msgId & 3) | (((msgId & 0x30) | (((msgId & 0xF00) | ((msgId >> 1) & 0x3000)) >> 2)) >> 2);

        return (g_ClientLegacy_RoutingTable[index >> 3] >> (index & 7)) & 1;
    }

    return (msgId & 0xB4E1) == 0x3000;
}

// OFFSET: 0x00489590/0x1400AB720
void NetClient::Send(CDataStore* msg, CONNECTION_ID connection) {
    this->m_critSect.Enter();

    if (this->m_netState == NS_CONNECTED) {
        uint32_t size = msg->m_size - msg->m_read;

        if (size) {
            int32_t target = NetClient::GetMessageTarget(msg, connection);

            if (this->m_suspended[target & 1]) {
                NetClient::HELDMESSAGE* held = this->m_heldMessages[target & 1].NewNode(2, 0, 0);
                held->m_connection = connection;

                uint32_t bytes = msg->m_size - msg->m_read;
                void* data;
                msg->GetDataInSitu(data, bytes);
                held->m_msg.PutData(data, bytes);
                held->m_msg.Finalize();

                this->m_critSect.Leave();
                return;
            }

            WowConnection* target_connection = this->m_connections[target];
            target_connection->Send(msg, 0);
            s_stats[0] += size;
            s_stats[1]++;
            this->m_bytesSent += size;

            if (!target_connection->m_encryptionType) {
                this->EnableEncryption(target_connection, NULL, 0);
            }
        }
    }

    this->m_critSect.Leave();
}

// OFFSET: 0x00489700/0x1400AB900
void NetClient::Send(JamClientMessage* msg, CONNECTION_ID connection) {
    CDataStore data;
    msg->Put(&data);
    data.Finalize();
    this->Send(&data, connection);
}

// OFFSET: 0x00489770/0x1400AB240
void NetClient::WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) {
    InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(&this->m_bytesReceived), msg->m_size);

    this->m_critSect.Enter();
    this->HandleMessage(connection, timeStamp, msg);
    this->m_critSect.Leave();
}

// OFFSET: 0x004897C0/0x1400AB9A0
void NetClient::WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_critSect.Enter();

    uint32_t index = 0;

    while (connection != this->m_connections[index]) {
        if (++index >= 4) {
            this->m_critSect.Leave();
            return;
        }
    }

    if (index & 2) {
        UserClientConnectToFailed failed;
        failed.m_serial = this->m_connectSerials[index & 1];
        failed.m_con = index & 1;
        this->Send(&failed, CONNECTION_ROUTED);
        this->KillConnection(connection);
        this->m_connections[index] = NULL;
        this->m_critSect.Leave();
        return;
    }

    if (!index) {
        this->m_netEventQueue->AddEvent(EVENT_ID_NET_CANTCONNECT, connection, this, NULL, 0);
    }

    this->m_critSect.Leave();
}

// PROVEN: 9e622144 IDENTICAL/NONE
// OFFSET: 0x00489890/0x1400ABB30
void NetClient::PollEventQueue() {
    this->m_netEventQueue->Poll();
    this->CleanupConnections();
}

// OFFSET: 0x00489930/0x1400ABB60
void NetClient::Destroy() {
    this->m_critSect.Enter();

    for (int32_t i = 0; i < 2; i++) {
        this->m_heldMessages[i].Clear();
    }

    if (!this->m_netState) {
        this->m_critSect.Leave();
        return;
    }

    this->Disconnect(DISCONNECT_REASON_SHUTDOWN);
    this->KillConnection(this->m_connections[CONNECTIONS_USER]);
    this->m_connections[CONNECTIONS_USER] = NULL;

    for (int32_t i = 0; i < 4; i++) {
        z_stream* stream = this->m_inflateStreams[i];

        if (stream) {
            this->m_inflateStreams[i] = NULL;
            inflateEnd(stream);
            delete stream;
        }
    }

    memset(this->m_handlers, 0, sizeof(this->m_handlers));
    memset(this->m_handlerParams, 0, sizeof(this->m_handlerParams));

    delete this->m_netEventQueue;
    this->m_netEventQueue = NULL;
    this->m_netState = NS_UNINITIALIZED;

    if (!NetClient::s_clientCount) {
        ConsolePrintf("Client net stats: %Lu bytes sent, %Lu bytes received, %Lu msec elapsed", s_stats[0], s_stats[3], OsGetAsyncTimeMs() - s_initializeTime);
    }

    this->m_critSect.Leave();

    this->CleanupConnections();

    if (--NetClient::s_clientCount == 0) {
        OsSleep(1);
        WowConnection::DestroyOsNet();
    }
}

// OFFSET: 0x00489B30/0x1400ABDF0
void NetClient::ConnectInternal(const SockAddr& address, uint16_t port) {
    this->m_critSect.Enter();

    if (this->m_netState != NS_INITIALIZED) {
        SErrDisplayAppFatal("Expected (m_netState == NS_INITIALIZED), got %d", this->m_netState);
    }

    this->m_suspended[0] = false;
    this->m_suspended[1] = true;
    this->m_connections[CONNECTIONS_USER]->SetEncryptionType(WC_ENCRYPT_NONE);
    this->m_netState = NS_CONNECTING;
    this->m_connections[CONNECTIONS_USER]->Connect(address, port, -1);
    this->DropExtraConnections(DISCONNECT_REASON_RECONNECT);

    this->m_critSect.Leave();
}

// OFFSET: 0x00489BC0
void NetClient::SendQueuedMessages(NetClient::CONNECTIONS connection) {
    TSList<NetClient::HELDMESSAGE>& list = this->m_heldMessages[connection & 1];

    for (NetClient::HELDMESSAGE* held = list.Head(); held; held = list.DeleteNode(held)) {
        this->Send(&held->m_msg, held->m_connection);
    }
}

// OFFSET: 0x00489D90
bool NetClient::ClientConnectionResumeCommsHandler(ClientConnectionResumeComms* msg) {
    NetClient* client = msg->m_netClient;
    WowConnection* connection = msg->m_connection;
    uint32_t index = 0;

    while (connection != client->m_connections[index]) {
        if (++index >= 4) {
            return true;
        }
    }

    bool* suspended = &client->m_suspended[index & 1];

    if (!*suspended) {
        NetClient::SendDisconnectReason(connection, DISCONNECT_REASON_PROTOCOL_VIOLATION);
        connection->Disconnect();
        return true;
    }

    if (index & 2) {
        uint32_t base = index & ~2u;
        WowConnection* old = client->m_connections[base];
        client->m_connections[base] = connection;
        client->m_connections[index] = NULL;
        client->m_inflateStreams[base] = client->m_inflateStreams[index];
        client->m_inflateStreams[index] = NULL;
        client->m_handshakeDone[base] = client->m_handshakeDone[index];
        client->m_handshakeDone[index] = false;

        if (old) {
            NetClient::SendDisconnectReason(old, DISCONNECT_REASON_REPLACED);
            client->KillConnection(old);
        }
    } else {
        NetClient::SendDisconnectReason(client->m_connections[index | 2], DISCONNECT_REASON_REPLACED);
        client->KillConnection(client->m_connections[index | 2]);
        client->DestroyInflateStream(index | 2);
        client->m_connections[index | 2] = NULL;
    }

    *suspended = false;
    client->SendQueuedMessages(static_cast<NetClient::CONNECTIONS>(index));

    return true;
}

// OFFSET: 0x006946E0
bool NetClient::JAMClientConnectionDispatch(WowConnection* connection, uint32_t timeStamp, uint16_t msgId, CDataStore* msg) {
    switch (((msgId & 2) | (((msgId & 0xC00) | ((msgId >> 2) & 0x1000)) >> 8)) >> 1) {
        case 0x0: {
            ClientConnectionResumeComms message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1: {
            ClientConnectionResetCompressionContext message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x3: {
            ClientConnectionNoop message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x5: {
            ClientConnectionConnectTo message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8: {
            ClientConnectionSuspendComms message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB: {
            ClientConnectionAuthChallenge message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE: {
            ClientConnectionDropNewConnection message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF: {
            ClientConnectionPong message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        default:
            return false;
    }
}

// OFFSET: 0x006668A0/0x14030D230
bool NetClient::JAMClientDispatch(WowConnection* connection, uint32_t timeStamp, uint16_t msgId, CDataStore* msg) {
    switch ((msgId & 7) | (((msgId & 0x10) | (((msgId & 0x100) | (((msgId & 0xC00) | ((msgId >> 1) & 0x3000)) >> 1)) >> 3)) >> 1)) {
        case 0x1: {
            ClientRaidMarkersChanged message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x4: {
            ClientLfGuildApplicantListUpdated message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x5: {
            ClientGuildMemberDailyReset message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x7: {
            ClientAccountInfoResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8: {
            ClientEnumCharactersResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x9: {
            ClientPlaySpellVisual message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xA: {
            ClientGuildEventLogQueryResults message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB: {
            ClientGuildKnownRecipes message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE: {
            ClientSetupResearchHistory message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x13: {
            ClientDumpRideTicketsResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x17: {
            ClientMoveCollisionEnable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x18: {
            ClientMoveSetCollisionHeight message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x19: {
            ClientLootContents message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A: {
            ClientGuildMoveComplete message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B: {
            ClientMoveSetActiveMover message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1C: {
            ClientShowRatings message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1D: {
            ClientWeeklySpellUsageUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x20: {
            ClientNotification message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x21: {
            ClientGuildXPEarned message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x22: {
            ClientGuildInvite message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x23: {
            ClientGameObjectActivateAnimKit message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x26: {
            ClientMoveUpdateRunSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x28: {
            ClientSplineMoveSetPitchRate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x29: {
            ClientSuspendToken message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x2C: {
            ClientGuildCriteriaData message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x2E: {
            ClientSplineMoveSetHover message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x2F: {
            ClientNewWorldAbort message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x31: {
            ClientRatedBgRating message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x32: {
            ClientMoveUnsetCanFly message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x33: {
            ClientItemRefundInfoResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x34: {
            ClientChatIgnoredAccountMuted message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x35: {
            ClientSetupCurrency message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x36: {
            ClientBattlefieldMgrQueueInvite message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x37: {
            ClientMoveSetSwimSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x38: {
            ClientRespondInspectAchievements message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x39: {
            ClientDifferentInstanceFromParty message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x3F: {
            ClientStreamingMoviesUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x46: {
            ClientTransferPending message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x47: {
            ClientVoidStorageFailed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x48: {
            ClientGuildRosterUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x49: {
            ClientGuildChallengeUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x4E: {
            ClientSummonRaidMemberValidateFailed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x53: {
            ClientClearBossEmotes message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x54: {
            ClientFailedPlayerCondition message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x55: {
            ClientInspectRatedBgStats message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x5B: {
            ClientMoveSkipTime message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x5D: {
            ClientHotfixNotify message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x60: {
            ClientItemExpirePurchaseRefund message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x65: {
            ClientLfGuildMembershipListUpdated message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x68: {
            ClientGuildMemberRecipes message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x6A: {
            ClientLfGuildRecruitListUpdated message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x6D: {
            ClientGuildReset message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x6F: {
            ClientGuildMembersWithRecipe message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x72: {
            ClientSplineMoveStopSwim message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x73: {
            ClientLfGuildBrowseUpdated message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x74: {
            ClientMoveSetWalkSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x76: {
            ClientVoidTransferResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x77: {
            ClientSpellinterruptlog message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x78: {
            ClientGuildRewardList message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x7C: {
            ClientCurrencyLootRemoved message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x7D: {
            ClientMoveUpdatePitchRate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x7E: {
            ClientCustomLoadScreen message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x80: {
            ClientCurrencyLootRestored message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x81: {
            ClientMessageBox message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x82: {
            ClientMoveSetFlightBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x85: {
            ClientMoveSetTurnRate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x86: {
            ClientSetPlayHoverAnim message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x87: {
            ClientRequestCemeteryListResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x89: {
            ClientMoveUpdateFlightSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8A: {
            ClientGuildBankLogQueryResults message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8B: {
            ClientMoveGravityEnable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8C: {
            ClientGuildRanks message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8D: {
            ClientMoveUpdateSwimBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8E: {
            ClientGuildFlaggedForRename message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x8F: {
            ClientGuildReputationWeeklyCap message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x90: {
            ClientWardenData message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x92: {
            ClientWorldServerInfo message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x94: {
            ClientLfgUpdateStatus message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x95: {
            ClientSplineMoveStartSwim message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x96: {
            ClientDisplayGameError message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x97: {
            ClientGameEventDebugLog message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x98: {
            ClientMoveCollisionDisable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x9A: {
            ClientPartyInvite message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x9B: {
            ClientDebugRuneRegen message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x9D: {
            ClientSplineMoveSetFlying message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xA1: {
            ClientRatedBgStats message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xA2: {
            ClientBattlefieldMgrEjectPending message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xA3: {
            ClientGuildPermissionsQueryResults message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xA5: {
            ClientSplineMoveSetWalkSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xAA: {
            ClientBattlegroundInfoThrottled message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xAB: {
            ClientBattlefieldMgrEntryInvite message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xAF: {
            ClientMoveSetLandWalk message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB0: {
            ClientGuildAchievementDeleted message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB1: {
            ClientBattlefieldStatusQueued message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB3: {
            ClientBattlefieldPortDenied message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB6: {
            ClientResearchComplete message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB7: {
            ClientGuildNews message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB8: {
            ClientSupercededSpells message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xB9: {
            ClientSplineMoveCollisionDisable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xBC: {
            ClientBattlefieldMgrStateChange message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xBE: {
            ClientSetDfFastLaunchResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xBF: {
            ClientLfGuildPostUpdated message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xC2: {
            ClientDontAutoPushSpellsToActionBar message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xC4: {
            ClientDBReply message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xC5: {
            ClientGuildAchievementMembers message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xC8: {
            ClientCommentatorPartyInfo message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xC9: {
            ClientGenerateRandomCharacterNameResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xCA: {
            ClientSplineMoveSetNormalFall message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xCB: {
            ClientSplineMoveSetFlightBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xCD: {
            ClientUpdateActionButtons message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xCE: {
            ClientLfgJoinResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xD0: {
            ClientSplineMoveSetFlightSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xD1: {
            ClientItemRemovePassive message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xD3: {
            ClientGuildChallengeCompleted message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xD4: {
            ClientSplineMoveSetSwimSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xD6: {
            ClientRoleChangedInform message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xDB: {
            ClientArenaTeamCommandResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xDF: {
            ClientWeeklySpellUsage message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE1: {
            ClientWeeklyResetCurrency message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE4: {
            ClientTimeSyncReq message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE5: {
            ClientPetAdded message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE6: {
            ClientSplineMoveGravityEnable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE8: {
            ClientSplineMoveCollisionEnable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xE9: {
            ClientGuildChangeNameResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xED: {
            ClientUpdateDungeonEncounterForLoot message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF1: {
            ClientMoveSetCanFly message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF3: {
            ClientGuildRoster message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF4: {
            ClientWargameCheckEntry message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF5: {
            ClientSplineMoveSetFeatherFall message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF6: {
            ClientMoveUpdateRunBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF7: {
            ClientSplineMoveSetLandWalk message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF8: {
            ClientGuildXp message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xF9: {
            ClientAreaTriggerMovementUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xFA: {
            ClientMoveUpdateKnockBack message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xFB: {
            ClientSplineMoveSetRunBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xFC: {
            ClientMissileCancel message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xFD: {
            ClientMoveSetRunSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0xFF: {
            ClientFeatureSystemStatus message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x101: {
            ClientPvpOptionsEnabled message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x102: {
            ClientSplineMoveSetWaterWalk message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x105: {
            ClientWeeklyLastReset message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x106: {
            ClientGuildPartyState message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x108: {
            ClientBattlegroundPlayerJoined message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x109: {
            ClientLoadCufProfiles message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x10A: {
            ClientMoveUpdateTeleport message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x10C: {
            ClientXpGainAborted message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x10D: {
            ClientGuildAchievementEarned message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x113: {
            ClientPetSlotUpdated message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x116: {
            ClientVoidStorageTransferChanges message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x117: {
            ClientAccountRestrictedWarning message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x119: {
            ClientBattlefieldMgrExitRequest message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x11B: {
            ClientMoveUnsetHover message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x11C: {
            ClientSplineMoveRoot message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x11E: {
            ClientMoveSetNormalFall message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x11F: {
            ClientSplineMoveSetRunSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x121: {
            ClientLfgSearchResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x122: {
            ClientMoveUpdateWalkSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x123: {
            ClientBattlefieldRatedInfo message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x126: {
            ClientLfGuildCommandResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x12A: {
            ClientMapObjEvents message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x12D: {
            ClientLfgSlotInvalid message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x12E: {
            ClientSplineMoveSetWalkMode message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x12F: {
            ClientGuildAchievementData message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x130: {
            ClientMoveTeleport message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x132: {
            ClientDisenchantCredit message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x134: {
            ClientQuestGiverQuestComplete message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x135: {
            ClientPlaySpellVisualKit message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x137: {
            ClientHotfixNotifyBlob message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x139: {
            ClientGuildCriteriaDeleted message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x13E: {
            ClientMoneyNotify message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x141: {
            ClientWaitQueueUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x142: {
            ClientLearnedSpell message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x144: {
            ClientReforgeResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x146: {
            ClientSplineMoveUnsetFlying message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x149: {
            ClientAllAchievementData message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x14C: {
            ClientBattlefieldPlayerPositions message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x150: {
            ClientBattlefieldStatusNeedconfirmation message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x151: {
            ClientSplineMoveSetSwimBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x152: {
            ClientMoveSetCanTransitionBetweenSwimAndFly message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x153: {
            ClientMoveUpdateCollisionHeight message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x155: {
            ClientStartTimer message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x156: {
            ClientBattlegroundPlayerLeft message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x158: {
            ClientSetCurrency message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x15A: {
            ClientWargameRequestSent message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x15C: {
            ClientClearCooldowns message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x15D: {
            ClientMoveUpdateSwimSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x160: {
            ClientBattlefieldMgrEntered message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x163: {
            ClientTradeStatus message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x166: {
            ClientMoveSetSwimBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x16A: {
            ClientPvpLogData message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x16B: {
            ClientMoveSetHover message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x16C: {
            ClientMoveKnockBack message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x170: {
            ClientGuildRanksUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x171: {
            ClientMoveUpdateTurnRate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x173: {
            ClientPetitionAlreadySigned message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x174: {
            ClientRequestPvpRewardsResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x177: {
            ClientAverageItemLevelInform message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x179: {
            ClientItemRefundResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x17C: {
            ClientGuildBankMoneyWithdrawn message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x17D: {
            ClientSplineMoveGravityDisable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x17E: {
            ClientAuthResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x180: {
            ClientPhaseShiftChange message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x181: {
            ClientMoveSetVehicleRecId message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x182: {
            ClientTradeUpdated message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x184: {
            ClientGuildMoveStarting message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x187: {
            ClientUpdateCurrencyWeekLimit message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x188: {
            ClientRolePollBegin message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x189: {
            ClientItemSendPassive message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x18F: {
            ClientForgeMasterSet message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x195: {
            ClientLfGuildApplicationsListChanged message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x196: {
            ClientMoveSetFlightSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x197: {
            ClientBattlefieldStatusFailed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x199: {
            ClientMoveSetRunBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x19D: {
            ClientBattlefieldList message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x19E: {
            ClientCategoryCooldown message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A0: {
            ClientMoveUpdateFlightBackSpeed message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A3: {
            ClientUpdateServerPlayerPosition message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A4: {
            ClientBattlefieldStatusActive message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A6: {
            ClientGuildNameChanged message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A7: {
            ClientGuildNewsDeleted message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A8: {
            ClientGuildReputationReactionChanged message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1A9: {
            ClientSuppressNPCGreetings message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1AD: {
            ClientServerInfoResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1AE: {
            ClientServerPerf message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B0: {
            ClientMoveSetCompoundState message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B1: {
            ClientQuestNpcQueryResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B2: {
            ClientBattlefieldStatusWaitforgroups message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B3: {
            ClientGuildBankTextQueryResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B7: {
            ClientSplineMoveSetRunMode message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B8: {
            ClientMoveSetPitchRate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1B9: {
            ClientMoveSetWaterWalk message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1BA: {
            ClientMoveGravityDisable message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1BC: {
            ClientVoidStorageContents message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1BE: {
            ClientSplineMoveUnroot message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1BF: {
            ClientWaitQueueFinish message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1C2: {
            ClientVoidItemSwapResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1C5: {
            ClientGuildBankQueryResults message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1CC: {
            ClientLfgQueueStatus message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1CD: {
            ClientSplineMoveSetTurnRate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1D2: {
            ClientMoveUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1D5: {
            ClientInspectHonorStats message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1D8: {
            ClientMoveSetFeatherFall message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1D9: {
            ClientNewWorld message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1DD: {
            ClientGuildMaxDailyXp message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1DE: {
            ClientBattlefieldMgrQueueRequestResponse message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1DF: {
            ClientTimeAdjustment message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1E0: {
            ClientGuildMemberUpdateNote message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1E1: {
            ClientLfgUpdateStatusNone message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1E7: {
            ClientSorStartExperienceIncomplete message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1E8: {
            ClientVendorInventory message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1EC: {
            ClientItemAddPassive message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1EF: {
            ClientAuraPointsDepleted message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1F0: {
            ClientMoveRoot message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1F1: {
            ClientBattlefieldStatus message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1F5: {
            ClientSplineMoveUnsetHover message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1F6: {
            ClientLfgProposalUpdate message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1FA: {
            ClientMoveUnsetCanTransitionBetweenSwimAndFly message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1FB: {
            ClientGuildCommandResult message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1FC: {
            ClientMoveUnroot message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        case 0x1FF: {
            ClientBattlefieldMgrEjected message(this, connection, msg, timeStamp);
            return message.Handle();
        }

        default:
            return false;
    }
}
