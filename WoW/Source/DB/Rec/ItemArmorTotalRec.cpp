#include "WoW/Source/DB/Rec/ItemArmorTotalRec.h"

// STATIC: 0x00C24754/0x140A19F00
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C2476C/0x140A19F18
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C24784/0x140A19F30
static const int32_t s_fieldTypes[6] = { 0, 0, 3, 3, 3, 3 };

// STATIC: 0x00C2479C/0x140A19F48
const DBCMeta ItemArmorTotalRec::s_meta = { "DBFilesClient\\ItemArmorTotal.dbc", 6, 24, ItemArmorTotalRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: b0f7843b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemArmorTotalRec::FixupStrings(void* record, const char* strings) {
}
