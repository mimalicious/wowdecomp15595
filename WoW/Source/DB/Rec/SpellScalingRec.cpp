#include "WoW/Source/DB/Rec/SpellScalingRec.h"

// STATIC: 0x00C20B58/0x140A154E8
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 16, 20, 32, 44, 56, 60 };

// STATIC: 0x00C20B80/0x140A15510
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 4, 4, 12, 12, 12, 4, 4 };

// STATIC: 0x00C20BA8/0x140A15538
static const int32_t s_fieldTypes[10] = { 0, 0, 0, 0, 0, 3, 3, 3, 3, 0 };

// STATIC: 0x00C20BD0/0x140A15560
const DBCMeta SpellScalingRec::s_meta = { "DBFilesClient\\SpellScaling.dbc", 10, 64, SpellScalingRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: b62f4f38 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellScalingRec::FixupStrings(void* record, const char* strings) {
}
