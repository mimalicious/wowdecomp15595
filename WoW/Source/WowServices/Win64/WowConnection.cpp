#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cerrno>
#include <cstring>
#include <malloc.h>
#include <new>
#include "WoW/Source/WowServices/WowConnection.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Panama.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowServices/WowConnectionNet.h"

// STATIC: 0x00DD44B0/0x140CBA6C0
static int32_t s_osNetDestroyed;

// OFFSET: 0x0054C690
static int32_t GetPacketSize(const uint8_t* data, int32_t length) {
    if (length < 2) {
        return -1;
    }

    if (data[0] & 0x80) {
        if (length < 3) {
            return -1;
        }

        return (((data[0] & 0x7F) << 8 | data[1]) << 8) | data[2];
    }

    return ((data[0] & 0x7F) << 8) | data[1];
}

// OFFSET: 0x0054C6E0/0x1401A64B0
static bool VerifyPacketChecksum(const uint8_t* data, int32_t length) {
    if (length < 1) {
        return false;
    }

    bool large = (data[0] & 0x80) != 0;
    int32_t headerSize = (large ? 1 : 0) + 10;
    int32_t size = GetPacketSize(data, length);

    if (size < 0 || headerSize + size > length) {
        return false;
    }

    const uint8_t* crcData = large ? data + 3 : data + 2;
    uint64_t expected = 0;

    for (uint32_t i = 0; i < 8; i++) {
        expected = (expected << 8) | crcData[i];
    }

    uint64_t crc;
    SCrc64Compute(&data[headerSize], length - headerSize, &crc, 7);

    return expected == crc;
}

// OFFSET: 0x0054DBD0/0x1401A7D10
static SockAddr ResolveHostAddr(const char* host, bool preferIPv6) {
    SockAddr result;
    char name[256];

    if (!host) {
        gethostname(name, sizeof(name));
        host = name;
    }

    addrinfo* info;
    int32_t error = getaddrinfo(host, NULL, NULL, &info);
    WSASetLastError(error);

    if (error) {
        return result;
    }

    for (addrinfo* entry = info; entry; entry = entry->ai_next) {
        if (preferIPv6 && entry->ai_family == AF_INET6) {
            memcpy(result.m_ipv6, &reinterpret_cast<const sockaddr_in6*>(entry->ai_addr)->sin6_addr, sizeof(result.m_ipv6));
            result.m_type = SOCKADDR_IPV6;
            return result;
        }

        if (entry->ai_family == AF_INET) {
            result.m_ipv4 = reinterpret_cast<const sockaddr_in*>(entry->ai_addr)->sin_addr.s_addr;
            result.m_type = SOCKADDR_IPV4;

            if (!preferIPv6) {
                return result;
            }
        }
    }

    return result;
}

// OFFSET: 0x0054DCA0/0x1401A7E10
SockAddr SockAddrFromSockaddr(const sockaddr* addr) {
    SockAddr result;

    if (addr->sa_family == AF_INET) {
        result.m_ipv4 = reinterpret_cast<const sockaddr_in*>(addr)->sin_addr.s_addr;
        result.m_type = SOCKADDR_IPV4;
    } else if (addr->sa_family == AF_INET6) {
        memcpy(result.m_ipv6, &reinterpret_cast<const sockaddr_in6*>(addr)->sin6_addr, sizeof(result.m_ipv6));
        result.m_type = SOCKADDR_IPV6;
    }

    return result;
}

static void LogSendDepthExceeded(WowConnection* connection) {
    SockAddr peerAddr = SockAddrFromSockaddr(reinterpret_cast<const sockaddr*>(&connection->m_peer.peerAddr));
    SockAddrString peer(peerAddr, reinterpret_cast<const sockaddr_in*>(&connection->m_peer.peerAddr)->sin_port);
    DebugLog(0x10001, 10, "WowConnection: MaxSendDepth Reached! m_sendDepth=%d, m_maxSendDepth=%d, Dropping connection to %s", connection->m_sendDepth, connection->m_maxSendDepth, peer.m_str);
}

// OFFSET: 0x0054D420/0x1401A73F0
bool WowConnection::GetLocalAddr(sockaddr* addr) {
    int32_t length = 32;

    return getsockname(this->m_sock, addr, &length) == 0;
}

// OFFSET: 0x0054D5E0/0x1401A7610
void WowConnection::AddRef() {
    this->m_refCount.Increment();
}

// OFFSET: 0x0054D7D0/0x1401A7850
void WowConnection::DestroyOsNet() {
    s_osNetDestroyed = 1;

    if (WowConnection::s_network) {
        WowConnection::s_network->Stop();
        WowConnection::s_network->PlatformDestroy();
        delete WowConnection::s_network;
        WowConnection::s_network = NULL;
        WDataStore::StaticDestroy();
    }
}

