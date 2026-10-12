#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"

// STATIC: 0x00C220CC/0x140A16E70
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C220E0/0x140A16E88
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C220F4/0x140A16EA0
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C22108/0x140A16EB8
const DBCMeta SpellAuraOptionsRec::s_meta = { "DBFilesClient\\SpellAuraOptions.dbc", 5, 20, SpellAuraOptionsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellAuraOptionsRec::FixupStrings(void* record, const char* strings) {
}
