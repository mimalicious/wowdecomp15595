#include "WoW/Source/DB/Rec/LfgDungeonExpansionRec.h"

// STATIC: 0x00C238F0/0x140A18C88
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C23910/0x140A18CA8
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23930/0x140A18CC8
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C23950/0x140A18CE8
const DBCMeta LfgDungeonExpansionRec::s_meta = { "DBFilesClient\\LfgDungeonExpansion.dbc", 8, 32, LfgDungeonExpansionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 0bbc8ea2 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LfgDungeonExpansionRec::FixupStrings(void* record, const char* strings) {
}
