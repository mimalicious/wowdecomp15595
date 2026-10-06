#include "WoW/Source/DB/Rec/SpellEquippedItemsRec.h"

// STATIC: %0x00B45098
const DBCMeta SpellEquippedItemsRec::s_meta = { "DBFilesClient\\SpellEquippedItems.dbc", 4, 16, SpellEquippedItemsRec::FixupStrings, 0 };

// PROVEN: bc0f144a IDENTICAL
// OFFSET: 0x00401000
void SpellEquippedItemsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellEquippedItemsRec::ConvertRow(SpellEquippedItemsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
