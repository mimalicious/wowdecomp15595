#ifndef WOW_SOURCE_DB_REC_SPELLAURARESTRICTIONSREC_H
#define WOW_SOURCE_DB_REC_SPELLAURARESTRICTIONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellAuraRestrictionsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_casterAuraState;
    /* 0008 */ int32_t m_targetAuraState;
    /* 000C */ int32_t m_excludeCasterAuraState;
    /* 0010 */ int32_t m_excludeTargetAuraState;
    /* 0014 */ int32_t m_casterAuraSpell;
    /* 0018 */ int32_t m_targetAuraSpell;
    /* 001C */ int32_t m_excludeCasterAuraSpell;
    /* 0020 */ int32_t m_excludeTargetAuraSpell;
};

#endif
