#if defined(WOWDECOMP_ARCH_64)

#include <cctype>
#include <cstdint>
#include <cstring>
#include <winsock2.h>
#include "Engine/Source/Os/SockAddr.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x00879860/0x1405B27A0
static void SockAddrFormatIPv6(const uint8_t* src, char* dst) {
    uint16_t words[8];

    for (int32_t i = 0; i < 8; i++) {
        words[i] = static_cast<uint16_t>((src[i * 2] << 8) | src[i * 2 + 1]);
    }

    int32_t curBase = -1;
    int32_t bestBase = -1;
    int32_t curLen = 0;
    int32_t bestLen = 0;

    for (int32_t i = 0; i < 8; i++) {
        if (words[i] != 0) {
            if (curBase != -1) {
                if (bestBase == -1 || curLen > bestLen) {
                    bestBase = curBase;
                    bestLen = curLen;
                }

                curBase = -1;
            }
        } else if (curBase == -1) {
            curBase = i;
            curLen = 1;
        } else {
            curLen++;
        }
    }

    if (curBase != -1 && (bestBase == -1 || curLen > bestLen)) {
        bestBase = curBase;
        bestLen = curLen;
    }

    if (bestBase != -1 && bestLen < 2) {
        bestBase = -1;
    }

    char* tp = dst;

    for (int32_t i = 0; i < 8; i++) {
        if (bestBase != -1 && i >= bestBase && i < bestBase + bestLen) {
            if (i == bestBase) {
                *tp++ = ':';
            }

            continue;
        }

        if (i != 0) {
            *tp++ = ':';
        }

        tp += SStrPrintf(tp, 0x7FFFFFFF, "%x", words[i]);
    }

    if (bestBase != -1 && bestBase + bestLen == 8) {
        *tp++ = ':';
    }

    *tp = '\0';
}

// OFFSET: 0x00879A90/0x1405B29B0
static int32_t SockAddrParseIPv4(const char* src, uint8_t* dst) {
    uint8_t tmp[4];
    uint8_t* tp = tmp;
    int32_t sawDigit = 0;
    int32_t octets = 0;
    *tp = 0;

    int32_t ch;

    while ((ch = *src++) != '\0') {
        if (static_cast<uint32_t>(ch - '0') <= 9) {
            uint32_t value = *tp * 10 + (ch - '0');

            if (value > 255) {
                return 0;
            }

            if (!sawDigit) {
                if (++octets > 4) {
                    return 0;
                }

                sawDigit = 1;
            }

            *tp = static_cast<uint8_t>(value);
        } else if (ch == '.' && sawDigit) {
            if (octets == 4) {
                return 0;
            }

            *++tp = 0;
            sawDigit = 0;
        } else {
            return 0;
        }
    }

    if (octets < 4) {
        return 0;
    }

    memcpy(dst, tmp, sizeof(tmp));
    return 1;
}

// OFFSET: 0x00879B20/0x1405B2A50
static int32_t SockAddrParseIPv6(const char* src, uint8_t* dst) {
    uint8_t tmp[16];
    memset(tmp, 0, sizeof(tmp));

    uint8_t* tp = tmp;
    uint8_t* endp = tmp + sizeof(tmp);
    uint8_t* colonp = NULL;

    if (*src == ':' && *++src != ':') {
        return 0;
    }

    const char* curtok = src;
    int32_t sawXdigit = 0;
    int32_t digits = 0;
    uint32_t value = 0;
    int32_t ch;

    while ((ch = toupper(*src)) != 0) {
        src++;

        if ((ch >= '0' && ch <= '9') || static_cast<uint32_t>(ch - 'A') <= 5) {
            if (digits >= 4) {
                return 0;
            }

            value <<= 4;
            value |= ch > '9' ? ch - 'A' + 10 : ch - '0';

            if (value > 0xFFFF) {
                return 0;
            }

            sawXdigit = 1;
            digits++;
            continue;
        }

        if (ch == ':') {
            curtok = src;

            if (!sawXdigit) {
                if (colonp) {
                    return 0;
                }

                colonp = tp;
                continue;
            }

            if (*src == '\0' || tp + 2 > endp) {
                return 0;
            }

            *tp++ = static_cast<uint8_t>(value >> 8);
            *tp++ = static_cast<uint8_t>(value);
            sawXdigit = 0;
            digits = 0;
            value = 0;
            continue;
        }

        if (ch == '.' && tp + 4 <= endp && SockAddrParseIPv4(curtok, tp) > 0) {
            tp += 4;
            sawXdigit = 0;
            break;
        }

        return 0;
    }

    if (sawXdigit) {
        if (tp + 2 > endp) {
            return 0;
        }

        *tp++ = static_cast<uint8_t>(value >> 8);
        *tp++ = static_cast<uint8_t>(value);
    }

    if (colonp) {
        int32_t n = static_cast<int32_t>(tp - colonp);

        for (int32_t i = 1; i <= n; i++) {
            endp[-i] = colonp[n - i];
            colonp[n - i] = 0;
        }

        tp = endp;
    }

    if (tp != endp) {
        return 0;
    }

    memcpy(dst, tmp, sizeof(tmp));
    return 1;
}

