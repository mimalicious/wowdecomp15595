#include "WoW/Source/DB/Rec/WorldMapContinentRec.h"

// STATIC: 0x00C1F2F4/0x140A13810
static const int32_t s_fieldOffsets[11] = { 0, 4, 8, 12, 16, 20, 24, 32, 36, 44, 52 };

// STATIC: 0x00C1F320/0x140A13840
static const int32_t s_fieldSizes[11] = { 4, 4, 4, 4, 4, 4, 8, 4, 8, 8, 4 };

// STATIC: 0x00C1F34C/0x140A13870
static const int32_t s_fieldTypes[11] = { 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0 };

// STATIC: 0x00C1F378/0x140A138A0
const DBCMeta WorldMapContinentRec::s_meta = { "DBFilesClient\\WorldMapContinent.dbc", 11, 56, WorldMapContinentRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void WorldMapContinentRec::FixupStrings(void* record, const char* strings) {
}
