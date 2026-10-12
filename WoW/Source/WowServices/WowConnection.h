#ifndef WOW_SOURCE_WOWSERVICES_WOWCONNECTION_H
#define WOW_SOURCE_WOWSERVICES_WOWCONNECTION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/Os/OsTcp.h"
#include "Engine/Source/Os/SockAddr.h"
#include "Engine/Source/Services/Panama.h"
#include "Engine/Source/Storm/H/Types.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/WowServices/Types.h"

class CDataStore;
struct sockaddr;
class WowConnectionResponse;

typedef Blizzard::Lock::Atomic ATOMIC32;

class WowConnectionNet;

class WowConnection {
    public:
    struct SENDNODE : public TSLinkedNode<WowConnection::SENDNODE> {
        /* 0008 */ uint8_t* m_data;
        /* 000C */ uint32_t m_size;
        /* 0010 */ uint32_t m_offset;
        /* 0014 */ uint32_t m_datasize;
        /* 0018 */ uint32_t m_queueTime;
        /* 001C */ uint8_t m_header[8];
        /* 0024 */ uint32_t m_sendTime;
        /* 0028 */ uint32_t m_allocsize;

        SENDNODE();
        void InitPacket(uint8_t* data, uint32_t size, const void* extraData, uint32_t extraSize, bool checksum);
        void SetPacket(const void* data, uint32_t size, bool checksum);
    };

    struct IDBITS {
        /* 0000 */ uint32_t m_count;
        /* 0004 */ TSFixedArray<uint32_t> m_bits;

        void Or(const WowConnection::IDBITS& source);
        void SetCount(uint32_t count);
    };

    static WowConnection::IDBITS s_idBits;
    static Blizzard::Lock::Atomic s_numConnections;
    static SCritSect s_idLock;
    static uint64_t s_sendBytes;
    static WowConnectionNet* s_network;
    static int32_t s_lagTestDelayMin;
    static int32_t s_lagTestDelayMax;
    static bool (*s_verifyAddr)(const NETADDR* addr);

    /* 0000 */ ATOMIC32 m_refCount;
    /* 0004 */ int32_t m_sock;
    /* 0008 */ int32_t m_oldsock;
    /* 000C */ uint8_t unk_000C;
    /* 000D */ uint8_t unk_000D[3];
    /* 0010 */ WOW_CONN_STATE m_connState;
    /* 0014 */ WowConnectionResponse* m_response;
    /* 0018 */ int32_t unk_0018;
    /* 001C */ uint8_t* m_readBuffer;
    /* 0020 */ int32_t m_readBytes;
    /* 0024 */ int32_t m_readBufferSize;
    /* 0028 */ SCritSect m_outLock;
    /* 0050 */ int32_t unk_0050;
    /* 0054 */ int32_t unk_0054;
    /* 0058 */ int32_t unk_0058;
    /* 005C */ int32_t unk_005C;
    /* 0060 */ int32_t unk_0060;
    /* 0064 */ SockAddr m_localAddress;
    /* 0078 */ uint16_t unk_0078;
    /* 007C */ void (*m_threadInit)();
    /* 0080 */ SockAddr m_connectAddress;
    /* 0094 */ uint16_t m_connectPort;
    /* 0098 */ int32_t m_connectRetryInterval;
    /* 009C */ int32_t unk_009C;
    /* 00A0 */ NETCONNADDR m_peer;
    /* 00E0 */ int32_t unk_00E0;
    /* 00E4 */ SCritSect m_responseLock;
    /* 010C */ int32_t m_responseRef;
    /* 0110 */ uintptr_t m_responseRefThread;
    /* 0114 */ WowConnectionResponse* m_pendingResponse;
    /* 0118 */ TSLink<WowConnection> m_netLink;
    /* 0120 */ TSList<WowConnection::SENDNODE> m_sendList;
    /* 012C */ int32_t m_sendDepth;
    /* 0130 */ int64_t m_sendDepthBytes;
    /* 0138 */ int32_t m_maxSendDepth;
    /* 013C */ uint32_t m_serviceFlags;
    /* 0140 */ TSList<WowConnection::SENDNODE> m_lagTestRecvList;
    /* 014C */ SCritSect m_lock;
    /* 0174 */ int32_t m_serviceCount;
    /* 0178 */ uint32_t m_createTime;
    /* 017C */ void* m_event;
    /* 0180 */ WOWC_TYPE m_type;
    /* 0184 */ bool m_notifyWriteReady;
    /* 0188 */ int32_t unk_0188;
    /* 018C */ bool m_checksumPackets;
    /* 018D */ SARC4Key m_sendKey;
    /* 028F */ SARC4Key m_receiveKey;
    /* 0391 */ uint8_t m_sendKeyInit[20];
    /* 03A5 */ uint8_t m_receiveKeyInit[20];
    /* 03BC */ WC_ENCRYPT_TYPE m_encryptionType;
    /* 03C0 */ uint8_t m_sendEncryptBytes;
    /* 03C1 */ uint8_t m_recvEncryptBytes;
    /* 03C4 */ Crypt::PanamaCypher* m_cryptPanamaSendCypher;
    /* 03C8 */ Crypt::PanamaCypher* m_cryptPanamaRecvCypher;
    /* 03D0 */ uint64_t m_bytesSent;
    /* 03D8 */ uint64_t m_bytesReceived;
    /* 03E0 */ bool m_preferIPv6;

