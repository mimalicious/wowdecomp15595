#include "WowConnection.h"
#include <cstdlib>
#include <cstring>
#include <new>
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SHA1.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Panama.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowServices/HMAC.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowServices/WowConnectionNet.h"

// STATIC: 0x00CA500C/0x140B43708
WowConnection::IDBITS WowConnection::s_idBits;

// STATIC: 0x00DD44B4/0x140CBA6C4
static int32_t s_sendNodeCount;

// STATIC: 0x00DD44B8/0x140CBA6C8
static uint64_t s_sendNodeBytes;

// STATIC: 0x00DD44C0
uint64_t WowConnection::s_sendBytes;

// STATIC: 0x00DD44C8/0x140CBA6D8
bool (*WowConnection::s_verifyAddr)(const NETADDR* addr);

// STATIC: 0x00DD44CC/0x140CBA6E0
WowConnectionNet* WowConnection::s_network;

// STATIC: 0x00DD44D0/0x140CBA6E8
int32_t WowConnection::s_lagTestDelayMin;

// STATIC: 0x00DD44D4
int32_t WowConnection::s_lagTestDelayMax;

// STATIC: 0x00DD44D8
Blizzard::Lock::Atomic WowConnection::s_numConnections;

// STATIC: 0x00DD44E8/0x140CBA700
SCritSect WowConnection::s_idLock;

// STATIC: 0x00DD4518/0x140CBA740
static uint8_t s_arc4drop1024[1024];

// STATIC: 0x00DD4918/0x140CBAB40
static uint8_t s_arc4drop1024Key[1024];

// STATIC: 0x00BAE6F0/0x140992B00
static const uint8_t s_defaultSeed[32] = { 0xCC, 0x98, 0xAE, 0x04, 0xE8, 0x97, 0xEA, 0xCA, 0x12, 0xDD, 0xC0, 0x93, 0x42, 0x91, 0x53, 0x57, 0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE, 0x2F, 0x43, 0x67, 0xCE };

// OFFSET: 0x0054D500/0x1401A74F0
static void WritePacketHeader(uint8_t* header, uint32_t size, const void* data, uint32_t extraSize, const void* extraData, bool checksum) {
    uint32_t total = size + extraSize;

    if (total > 0x7FFF) {
        *header++ = static_cast<uint8_t>(total >> 16) | 0x80;
    }

    *header++ = static_cast<uint8_t>(total >> 8);
    *header++ = static_cast<uint8_t>(total);

    if (checksum) {
        uint64_t crc;
        SCrc64Compute(NULL, 0, &crc, 1);

        if (data) {
            SCrc64Compute(data, size, &crc, 2);
        }

        if (extraData) {
            SCrc64Compute(extraData, extraSize, &crc, 2);
        }

        SCrc64Compute(NULL, 0, &crc, 4);

        for (int32_t shift = 56; shift >= 0; shift -= 8) {
            *header++ = static_cast<uint8_t>(crc >> shift);
        }
    }
}

// PROVEN: 89de7c95 IDENTICAL/NONE
// OFFSET: 0x0054C800
void WowConnection::Encrypt(uint8_t* data, uint32_t length) {
    SARC4ProcessBuffer(data, length, &this->m_sendKey, &this->m_sendKey);
}

// OFFSET: 0x0054D2D0/0x1401A7270
void WowConnection::SetResponse(WowConnectionResponse* response, bool deferred) {
    while (true) {
        this->m_responseLock.Enter();

        if (!this->m_responseRef || this->m_responseRefThread == SGetCurrentThreadId()) {
            break;
        }

        if (deferred) {
            this->m_pendingResponse = response;
            this->m_responseLock.Leave();
            return;
        }

        this->m_responseLock.Leave();
        OsSleep(50);
    }

    this->m_response = response;
    this->m_pendingResponse = NULL;
    this->m_responseLock.Leave();
}

// PROVEN: c2c63a8e IDENTICAL/DIFFERENT
// OFFSET: 0x0054D350/0x1401A7310
void WowConnection::AcquireResponseRef() {
    this->m_responseLock.Enter();
    this->m_responseRef++;
    this->m_responseRefThread = SGetCurrentThreadId();
    this->m_responseLock.Leave();
}

// PROVEN: a1e79162 IDENTICAL/DIFFERENT
// OFFSET: 0x0054D380/0x1401A7350
void WowConnection::ReleaseResponseRef() {
    this->m_responseLock.Enter();

    WowConnectionResponse* pendingResponse = this->m_pendingResponse;
    this->m_responseRef--;

    if (pendingResponse) {
        this->m_response = pendingResponse;
        this->m_pendingResponse = NULL;
    }

    this->m_responseLock.Leave();
}

