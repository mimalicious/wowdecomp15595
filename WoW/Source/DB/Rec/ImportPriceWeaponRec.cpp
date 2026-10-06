#include "WoW/Source/DB/Rec/ImportPriceWeaponRec.h"

// STATIC: 0x00C24860/0x140A1A050
const DBCMeta ImportPriceWeaponRec::s_meta = { "DBFilesClient\\ImportPriceWeapon.dbc", 2, 8, ImportPriceWeaponRec::FixupStrings, 0 };

// PROVEN: f5c4c2e0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ImportPriceWeaponRec::FixupStrings(void* record, const char* strings) {
}
