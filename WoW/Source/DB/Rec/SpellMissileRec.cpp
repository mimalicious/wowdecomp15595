#include "WoW/Source/DB/Rec/SpellMissileRec.h"

// STATIC: 0x00C20F0C/0x140A15968
static const int32_t s_fieldOffsets[15] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56 };

// STATIC: 0x00C20F48/0x140A159A8
static const int32_t s_fieldSizes[15] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C20F84/0x140A159E8
static const int32_t s_fieldTypes[15] = { 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };

// STATIC: 0x00C20FC0/0x140A15A28
const DBCMeta SpellMissileRec::s_meta = { "DBFilesClient\\SpellMissile.dbc", 15, 60, SpellMissileRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellMissileRec::FixupStrings(void* record, const char* strings) {
}
