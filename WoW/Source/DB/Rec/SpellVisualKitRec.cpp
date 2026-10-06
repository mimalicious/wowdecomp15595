#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"

// STATIC: 0x00C1CA40/0x140A10760
static const int32_t s_fieldOffsets[22] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 60, 64, 68, 72, 88, 104, 120, 136, 152 };

// STATIC: 0x00C1CA98/0x140A107F0
static const int32_t s_fieldSizes[22] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 4, 4, 4, 16, 16, 16, 16, 16, 4 };

// STATIC: 0x00C1CAF0/0x140A10850
static const int32_t s_fieldTypes[22] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0 };

// STATIC: 0x00C1CB48/0x140A107B8
const DBCMeta SpellVisualKitRec::s_meta = { "DBFilesClient\\SpellVisualKit.dbc", 22, 156, SpellVisualKitRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: e5b30eb5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellVisualKitRec::FixupStrings(void* record, const char* strings) {
}
