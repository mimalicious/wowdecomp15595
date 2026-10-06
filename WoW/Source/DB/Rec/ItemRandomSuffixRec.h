#ifndef WOW_SOURCE_DB_REC_ITEMRANDOMSUFFIXREC_H
#define WOW_SOURCE_DB_REC_ITEMRANDOMSUFFIXREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemRandomSuffixRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_internalName;
    /* 000C */ int32_t m_enchantment [5];
    /* 0020 */ int32_t m_allocationPct [5];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ItemRandomSuffixRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
