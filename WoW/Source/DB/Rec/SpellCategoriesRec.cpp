#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"

// STATIC: 0x00C21D84/0x140A16A58
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C21DA0/0x140A16A78
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C21DBC/0x140A16A98
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C21DD8/0x140A16AB8
const DBCMeta SpellCategoriesRec::s_meta = { "DBFilesClient\\SpellCategories.dbc", 7, 28, SpellCategoriesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellCategoriesRec::FixupStrings(void* record, const char* strings) {
}
