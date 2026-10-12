#include "WoW/Source/DB/Rec/WorldStateZoneSoundsRec.h"

// STATIC: 0x00C1EE68/0x140A132A0
static const int32_t s_fieldOffsets[8] = { 4, 8, 12, 16, 20, 24, 28, 32 };

// STATIC: 0x00C1EE88/0x140A132C0
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1EEA8/0x140A132E0
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1EEC8/0x140A13300
const DBCMeta WorldStateZoneSoundsRec::s_meta = { "DBFilesClient\\WorldStateZoneSounds.dbc", 8, 36, WorldStateZoneSoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void WorldStateZoneSoundsRec::FixupStrings(void* record, const char* strings) {
}
