#ifndef WOW_SOURCE_DB_REC_SKILLLINEABILITYREC_H
#define WOW_SOURCE_DB_REC_SKILLLINEABILITYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class SkillLineAbilityRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_skillLine;
    /* 0008 */ int32_t m_spell;
    /* 000C */ int32_t m_raceMask;
    /* 0010 */ int32_t m_classMask;
    /* 0014 */ int32_t m_excludeRace;
    /* 0018 */ int32_t m_excludeClass;
    /* 001C */ int32_t m_minSkillLineRank;
    /* 0020 */ int32_t m_supercededBySpell;
    /* 0024 */ int32_t m_acquireMethod;
    /* 0028 */ int32_t m_trivialSkillLineRankHigh;
    /* 002C */ int32_t m_trivialSkillLineRankLow;
    /* 0030 */ int32_t m_characterPoints [2];

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(SkillLineAbilityRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif
