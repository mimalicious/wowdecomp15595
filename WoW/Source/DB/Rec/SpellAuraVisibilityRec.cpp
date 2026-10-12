#include "WoW/Source/DB/Rec/SpellAuraVisibilityRec.h"

// STATIC: 0x00C21FA4/0x140A16D08
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C21FB4/0x140A16D18
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C21FC4/0x140A16D28
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C21FD4/0x140A16D38
const DBCMeta SpellAuraVisibilityRec::s_meta = { "DBFilesClient\\SpellAuraVisibility.dbc", 4, 16, SpellAuraVisibilityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellAuraVisibilityRec::FixupStrings(void* record, const char* strings) {
}
