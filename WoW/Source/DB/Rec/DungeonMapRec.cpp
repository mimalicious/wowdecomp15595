#include "WoW/Source/DB/Rec/DungeonMapRec.h"

// STATIC: 0x00C25A98/0x140A1B7C0
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 20, 28 };

// STATIC: 0x00C25AB0/0x140A1B7D8
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 8, 8, 4 };

// STATIC: 0x00C25AC8/0x140A1B7F0
static const int32_t s_fieldTypes[6] = { 0, 0, 0, 3, 3, 0 };

// STATIC: 0x00C25AE0/0x140A1B808
const DBCMeta DungeonMapRec::s_meta = { "DBFilesClient\\DungeonMap.dbc", 6, 32, DungeonMapRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void DungeonMapRec::FixupStrings(void* record, const char* strings) {
}
