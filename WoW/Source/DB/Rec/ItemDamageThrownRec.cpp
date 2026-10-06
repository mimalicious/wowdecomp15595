#include "WoW/Source/DB/Rec/ItemDamageThrownRec.h"

// STATIC: 0x00C244A8/0x140A19B58
const DBCMeta ItemDamageThrownRec::s_meta = { "DBFilesClient\\ItemDamageThrown.dbc", 3, 36, ItemDamageThrownRec::FixupStrings, 0 };

// PROVEN: a975068d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageThrownRec::FixupStrings(void* record, const char* strings) {
}
