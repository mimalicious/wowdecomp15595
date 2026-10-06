#include "WoW/Source/DB/Rec/FootstepTerrainLookupRec.h"

// STATIC: 0x00C25524/0x140A1B0E0
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C25538/0x140A1B0F8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C2554C/0x140A1B110
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C25560/0x140A1B128
const DBCMeta FootstepTerrainLookupRec::s_meta = { "DBFilesClient\\FootstepTerrainLookup.dbc", 5, 20, FootstepTerrainLookupRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 4a49423a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void FootstepTerrainLookupRec::FixupStrings(void* record, const char* strings) {
}
