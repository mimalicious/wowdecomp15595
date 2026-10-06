#ifndef WOW_SOURCE_DB_REC_SPELLMISSILEMOTIONREC_H
#define WOW_SOURCE_DB_REC_SPELLMISSILEMOTIONREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellMissileMotionRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_scriptBody;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_missileCount;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SpellMissileMotionRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
