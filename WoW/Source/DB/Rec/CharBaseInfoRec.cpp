#include "WoW/Source/DB/Rec/CharBaseInfoRec.h"

// STATIC: 0x00C1E8C8/0x140A12BB8
static const int32_t s_fieldOffsets[2] = { 4, 5 };

// STATIC: 0x00C1E8D0/0x140A12BC0
static const int32_t s_fieldSizes[2] = { 1, 1 };

// STATIC: 0x00C1E8D8/0x140A12BC8
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1E8E0/0x140A12BD0
const DBCMeta CharBaseInfoRec::s_meta = { "DBFilesClient\\CharBaseInfo.dbc", 2, 8, CharBaseInfoRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void CharBaseInfoRec::FixupStrings(void* record, const char* strings) {
}
