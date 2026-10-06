#include "WoW/Source/DB/Rec/gtChanceToMeleeCritBaseRec.h"

// STATIC: %0x00B466D4
const DBCMeta gtChanceToMeleeCritBaseRec::s_meta = { "DBFilesClient\\gtChanceToMeleeCritBase.dbc", 1, 8, gtChanceToMeleeCritBaseRec::FixupStrings, 0 };

// PROVEN: 2734f430 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtChanceToMeleeCritBaseRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtChanceToMeleeCritBaseRec::ConvertRow(gtChanceToMeleeCritBaseRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif
