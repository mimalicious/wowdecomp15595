#include "WoW/Source/DB/Rec/PvpDifficultyRec.h"

// STATIC: %0x00B45978
const DBCMeta PvpDifficultyRec::s_meta = { "DBFilesClient\\PvpDifficulty.dbc", 6, 24, PvpDifficultyRec::FixupStrings, 0 };

// PROVEN: b210418e IDENTICAL/NONE
// OFFSET: 0x00401000
void PvpDifficultyRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void PvpDifficultyRec::ConvertRow(PvpDifficultyRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_rangeIndex, &row[0x0008], sizeof(rec->m_rangeIndex));
    memcpy(&rec->m_minLevel, &row[0x000C], sizeof(rec->m_minLevel));
    memcpy(&rec->m_maxLevel, &row[0x0010], sizeof(rec->m_maxLevel));
    memcpy(&rec->m_difficulty, &row[0x0014], sizeof(rec->m_difficulty));
}
#endif
