#include "WoW/Source/DB/Rec/AnimKitConfigRec.h"

// STATIC: 0x00C1EB10/0x140A12E88
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1EB18/0x140A12E90
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1EB20/0x140A12E98
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1EB28/0x140A12EA0
const DBCMeta AnimKitConfigRec::s_meta = { "DBFilesClient\\AnimKitConfig.dbc", 2, 8, AnimKitConfigRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6975b215 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimKitConfigRec::FixupStrings(void* record, const char* strings) {
}
