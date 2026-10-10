#ifndef WOW_SOURCE_DB_REC_UNITPOWERBARREC_H
#define WOW_SOURCE_DB_REC_UNITPOWERBARREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class UnitPowerBarRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_minPower;
    /* 0008 */ int32_t m_maxPower;
    /* 000C */ int32_t m_startPower;
    /* 0010 */ int32_t m_centerPower;
    /* 0014 */ float m_regenerationPeace;
    /* 0018 */ float m_regenerationCombat;
    /* 001C */ int32_t m_barType;
    /* 0020 */ int32_t unk_0020;
    /* 0024 */ int32_t unk_0024;
    /* 0028 */ int32_t unk_0028;
    /* 002C */ int32_t unk_002C;
    /* 0030 */ int32_t unk_0030;
    /* 0034 */ int32_t unk_0034;
    /* 0038 */ int32_t unk_0038;
    /* 003C */ int32_t unk_003C;
    /* 0040 */ int32_t unk_0040;
    /* 0044 */ int32_t unk_0044;
    /* 0048 */ int32_t unk_0048;
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t m_flags;
    /* 0054 */ DBCString unk_0054;
    /* 0058 */ DBCString m_costString;
    /* 005C */ DBCString m_outOfError;
    /* 0060 */ DBCString unk_0060;
    /* 0064 */ float unk_0064;
    /* 0068 */ float unk_0068;
};

#endif
