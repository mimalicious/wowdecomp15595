#ifndef WOW_SOURCE_DB_REC_FACTIONGROUPREC_H
#define WOW_SOURCE_DB_REC_FACTIONGROUPREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class FactionGroupRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_maskID;
    /* 0008 */ DBCString m_internalName;
    /* 000C */ DBCString m_name;
};

#endif
