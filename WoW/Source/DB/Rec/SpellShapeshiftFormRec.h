#ifndef WOW_SOURCE_DB_REC_SPELLSHAPESHIFTFORMREC_H
#define WOW_SOURCE_DB_REC_SPELLSHAPESHIFTFORMREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellShapeshiftFormRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_bonusActionBar;
    /* 0008 */ DBCString m_name;
    /* 000C */ int32_t m_flags;
    /* 0010 */ int32_t m_creatureType;
    /* 0014 */ int32_t m_attackIconID;
    /* 0018 */ int32_t m_combatRoundTime;
    /* 001C */ int32_t m_creatureDisplayID [4];
    /* 002C */ int32_t m_presetSpellID [8];
    /* 004C */ int32_t unk_004C;
    /* 0050 */ int32_t unk_0050;

    static void FixupStrings(void* record, const char* strings);
};

#endif
