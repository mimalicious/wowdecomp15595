#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"

// STATIC: 0x00C21E1C/0x140A16B18
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C21E2C/0x140A16B28
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C21E3C/0x140A16B38
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C21E4C/0x140A16B48
const DBCMeta SpellCastTimesRec::s_meta = { "DBFilesClient\\SpellCastTimes.dbc", 4, 16, SpellCastTimesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellCastTimesRec::FixupStrings(void* record, const char* strings) {
}
