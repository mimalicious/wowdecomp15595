#include "WoW/Source/DB/Rec/SpellVisualRec.h"

// STATIC: 0x00C20718/0x140A14FE0
static const int32_t s_fieldOffsets[29] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 116, 128 };

// STATIC: 0x00C20790/0x140A15060
static const int32_t s_fieldSizes[29] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 12, 4 };

// STATIC: 0x00C20808/0x140A150E0
static const int32_t s_fieldTypes[29] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0 };

// STATIC: 0x00C2087C/0x140A15158
const DBCMeta SpellVisualRec::s_meta = { "DBFilesClient\\SpellVisual.dbc", 29, 132, SpellVisualRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellVisualRec::FixupStrings(void* record, const char* strings) {
}
