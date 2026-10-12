#include "WoW/Source/DB/Rec/SpellPowerRec.h"

// STATIC: 0x00C20CF0/0x140A156E0
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C20D10/0x140A15700
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C20D30/0x140A15720
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 0, 0, 3 };

// STATIC: 0x00C20D50/0x140A15740
const DBCMeta SpellPowerRec::s_meta = { "DBFilesClient\\SpellPower.dbc", 8, 32, SpellPowerRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellPowerRec::FixupStrings(void* record, const char* strings) {
}
