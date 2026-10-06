#include "WoW/Source/DB/Rec/ItemDisenchantLootRec.h"

// STATIC: 0x00C242AC/0x140A198B0
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C242C8/0x140A198D0
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C242E4/0x140A198F0
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C24300/0x140A19910
const DBCMeta ItemDisenchantLootRec::s_meta = { "DBFilesClient\\ItemDisenchantLoot.dbc", 7, 28, ItemDisenchantLootRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 4eba0f6a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDisenchantLootRec::FixupStrings(void* record, const char* strings) {
}
