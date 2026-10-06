#include "WoW/Source/DB/Rec/gtChanceToMeleeCritRec.h"

// STATIC: %0x00B46710
const DBCMeta gtChanceToMeleeCritRec::s_meta = { "DBFilesClient\\gtChanceToMeleeCrit.dbc", 1, 8, gtChanceToMeleeCritRec::FixupStrings, 0 };

// PROVEN: 7bd782d2 IDENTICAL
// OFFSET: 0x00401000
void gtChanceToMeleeCritRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtChanceToMeleeCritRec::ConvertRow(gtChanceToMeleeCritRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
