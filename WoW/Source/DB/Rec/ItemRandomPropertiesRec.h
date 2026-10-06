#ifndef WOW_SOURCE_DB_REC_ITEMRANDOMPROPERTIESREC_H
#define WOW_SOURCE_DB_REC_ITEMRANDOMPROPERTIESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemRandomPropertiesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_enchantment [5];
    /* 001C */ DBCString m_nameLang;

    static void FixupStrings(void* record, const char* strings);
};

#endif
