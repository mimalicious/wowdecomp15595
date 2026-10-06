#include "WoW/Source/DB/Rec/AnimReplacementSetRec.h"

// STATIC: 0x00C1DA8C/0x140A11A90
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1DA94/0x140A11A98
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1DA9C/0x140A11AA0
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1DAA4/0x140A11AA8
const DBCMeta AnimReplacementSetRec::s_meta = { "DBFilesClient\\AnimReplacementSet.dbc", 2, 8, AnimReplacementSetRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 28906208 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimReplacementSetRec::FixupStrings(void* record, const char* strings) {
}
