#include "WoW/Source/DB/Rec/TerrainTypeSoundsRec.h"

// STATIC: %0x00B4491C
const DBCMeta TerrainTypeSoundsRec::s_meta = { "DBFilesClient\\TerrainTypeSounds.dbc", 1, 4, TerrainTypeSoundsRec::FixupStrings, 0 };

// PROVEN: 89654c27 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TerrainTypeSoundsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void TerrainTypeSoundsRec::ConvertRow(TerrainTypeSoundsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
}
#endif
