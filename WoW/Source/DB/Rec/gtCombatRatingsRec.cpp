#include "WoW/Source/DB/Rec/gtCombatRatingsRec.h"

// STATIC: %0x00B46748
const DBCMeta gtCombatRatingsRec::s_meta = { "DBFilesClient\\gtCombatRatings.dbc", 1, 8, gtCombatRatingsRec::FixupStrings, 0 };

// PROVEN: 13741388 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtCombatRatingsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtCombatRatingsRec::ConvertRow(gtCombatRatingsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
