#include "WoW/Source/DB/Rec/gtChanceToSpellCritBaseRec.h"

// STATIC: %0x00B46658
const DBCMeta gtChanceToSpellCritBaseRec::s_meta = { "DBFilesClient\\gtChanceToSpellCritBase.dbc", 1, 8, gtChanceToSpellCritBaseRec::FixupStrings, 0 };

// PROVEN: e7dff06a IDENTICAL
// OFFSET: 0x00401000
void gtChanceToSpellCritBaseRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtChanceToSpellCritBaseRec::ConvertRow(gtChanceToSpellCritBaseRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
