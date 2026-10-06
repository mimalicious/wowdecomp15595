#include "WoW/Source/DB/Rec/TerrainMaterialRec.h"

// STATIC: %0x00B409E8
const DBCMeta TerrainMaterialRec::s_meta = { "DBFilesClient\\TerrainMaterial.dbc", 4, 16, TerrainMaterialRec::FixupStrings, 0 };

// OFFSET: %0x0097C2F0
void TerrainMaterialRec::FixupStrings(void* record, const char* strings) {
    TerrainMaterialRec* rec = static_cast<TerrainMaterialRec*>(record);
    rec->unk_0004 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0004)];
    rec->unk_000C = &strings[reinterpret_cast<uintptr_t>(rec->unk_000C)];
}

#if defined(WOWDECOMP_ARCH_64)
void TerrainMaterialRec::ConvertRow(TerrainMaterialRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->unk_0004 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    rec->unk_000C = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
}
#endif
