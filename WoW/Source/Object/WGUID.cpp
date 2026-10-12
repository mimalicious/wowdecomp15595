#include "WGUID.h"
#include "Engine/Source/Base/CDataStore.h"

// OFFSET: 0x00438C80/0x1400430B0
CDataStore& operator>>(CDataStore& msg, SmartGUID& guid) {
    guid.m_guid = 0;

    uint8_t mask;
    msg.Get(mask);

    for (uint32_t i = 0, bit = 0; i < 64; i += 8, bit++) {
        if (mask & (1 << bit)) {
            uint8_t byte;
            msg.Get(byte);
            guid.m_guid |= static_cast<uint64_t>(byte) << i;
        }
    }

    return msg;
}

// OFFSET: 0x00438CE0/0x140043120
CDataStore& SmartGUID::Put(CDataStore& msg) const {
    uint8_t mask = 0;
    uint32_t maskPos = msg.m_size;
    msg.Put(static_cast<uint8_t>(0));

    for (uint32_t i = 0, bit = 0; i < 64; i += 8, bit++) {
        uint8_t byte = static_cast<uint8_t>(this->m_guid >> i);

        if (byte) {
            mask |= 1 << bit;
            msg.Put(byte);
        }
    }

    return msg.Set(maskPos, mask);
}

// OFFSET: 0x00438D60/0x1400431B0
CDataStore& operator<<(CDataStore& msg, const SmartGUID& guid) {
    guid.Put(msg);
    return msg;
}
