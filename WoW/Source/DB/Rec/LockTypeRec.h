#ifndef WOW_SOURCE_DB_REC_LOCKTYPEREC_H
#define WOW_SOURCE_DB_REC_LOCKTYPEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LockTypeRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_resourceName;
    /* 000C */ const char* m_verb;
    /* 0010 */ const char* m_cursorName;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(LockTypeRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
