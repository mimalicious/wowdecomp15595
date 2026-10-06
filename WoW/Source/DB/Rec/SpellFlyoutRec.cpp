#include "WoW/Source/DB/Rec/SpellFlyoutRec.h"

// STATIC: %0x00B4505C
const DBCMeta SpellFlyoutRec::s_meta = { "DBFilesClient\\SpellFlyout.dbc", 7, 28, SpellFlyoutRec::FixupStrings, 0 };

// OFFSET: %0x0097C220
void SpellFlyoutRec::FixupStrings(void* record, const char* strings) {
    SpellFlyoutRec* rec = static_cast<SpellFlyoutRec*>(record);
    rec->unk_0014 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0014)];
    rec->unk_0018 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0018)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellFlyoutRec::ConvertRow(SpellFlyoutRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    rec->unk_0014 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->unk_0018 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
}
#endif
