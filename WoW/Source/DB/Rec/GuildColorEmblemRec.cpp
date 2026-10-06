#include "WoW/Source/DB/Rec/GuildColorEmblemRec.h"

// STATIC: 0x00C1D338/0x140A11200
static const int32_t s_fieldOffsets[4] = { 0, 4, 5, 6 };

// STATIC: 0x00C1D348/0x140A11210
static const int32_t s_fieldSizes[4] = { 4, 1, 1, 1 };

// STATIC: 0x00C1D358/0x140A11220
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1D368/0x140A11230
const DBCMeta GuildColorEmblemRec::s_meta = { "DBFilesClient\\GuildColorEmblem.dbc", 4, 8, GuildColorEmblemRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6cd5771c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildColorEmblemRec::FixupStrings(void* record, const char* strings) {
}