// OFFSET: 0x0054D2B0
void WowConnection::FreeSendNode(WowConnection::SENDNODE* node) {
    WDataStore::FreeBuffer(node, node->m_allocsize);
}

// OFFSET: 0x0054DD00/0x1401A7E60
void WowConnection::CloseSocket(int32_t sock) {
    closesocket(sock);

    if (sock >= 0) {
        WowConnection::s_idLock.Enter();
        WowConnection::s_idBits.m_bits.m_data[static_cast<uint32_t>(sock) >> 5] &= ~(1 << (sock & 0x1F));
        WowConnection::s_idLock.Leave();
    }
}

// OFFSET: 0x0054DE20
void WowConnection::DoStreamReads() {
    char buffer[4096];
    int32_t bytes;

    while (true) {
        do {
            bytes = recv(this->m_sock, buffer, sizeof(buffer), 0);

            if (bytes > 0) {
                this->m_bytesReceived += bytes;
            }
        } while (bytes < 0 && WSAGetLastError() == WSAEINTR);

        if (bytes <= 0) {
            break;
        }

        this->AcquireResponseRef();
        WowConnectionResponse* response = this->m_response;
        this->m_lock.Leave();

        if (response) {
            response->WCDataReady(this, OsGetAsyncTimeMsPrecise(), reinterpret_cast<uint8_t*>(buffer), bytes);
        }

        this->m_lock.Enter();
        this->ReleaseResponseRef();

        if (this->m_connState == WOWC_DISCONNECTING) {
            return;
        }
    }

    if (bytes < 0 && WSAGetLastError() == WSAEWOULDBLOCK) {
        return;
    }

    this->AcquireResponseRef();
    WowConnectionResponse* response = this->m_response;
    WowConnection::s_network->Remove(this);
    WowConnection::CloseSocket(this->m_sock);
    this->SetState(WOWC_DISCONNECTED);

    int32_t sock = this->m_sock;
    this->m_lock.Leave();

    if (response && sock >= 0) {
        response->WCDisconnected(this, OsGetAsyncTimeMsPrecise(), &this->m_peer);
    }

    this->m_lock.Enter();
    this->m_sock = -1;
    this->ReleaseResponseRef();
}

// OFFSET: 0x0054E000/0x1401A8230
WowConnection::WowConnection(int32_t sock, const NETADDR* addr, WowConnectionResponse* response) {
    this->Init(response, NULL);

    int32_t length = 32;
    getpeername(sock, reinterpret_cast<sockaddr*>(&this->m_peer.peerAddr), &length);
    length = 32;
    getsockname(sock, reinterpret_cast<sockaddr*>(&this->m_peer.selfAddr), &length);
    this->m_sock = sock;
    this->m_connState = WOWC_CONNECTED;
}

// OFFSET: 0x0054E0F0/0x1401A8350
WowConnection::WowConnection(WowConnectionResponse* response, void (*threadInit)()) {
    this->Init(response, threadInit);
    this->m_sock = -1;
}

// OFFSET: 0x0054E1B0/0x1401A8430
WowConnection::~WowConnection() {
    WowConnection::s_numConnections.Decrement();

    if (WowConnection::s_network) {
        WowConnection::s_network->Remove(this);
        WowConnection::s_network->PlatformDestruct(this);
    }

    if (this->m_sock >= 0) {
        WowConnection::CloseSocket(this->m_sock);
        this->m_sock = -1;
    }

    if (this->m_readBuffer) {
        SMemFree(this->m_readBuffer, __FILE__, __LINE__, 0);
    }

    delete this->m_cryptPanamaSendCypher;
    delete this->m_cryptPanamaRecvCypher;

    this->FreeSendList();
    memset(&this->m_receiveKey, 0, sizeof(this->m_receiveKey));
    memset(&this->m_sendKey, 0, sizeof(this->m_sendKey));
}

