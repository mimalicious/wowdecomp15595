#ifndef WOW_SOURCE_DB_REC_ITEMGROUPSOUNDSREC_H
#define WOW_SOURCE_DB_REC_ITEMGROUPSOUNDSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemGroupSoundsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_sound [4];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ItemGroupSoundsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
