#include "WoW/Source/DB/Rec/SpellDurationRec.h"

// STATIC: %0x00B4510C
const DBCMeta SpellDurationRec::s_meta = { "DBFilesClient\\SpellDuration.dbc", 4, 16, SpellDurationRec::FixupStrings, 0 };

// PROVEN: 0cfcc569 IDENTICAL/NONE
// OFFSET: 0x00401000
void SpellDurationRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellDurationRec::ConvertRow(SpellDurationRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_duration, &row[0x0004], sizeof(rec->m_duration));
    memcpy(&rec->m_durationPerLevel, &row[0x0008], sizeof(rec->m_durationPerLevel));
    memcpy(&rec->m_maxDuration, &row[0x000C], sizeof(rec->m_maxDuration));
}
#endif
