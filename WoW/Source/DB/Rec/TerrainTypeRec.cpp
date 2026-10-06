#include "WoW/Source/DB/Rec/TerrainTypeRec.h"

// STATIC: %0x00B44950
const DBCMeta TerrainTypeRec::s_meta = { "DBFilesClient\\TerrainType.dbc", 6, 28, TerrainTypeRec::FixupStrings, 0 };

// OFFSET: %0x0097C380
void TerrainTypeRec::FixupStrings(void* record, const char* strings) {
    TerrainTypeRec* rec = static_cast<TerrainTypeRec*>(record);
    rec->m_terrainDesc = &strings[reinterpret_cast<uintptr_t>(rec->m_terrainDesc)];
}

#if defined(WOWDECOMP_ARCH_64)
void TerrainTypeRec::ConvertRow(TerrainTypeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_terrainID, &row[0x0004], sizeof(rec->m_terrainID));
    rec->m_terrainDesc = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_footstepSprayRun, &row[0x000C], sizeof(rec->m_footstepSprayRun));
    memcpy(&rec->m_footstepSprayWalk, &row[0x0010], sizeof(rec->m_footstepSprayWalk));
    memcpy(&rec->m_soundID, &row[0x0014], sizeof(rec->m_soundID));
    memcpy(&rec->m_flags, &row[0x0018], sizeof(rec->m_flags));
}
#endif
