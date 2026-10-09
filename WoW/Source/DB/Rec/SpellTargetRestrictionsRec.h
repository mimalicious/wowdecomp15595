#ifndef WOW_SOURCE_DB_REC_SPELLTARGETRESTRICTIONSREC_H
#define WOW_SOURCE_DB_REC_SPELLTARGETRESTRICTIONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellTargetRestrictionsRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ float m_maxTargetRadius;
    /* 0008 */ int32_t m_maxAffectedTargets;
    /* 000C */ int32_t m_maxTargetLevel;
    /* 0010 */ int32_t m_targetCreatureType;
    /* 0014 */ int32_t m_targets;
};

#endif
