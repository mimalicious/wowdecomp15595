#include "WoW/Source/DB/Rec/TerrainTypeRec.h"

// STATIC: 0x00C27B54/0x140A1E120
const DBCMeta TerrainTypeRec::s_meta = { "DBFilesClient\\TerrainType.dbc", 6, 28, TerrainTypeRec::FixupStrings, 0 };

// PROVEN: ca9ae632 IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void TerrainTypeRec::FixupStrings(void* record, const char* strings) {
    TerrainTypeRec* rec = static_cast<TerrainTypeRec*>(record);
    DBCFixupString(rec->m_terrainDesc, strings);
}
