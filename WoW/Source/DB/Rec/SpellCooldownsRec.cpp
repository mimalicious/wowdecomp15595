#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"

// STATIC: %0x00B45220
const DBCMeta SpellCooldownsRec::s_meta = { "DBFilesClient\\SpellCooldowns.dbc", 4, 16, SpellCooldownsRec::FixupStrings, 0 };

// PROVEN: 1e47ea09 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellCooldownsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellCooldownsRec::ConvertRow(SpellCooldownsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
