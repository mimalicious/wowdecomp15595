#include "WoW/Source/DB/Rec/TerrainTypeRec.h"

// STATIC: 0x00C27B0C/0x140A1E0D8
static const int32_t s_fieldOffsets[6] = { 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C27B24/0x140A1E0F0
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C27B3C/0x140A1E108
static const int32_t s_fieldTypes[6] = { 0, 2, 0, 0, 0, 0 };

// STATIC: 0x00C27B54/0x140A1E120
const DBCMeta TerrainTypeRec::s_meta = { "DBFilesClient\\TerrainType.dbc", 6, 28, TerrainTypeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECA0/0x1407D8230
void TerrainTypeRec::FixupStrings(void* record, const char* strings) {
    TerrainTypeRec* rec = static_cast<TerrainTypeRec*>(record);
    DBCFixupString(rec->m_terrainDesc, strings);
}
