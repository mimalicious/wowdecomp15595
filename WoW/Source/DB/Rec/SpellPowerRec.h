#ifndef WOW_SOURCE_DB_REC_SPELLPOWERREC_H
#define WOW_SOURCE_DB_REC_SPELLPOWERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellPowerRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_manaCost;
    /* 0008 */ int32_t m_manaCostPerLevel;
    /* 000C */ int32_t m_manaCostPercentage;
    /* 0010 */ int32_t m_manaPerSecond;
    /* 0014 */ int32_t m_powerDisplayID;
    /* 0018 */ int32_t m_unitPowerBarID;
    /* 001C */ float m_manaCostPercentageFloat;
};

#endif
