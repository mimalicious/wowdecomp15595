#ifndef WOW_SOURCE_DB_REC_LIQUIDMATERIALREC_H
#define WOW_SOURCE_DB_REC_LIQUIDMATERIALREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LiquidMaterialRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_lvf;
    /* 0008 */ int32_t m_flags;
};

#endif
