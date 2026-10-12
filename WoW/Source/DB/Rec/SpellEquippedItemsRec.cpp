#include "WoW/Source/DB/Rec/SpellEquippedItemsRec.h"

// STATIC: 0x00C2149C/0x140A16028
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C214AC/0x140A16038
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C214BC/0x140A16048
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C214CC/0x140A16058
const DBCMeta SpellEquippedItemsRec::s_meta = { "DBFilesClient\\SpellEquippedItems.dbc", 4, 16, SpellEquippedItemsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellEquippedItemsRec::FixupStrings(void* record, const char* strings) {
}
