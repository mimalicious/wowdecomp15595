#ifndef WOW_SOURCE_DB_REC_IMPORTPRICEARMORREC_H
#define WOW_SOURCE_DB_REC_IMPORTPRICEARMORREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ImportPriceArmorRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_clothFactor;
    /* 0008 */ float m_leatherFactor;
    /* 000C */ float m_mailFactor;
    /* 0010 */ float m_plateFactor;
};

#endif
