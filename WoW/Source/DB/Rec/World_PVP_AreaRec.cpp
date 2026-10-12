#include "WoW/Source/DB/Rec/World_PVP_AreaRec.h"

// STATIC: 0x00C1F5A0/0x140A13B38
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C1F5BC/0x140A13B58
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1F5D8/0x140A13B78
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1F5F4/0x140A13B98
const DBCMeta World_PVP_AreaRec::s_meta = { "DBFilesClient\\World_PVP_Area.dbc", 7, 28, World_PVP_AreaRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void World_PVP_AreaRec::FixupStrings(void* record, const char* strings) {
}
