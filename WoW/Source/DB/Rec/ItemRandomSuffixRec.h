#ifndef WOW_SOURCE_DB_REC_ITEMRANDOMSUFFIXREC_H
#define WOW_SOURCE_DB_REC_ITEMRANDOMSUFFIXREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemRandomSuffixRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ DBCString m_internalName;
    /* 000C */ int32_t m_enchantment [5];
    /* 0020 */ int32_t m_allocationPct [5];

    static void FixupStrings(void* record, const char* strings);
};

#endif
