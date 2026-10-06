#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"

// STATIC: %0x00B45500
const DBCMeta SkillRaceClassInfoRec::s_meta = { "DBFilesClient\\SkillRaceClassInfo.dbc", 9, 36, SkillRaceClassInfoRec::FixupStrings, 0 };

// PROVEN: 9013b2aa IDENTICAL/NONE
// OFFSET: 0x00401000
void SkillRaceClassInfoRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SkillRaceClassInfoRec::ConvertRow(SkillRaceClassInfoRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_skillID, &row[0x0004], sizeof(rec->m_skillID));
    memcpy(&rec->m_raceMask, &row[0x0008], sizeof(rec->m_raceMask));
    memcpy(&rec->m_classMask, &row[0x000C], sizeof(rec->m_classMask));
    memcpy(&rec->m_flags, &row[0x0010], sizeof(rec->m_flags));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->m_skillTierID, &row[0x001C], sizeof(rec->m_skillTierID));
    memcpy(&rec->m_skillCostIndex, &row[0x0020], sizeof(rec->m_skillCostIndex));
}
#endif
