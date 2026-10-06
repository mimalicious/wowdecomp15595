#include "WoW/Source/DB/Rec/WorldMapTransformsRec.h"

// STATIC: %0x00B444A4
const DBCMeta WorldMapTransformsRec::s_meta = { "DBFilesClient\\WorldMapTransforms.dbc", 8, 44, WorldMapTransformsRec::FixupStrings, 0 };

// PROVEN: c3931a06 IDENTICAL/NONE
// OFFSET: 0x00401000
void WorldMapTransformsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void WorldMapTransformsRec::ConvertRow(WorldMapTransformsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
}
#endif
