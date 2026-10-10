#ifndef WOW_SOURCE_DB_REC_SPELLTOTEMSREC_H
#define WOW_SOURCE_DB_REC_SPELLTOTEMSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellTotemsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_requiredTotemCategoryID[2];
    /* 000C */ int32_t m_totem[2];
};

#endif
