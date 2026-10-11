#include <intrin.h>
#include <cstdint>
#include "Engine/Source/Os/OsLibrary.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"

// INLINED
void AdditionalCheck1() {
    if (!s_botCheckEnabled || s_botCheckDelay <= 0 || s_botCheckDelay > 420) {
        return;
    }

    if (_InterlockedCompareExchange(&s_additionalCheckArmed, 0, 1) != 1) {
        return;
    }

    AdditionalCheckInterface iface;
    iface.m_memAlloc = Client_MemAlloc;
    iface.m_memFree = Client_MemFree;
    iface.m_loadLibrary = OsLoadLibrary;
    iface.m_getLibraryEntryPoint = OsGetLibraryEntryPoint;
    iface.m_freeLibrary = OsFreeLibrary;
    uint8_t detected = 0;

    __try {
        if (*reinterpret_cast<const uint8_t*>(&OsValidateFunctionPointer) == 0xE9) {
            return;
        }

        detected = AdditionalCheck1Scan(&iface, 0);
    } __except (1) {
    }

    if (detected) {
        SendBotDetected2(CMSG_BOT_DETECTED2, 0x60231D69);
    }
}

// INLINED
void InitTokenSeed(const uint8_t* key) {
}

// INLINED
int32_t Packet_SMSG_CHECK_FOR_BOTS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t key[16];
    msg->GetArray(key, sizeof(key));

    return 1;
}

// INLINED
int32_t GatherAdditionalChecks(uint8_t* check1, uint8_t* check2, uint8_t* check3) {
    *check1 = 0;
    *check2 = 0;
    *check3 = 0;

    return 0;
}

uint8_t AdditionalCheck1Scan(const AdditionalCheckInterface* iface, int32_t a2) {
    return 0;
}

// INLINED
void LoadEncryptedModule(const uint8_t* key, uint32_t length, void (*callback)()) {
}
