#include "WoW/Source/DB/Rec/TerrainMaterialRec.h"

// STATIC: 0x00C27B98/0x140A1E180
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C27BA8/0x140A1E190
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C27BB8/0x140A1E1A0
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 2 };

// STATIC: 0x00C27BC8/0x140A1E1B0
const DBCMeta TerrainMaterialRec::s_meta = { "DBFilesClient\\TerrainMaterial.dbc", 4, 16, TerrainMaterialRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a234b835 IDENTICAL/NONE
// OFFSET: 0x00A3B710/0x1407E8520
void TerrainMaterialRec::FixupStrings(void* record, const char* strings) {
    TerrainMaterialRec* rec = static_cast<TerrainMaterialRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
    DBCFixupString(rec->unk_000C, strings);
}