    static void CloseSocket(int32_t sock);
    static WowConnection::SENDNODE* CopySendNode(const WowConnection::SENDNODE* source);
    static void FreeSendNode(WowConnection::SENDNODE* node);
    static int32_t CreateSocket(int32_t type);
    static void DestroyOsNet();
    static int32_t InitOsNet(bool (*netAddrFilter)(const NETADDR* addr), void (*threadInit)(), int32_t numThreads, bool useEngine);
    static void MarkSocketId(int32_t sock);

    WowConnection(WowConnectionResponse* response, void (*threadInit)());
    WowConnection(int32_t sock, const NETADDR* addr, WowConnectionResponse* response);
    ~WowConnection();
    void AcquireResponseRef();
    void AddRef();
    void CheckAccept();
    void CheckConnect();
    bool Connect(const char* address, int32_t timeout);
    bool Connect(const char* host, uint16_t port, int32_t timeout);
    bool Connect(const SockAddr& address, uint16_t port, int32_t timeout);
    void Disconnect();
    void DoDisconnect();
    void DoExceptions();
    void DoMessageReads();
    void DoReads();
    void DoStreamReads();
    void DoWrites();
    void Encrypt(uint8_t* data, uint32_t length);
    void FreeSendList();
    bool GetLocalAddr(sockaddr* addr);
    void Init(WowConnectionResponse* response, void (*threadInit)());
    WowConnection::SENDNODE* AllocSendNodePacket(uint32_t size, uint32_t extraSize, bool checksum);
    WowConnection::SENDNODE* NewSendNodePacket(const void* data, uint32_t size, bool checksum);
    WowConnection::SENDNODE* NewSendNodeRaw(const void* data, uint32_t size);
    int32_t Release();
    void ReleaseResponseRef();
    int32_t Send(CDataStore* msg, int32_t headroom);
    int32_t SendRaw(uint8_t* data, int32_t size);
    void SetEncryptionKey(const uint8_t* key, uint8_t keyLength, uint8_t direction, const uint8_t* seed, uint8_t seedLength);
    void SetEncryptionType(WC_ENCRYPT_TYPE type);
    void SetResponse(WowConnectionResponse* response, bool deferred);
    void SetSendTime(WowConnection::SENDNODE* node);
    void SetState(WOW_CONN_STATE state);
    void SetType(WOWC_TYPE type);
    void StartConnect();
};

class WowConnectionResponse {
    public:
    virtual ~WowConnectionResponse();
    virtual void WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) = 0;
    virtual void WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) = 0;
    virtual void WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) = 0;
    virtual void WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) = 0;
    virtual void WCDataReady(WowConnection* connection, uint32_t timeStamp, uint8_t* data, int32_t length);
    virtual void WCWriteReady(WowConnection* connection);
    virtual void NotifyAboutToDoReads();
    virtual void NotifySendDepthExceeded(WowConnection* connection);
    virtual bool WCShouldBlockReads();
};

class ServerConnection {
    public:
    /* 0000 */ uint8_t unk_0000[4];
    /* 0004 */ uint32_t unk_0004;
    /* 0008 */ uint8_t unk_0008[8];
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint8_t unk_0018[180];
    /* 00CC */ uint32_t unk_00CC;
    /* 00D0 */ uint32_t unk_00D0;
    /* 00D4 */ uint32_t unk_00D4;
    /* 00D8 */ uint8_t unk_00D8[16];
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint32_t unk_00EC;
    /* 00F0 */ uint32_t unk_00F0;
    /* 00F4 */ uint32_t unk_00F4;
    /* 00F8 */ uint8_t unk_00F8[596];
    /* 034C */ uint32_t unk_034C;
    /* 0350 */ uint32_t unk_0350;
    /* 0354 */ uint32_t unk_0354;
    /* 0358 */ uint32_t unk_0358;
    /* 035C */ uint32_t unk_035C;
    /* 0360 */ uint32_t unk_0360;
    /* 0364 */ uint32_t unk_0364;
    /* 0368 */ uint32_t unk_0368;
    /* 036C */ uint32_t unk_036C;
    /* 0370 */ uint32_t unk_0370;
    /* 0374 */ uint8_t unk_0374;
    /* 0375 */ uint8_t unk_0375;
};

class WoWConnection {
    public:
    /* 0000 */ uint8_t unk_0000[232];
    /* 00E8 */ uint32_t unk_00E8;
    /* 00EC */ uint8_t unk_00EC[24];
    /* 0104 */ uint32_t unk_0104;
};

SockAddr SockAddrFromSockaddr(const sockaddr* addr);

#endif