// OFFSET: 0x0054E5A0/0x1401A8970
int32_t WowConnection::SendRaw(uint8_t* data, int32_t size) {
    WowConnection::s_sendBytes += size;

    int32_t result = 2;
    this->m_lock.Enter();

    if (size <= 0 || this->m_connState != WOWC_CONNECTED) {
        this->m_lock.Leave();
        return result;
    }

    int32_t sent;

    if (this->m_sendList.Head()) {
        WowConnection::SENDNODE* node = this->NewSendNodeRaw(data, size);
        node->m_queueTime = OsGetAsyncTimeMs();
        this->m_sendList.LinkToTail(node);
        result = 1;
        this->m_sendDepth++;
        this->m_sendDepthBytes += node->m_size;

        if (this->m_sendDepth >= this->m_maxSendDepth) {
            this->SetState(WOWC_DISCONNECTING);

            if (this->m_response) {
                this->m_response->NotifySendDepthExceeded(this);
            }

            LogSendDepthExceeded(this);
            this->m_lock.Leave();

            return 2;
        }

        WowConnection::s_network->PlatformChangeState(this, this->m_connState);
        this->m_lock.Leave();

        return result;
    }

    sent = send(this->m_sock, reinterpret_cast<char*>(data), size, 0);

    if (sent > 0) {
        this->m_bytesSent += sent;
    }

    if (sent == size) {
        this->m_lock.Leave();
        return 0;
    }

    if (sent < 0) {
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            sent = 0;
        } else if (this->m_sendDepth < this->m_maxSendDepth) {
            this->SetState(WOWC_DISCONNECTING);
            this->m_lock.Leave();
            return 2;
        }
    }

    WowConnection::SENDNODE* node = this->NewSendNodeRaw(data + sent, size - sent);
    node->m_queueTime = OsGetAsyncTimeMs();
    this->m_sendList.LinkToTail(node);
    result = 1;
    this->m_sendDepth++;
    this->m_sendDepthBytes += node->m_size;
    WowConnection::s_network->PlatformChangeState(this, this->m_connState);
    this->m_lock.Leave();

    return result;
}

// OFFSET: 0x0054E7B0/0x1401A8C30
void WowConnection::DoMessageReads() {
    if (!this->m_readBuffer) {
        this->m_readBuffer = static_cast<uint8_t*>(SMemAlloc(1024, __FILE__, __LINE__, 0));
        this->m_readBufferSize = 1024;
        this->m_readBytes = 0;
    }

    while (true) {
        WowConnection::SENDNODE* node = this->m_lagTestRecvList.Head();

        if (!node || static_cast<int32_t>(OsGetAsyncTimeMsPrecise() - node->m_sendTime) < 0) {
            break;
        }

        CDataStore msg(node->m_data, node->m_size);

        this->AcquireResponseRef();
        WowConnectionResponse* response = this->m_response;

        if (response) {
            this->m_lock.Leave();
            response->WCMessageReady(this, OsGetAsyncTimeMsPrecise(), &msg);
            this->m_lock.Enter();
        }

        this->ReleaseResponseRef();
        this->m_lagTestRecvList.UnlinkNode(node);
        WDataStore::FreeBuffer(node, node->m_allocsize);
    }

    int32_t bytes;

    do {
        this->AcquireResponseRef();
        bool block = this->m_response ? this->m_response->WCShouldBlockReads() : false;
        this->ReleaseResponseRef();

        if (block) {
            return;
        }

        int32_t readBytes = this->m_readBytes;
        bool checksum = this->m_checksumPackets;
        uint8_t* buffer = this->m_readBuffer;
        int32_t headerSize = -1;

        if (readBytes >= 1) {
            if (buffer[0] & 0x80) {
                headerSize = checksum ? 11 : 3;
            } else {
                headerSize = checksum ? 10 : 2;
            }
        }

        int32_t packetSize = -1;

        if (readBytes >= 2) {
            if (!(buffer[0] & 0x80)) {
                packetSize = ((buffer[0] & 0x7F) << 8) | buffer[1];
            } else if (readBytes >= 3) {
                packetSize = (((buffer[0] & 0x7F) << 8 | buffer[1]) << 8) | buffer[2];
            }
        }

        int32_t totalSize = headerSize < 0 || packetSize < 0 ? -1 : packetSize + headerSize;

        if (headerSize < 0) {
            headerSize = 2;
        }

        if (this->m_readBytes >= this->m_readBufferSize) {
            this->m_readBuffer = static_cast<uint8_t*>(SMemReAlloc(buffer, 2 * this->m_readBufferSize, __FILE__, __LINE__, 0));
            this->m_readBufferSize *= 2;

            if (static_cast<uint32_t>(this->m_readBufferSize) > 32000000) {
                this->Disconnect();
                return;
            }
        }

        int32_t toRead;

        if (totalSize >= 0) {
            toRead = this->m_readBufferSize - this->m_readBytes;

            if (toRead >= totalSize - this->m_readBytes) {
                toRead = totalSize - this->m_readBytes;
            }
        } else {
            toRead = headerSize - this->m_readBytes;
        }

        bytes = 0;

        if (toRead > 0) {
            do {
                bytes = recv(this->m_sock, reinterpret_cast<char*>(this->m_readBuffer + this->m_readBytes), toRead, 0);

                if (bytes > 0) {
                    this->m_bytesReceived += bytes;
                }
            } while (bytes < 0 && WSAGetLastError() == WSAEINTR);

            if (bytes <= 0) {
                if (bytes < 0 && WSAGetLastError() == WSAEWOULDBLOCK) {
                    return;
                }

                this->AcquireResponseRef();
                WowConnectionResponse* response = this->m_response;
                WowConnection::s_network->Remove(this);
                WowConnection::CloseSocket(this->m_sock);
                this->SetState(WOWC_DISCONNECTED);

                int32_t sock = this->m_sock;
                this->m_lock.Leave();

                if (response && sock >= 0) {
                    response->WCDisconnected(this, OsGetAsyncTimeMsPrecise(), &this->m_peer);
                }

                this->m_lock.Enter();
                this->m_sock = -1;
                this->ReleaseResponseRef();
                return;
            }
        }

        if (this->m_encryptionType == WC_ENCRYPT_ARC4) {
            int32_t length = headerSize + this->m_recvEncryptBytes - this->m_readBytes;

            if (length <= 0) {
                length = 0;
            }

            if (length >= bytes) {
                length = bytes;
            }

            SARC4ProcessBuffer(this->m_readBuffer + this->m_readBytes, length, &this->m_receiveKey, &this->m_receiveKey);
        } else if (this->m_encryptionType == WC_ENCRYPT_PANAMA) {
            this->m_cryptPanamaRecvCypher->ProcessBuffer(this->m_readBuffer + this->m_readBytes, bytes);
        }

        this->m_readBytes += bytes;

        if (totalSize >= 0 && this->m_readBytes >= totalSize) {
            if (!checksum || VerifyPacketChecksum(this->m_readBuffer, this->m_readBytes)) {
                CDataStore msg(this->m_readBuffer + headerSize, totalSize - headerSize);

                this->AcquireResponseRef();
                WowConnectionResponse* response = this->m_response;

                if (WowConnection::s_lagTestDelayMin || this->m_lagTestRecvList.Head()) {
                    WowConnection::SENDNODE* node = this->NewSendNodeRaw(this->m_readBuffer + headerSize, totalSize - headerSize);
                    this->m_lagTestRecvList.LinkToTail(node);
                } else if (response) {
                    this->m_lock.Leave();
                    response->WCMessageReady(this, OsGetAsyncTimeMsPrecise(), &msg);
                    this->m_lock.Enter();
                }

                this->m_readBytes = 0;
                this->ReleaseResponseRef();
            } else {
                this->SetState(WOWC_DISCONNECTING);
            }
        }
    } while (bytes > 0);
}

