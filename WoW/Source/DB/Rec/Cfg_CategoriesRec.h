#ifndef WOW_SOURCE_DB_REC_CFG_CATEGORIESREC_H
#define WOW_SOURCE_DB_REC_CFG_CATEGORIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class Cfg_CategoriesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_localeMask;
    /* 0008 */ int32_t m_createCharsetMask;
    /* 000C */ int32_t m_existingCharsetMask;
    /* 0010 */ int32_t m_flags;
    /* 0014 */ DBCString m_name;
};

#endif
