#include "WoW/Source/DB/Rec/ItemReforgeRec.h"

// STATIC: %0x00B45F3C
const DBCMeta ItemReforgeRec::s_meta = { "DBFilesClient\\ItemReforge.dbc", 5, 20, ItemReforgeRec::FixupStrings, 0 };

// PROVEN: 225f83aa IDENTICAL/NONE
// OFFSET: 0x00401000
void ItemReforgeRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ItemReforgeRec::ConvertRow(ItemReforgeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
}
#endif
