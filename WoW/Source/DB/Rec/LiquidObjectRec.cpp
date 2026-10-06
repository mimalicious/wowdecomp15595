#include "WoW/Source/DB/Rec/LiquidObjectRec.h"

// STATIC: %0x00B40AFC
const DBCMeta LiquidObjectRec::s_meta = { "DBFilesClient\\LiquidObject.dbc", 6, 24, LiquidObjectRec::FixupStrings, 0 };

// PROVEN: eefa6274 IDENTICAL
// OFFSET: 0x00401000
void LiquidObjectRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LiquidObjectRec::ConvertRow(LiquidObjectRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
}
#endif
