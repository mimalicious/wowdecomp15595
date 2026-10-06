#ifndef WOW_SOURCE_DB_REC_LIGHTFLOATBANDREC_H
#define WOW_SOURCE_DB_REC_LIGHTFLOATBANDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LightFloatBandRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_num;
    /* 0008 */ int32_t m_time [16];
    /* 0048 */ float m_data [16];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(LightFloatBandRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
