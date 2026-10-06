#include "WoW/Source/DB/Rec/TerrainMaterialRec.h"

// STATIC: 0x00C27BC8/0x140A1E1B0
const DBCMeta TerrainMaterialRec::s_meta = { "DBFilesClient\\TerrainMaterial.dbc", 4, 16, TerrainMaterialRec::FixupStrings, 0 };

// PROVEN: a234b835 IDENTICAL/NONE
// OFFSET: 0x00A3B710/0x1407E8520
void TerrainMaterialRec::FixupStrings(void* record, const char* strings) {
    TerrainMaterialRec* rec = static_cast<TerrainMaterialRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
    DBCFixupString(rec->unk_000C, strings);
}
