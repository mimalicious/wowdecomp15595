#include "WoW/Source/DB/Rec/gtChanceToSpellCritRec.h"

// STATIC: %0x00B46694
const DBCMeta gtChanceToSpellCritRec::s_meta = { "DBFilesClient\\gtChanceToSpellCrit.dbc", 1, 8, gtChanceToSpellCritRec::FixupStrings, 0 };

// PROVEN: 6340825f IDENTICAL
// OFFSET: 0x00401000
void gtChanceToSpellCritRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtChanceToSpellCritRec::ConvertRow(gtChanceToSpellCritRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
