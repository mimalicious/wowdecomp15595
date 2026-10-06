#include "WoW/Source/DB/Rec/CastableRaidBuffsRec.h"

// STATIC: %0x00B47024
const DBCMeta CastableRaidBuffsRec::s_meta = { "DBFilesClient\\CastableRaidBuffs.dbc", 3, 12, CastableRaidBuffsRec::FixupStrings, 0 };

// PROVEN: 7cf93d9b IDENTICAL
// OFFSET: 0x00401000
void CastableRaidBuffsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CastableRaidBuffsRec::ConvertRow(CastableRaidBuffsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
}
#endif
