#if defined(WOWDECOMP_ARCH_64)

#include <winsock2.h>
#include <windows.h>
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Battlenet/Tumor/TumorManager.h"

// OFFSET: 0x00627E40/0x1402B9E90
uint32_t BattlenetApplication::Resolve(uint32_t token, const char* hostname) {
    if (!this->m_login || !g_tumorManager) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    Battlenet::SystemClient& client = g_tumorManager->m_tumor;
    hostent* host = gethostbyname(hostname);

    if (host) {
        Battlenet::Client::DNSInfo info;
        info.m_addresses.SetCount(1);

        BSN::FixedBlob<4>& address = info.m_addresses[0];
        address.m_data[0] = host->h_addr_list[0][0];
        address.m_data[1] = host->h_addr_list[0][1];
        address.m_data[2] = host->h_addr_list[0][2];
        address.m_data[3] = host->h_addr_list[0][3];

        info.m_canonicalName = host->h_name;
        client.Resolved(token, info);
        return 0;
    }

    char name[256];
    const char* separator = SStrChr(hostname, ':');

    if (separator) {
        int32_t length = separator - hostname;

        if (length >= 255) {
            length = 255;
        }

        SStrCopy(name, hostname, length);
        name[length] = '\0';
    } else {
        SStrCopy(name, hostname, sizeof(name));
    }

    uint32_t ip = inet_addr(name);

    if (ip == INADDR_NONE) {
        BattlenetLogin::LogConnectionStatus("Failed to resolve %s", hostname);

        Battlenet::Client::DNSInfo info;
        info.m_canonicalName = "";
        client.Resolved(token, info);
        return 0;
    }

    Battlenet::Client::DNSInfo info;
    info.m_addresses.SetCount(1);

    BSN::FixedBlob<4>& address = info.m_addresses[0];
    address.m_data[0] = ip;
    address.m_data[1] = ip >> 8;
    address.m_data[2] = ip >> 16;
    address.m_data[3] = ip >> 24;

    info.m_canonicalName = "";
    client.Resolved(token, info);
    return 0;
}

#endif
