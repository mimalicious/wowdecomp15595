#include "WoW/Source/DB/Rec/SpellEquippedItemsRec.h"

// STATIC: 0x00C214CC/0x140A16058
const DBCMeta SpellEquippedItemsRec::s_meta = { "DBFilesClient\\SpellEquippedItems.dbc", 4, 16, SpellEquippedItemsRec::FixupStrings, 0 };

// PROVEN: bc0f144a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellEquippedItemsRec::FixupStrings(void* record, const char* strings) {
}
