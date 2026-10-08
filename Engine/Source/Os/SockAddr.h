#ifndef ENGINE_SOURCE_OS_SOCKADDR_H
#define ENGINE_SOURCE_OS_SOCKADDR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/storm.h"

enum SOCKADDR_TYPE {
    SOCKADDR_NONE = 0,
    SOCKADDR_IPV4 = 1,
    SOCKADDR_IPV6 = 2,
    SOCKADDR_HOSTNAME = 3,
};

class SockAddr {
    public:
    union {
        /* 0000 */ uint32_t m_ipv4;
        /* 0000 */ uint8_t m_ipv6[16];
        /* 0000 */ char* m_hostname;
    };
    /* 0010 */ int32_t m_type;

    ~SockAddr();
    void Normalize();
    bool Parse(const char* str);
};

class SockAddrString {
    public:
    /* 0000 */ char m_str[64];

    SockAddrString(SockAddr* addr, uint16_t port);
    SockAddrString(const SockAddr& addr);
    SockAddrString(const SockAddr& addr, uint16_t port);
    void FormatIPv4(uint32_t address);
    void FormatIPv4(uint32_t address, uint16_t port);
};

#include "Engine/Source/Os/SockAddr.inl"

#endif