// OFFSET: 0x0054EE10/0x1401A9410
void WowConnection::MarkSocketId(int32_t sock) {
    if (sock < 0) {
        return;
    }

    WowConnection::s_idLock.Enter();

    if (sock >= static_cast<int32_t>(WowConnection::s_idBits.m_count)) {
        uint32_t count = WowConnection::s_idBits.m_count;

        do {
            count *= 2;
        } while (sock >= static_cast<int32_t>(count));

        WowConnection::IDBITS bits;
        bits.m_count = 0;
        bits.SetCount(WowConnection::s_idBits.m_count);

        if (bits.m_count) {
            memset(bits.m_bits.m_data, 0, sizeof(uint32_t) * bits.m_bits.m_count);
        }

        bits.Or(WowConnection::s_idBits);
        WowConnection::s_idBits.SetCount(count);

        if (WowConnection::s_idBits.m_count) {
            memset(WowConnection::s_idBits.m_bits.m_data, 0, sizeof(uint32_t) * WowConnection::s_idBits.m_bits.m_count);
        }

        WowConnection::s_idBits.Or(bits);
    }

    WowConnection::s_idBits.m_bits.m_data[static_cast<uint32_t>(sock) >> 5] |= 1 << (sock & 0x1F);
    WowConnection::s_idLock.Leave();
}

// OFFSET: 0x0054EF10/0x1401A9620
int32_t WowConnection::CreateSocket(int32_t type) {
    int32_t family;

    if (type == SOCKADDR_IPV6) {
        family = AF_INET6;
    } else if (type == SOCKADDR_HOSTNAME) {
        family = AF_UNIX;
    } else {
        family = AF_INET;
    }

    int32_t sock = static_cast<int32_t>(socket(family, SOCK_STREAM, 0));

    if (sock < 0 && family == AF_INET6 && WSAGetLastError() == WSAEAFNOSUPPORT) {
        CVar* useIPv6 = CVar::Lookup("useIPv6");

        if (useIPv6) {
            useIPv6->Set(0, true, false, false, true);
        }
    }

    WowConnection::MarkSocketId(sock);

    return sock;
}

