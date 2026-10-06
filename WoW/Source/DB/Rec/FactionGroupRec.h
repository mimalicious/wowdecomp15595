#ifndef WOW_SOURCE_DB_REC_FACTIONGROUPREC_H
#define WOW_SOURCE_DB_REC_FACTIONGROUPREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class FactionGroupRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_maskID;
    /* 0008 */ const char* m_internalName;
    /* 000C */ const char* m_name;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(FactionGroupRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
