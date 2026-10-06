#ifndef WOW_SOURCE_DB_REC_SPELLRADIUSREC_H
#define WOW_SOURCE_DB_REC_SPELLRADIUSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellRadiusRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_radius;
    /* 0008 */ float m_radiusPerLevel;
    /* 000C */ float m_radiusMax;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SpellRadiusRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
