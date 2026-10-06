#include "WoW/Source/DB/Rec/WorldChunkSoundsRec.h"

// STATIC: 0x00C1EE20/0x140A13240
const DBCMeta WorldChunkSoundsRec::s_meta = { "DBFilesClient\\WorldChunkSounds.dbc", 9, 40, WorldChunkSoundsRec::FixupStrings, 0 };

// PROVEN: c8dbc3be IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void WorldChunkSoundsRec::FixupStrings(void* record, const char* strings) {
}