// OFFSET: 0x0054EF90/0x1401A96B0
int32_t WowConnection::Release() {
    int32_t refCount = this->m_refCount.Decrement();

    if (refCount <= 0) {
        if (WowConnection::s_network) {
            WowConnection::s_network->Delete(this);
        } else {
            delete this;
        }
    }

    return refCount;
}

// OFFSET: 0x0054EFE0/0x1401A9720
void WowConnection::DoDisconnect() {
    this->m_lock.Enter();

    if (this->m_sock >= 0) {
        WowConnection::s_network->Remove(this);
        WowConnection::CloseSocket(this->m_sock);
    }

    this->SetState(WOWC_DISCONNECTED);
    this->AddRef();
    this->AcquireResponseRef();

    WowConnectionResponse* response = this->m_response;
    int32_t sock = this->m_sock;
    this->m_lock.Leave();

    if (response && sock >= 0) {
        response->WCDisconnected(this, OsGetAsyncTimeMsPrecise(), &this->m_peer);
    }

    this->m_lock.Enter();
    this->m_sock = -1;
    this->ReleaseResponseRef();
    this->m_lock.Leave();
    this->Release();
}

// OFFSET: 0x0054F170/0x1401A9920
int32_t WowConnection::Send(CDataStore* msg, int32_t headroom) {
    uint32_t size = msg->m_size;
    void* data;
    msg->GetDataInSitu(data, size);

    bool checksum = this->m_checksumPackets;
    int32_t result = 2;
    uint32_t headerSize = (size > 0x7FFF) + 2 + (checksum ? 8 : 0);
    WowConnection::s_sendBytes += size;

    this->m_lock.Enter();

    if (size > 0 && this->m_connState == WOWC_CONNECTED) {
        if (WowConnection::s_lagTestDelayMin || this->m_sendList.Head()) {
            WowConnection::SENDNODE* node = this->NewSendNodePacket(data, size, checksum);

            if (this->m_encryptionType == WC_ENCRYPT_ARC4) {
                uint32_t length = node->m_size + this->m_sendEncryptBytes - node->m_datasize;

                if (length > node->m_size) {
                    length = node->m_size;
                }

                SARC4ProcessBuffer(node->m_data, length, &this->m_sendKey, &this->m_sendKey);
            } else if (this->m_encryptionType == WC_ENCRYPT_PANAMA) {
                this->m_cryptPanamaSendCypher->ProcessBuffer(node->m_data, node->m_size);
            }

            node->m_queueTime = OsGetAsyncTimeMsPrecise();
            this->m_sendList.LinkToTail(node);
            this->m_sendDepth++;
            this->m_sendDepthBytes += node->m_size;
            WowConnection::s_network->PlatformChangeState(this, this->m_connState);

            if (this->m_sendDepth < this->m_maxSendDepth) {
                result = 1;
            } else {
                this->SetState(WOWC_DISCONNECTING);
                result = 2;

                if (this->m_response) {
                    this->m_response->NotifySendDepthExceeded(this);
                }

                LogSendDepthExceeded(this);
            }
        } else {
            WowConnection::SENDNODE* node;
            bool stackNode;

            if (size >= 0x2000) {
                stackNode = false;
                node = this->NewSendNodePacket(data, size, checksum);
            } else {
                stackNode = true;

                if (static_cast<uint32_t>(headroom) < headerSize || this->m_encryptionType == WC_ENCRYPT_PANAMA) {
                    void* buffer = alloca(size + headerSize + sizeof(WowConnection::SENDNODE));
                    node = new (buffer) WowConnection::SENDNODE();
                    node->m_data = reinterpret_cast<uint8_t*>(node + 1);
                    node->m_size = size + headerSize;
                    node->SetPacket(data, size, checksum);
                } else {
                    void* buffer = alloca(sizeof(WowConnection::SENDNODE));
                    node = new (buffer) WowConnection::SENDNODE();
                    node->InitPacket(static_cast<uint8_t*>(data), size, NULL, 0, checksum);
                }
            }

            if (this->m_encryptionType == WC_ENCRYPT_ARC4) {
                uint32_t length = node->m_size + this->m_sendEncryptBytes - node->m_datasize;

                if (length > node->m_size) {
                    length = node->m_size;
                }

                this->Encrypt(node->m_data, length);
            } else if (this->m_encryptionType == WC_ENCRYPT_PANAMA) {
                this->m_cryptPanamaSendCypher->ProcessBuffer(node->m_data, node->m_size);
            }

            int32_t sent = send(this->m_sock, reinterpret_cast<char*>(node->m_data), node->m_size, 0);

            if (sent > 0) {
                this->m_bytesSent += sent;
            }

            if (sent == static_cast<int32_t>(node->m_size)) {
                if (!stackNode) {
                    WowConnection::FreeSendNode(node);
                }

                result = 0;
            } else {
                bool wouldBlock = WSAGetLastError() == WSAEWOULDBLOCK;

                if (stackNode) {
                    node = WowConnection::CopySendNode(node);
                }

                if (sent > 0) {
                    node->m_offset += sent;
                    result = 1;
                } else if (!wouldBlock) {
                    this->SetState(WOWC_DISCONNECTING);
                    result = 2;
                }

                node->m_queueTime = OsGetAsyncTimeMs();
                this->m_sendList.LinkToTail(node);
                this->m_sendDepth++;
                this->m_sendDepthBytes += node->m_size;

                if (this->m_sendDepth >= this->m_maxSendDepth) {
                    this->SetState(WOWC_DISCONNECTING);
                    result = 2;

                    if (this->m_response) {
                        this->m_response->NotifySendDepthExceeded(this);
                    }

                    LogSendDepthExceeded(this);
                }

                WowConnection::s_network->PlatformChangeState(this, this->m_connState);
            }
        }
    }

    this->m_lock.Leave();

    return result;
}

