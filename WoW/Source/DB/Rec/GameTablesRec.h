#ifndef WOW_SOURCE_DB_REC_GAMETABLESREC_H
#define WOW_SOURCE_DB_REC_GAMETABLESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GameTablesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_numRows;
    /* 000C */ int32_t m_numColumns;

    static void FixupStrings(void* record, const char* strings);
};

#endif
