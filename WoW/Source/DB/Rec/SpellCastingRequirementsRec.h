#ifndef WOW_SOURCE_DB_REC_SPELLCASTINGREQUIREMENTSREC_H
#define WOW_SOURCE_DB_REC_SPELLCASTINGREQUIREMENTSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellCastingRequirementsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_facingCasterFlags;
    /* 0008 */ int32_t m_minFactionID;
    /* 000C */ int32_t m_minReputation;
    /* 0010 */ int32_t m_requiredAreasID;
    /* 0014 */ int32_t m_requiredAuraVision;
    /* 0018 */ int32_t m_requiresSpellFocus;
};

#endif
