#ifndef WOW_SOURCE_DB_REC_OVERRIDESPELLDATAREC_H
#define WOW_SOURCE_DB_REC_OVERRIDESPELLDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class OverrideSpellDataRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [10];
    /* 002C */ int32_t m_flags;
    /* 0030 */ DBCString m_overrideBarType;
};

#endif
