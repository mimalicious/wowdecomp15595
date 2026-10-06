#ifndef WOW_SOURCE_DB_REC_SPELLMISSILEMOTIONREC_H
#define WOW_SOURCE_DB_REC_SPELLMISSILEMOTIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellMissileMotionRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ DBCString m_scriptBody;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_missileCount;

    static void FixupStrings(void* record, const char* strings);
};

#endif
