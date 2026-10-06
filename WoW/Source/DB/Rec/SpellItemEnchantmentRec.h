#ifndef WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTREC_H
#define WOW_SOURCE_DB_REC_SPELLITEMENCHANTMENTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SpellItemEnchantmentRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_charges;
    /* 0008 */ int32_t m_effect [3];
    /* 0014 */ int32_t m_effectPointsMin [3];
    /* 0020 */ int32_t m_effectPointsMax [3];
    /* 002C */ int32_t m_effectArg [3];
    /* 0038 */ DBCString m_name;
    /* 003C */ int32_t m_itemVisual;
    /* 0040 */ int32_t m_flags;
    /* 0044 */ int32_t m_srcItemID;
    /* 0048 */ int32_t m_conditionID;
    /* 004C */ int32_t m_requiredSkillID;
    /* 0050 */ int32_t m_requiredSkillRank;
    /* 0054 */ int32_t m_minLevel;
    /* 0058 */ int32_t unk_0058;
};

#endif
