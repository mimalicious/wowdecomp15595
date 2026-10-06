#include "WoW/Source/DB/Rec/SpellItemEnchantmentConditionRec.h"

// STATIC: 0x00C210D8/0x140A15B88
static const int32_t s_fieldOffsets[7] = { 0, 4, 12, 32, 37, 44, 64 };

// STATIC: 0x00C210F4/0x140A15BA8
static const int32_t s_fieldSizes[7] = { 4, 5, 20, 5, 5, 20, 5 };

// STATIC: 0x00C21110/0x140A15BC8
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C2112C/0x140A15BE8
const DBCMeta SpellItemEnchantmentConditionRec::s_meta = { "DBFilesClient\\SpellItemEnchantmentCondition.dbc", 7, 72, SpellItemEnchantmentConditionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 43b47d34 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellItemEnchantmentConditionRec::FixupStrings(void* record, const char* strings) {
}
