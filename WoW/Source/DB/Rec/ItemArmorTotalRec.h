#ifndef WOW_SOURCE_DB_REC_ITEMARMORTOTALREC_H
#define WOW_SOURCE_DB_REC_ITEMARMORTOTALREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemArmorTotalRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_itemLevel;
    /* 0008 */ float m_value [4];
};

#endif
