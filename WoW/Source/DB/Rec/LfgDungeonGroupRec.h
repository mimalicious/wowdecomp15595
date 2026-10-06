#ifndef WOW_SOURCE_DB_REC_LFGDUNGEONGROUPREC_H
#define WOW_SOURCE_DB_REC_LFGDUNGEONGROUPREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LfgDungeonGroupRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_orderIndex;
    /* 000C */ int32_t m_parentGroupID;
    /* 0010 */ int32_t m_typeID;

    static void FixupStrings(void* record, const char* strings);
};

#endif
