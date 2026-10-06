#include "WoW/Source/DB/Rec/ItemDamageAmmoRec.h"

// STATIC: 0x00C24650/0x140A19DA0
const DBCMeta ItemDamageAmmoRec::s_meta = { "DBFilesClient\\ItemDamageAmmo.dbc", 3, 36, ItemDamageAmmoRec::FixupStrings, 0 };

// PROVEN: 52032360 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageAmmoRec::FixupStrings(void* record, const char* strings) {
}
