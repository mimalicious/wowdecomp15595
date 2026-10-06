#include "WoW/Source/DB/Rec/DungeonMapChunkRec.h"

// STATIC: 0x00C25A58/0x140A1B768
const DBCMeta DungeonMapChunkRec::s_meta = { "DBFilesClient\\DungeonMapChunk.dbc", 5, 20, DungeonMapChunkRec::FixupStrings, 0 };

// PROVEN: cc8cc2c5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DungeonMapChunkRec::FixupStrings(void* record, const char* strings) {
}
