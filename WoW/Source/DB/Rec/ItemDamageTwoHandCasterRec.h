#ifndef WOW_SOURCE_DB_REC_ITEMDAMAGETWOHANDCASTERREC_H
#define WOW_SOURCE_DB_REC_ITEMDAMAGETWOHANDCASTERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemDamageTwoHandCasterRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_quality [7];
    /* 0020 */ int32_t m_itemLevel;
};

#endif
