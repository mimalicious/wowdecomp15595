#include "WoW/Source/DB/Rec/AnimKitRec.h"

// STATIC: 0x00C1EB64/0x140A12EF8
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C1EB70/0x140A12F08
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C1EB7C/0x140A12F18
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C1EB88/0x140A12F28
const DBCMeta AnimKitRec::s_meta = { "DBFilesClient\\AnimKit.dbc", 3, 12, AnimKitRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void AnimKitRec::FixupStrings(void* record, const char* strings) {
}
