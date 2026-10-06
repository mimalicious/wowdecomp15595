#ifndef WOW_SOURCE_DB_REC_LIGHTINTBANDREC_H
#define WOW_SOURCE_DB_REC_LIGHTINTBANDREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LightIntBandRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_num;
    /* 0008 */ int32_t m_time [16];
    /* 0048 */ int32_t m_data [16];
};

#endif
