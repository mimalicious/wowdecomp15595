#include "WoW/Source/DB/Rec/ArmorLocationRec.h"

// STATIC: %0x00B4719C
const DBCMeta ArmorLocationRec::s_meta = { "DBFilesClient\\ArmorLocation.dbc", 6, 24, ArmorLocationRec::FixupStrings, 0 };

// PROVEN: 708698ed IDENTICAL
// OFFSET: 0x00401000
void ArmorLocationRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ArmorLocationRec::ConvertRow(ArmorLocationRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
}
#endif
