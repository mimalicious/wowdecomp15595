#ifndef WOW_SOURCE_DB_REC_ZONELIGHTPOINTREC_H
#define WOW_SOURCE_DB_REC_ZONELIGHTPOINTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ZoneLightPointRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_zoneLightID;
    /* 0008 */ float m_pos[2];
    /* 0010 */ int32_t m_pointOrder;
};

#endif
