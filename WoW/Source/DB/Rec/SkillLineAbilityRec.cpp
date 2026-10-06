#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"

// STATIC: %0x00B455E4
const DBCMeta SkillLineAbilityRec::s_meta = { "DBFilesClient\\SkillLineAbility.dbc", 14, 56, SkillLineAbilityRec::FixupStrings, 0 };

// PROVEN: e3e386b0 IDENTICAL
// OFFSET: 0x00401000
void SkillLineAbilityRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SkillLineAbilityRec::ConvertRow(SkillLineAbilityRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_skillLine, &row[0x0004], sizeof(rec->m_skillLine));
    memcpy(&rec->m_spell, &row[0x0008], sizeof(rec->m_spell));
    memcpy(&rec->m_raceMask, &row[0x000C], sizeof(rec->m_raceMask));
    memcpy(&rec->m_classMask, &row[0x0010], sizeof(rec->m_classMask));
    memcpy(&rec->m_excludeRace, &row[0x0014], sizeof(rec->m_excludeRace));
    memcpy(&rec->m_excludeClass, &row[0x0018], sizeof(rec->m_excludeClass));
    memcpy(&rec->m_minSkillLineRank, &row[0x001C], sizeof(rec->m_minSkillLineRank));
    memcpy(&rec->m_supercededBySpell, &row[0x0020], sizeof(rec->m_supercededBySpell));
    memcpy(&rec->m_acquireMethod, &row[0x0024], sizeof(rec->m_acquireMethod));
    memcpy(&rec->m_trivialSkillLineRankHigh, &row[0x0028], sizeof(rec->m_trivialSkillLineRankHigh));
    memcpy(&rec->m_trivialSkillLineRankLow, &row[0x002C], sizeof(rec->m_trivialSkillLineRankLow));
    memcpy(rec->m_characterPoints, &row[0x0030], sizeof(rec->m_characterPoints));
}
#endif