// PROVEN: 46811325 IDENTICAL/NONE
// OFFSET: 0x0054D3C0
void WowConnection::SetState(WOW_CONN_STATE state) {
    WOW_CONN_STATE oldState = this->m_connState;
    this->m_connState = state;

    if (WowConnection::s_network) {
        WowConnection::s_network->PlatformChangeState(this, oldState);
    }
}

// PROVEN: d1542e3d IDENTICAL/IDENTICAL
// OFFSET: 0x0054D3F0/0x1401A73A0
void WowConnection::SetType(WOWC_TYPE type) {
    this->m_lock.Enter();
    this->m_type = type;
    this->m_lock.Leave();
}

// OFFSET: 0x0054D450/0x1401A7420
void WowConnection::SetEncryptionType(WC_ENCRYPT_TYPE type) {
    this->m_lock.Enter();
    this->m_encryptionType = type;

    if (type == WC_ENCRYPT_ARC4) {
        SARC4PrepareKey(this->m_sendKeyInit, sizeof(this->m_sendKeyInit), &this->m_sendKey);
        SARC4PrepareKey(this->m_receiveKeyInit, sizeof(this->m_receiveKeyInit), &this->m_receiveKey);
        SARC4ProcessBuffer(s_arc4drop1024, sizeof(s_arc4drop1024), &this->m_sendKey, &this->m_sendKey);
        SARC4ProcessBuffer(s_arc4drop1024, sizeof(s_arc4drop1024), &this->m_receiveKey, &this->m_receiveKey);
    } else if (type == WC_ENCRYPT_PANAMA) {
        this->m_checksumPackets = true;
    }

    this->m_lock.Leave();
}

// OFFSET: 0x0054D5F0/0x1401A7620
void WowConnection::Init(WowConnectionResponse* response, void (*threadInit)()) {
    WowConnection::s_numConnections.Increment();

    this->m_refCount.m_value = 1;
    this->m_responseRef = 0;
    this->m_pendingResponse = NULL;
    this->m_maxSendDepth = 100000;
    this->m_sendDepth = 0;
    this->m_sendDepthBytes = 0;
    this->m_oldsock = -1;
    this->m_notifyWriteReady = false;
    this->unk_0188 = -1;
    this->SetState(WOWC_UNINITIALIZED);
    this->m_response = response;
    this->m_threadInit = threadInit;
    this->unk_000C = 0;
    this->unk_0018 = 0;
    this->unk_0050 = 0;
    this->unk_0054 = 0;
    this->unk_0058 = 0;
    this->unk_005C = 0;
    this->unk_0060 = 0;
    this->unk_0078 = 0;
    this->unk_009C = 0;
    this->m_connectPort = 0;
    this->m_connectRetryInterval = -1;
    memset(&this->m_peer, 0, sizeof(this->m_peer));
    this->unk_00E0 = 0;
    this->m_serviceFlags = 0;
    this->m_serviceCount = 0;
    this->m_createTime = OsGetAsyncTimeMsPrecise();
    this->m_readBuffer = NULL;
    this->m_readBytes = 0;
    this->m_readBufferSize = 0;
    this->m_event = NULL;
    this->m_encryptionType = WC_ENCRYPT_NONE;
    this->m_sendEncryptBytes = 0;
    this->m_recvEncryptBytes = 0;
    memset(this->m_sendKeyInit, 0, sizeof(this->m_sendKeyInit));
    memset(this->m_receiveKeyInit, 0, sizeof(this->m_receiveKeyInit));
    this->m_cryptPanamaSendCypher = NULL;
    this->m_cryptPanamaRecvCypher = NULL;
    this->m_checksumPackets = false;
    this->m_bytesSent = 0;
    this->m_bytesReceived = 0;
    this->m_preferIPv6 = false;
    this->SetState(WOWC_INITIALIZED);
    this->m_type = WOWC_TYPE_MESSAGES;
}

// OFFSET: 0x0054D790/0x1401A77F0
void WowConnection::Disconnect() {
    this->m_lock.Enter();

    if (this->m_sock >= 0 && this->m_connState == WOWC_CONNECTED) {
        this->SetState(WOWC_DISCONNECTING);
    }

    this->m_lock.Leave();
}

