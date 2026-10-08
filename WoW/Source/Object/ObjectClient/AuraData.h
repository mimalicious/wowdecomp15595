#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_AURADATA_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_AURADATA_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CDataStore;
class CGUnit_C;

#pragma pack(push, 4)
class AuraData {
    public:
    /* 0000 */ WOWGUID m_creator;
    /* 0008 */ int32_t m_spellId;
    /* 000C */ uint16_t m_flags;
    /* 000E */ uint8_t m_level;
    /* 000F */ uint8_t m_stackCount;
    /* 0010 */ int32_t m_duration;
    /* 0014 */ int32_t m_endTime;
    /* 0018 */ int32_t m_effectAmounts [3];
    /* 0024 */ uint32_t unk_0024;

    void Unpack(CGUnit_C* unit, uint32_t time, CDataStore* msg);
};

class SortedAuraInfo {
    public:
    /* 0000 */ uint32_t m_auraIndex;
    /* 0004 */ int32_t m_spellId;
};
#pragma pack(pop)

#endif
