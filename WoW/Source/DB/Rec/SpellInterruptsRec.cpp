#include "WoW/Source/DB/Rec/SpellInterruptsRec.h"

// STATIC: %0x00B44F88
const DBCMeta SpellInterruptsRec::s_meta = { "DBFilesClient\\SpellInterrupts.dbc", 4, 24, SpellInterruptsRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void SpellInterruptsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellInterruptsRec::ConvertRow(SpellInterruptsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
}
#endif
