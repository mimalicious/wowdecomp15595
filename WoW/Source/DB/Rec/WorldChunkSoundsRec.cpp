#include "WoW/Source/DB/Rec/WorldChunkSoundsRec.h"

// STATIC: 0x00C1EDB4/0x140A131C8
static const int32_t s_fieldOffsets[9] = { 4, 8, 12, 16, 20, 24, 28, 32, 36 };

// STATIC: 0x00C1EDD8/0x140A131F0
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1EDFC/0x140A13218
static const int32_t s_fieldTypes[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1EE20/0x140A13240
const DBCMeta WorldChunkSoundsRec::s_meta = { "DBFilesClient\\WorldChunkSounds.dbc", 9, 40, WorldChunkSoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void WorldChunkSoundsRec::FixupStrings(void* record, const char* strings) {
}
