#ifndef WOW_SOURCE_DB_REC_SPELLDISPELTYPEREC_H
#define WOW_SOURCE_DB_REC_SPELLDISPELTYPEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellDispelTypeRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_mask;
    /* 000C */ int32_t m_immunityPossible;
    /* 0010 */ DBCString m_internalName;
};

#endif
