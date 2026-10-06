#include "WoW/Source/DB/Rec/AreaAssignmentRec.h"

// STATIC: %0x00B47208
const DBCMeta AreaAssignmentRec::s_meta = { "DBFilesClient\\AreaAssignment.dbc", 4, 20, AreaAssignmentRec::FixupStrings, 0 };

// PROVEN: 15a4a205 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AreaAssignmentRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AreaAssignmentRec::ConvertRow(AreaAssignmentRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
}
#endif
