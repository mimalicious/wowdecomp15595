#include "WoW/Source/DB/Rec/WorldSafeLocsRec.h"

// STATIC: %0x00B44468
const DBCMeta WorldSafeLocsRec::s_meta = { "DBFilesClient\\WorldSafeLocs.dbc", 4, 24, WorldSafeLocsRec::FixupStrings, 0 };

// OFFSET: %0x0097C410
void WorldSafeLocsRec::FixupStrings(void* record, const char* strings) {
    WorldSafeLocsRec* rec = static_cast<WorldSafeLocsRec*>(record);
    rec->m_areaName = &strings[reinterpret_cast<uintptr_t>(rec->m_areaName)];
}

#if defined(WOWDECOMP_ARCH_64)
void WorldSafeLocsRec::ConvertRow(WorldSafeLocsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_continent, &row[0x0004], sizeof(rec->m_continent));
    memcpy(rec->m_loc, &row[0x0008], sizeof(rec->m_loc));
    rec->m_areaName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
}
#endif
