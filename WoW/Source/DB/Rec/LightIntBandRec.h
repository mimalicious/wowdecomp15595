#ifndef WOW_SOURCE_DB_REC_LIGHTINTBANDREC_H
#define WOW_SOURCE_DB_REC_LIGHTINTBANDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LightIntBandRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_num;
    /* 0008 */ int32_t m_time [16];
    /* 0048 */ int32_t m_data [16];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(LightIntBandRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
