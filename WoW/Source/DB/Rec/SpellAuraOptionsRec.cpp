#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"

// STATIC: %0x00B453E8
const DBCMeta SpellAuraOptionsRec::s_meta = { "DBFilesClient\\SpellAuraOptions.dbc", 5, 20, SpellAuraOptionsRec::FixupStrings, 0 };

// PROVEN: 3f176059 IDENTICAL/NONE
// OFFSET: 0x00401000
void SpellAuraOptionsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellAuraOptionsRec::ConvertRow(SpellAuraOptionsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
}
#endif
