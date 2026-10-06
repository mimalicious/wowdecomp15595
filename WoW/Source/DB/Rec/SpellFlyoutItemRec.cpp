#include "WoW/Source/DB/Rec/SpellFlyoutItemRec.h"

// STATIC: %0x00B45028
const DBCMeta SpellFlyoutItemRec::s_meta = { "DBFilesClient\\SpellFlyoutItem.dbc", 4, 16, SpellFlyoutItemRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void SpellFlyoutItemRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellFlyoutItemRec::ConvertRow(SpellFlyoutItemRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
