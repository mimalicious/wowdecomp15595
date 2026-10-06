#ifndef WOW_SOURCE_DB_REC_SPELLDISPELTYPEREC_H
#define WOW_SOURCE_DB_REC_SPELLDISPELTYPEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellDispelTypeRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_mask;
    /* 000C */ int32_t m_immunityPossible;
    /* 0010 */ const char* m_internalName;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SpellDispelTypeRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