// OFFSET: 0x0054F580/0x1401A9ED0
void WowConnection::CheckConnect() {
    int32_t error;
    int32_t length = sizeof(error);

    if (getsockopt(this->m_sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &length)) {
        return;
    }

    if (error) {
        WowConnection::s_network->Remove(this);
        WowConnection::CloseSocket(this->m_sock);
        this->m_sock = -1;
        this->SetState(WOWC_DISCONNECTED);
        this->AddRef();
        this->AcquireResponseRef();

        WowConnectionResponse* response = this->m_response;
        this->m_lock.Leave();

        if (response) {
            response->WCCantConnect(this, OsGetAsyncTimeMsPrecise(), &this->m_peer);
        }
    } else {
        this->SetState(WOWC_CONNECTED);
        this->AddRef();
        this->AcquireResponseRef();

        WowConnectionResponse* response = this->m_response;
        this->m_lock.Leave();

        int32_t nameLength = 32;
        getpeername(this->m_sock, reinterpret_cast<sockaddr*>(&this->m_peer.peerAddr), &nameLength);
        nameLength = 32;
        getsockname(this->m_sock, reinterpret_cast<sockaddr*>(&this->m_peer.selfAddr), &nameLength);

        if (response) {
            response->WCConnected(this, NULL, OsGetAsyncTimeMsPrecise(), &this->m_peer);
        }
    }

    this->m_lock.Enter();
    this->ReleaseResponseRef();
    this->Release();
}

// OFFSET: 0x0054F760/0x1401AA140
void WowConnection::CheckAccept() {
    for (int32_t i = 0; i < 10000; i++) {
        sockaddr addr[8];
        memset(addr, 0, sizeof(addr));
        int32_t addrLength = sizeof(addr);
        int32_t sock = static_cast<int32_t>(accept(this->m_sock, addr, &addrLength));

        if (sock < 0) {
            break;
        }

        if (WowConnection::s_verifyAddr) {
            sockaddr peer[2];
            int32_t peerLength = 32;
            getpeername(sock, peer, &peerLength);

            if (!WowConnection::s_verifyAddr(reinterpret_cast<const NETADDR*>(peer))) {
                closesocket(sock);
                continue;
            }
        }

        WowConnection::MarkSocketId(sock);
        u_long nonBlocking = 1;
        ioctlsocket(sock, FIONBIO, &nonBlocking);

        WowConnection* inbound = new (__FILE__, __LINE__) WowConnection(sock, reinterpret_cast<const NETADDR*>(addr), this->m_response);
        inbound->AddRef();
        this->AddRef();
        this->AcquireResponseRef();

        WowConnectionResponse* response = this->m_response;
        this->m_lock.Leave();

        if (response) {
            response->WCConnected(this, inbound, OsGetAsyncTimeMs(), &inbound->m_peer);
        }

        WowConnection::s_network->Add(inbound);
        WowConnection::s_network->PlatformChangeState(inbound, inbound->m_connState);
        inbound->Release();

        this->m_lock.Enter();
        this->ReleaseResponseRef();
        this->Release();
    }
}

