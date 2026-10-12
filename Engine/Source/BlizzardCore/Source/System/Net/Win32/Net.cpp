#if defined(WOWDECOMP_ARCH_32)

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <winsock2.h>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/Net/Net.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"

// STATIC: 0x00EB57D4
int32_t System_Net::s_lastError;

// OFFSET: 0x007AD900/0x1404A3590
int32_t Blizzard::Net::TranslateError(int32_t context) {
    int32_t error = WSAGetLastError();

    if (!error) {
        return 0;
    }

    if (context == 7000) {
        if (error == WSAEWOULDBLOCK) {
            return 9000;
        }
    } else if (context > 7000 && context <= 7002 && error == WSAEWOULDBLOCK) {
        return 9001;
    }

    return error;
}

// OFFSET: 0x007AD710
bool Blizzard::Net::SetBlocking(Socket s) {
    u_long nonBlocking = 0;
    int result = ioctlsocket(s, FIONBIO, &nonBlocking);

    if (result) {
        System_Net::s_lastError = 8002;
    }

    return result == 0;
}

// OFFSET: 0x007AD750
bool Blizzard::Net::SetNonBlocking(Socket s) {
    u_long nonBlocking = 1;
    int result = ioctlsocket(s, FIONBIO, &nonBlocking);

    if (result) {
        System_Net::s_lastError = 8001;
    }

    return result == 0;
}

// OFFSET: 0x007AD790/0x1404A33B0
bool Blizzard::Net::Select(Socket* readSockets, Socket* writeSockets, int64_t timeout) {
    fd_set readSet;
    fd_set writeSet;
    readSet.fd_count = 0;
    writeSet.fd_count = 0;
    int32_t total = 0;

    if (readSockets) {
        for (Socket* s = readSockets; *s != INVALID_SOCKET; s++, total++) {
            if (readSet.fd_count < FD_SETSIZE) {
                readSet.fd_array[readSet.fd_count++] = *s;
            }
        }
    }

    if (writeSockets) {
        for (Socket* s = writeSockets; *s != INVALID_SOCKET; s++, total++) {
            if (writeSet.fd_count < FD_SETSIZE) {
                writeSet.fd_array[writeSet.fd_count++] = *s;
            }
        }
    }

    if (!total) {
        if (timeout > 0) {
            Sleep(static_cast<DWORD>(timeout / 1000));
        }

        return false;
    }

    timeval time;
    time.tv_sec = static_cast<long>(timeout / 1000000);
    time.tv_usec = static_cast<long>(timeout % 1000000);
    int result = select(0, readSockets ? &readSet : NULL, writeSockets ? &writeSet : NULL, NULL, &time);

    if (readSockets) {
        Socket* out = readSockets;
        int32_t count = readSet.fd_count;

        if (count > 0) {
            memcpy(readSockets, readSet.fd_array, count * sizeof(Socket));
            out = &readSockets[count];
        }

        *out = INVALID_SOCKET;
    }

    if (writeSockets) {
        Socket* out = writeSockets;
        int32_t count = writeSet.fd_count;

        if (count > 0) {
            memcpy(writeSockets, writeSet.fd_array, count * sizeof(Socket));
            out = &writeSockets[count];
        }

        *out = INVALID_SOCKET;
    }

    return result > 0;
}

// OFFSET: 0x007B58D0/0x1404AB870
bool Blizzard::Net::CreateSocket(Socket* s, int32_t receiveBuffer, int32_t sendBuffer) {
    *s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*s == INVALID_SOCKET) {
        System_Net::s_lastError = 8000;
        return false;
    }

    if (receiveBuffer != -1) {
        if (!receiveBuffer) {
            receiveBuffer = 0x4000;
        }

        int32_t current;
        int length = sizeof(current);
        getsockopt(*s, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&current), &length);

        if (current != receiveBuffer) {
            current = receiveBuffer;
            setsockopt(*s, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&current), sizeof(current));
        }
    }

    if (sendBuffer != -1) {
        if (!sendBuffer) {
            sendBuffer = 0x4000;
        }

        int32_t current;
        int length = sizeof(current);
        getsockopt(*s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&current), &length);

        if (current != sendBuffer) {
            current = sendBuffer;
            setsockopt(*s, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&current), sizeof(current));
        }
    }

    return true;
}

