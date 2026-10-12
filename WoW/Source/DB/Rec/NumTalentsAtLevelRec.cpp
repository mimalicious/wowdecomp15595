#include "WoW/Source/DB/Rec/NumTalentsAtLevelRec.h"

// STATIC: 0x00C2321C/0x140A183F8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C23224/0x140A18400
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C2322C/0x140A18408
static const int32_t s_fieldTypes[2] = { 0, 3 };

// STATIC: 0x00C23234/0x140A18410
const DBCMeta NumTalentsAtLevelRec::s_meta = { "DBFilesClient\\NumTalentsAtLevel.dbc", 2, 8, NumTalentsAtLevelRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void NumTalentsAtLevelRec::FixupStrings(void* record, const char* strings) {
}
