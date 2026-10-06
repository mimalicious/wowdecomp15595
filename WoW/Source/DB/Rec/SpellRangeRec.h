#ifndef WOW_SOURCE_DB_REC_SPELLRANGEREC_H
#define WOW_SOURCE_DB_REC_SPELLRANGEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellRangeRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_rangeMin [2];
    /* 000C */ float m_rangeMax [2];
    /* 0014 */ int32_t m_flags;
    /* 0018 */ DBCString m_displayName;
    /* 001C */ DBCString m_displayNameShort;
};

#endif
