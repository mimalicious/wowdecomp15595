#ifndef WOW_SOURCE_DB_REC_MOUNTTYPEREC_H
#define WOW_SOURCE_DB_REC_MOUNTTYPEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MountTypeRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_capability [24];
};

#endif
