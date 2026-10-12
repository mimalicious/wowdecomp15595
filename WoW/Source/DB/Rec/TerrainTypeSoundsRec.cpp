#include "WoW/Source/DB/Rec/TerrainTypeSoundsRec.h"

// STATIC: 0x00C20070/0x140A147C8
static const int32_t s_fieldOffsets[1] = { 0 };

// STATIC: 0x00C20074/0x140A147CC
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C20078/0x140A147D0
static const int32_t s_fieldTypes[1] = { 0 };

// STATIC: 0x00C2007C/0x140A147D8
const DBCMeta TerrainTypeSoundsRec::s_meta = { "DBFilesClient\\TerrainTypeSounds.dbc", 1, 4, TerrainTypeSoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void TerrainTypeSoundsRec::FixupStrings(void* record, const char* strings) {
}