// OFFSET: 0x0054D830/0x1401A78D0
void WowConnection::SetEncryptionKey(const uint8_t* key, uint8_t keyLength, uint8_t direction, const uint8_t* seed, uint8_t seedLength) {
    if (!seed) {
        seed = s_defaultSeed;
        seedLength = sizeof(s_defaultSeed);
    }

    uint32_t halfLength = seedLength >> 1;
    const uint8_t* seeds[2] = { seed, seed + halfLength };

    HMAC<SHA1> hmac;
    SHA1::Digest digest;

    hmac.Init(seeds[direction], halfLength);
    hmac.Process(key, keyLength);
    hmac.Finish(digest);
    memcpy(this->m_sendKeyInit, &digest, sizeof(digest));

    hmac.Init(seeds[direction ^ 1], halfLength);
    hmac.Process(key, keyLength);
    hmac.Finish(digest);
    memcpy(this->m_receiveKeyInit, &digest, sizeof(digest));

    SARC4PrepareKey(this->m_sendKeyInit, sizeof(this->m_sendKeyInit), &this->m_sendKey);
    SARC4PrepareKey(this->m_receiveKeyInit, sizeof(this->m_receiveKeyInit), &this->m_receiveKey);
    SARC4ProcessBuffer(s_arc4drop1024Key, sizeof(s_arc4drop1024Key), &this->m_sendKey, &this->m_sendKey);
    SARC4ProcessBuffer(s_arc4drop1024Key, sizeof(s_arc4drop1024Key), &this->m_receiveKey, &this->m_receiveKey);
}

// PROVEN: 2975dc81 IDENTICAL/NONE
// OFFSET: 0x0054D9F0
void WowConnection::IDBITS::Or(const WowConnection::IDBITS& source) {
    for (int32_t i = 0; i < static_cast<int32_t>(source.m_bits.m_count); i++) {
        this->m_bits.m_data[i] |= source.m_bits.m_data[i];
    }
}

// OFFSET: 0x0054DA20/0x1401A7B20
void WowConnection::SENDNODE::InitPacket(uint8_t* data, uint32_t size, const void* extraData, uint32_t extraSize, bool checksum) {
    this->m_datasize = size;

    uint32_t headerSize = (checksum ? 8 : 0) + (size + extraSize > 0x7FFF) + 2;
    this->m_size = size + headerSize;
    this->m_data = data - headerSize;
    WritePacketHeader(this->m_data, size, data, extraSize, extraData, checksum);

    uint32_t headerCopy = this->m_size;

    if (headerCopy > sizeof(this->m_header)) {
        headerCopy = sizeof(this->m_header);
    }

    memcpy(this->m_header, this->m_data, headerCopy);
}

// OFFSET: 0x0054DAA0/0x1401A7BC0
void WowConnection::SENDNODE::SetPacket(const void* data, uint32_t size, bool checksum) {
    this->m_datasize = size;
    WritePacketHeader(this->m_data, size, data, 0, NULL, checksum);
    memcpy(this->m_data + (checksum ? 8 : 0) + (size > 0x7FFF) + 2, data, size);

    uint32_t headerCopy = this->m_size;

    if (headerCopy > sizeof(this->m_header)) {
        headerCopy = sizeof(this->m_header);
    }

    memcpy(this->m_header, this->m_data, headerCopy);
}

// OFFSET: 0x0054DB20/0x1401A7C70
void WowConnection::SetSendTime(WowConnection::SENDNODE* node) {
    int32_t delay = WowConnection::s_lagTestDelayMin;

    if (delay) {
        if (static_cast<uint32_t>(WowConnection::s_lagTestDelayMax) > static_cast<uint32_t>(delay)) {
            delay = static_cast<int32_t>(delay - static_cast<int64_t>(static_cast<double>(static_cast<uint32_t>((WowConnection::s_lagTestDelayMax - delay + 1) * rand())) * -0.000030517578125));
        }

        node->m_sendTime = delay + OsGetAsyncTimeMsPrecise();
    } else if (this->m_lagTestRecvList.Head() || this->m_sendList.Head()) {
        node->m_sendTime = OsGetAsyncTimeMsPrecise();
    }
}

// OFFSET: 0x0054DD50/0x1401A7EC0
void WowConnection::FreeSendList() {
    WowConnection::SENDNODE* node = this->m_sendList.Head();

    while (node) {
        WowConnection::SENDNODE* next = this->m_sendList.RawNext(node);
        this->m_sendList.UnlinkNode(node);
        WDataStore::FreeBuffer(node, node->m_allocsize);
        node = TSList<WowConnection::SENDNODE>::IsEnd(next) ? NULL : next;
    }

    node = this->m_lagTestRecvList.Head();

    while (node) {
        WowConnection::SENDNODE* next = this->m_lagTestRecvList.RawNext(node);
        this->m_lagTestRecvList.UnlinkNode(node);
        WDataStore::FreeBuffer(node, node->m_allocsize);
        node = TSList<WowConnection::SENDNODE>::IsEnd(next) ? NULL : next;
    }
}

