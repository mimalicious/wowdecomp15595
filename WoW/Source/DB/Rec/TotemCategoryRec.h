#ifndef WOW_SOURCE_DB_REC_TOTEMCATEGORYREC_H
#define WOW_SOURCE_DB_REC_TOTEMCATEGORYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TotemCategoryRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_totemCategoryType;
    /* 000C */ int32_t m_totemCategoryMask;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(TotemCategoryRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
