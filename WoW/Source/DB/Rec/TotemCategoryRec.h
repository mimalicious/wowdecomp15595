#ifndef WOW_SOURCE_DB_REC_TOTEMCATEGORYREC_H
#define WOW_SOURCE_DB_REC_TOTEMCATEGORYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class TotemCategoryRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_totemCategoryType;
    /* 000C */ int32_t m_totemCategoryMask;
};

#endif
