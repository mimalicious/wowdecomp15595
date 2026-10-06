#include "WoW/Source/DB/Rec/SpellDifficultyRec.h"

// STATIC: %0x00B4517C
const DBCMeta SpellDifficultyRec::s_meta = { "DBFilesClient\\SpellDifficulty.dbc", 2, 20, SpellDifficultyRec::FixupStrings, 0 };

// PROVEN: 52c001bf IDENTICAL
// OFFSET: 0x00401000
void SpellDifficultyRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellDifficultyRec::ConvertRow(SpellDifficultyRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_difficultySpellID, &row[0x0004], sizeof(rec->m_difficultySpellID));
}
#endif
