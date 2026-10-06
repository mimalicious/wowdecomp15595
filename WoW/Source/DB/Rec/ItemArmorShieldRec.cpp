#include "WoW/Source/DB/Rec/ItemArmorShieldRec.h"

// STATIC: 0x00C24710/0x140A19EA0
const DBCMeta ItemArmorShieldRec::s_meta = { "DBFilesClient\\ItemArmorShield.dbc", 3, 36, ItemArmorShieldRec::FixupStrings, 0 };

// PROVEN: 6c0f0481 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemArmorShieldRec::FixupStrings(void* record, const char* strings) {
}
