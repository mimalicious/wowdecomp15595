#include "WoW/Source/DB/Rec/LockRec.h"

// STATIC: %0x00B45CE8
const DBCMeta LockRec::s_meta = { "DBFilesClient\\Lock.dbc", 5, 132, LockRec::FixupStrings, 0 };

// PROVEN: 52812e8c IDENTICAL
// OFFSET: 0x00401000
void LockRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LockRec::ConvertRow(LockRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_type, &row[0x0004], sizeof(rec->m_type));
    memcpy(rec->m_index, &row[0x0024], sizeof(rec->m_index));
    memcpy(rec->m_skill, &row[0x0044], sizeof(rec->m_skill));
    memcpy(rec->m_action, &row[0x0064], sizeof(rec->m_action));
}
#endif
