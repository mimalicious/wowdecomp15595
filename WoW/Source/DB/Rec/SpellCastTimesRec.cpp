#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"

// STATIC: %0x00B45334
const DBCMeta SpellCastTimesRec::s_meta = { "DBFilesClient\\SpellCastTimes.dbc", 4, 16, SpellCastTimesRec::FixupStrings, 0 };

// PROVEN: e918ec1d IDENTICAL
// OFFSET: 0x00401000
void SpellCastTimesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellCastTimesRec::ConvertRow(SpellCastTimesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_base, &row[0x0004], sizeof(rec->m_base));
    memcpy(&rec->m_perLevel, &row[0x0008], sizeof(rec->m_perLevel));
    memcpy(&rec->m_minimum, &row[0x000C], sizeof(rec->m_minimum));
}
#endif