// OFFSET: 0x00879CD0/0x1405B2C60
bool SockAddr::Parse(const char* str) {
    uint32_t ipv4;

    if (SockAddrParseIPv4(str, reinterpret_cast<uint8_t*>(&ipv4))) {
        this->m_type = SOCKADDR_IPV4;
        this->m_ipv4 = ipv4;
        return true;
    }

    uint8_t ipv6[16];

    if (SockAddrParseIPv6(str, ipv6)) {
        memcpy(this->m_ipv6, ipv6, sizeof(ipv6));
        this->m_type = SOCKADDR_IPV6;
        return true;
    }

    return false;
}

// OFFSET: 0x00879D50/0x1405B2CE0
void SockAddr::Normalize() {
    switch (this->m_type) {
        case SOCKADDR_NONE: {
            memset(this->m_ipv6, 0, sizeof(this->m_ipv6));
            this->m_type = SOCKADDR_NONE;
            break;
        }

        case SOCKADDR_IPV4: {
            uint32_t ipv4 = this->m_ipv4;
            memset(this->m_ipv6, 0, sizeof(this->m_ipv6));
            this->m_type = SOCKADDR_NONE;

            if (ipv4) {
                this->m_type = SOCKADDR_IPV4;
                this->m_ipv4 = ipv4;
            }

            break;
        }

        case SOCKADDR_IPV6: {
            for (int32_t i = 0; i < 10; i++) {
                if (this->m_ipv6[i]) {
                    return;
                }
            }

            uint32_t address = (this->m_ipv6[12] << 24) | (this->m_ipv6[13] << 16) | (this->m_ipv6[14] << 8) | this->m_ipv6[15];
            uint32_t prefix = (this->m_ipv6[10] << 8) | this->m_ipv6[11];

            if (prefix && prefix != 0xFFFF) {
                return;
            }

            if (address == 1) {
                address = 0x7F000001;
            }

            memset(this->m_ipv6, 0, sizeof(this->m_ipv6));
            this->m_type = SOCKADDR_IPV4;
            this->m_ipv4 = htonl(address);
            break;
        }
    }
}

// OFFSET: 0x00879E20
void SockAddrString::FormatIPv4(uint32_t address) {
    uint32_t host = ntohl(address);
    SStrPrintf(this->m_str, "%d.%d.%d.%d", host >> 24, (host >> 16) & 0xFF, (host >> 8) & 0xFF, host & 0xFF);
}

// OFFSET: 0x00879E70/0x1405B2DC0
void SockAddrString::FormatIPv4(uint32_t address, uint16_t port) {
    uint32_t host = ntohl(address);
    SStrPrintf(this->m_str, "%d.%d.%d.%d:%d", host >> 24, (host >> 16) & 0xFF, (host >> 8) & 0xFF, host & 0xFF, port);
}

// OFFSET: 0x00879EC0/0x1405B2E20
SockAddrString::SockAddrString(SockAddr* addr, uint16_t port) {
    addr->Normalize();

    switch (addr->m_type) {
        case SOCKADDR_NONE: {
            SStrPrintf(this->m_str, "0.0.0.0:%d", port);
            break;
        }

        case SOCKADDR_IPV4: {
            this->FormatIPv4(addr->m_ipv4, port);
            break;
        }

        case SOCKADDR_IPV6: {
            char ipv6[64];
            SockAddrFormatIPv6(addr->m_ipv6, ipv6);
            SStrPrintf(this->m_str, "[%s]:%d", ipv6, port);
            break;
        }
    }
}

// OFFSET: 0x00879F50/0x1405B2EE0
SockAddrString::SockAddrString(const SockAddr& addr) {
    SockAddr normalized = addr;
    normalized.Normalize();

    switch (normalized.m_type) {
        case SOCKADDR_NONE: {
            strcpy(this->m_str, "0.0.0.0");
            break;
        }

        case SOCKADDR_IPV4: {
            this->FormatIPv4(normalized.m_ipv4);
            break;
        }

        case SOCKADDR_IPV6: {
            SockAddrFormatIPv6(normalized.m_ipv6, this->m_str);
            break;
        }

        case SOCKADDR_HOSTNAME: {
            SStrPrintf(this->m_str, "{%s}", addr.m_hostname);
            break;
        }
    }
}

// OFFSET: 0x0087A020/0x1405B3010
SockAddrString::SockAddrString(const SockAddr& addr, uint16_t port) {
    SockAddr normalized = addr;
    normalized.Normalize();

    switch (normalized.m_type) {
        case SOCKADDR_NONE: {
            SStrPrintf(this->m_str, "0.0.0.0:%d", port);
            break;
        }

        case SOCKADDR_IPV4: {
            this->FormatIPv4(normalized.m_ipv4, port);
            break;
        }

        case SOCKADDR_IPV6: {
            char ipv6[64];
            SockAddrFormatIPv6(normalized.m_ipv6, ipv6);
            SStrPrintf(this->m_str, "[%s]:%d", ipv6, port);
            break;
        }
    }
}

#endif
