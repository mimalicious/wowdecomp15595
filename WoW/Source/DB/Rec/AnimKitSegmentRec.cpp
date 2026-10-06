#include "WoW/Source/DB/Rec/AnimKitSegmentRec.h"

// STATIC: 0x00C1DB60/0x140A11BA0
static const int32_t s_fieldOffsets[16] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60 };

// STATIC: 0x00C1DBA0/0x140A11BE0
static const int32_t s_fieldSizes[16] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1DBE0/0x140A11C20
static const int32_t s_fieldTypes[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0 };

// STATIC: 0x00C1DC20/0x140A11C60
const DBCMeta AnimKitSegmentRec::s_meta = { "DBFilesClient\\AnimKitSegment.dbc", 16, 64, AnimKitSegmentRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ee1aba6e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimKitSegmentRec::FixupStrings(void* record, const char* strings) {
}
