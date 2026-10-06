#include "WoW/Source/DB/Rec/ZoneLightPointRec.h"

// STATIC: %0x00B44394
const DBCMeta ZoneLightPointRec::s_meta = { "DBFilesClient\\ZoneLightPoint.dbc", 4, 20, ZoneLightPointRec::FixupStrings, 0 };

// PROVEN: 7168c7c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ZoneLightPointRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ZoneLightPointRec::ConvertRow(ZoneLightPointRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
}
#endif
