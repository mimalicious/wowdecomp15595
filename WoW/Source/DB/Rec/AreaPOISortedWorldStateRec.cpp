#include "WoW/Source/DB/Rec/AreaPOISortedWorldStateRec.h"

// STATIC: %0x00B47248
const DBCMeta AreaPOISortedWorldStateRec::s_meta = { "DBFilesClient\\AreaPOISortedWorldState.dbc", 3, 12, AreaPOISortedWorldStateRec::FixupStrings, 0 };

// PROVEN: a0d3284a IDENTICAL
// OFFSET: 0x00401000
void AreaPOISortedWorldStateRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AreaPOISortedWorldStateRec::ConvertRow(AreaPOISortedWorldStateRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
}
#endif
