#include "WoW/Source/DB/Rec/DungeonMapChunkRec.h"

// STATIC: 0x00C25A1C/0x140A1B720
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C25A30/0x140A1B738
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C25A44/0x140A1B750
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 3 };

// STATIC: 0x00C25A58/0x140A1B768
const DBCMeta DungeonMapChunkRec::s_meta = { "DBFilesClient\\DungeonMapChunk.dbc", 5, 20, DungeonMapChunkRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: cc8cc2c5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DungeonMapChunkRec::FixupStrings(void* record, const char* strings) {
}
