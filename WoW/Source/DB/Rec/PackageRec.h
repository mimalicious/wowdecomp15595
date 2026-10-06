#ifndef WOW_SOURCE_DB_REC_PACKAGEREC_H
#define WOW_SOURCE_DB_REC_PACKAGEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class PackageRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_icon;
    /* 0008 */ int32_t m_cost;
    /* 000C */ DBCString m_name;

    static void FixupStrings(void* record, const char* strings);
};

#endif
