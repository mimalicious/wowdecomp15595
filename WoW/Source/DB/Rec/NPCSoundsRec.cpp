#include "WoW/Source/DB/Rec/NPCSoundsRec.h"

// STATIC: %0x00B45B8C
const DBCMeta NPCSoundsRec::s_meta = { "DBFilesClient\\NPCSounds.dbc", 2, 20, NPCSoundsRec::FixupStrings, 0 };

// PROVEN: 954e42c9 IDENTICAL
// OFFSET: 0x00401000
void NPCSoundsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void NPCSoundsRec::ConvertRow(NPCSoundsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_soundID, &row[0x0004], sizeof(rec->m_soundID));
}
#endif
