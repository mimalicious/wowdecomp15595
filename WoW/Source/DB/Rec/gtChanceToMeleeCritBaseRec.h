#ifndef WOW_SOURCE_DB_REC_GTCHANCETOMELEECRITBASEREC_H
#define WOW_SOURCE_DB_REC_GTCHANCETOMELEECRITBASEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class gtChanceToMeleeCritBaseRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_data;

    static void FixupStrings(void* record, const char* strings);
};

#endif
