#ifndef WOW_SOURCE_DB_REC_CURRENCYTYPESREC_H
#define WOW_SOURCE_DB_REC_CURRENCYTYPESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CurrencyTypesRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_categoryID;
    /* 0008 */ DBCString m_name;
    /* 000C */ DBCString m_inventoryIcon;
    /* 0010 */ DBCString m_inventoryIcon2;
    /* 0014 */ int32_t m_spellWeight;
    /* 0018 */ int32_t m_spellCategory;
    /* 001C */ int32_t m_totalCap;
    /* 0020 */ int32_t m_weeklyCap;
    /* 0024 */ int32_t m_flags;
    /* 0028 */ DBCString m_description;
};

#endif
