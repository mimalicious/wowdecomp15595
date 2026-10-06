#include "WoW/Source/DB/Rec/SpellClassOptionsRec.h"

// STATIC: %0x00B45258
const DBCMeta SpellClassOptionsRec::s_meta = { "DBFilesClient\\SpellClassOptions.dbc", 5, 28, SpellClassOptionsRec::FixupStrings, 0 };

// OFFSET: %0x0097C260
void SpellClassOptionsRec::FixupStrings(void* record, const char* strings) {
    SpellClassOptionsRec* rec = static_cast<SpellClassOptionsRec*>(record);
    rec->unk_0018 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0018)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellClassOptionsRec::ConvertRow(SpellClassOptionsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    rec->unk_0018 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
}
#endif
