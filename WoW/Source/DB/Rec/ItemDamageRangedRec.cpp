#include "WoW/Source/DB/Rec/ItemDamageRangedRec.h"

// STATIC: 0x00C24510/0x140A19BE8
const DBCMeta ItemDamageRangedRec::s_meta = { "DBFilesClient\\ItemDamageRanged.dbc", 3, 36, ItemDamageRangedRec::FixupStrings, 0 };

// PROVEN: 8bf0d127 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageRangedRec::FixupStrings(void* record, const char* strings) {
}
