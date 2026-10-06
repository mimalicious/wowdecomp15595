#include "WoW/Source/DB/Rec/SpellCategoryRec.h"

// STATIC: %0x00B452C4
const DBCMeta SpellCategoryRec::s_meta = { "DBFilesClient\\SpellCategory.dbc", 2, 8, SpellCategoryRec::FixupStrings, 0 };

// PROVEN: f4838ce3 IDENTICAL/NONE
// OFFSET: 0x00401000
void SpellCategoryRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellCategoryRec::ConvertRow(SpellCategoryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
}
#endif
