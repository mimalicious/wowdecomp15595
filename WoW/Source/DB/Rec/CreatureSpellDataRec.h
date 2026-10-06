#ifndef WOW_SOURCE_DB_REC_CREATURESPELLDATAREC_H
#define WOW_SOURCE_DB_REC_CREATURESPELLDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CreatureSpellDataRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [4];
    /* 0014 */ int32_t m_availability [4];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(CreatureSpellDataRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
