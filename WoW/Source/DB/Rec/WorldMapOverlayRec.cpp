#include "WoW/Source/DB/Rec/WorldMapOverlayRec.h"

// STATIC: %0x00B444DC
const DBCMeta WorldMapOverlayRec::s_meta = { "DBFilesClient\\WorldMapOverlay.dbc", 12, 60, WorldMapOverlayRec::FixupStrings, 0 };

// OFFSET: %0x0097C260
void WorldMapOverlayRec::FixupStrings(void* record, const char* strings) {
    WorldMapOverlayRec* rec = static_cast<WorldMapOverlayRec*>(record);
    rec->unk_0018 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0018)];
}

#if defined(WOWDECOMP_ARCH_64)
void WorldMapOverlayRec::ConvertRow(WorldMapOverlayRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_mapAreaID, &row[0x0004], sizeof(rec->m_mapAreaID));
    memcpy(rec->m_areaID, &row[0x0008], sizeof(rec->m_areaID));
    rec->unk_0018 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
}
#endif
