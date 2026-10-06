#include "WoW/Source/DB/Rec/CastableRaidBuffsRec.h"

// STATIC: 0x00C26588/0x140A1C4D8
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C26594/0x140A1C4E8
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C265A0/0x140A1C4F8
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C265AC/0x140A1C508
const DBCMeta CastableRaidBuffsRec::s_meta = { "DBFilesClient\\CastableRaidBuffs.dbc", 3, 12, CastableRaidBuffsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7cf93d9b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CastableRaidBuffsRec::FixupStrings(void* record, const char* strings) {
}
