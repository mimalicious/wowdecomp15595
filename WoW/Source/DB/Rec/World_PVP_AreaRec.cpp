#include "WoW/Source/DB/Rec/World_PVP_AreaRec.h"

// STATIC: %0x00B44628
const DBCMeta World_PVP_AreaRec::s_meta = { "DBFilesClient\\World_PVP_Area.dbc", 7, 28, World_PVP_AreaRec::FixupStrings, 0 };

// PROVEN: eadd3d2d IDENTICAL
// OFFSET: 0x00401000
void World_PVP_AreaRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void World_PVP_AreaRec::ConvertRow(World_PVP_AreaRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
}
#endif
