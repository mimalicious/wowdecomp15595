#ifndef WOW_SOURCE_DB_REC_LOCKTYPEREC_H
#define WOW_SOURCE_DB_REC_LOCKTYPEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LockTypeRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ DBCString m_resourceName;
    /* 000C */ DBCString m_verb;
    /* 0010 */ DBCString m_cursorName;

    static void FixupStrings(void* record, const char* strings);
};

#endif
