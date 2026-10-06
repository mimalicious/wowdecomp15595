#include "WoW/Source/DB/Rec/SpellEffectRec.h"

// STATIC: 0x00C1CC40/0x140A109A0
static const int32_t s_fieldOffsets[22] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 56, 60, 68, 72, 84, 88, 96, 100, 104 };

// STATIC: 0x00C1CC98/0x140A10A30
static const int32_t s_fieldSizes[22] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 8, 4, 12, 4, 8, 4, 4, 4 };

// STATIC: 0x00C1CCF0/0x140A10A90
static const int32_t s_fieldTypes[22] = { 0, 0, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1CD48/0x140A109F8
const DBCMeta SpellEffectRec::s_meta = { "DBFilesClient\\SpellEffect.dbc", 22, 108, SpellEffectRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 2618e038 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellEffectRec::FixupStrings(void* record, const char* strings) {
}
