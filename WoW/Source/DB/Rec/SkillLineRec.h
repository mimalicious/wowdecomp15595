#ifndef WOW_SOURCE_DB_REC_SKILLLINEREC_H
#define WOW_SOURCE_DB_REC_SKILLLINEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SkillLineRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_categoryID;
    /* 0008 */ DBCString m_displayName;
    /* 000C */ DBCString m_description;
    /* 0010 */ int32_t m_spellIconID;
    /* 0014 */ DBCString m_alternateVerb;
    /* 0018 */ int32_t m_canLink;
};

#endif