// OFFSET: 0x0054F990/0x1401AA3F0
void WowConnection::DoWrites() {
    this->AddRef();
    this->m_lock.Enter();

    bool disconnected = false;

    if (this->m_connState == WOWC_CONNECTING) {
        this->CheckConnect();
    } else if (this->m_sendList.Head()) {
        while (true) {
            WowConnection::SENDNODE* node = this->m_sendList.Head();

            if (!node || WowConnection::s_lagTestDelayMin && static_cast<int32_t>(OsGetAsyncTimeMsPrecise() - node->m_sendTime) < 0) {
                break;
            }

            uint32_t bytes = node->m_size - node->m_offset;

            if (bytes >= 0x400) {
                bytes = 0x400;
            }

            if (bytes) {
                int32_t sent = send(this->m_sock, reinterpret_cast<char*>(node->m_data + node->m_offset), bytes, 0);

                if (sent > 0) {
                    this->m_bytesSent += sent;
                }

                if (sent != static_cast<int32_t>(bytes)) {
                    if (sent > 0) {
                        node->m_offset += sent;
                        break;
                    }

                    if (sent >= 0 || WSAGetLastError() == WSAEWOULDBLOCK) {
                        break;
                    }

                    disconnected = true;
                }
            }

            if (!disconnected) {
                if (bytes == node->m_size - node->m_offset) {
                    this->m_sendList.UnlinkNode(node);
                    this->m_sendDepth--;
                    this->m_sendDepthBytes -= node->m_size;
                    WDataStore::FreeBuffer(node, node->m_allocsize);
                } else {
                    node->m_offset += bytes;
                }
            }

            if (!this->m_sendList.Head() || disconnected) {
                break;
            }
        }
    } else {
        WowConnection::s_network->PlatformChangeState(this, this->m_connState);
    }

    WowConnectionResponse* response = NULL;
    int32_t sock = -1;
    bool writeReady = false;

    if (disconnected) {
        this->AcquireResponseRef();
        response = this->m_response;
        WowConnection::s_network->Remove(this);
        WowConnection::CloseSocket(this->m_sock);
        sock = this->m_sock;
        this->SetState(WOWC_DISCONNECTED);
    } else if (this->m_notifyWriteReady && !this->m_sendList.Head()) {
        writeReady = true;
        this->m_notifyWriteReady = false;
        this->AcquireResponseRef();
        response = this->m_response;
    }

    this->m_lock.Leave();

    if (disconnected) {
        if (response && sock >= 0) {
            response->WCDisconnected(this, OsGetAsyncTimeMsPrecise(), &this->m_peer);
        }

        this->m_lock.Enter();
        this->m_sock = -1;
        this->ReleaseResponseRef();
        this->m_lock.Leave();
    } else if (writeReady) {
        if (response) {
            response->WCWriteReady(this);
        }

        this->m_lock.Enter();
        this->ReleaseResponseRef();
        this->m_lock.Leave();
    }

    this->Release();
}

// OFFSET: 0x0054FCA0/0x1401AA760
void WowConnection::DoReads() {
    this->AddRef();
    this->m_lock.Enter();

    if (this->m_connState == WOWC_LISTENING) {
        this->CheckAccept();
    } else if (this->m_connState == WOWC_CONNECTED) {
        if (this->m_type == WOWC_TYPE_MESSAGES) {
            this->DoMessageReads();
        } else if (this->m_type == WOWC_TYPE_STREAM) {
            this->DoStreamReads();
        }
    }

    this->m_lock.Leave();
    this->Release();
}

// OFFSET: 0x0054FD40/0x1401AA7E0
void WowConnection::DoExceptions() {
    this->AddRef();
    this->m_lock.Enter();

    if (this->m_connState == WOWC_CONNECTING) {
        this->CheckConnect();
    }

    this->m_lock.Leave();
    this->Release();
}

