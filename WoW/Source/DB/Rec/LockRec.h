#ifndef WOW_SOURCE_DB_REC_LOCKREC_H
#define WOW_SOURCE_DB_REC_LOCKREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LockRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_type [8];
    /* 0024 */ int32_t m_index [8];
    /* 0044 */ int32_t m_skill [8];
    /* 0064 */ int32_t m_action [8];

    static void FixupStrings(void* record, const char* strings);
};

#endif