// PROVEN: 4ef80ffd IDENTICAL/NONE
// OFFSET: 0x0054DFE0
WowConnection::SENDNODE::SENDNODE() {
    this->m_allocsize = 0;
    this->m_size = 0;
    this->m_data = NULL;
    this->m_queueTime = 0;
    this->m_sendTime = 0;
    this->m_offset = 0;
}

// OFFSET: 0x0054E3E0/0x1401A8750
WowConnection::SENDNODE* WowConnection::NewSendNodeRaw(const void* data, uint32_t size) {
    s_sendNodeCount++;
    s_sendNodeBytes += size;

    WowConnection::SENDNODE* node = new (WDataStore::AllocBuffer(size + sizeof(WowConnection::SENDNODE))) WowConnection::SENDNODE();
    node->m_allocsize = size + sizeof(WowConnection::SENDNODE);
    node->m_data = reinterpret_cast<uint8_t*>(node + 1);
    node->m_size = size;
    node->m_datasize = size;
    memcpy(node + 1, data, size);
    memset(node->m_header, 0, sizeof(node->m_header));
    this->SetSendTime(node);

    return node;
}

// OFFSET: 0x0054E470/0x1401A8800
WowConnection::SENDNODE* WowConnection::AllocSendNodePacket(uint32_t size, uint32_t extraSize, bool checksum) {
    s_sendNodeCount++;
    s_sendNodeBytes += size;

    uint32_t packetSize = (checksum ? 8 : 0) + (extraSize > 0x7FFF) + 2 + size;
    WowConnection::SENDNODE* node = new (WDataStore::AllocBuffer(packetSize + sizeof(WowConnection::SENDNODE))) WowConnection::SENDNODE();
    node->m_allocsize = packetSize + sizeof(WowConnection::SENDNODE);
    node->m_data = reinterpret_cast<uint8_t*>(node + 1);
    node->m_size = packetSize;
    this->SetSendTime(node);

    return node;
}

// OFFSET: 0x0054E500/0x1401A88C0
WowConnection::SENDNODE* WowConnection::CopySendNode(const WowConnection::SENDNODE* source) {
    s_sendNodeCount++;
    s_sendNodeBytes += source->m_datasize;

    uint32_t allocSize = source->m_size + sizeof(WowConnection::SENDNODE);
    WowConnection::SENDNODE* node = new (WDataStore::AllocBuffer(allocSize)) WowConnection::SENDNODE();
    node->m_allocsize = allocSize;
    node->m_data = reinterpret_cast<uint8_t*>(node + 1);
    node->m_size = source->m_size;
    node->m_offset = source->m_offset;
    node->m_datasize = source->m_datasize;
    node->m_sendTime = source->m_sendTime;
    node->m_queueTime = source->m_queueTime;
    memcpy(node->m_data, source->m_data, source->m_size);
    memcpy(node->m_header, source->m_header, sizeof(node->m_header));

    return node;
}

// OFFSET: 0x0054EDC0
void WowConnection::IDBITS::SetCount(uint32_t count) {
    this->m_count = count;
    this->m_bits.SetCount(count ? ((count - 1) >> 5) + 1 : 0);
}

// OFFSET: 0x0054F120/0x1401A98A0
WowConnection::SENDNODE* WowConnection::NewSendNodePacket(const void* data, uint32_t size, bool checksum) {
    s_sendNodeCount++;
    s_sendNodeBytes += size;

    WowConnection::SENDNODE* node = this->AllocSendNodePacket(size, size, checksum);
    node->SetPacket(data, size, checksum);
    this->SetSendTime(node);

    return node;
}

// PROVEN: 4a12b654 DIFFERENT/IDENTICAL
// OFFSET: 0x00487A80/0x1400A8E80
WowConnectionResponse::~WowConnectionResponse() {
}

void WowConnectionResponse::WCDataReady(WowConnection* connection, uint32_t timeStamp, uint8_t* data, int32_t length) {
}

void WowConnectionResponse::WCWriteReady(WowConnection* connection) {
}

void WowConnectionResponse::NotifyAboutToDoReads() {
}

void WowConnectionResponse::NotifySendDepthExceeded(WowConnection* connection) {
}

bool WowConnectionResponse::WCShouldBlockReads() {
    return false;
}
