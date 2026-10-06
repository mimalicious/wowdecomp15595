#include "WoW/Source/DB/Rec/ItemArmorShieldRec.h"

// STATIC: 0x00C246EC/0x140A19E70
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C246F8/0x140A19E80
static const int32_t s_fieldSizes[3] = { 4, 4, 28 };

// STATIC: 0x00C24704/0x140A19E90
static const int32_t s_fieldTypes[3] = { 0, 0, 3 };

// STATIC: 0x00C24710/0x140A19EA0
const DBCMeta ItemArmorShieldRec::s_meta = { "DBFilesClient\\ItemArmorShield.dbc", 3, 36, ItemArmorShieldRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6c0f0481 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemArmorShieldRec::FixupStrings(void* record, const char* strings) {
}
