#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_NET_NET_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_NET_NET_H

#include <cstdint>

struct sockaddr;
struct sockaddr_in;

namespace Blizzard {
namespace Net {

typedef uintptr_t Socket;

struct Address {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ uint32_t m_ip;
    /* 0008 */ uint32_t unk_0008;
    /* 000C */ uint32_t unk_000C;
};

bool CloseSocket(Socket* s, bool linger);
bool Connect(Socket s, const sockaddr* address);
bool ConnectAddress(sockaddr_in* address, Socket* s, int32_t receiveBuffer, int32_t sendBuffer, bool blocking);
bool ConnectHost(const char* host, uint16_t port, Socket* s, int32_t receiveBuffer, int32_t sendBuffer, bool blocking);
bool CreateSocket(Socket* s, int32_t receiveBuffer, int32_t sendBuffer);
bool ResolveAddress(const char* name, sockaddr_in* address);
bool ResolveHost(const char* name, Address* addresses, int32_t* count);
bool Select(Socket* readSockets, Socket* writeSockets, int64_t timeout);
bool SetBlocking(Socket s);
bool SetNonBlocking(Socket s);
int32_t TranslateError(int32_t context);

}
}

namespace System_Net {

extern int32_t s_lastError;

int __cdecl CompareAddresses(const void* a, const void* b);

}

#endif
