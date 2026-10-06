#include "WoW/Source/DB/Rec/LfgDungeonExpansionRec.h"

// STATIC: %0x00B45E3C
const DBCMeta LfgDungeonExpansionRec::s_meta = { "DBFilesClient\\LfgDungeonExpansion.dbc", 8, 32, LfgDungeonExpansionRec::FixupStrings, 0 };

// PROVEN: 0bbc8ea2 IDENTICAL
// OFFSET: 0x00401000
void LfgDungeonExpansionRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LfgDungeonExpansionRec::ConvertRow(LfgDungeonExpansionRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_lfgID, &row[0x0004], sizeof(rec->m_lfgID));
    memcpy(&rec->m_expansionLevel, &row[0x0008], sizeof(rec->m_expansionLevel));
    memcpy(&rec->m_randomID, &row[0x000C], sizeof(rec->m_randomID));
    memcpy(&rec->m_hardLevelMin, &row[0x0010], sizeof(rec->m_hardLevelMin));
    memcpy(&rec->m_hardLevelMax, &row[0x0014], sizeof(rec->m_hardLevelMax));
    memcpy(&rec->m_targetLevelMin, &row[0x0018], sizeof(rec->m_targetLevelMin));
    memcpy(&rec->m_targetLevelMax, &row[0x001C], sizeof(rec->m_targetLevelMax));
}
#endif
