#ifndef WOW_SOURCE_DB_REC_SPELLCOOLDOWNSREC_H
#define WOW_SOURCE_DB_REC_SPELLCOOLDOWNSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellCooldownsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_categoryRecoveryTime;
    /* 0008 */ int32_t m_recoveryTime;
    /* 000C */ int32_t m_startRecoveryTime;
};

#endif
