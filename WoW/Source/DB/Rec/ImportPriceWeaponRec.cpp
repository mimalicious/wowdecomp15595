#include "WoW/Source/DB/Rec/ImportPriceWeaponRec.h"

// STATIC: 0x00C24848/0x140A1A038
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C24850/0x140A1A040
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C24858/0x140A1A048
static const int32_t s_fieldTypes[2] = { 0, 3 };

// STATIC: 0x00C24860/0x140A1A050
const DBCMeta ImportPriceWeaponRec::s_meta = { "DBFilesClient\\ImportPriceWeapon.dbc", 2, 8, ImportPriceWeaponRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: f5c4c2e0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ImportPriceWeaponRec::FixupStrings(void* record, const char* strings) {
}
