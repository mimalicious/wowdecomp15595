#include "WoW/Source/DB/Rec/WorldMapAreaRec.h"

// STATIC: %0x00B44548
const DBCMeta WorldMapAreaRec::s_meta = { "DBFilesClient\\WorldMapArea.dbc", 12, 48, WorldMapAreaRec::FixupStrings, 0 };

// OFFSET: %0x0097C4E0
void WorldMapAreaRec::FixupStrings(void* record, const char* strings) {
    WorldMapAreaRec* rec = static_cast<WorldMapAreaRec*>(record);
    rec->m_areaName = &strings[reinterpret_cast<uintptr_t>(rec->m_areaName)];
}

#if defined(WOWDECOMP_ARCH_64)
void WorldMapAreaRec::ConvertRow(WorldMapAreaRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapID, &row[0x0004], sizeof(rec->m_mapID));
    memcpy(&rec->m_areaID, &row[0x0008], sizeof(rec->m_areaID));
    rec->m_areaName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
}
#endif
