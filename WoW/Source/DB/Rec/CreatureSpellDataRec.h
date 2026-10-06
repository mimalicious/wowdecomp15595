#ifndef WOW_SOURCE_DB_REC_CREATURESPELLDATAREC_H
#define WOW_SOURCE_DB_REC_CREATURESPELLDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CreatureSpellDataRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_spells [4];
    /* 0014 */ int32_t m_availability [4];
};

#endif