// OFFSET: 0x007B59B0/0x1404AB9A0
bool Blizzard::Net::CloseSocket(Socket* s, bool linger) {
    if (*s != INVALID_SOCKET) {
        int error = WSAGetLastError();
        SetBlocking(*s);

        if (linger) {
            u_long value = 1;
            setsockopt(*s, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*>(&value), sizeof(value));
        }

        closesocket(*s);
        *s = INVALID_SOCKET;
        WSAGetLastError();
        WSASetLastError(error);
    }

    return true;
}

// OFFSET: 0x007BD190/0x1404B57F0
bool Blizzard::Net::Connect(Socket s, const sockaddr* address) {
    if (!connect(s, address, sizeof(sockaddr_in))) {
        return true;
    }

    int error = WSAGetLastError();

    if (!error || (error != WSAEWOULDBLOCK && error != 9000)) {
        CloseSocket(&s, false);
        System_Net::s_lastError = 8009;
    }

    return false;
}

// OFFSET: 0x007C0960/0x1404BA490
bool Blizzard::Net::ConnectAddress(sockaddr_in* address, Socket* s, int32_t receiveBuffer, int32_t sendBuffer, bool blocking) {
    address->sin_family = AF_INET;

    if (!CreateSocket(s, receiveBuffer, sendBuffer)) {
        return false;
    }

    if (!(blocking ? SetBlocking(*s) : SetNonBlocking(*s))) {
        CloseSocket(s, false);
        return false;
    }

    return Connect(*s, reinterpret_cast<sockaddr*>(address));
}

// OFFSET: 0x007BD120/0x1404B5780
int __cdecl System_Net::CompareAddresses(const void* a, const void* b) {
    uint32_t x = static_cast<const Blizzard::Net::Address*>(a)->m_ip;
    uint32_t y = static_cast<const Blizzard::Net::Address*>(b)->m_ip;
    uint32_t first = (((x & 0xFF0000) | (x >> 16)) >> 8) | (((x & 0xFF00) | (x << 16)) << 8);
    uint32_t second = (((y & 0xFF0000) | (y >> 16)) >> 8) | (((y & 0xFF00) | (y << 16)) << 8);

    if (first == second) {
        return 0;
    }

    return first < second ? -1 : 1;
}

// OFFSET: 0x007C4CF0/0x1404BFB90
bool Blizzard::Net::ResolveHost(const char* name, Address* addresses, int32_t* count) {
    char localName[268];

    if (!name) {
        gethostname(localName, 256);
        name = localName;
    }

    hostent* host = gethostbyname(name);

    if (!host) {
        return false;
    }

    int32_t found = 0;

    for (char** entry = host->h_addr_list; *entry && found < *count; entry++, found++) {
        addresses[found].unk_0000 = 0;
        addresses[found].unk_0008 = 0;
        addresses[found].unk_000C = 0;
        addresses[found].m_ip = *reinterpret_cast<uint32_t*>(*entry);
    }

    *count = found;

    if (!found) {
        return false;
    }

    qsort(addresses, found, sizeof(Address), System_Net::CompareAddresses);
    srand(static_cast<uint32_t>(Blizzard::Time::Milliseconds()));

    for (int32_t i = 0; i < 5; i++) {
        rand();
    }

    for (int32_t i = 0; i < found - 1; i++) {
        int32_t j = i + rand() % (found - i);

        if (j != i) {
            Address swap = addresses[i];
            addresses[i] = addresses[j];
            addresses[j] = swap;
        }
    }

    return true;
}

// OFFSET: 0x007C64A0
bool Blizzard::Net::ResolveAddress(const char* name, sockaddr_in* address) {
    Address addresses[64];
    int32_t count = 64;

    if (!ResolveHost(name, addresses, &count)) {
        return false;
    }

    memcpy(&address->sin_addr, &addresses[0].m_ip, sizeof(address->sin_addr));

    return true;
}

// OFFSET: 0x007C6510/0x1404C16F0
bool Blizzard::Net::ConnectHost(const char* host, uint16_t port, Socket* s, int32_t receiveBuffer, int32_t sendBuffer, bool blocking) {
    sockaddr_in address;
    memset(&address.sin_addr, 0, 12);
    *reinterpret_cast<uint32_t*>(&address.sin_family) = AF_INET;

    if (!ResolveAddress(host, &address)) {
        System_Net::s_lastError = 8004;
        CloseSocket(s, false);
        return false;
    }

    address.sin_port = htons(port);

    return ConnectAddress(&address, s, receiveBuffer, sendBuffer, blocking);
}

#endif
