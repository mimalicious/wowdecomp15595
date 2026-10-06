#include "WoW/Source/DB/Rec/TalentRec.h"

// STATIC: %0x00B44A84
const DBCMeta TalentRec::s_meta = { "DBFilesClient\\Talent.dbc", 10, 76, TalentRec::FixupStrings, 0 };

// PROVEN: bc046477 IDENTICAL/NONE
// OFFSET: 0x00401000
void TalentRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TalentRec::ConvertRow(TalentRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_tabID, &row[0x0004], sizeof(rec->m_tabID));
    memcpy(&rec->m_tierID, &row[0x0008], sizeof(rec->m_tierID));
    memcpy(&rec->m_columnIndex, &row[0x000C], sizeof(rec->m_columnIndex));
    memcpy(rec->m_spellRank, &row[0x0010], sizeof(rec->m_spellRank));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
    memcpy(&rec->m_flags, &row[0x003C], sizeof(rec->m_flags));
    memcpy(&rec->m_requiredSpellID, &row[0x0040], sizeof(rec->m_requiredSpellID));
    memcpy(rec->m_categoryMask, &row[0x0044], sizeof(rec->m_categoryMask));
}
#endif