// OFFSET: 0x0054FDB0/0x1401AA830
void WowConnection::StartConnect() {
    if (this->m_sock >= 0) {
        if (this->m_netLink.IsLinked()) {
            WowConnection::s_network->Remove(this);
        }

        WowConnection::CloseSocket(this->m_sock);
        this->m_sock = -1;
    }

    this->m_lock.Enter();
    this->m_sock = WowConnection::CreateSocket(this->m_connectAddress.m_type);

    bool connecting = false;

    if (this->m_sock >= 0) {
        u_long nonBlocking = 1;
        ioctlsocket(this->m_sock, FIONBIO, &nonBlocking);

        sockaddr_in6 address6;
        sockaddr_in address4;
        const sockaddr* address = NULL;
        int32_t addressLength = 0;

        if (this->m_connectAddress.m_type == SOCKADDR_IPV6) {
            memset(&address6, 0, sizeof(address6));
            address6.sin6_family = AF_INET6;
            address6.sin6_port = htons(this->m_connectPort);
            memcpy(&address6.sin6_addr, this->m_connectAddress.m_ipv6, sizeof(this->m_connectAddress.m_ipv6));
            address = reinterpret_cast<const sockaddr*>(&address6);
            addressLength = sizeof(address6);
        } else if (this->m_connectAddress.m_type == SOCKADDR_IPV4) {
            memset(&address4, 0, sizeof(address4));
            address4.sin_family = AF_INET;
            address4.sin_port = htons(this->m_connectPort);
            address4.sin_addr.s_addr = this->m_connectAddress.m_ipv4;
            address = reinterpret_cast<const sockaddr*>(&address4);
            addressLength = sizeof(address4);
        }

        if (!address) {
            this->m_lock.Leave();
            this->AcquireResponseRef();

            if (this->m_response) {
                this->m_response->WCCantConnect(this, OsGetAsyncTimeMsPrecise(), &this->m_peer);
            }

            this->ReleaseResponseRef();
            return;
        }

        if (!this->m_netLink.IsLinked()) {
            WowConnection::s_network->Add(this);
        }

        this->m_readBytes = 0;
        this->SetState(WOWC_CONNECTING);

        if (connect(this->m_sock, address, addressLength) >= 0 || WSAGetLastError() == WSAEWOULDBLOCK) {
            connecting = true;
        } else {
            static_cast<void>(errno);
            WowConnection::s_network->Remove(this);
            WowConnection::CloseSocket(this->m_sock);
            this->m_sock = -1;
        }
    }

    if (!connecting) {
        this->SetState(WOWC_ERROR);
    }

    this->m_lock.Leave();

    if (!connecting) {
        this->AcquireResponseRef();

        if (this->m_response) {
            this->m_response->WCCantConnect(this, OsGetAsyncTimeMsPrecise(), &this->m_peer);
        }

        this->ReleaseResponseRef();
    }
}

// OFFSET: 0x00550000/0x1401AAB50
bool WowConnection::Connect(const SockAddr& address, uint16_t port, int32_t timeout) {
    memcpy(&this->m_connectAddress, &address, sizeof(address));
    this->m_connectPort = port;
    this->StartConnect();

    return true;
}

// OFFSET: 0x00550050/0x1401AAB90
bool WowConnection::Connect(const char* host, uint16_t port, int32_t timeout) {
    memset(&this->m_connectAddress, 0, sizeof(this->m_connectAddress));

    if (!this->m_connectAddress.Parse(host)) {
        SockAddr address = ResolveHostAddr(host, this->m_preferIPv6);
        memcpy(&this->m_connectAddress, &address, sizeof(address));
    }

    this->m_connectPort = port;
    this->StartConnect();

    return true;
}

// OFFSET: 0x005500F0/0x1401AAC50
int32_t WowConnection::InitOsNet(bool (*netAddrFilter)(const NETADDR* addr), void (*threadInit)(), int32_t numThreads, bool useEngine) {
    if (WowConnection::s_network) {
        return 1;
    }

    WowConnection::s_idBits.SetCount(256);

    if (WowConnection::s_idBits.m_count) {
        memset(WowConnection::s_idBits.m_bits.m_data, 0, sizeof(uint32_t) * WowConnection::s_idBits.m_bits.m_count);
    }

    WDataStore::StaticInitialize();
    WowConnection::s_verifyAddr = netAddrFilter;
    s_osNetDestroyed = 0;

    if (static_cast<uint32_t>(numThreads) > 32) {
        numThreads = 32;
    }

    WowConnection::s_network = new (__FILE__, __LINE__) WowConnectionNet(numThreads, threadInit);
    WowConnection::s_network->PlatformInit(useEngine);
    WowConnection::s_network->Start();

    return 1;
}

// OFFSET: 0x005501C0/0x1401AAD70
bool WowConnection::Connect(const char* address, int32_t timeout) {
    char host[256];
    const char* last = strrchr(address, ':');
    const char* first = strchr(address, ':');

    if (last && (last == first || last[-1] == ']')) {
        this->m_connectPort = SStrToInt(last + 1);

        if (last != first && last[-1] == ']' && *address == '[') {
            address++;
            last--;
        }

        uint32_t length = static_cast<uint32_t>(last - address) + 1;

        if (length > sizeof(host)) {
            length = sizeof(host);
        }

        SStrCopy(host, address, length);
    } else {
        this->m_connectPort = 0;
        SStrCopy(host, address, sizeof(host));
    }

    this->Connect(host, this->m_connectPort, timeout);

    return true;
}
