#include "WoW/Source/DB/Rec/ItemDisenchantLootRec.h"

// STATIC: 0x00C24300/0x140A19910
const DBCMeta ItemDisenchantLootRec::s_meta = { "DBFilesClient\\ItemDisenchantLoot.dbc", 7, 28, ItemDisenchantLootRec::FixupStrings, 0 };

// PROVEN: 4eba0f6a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDisenchantLootRec::FixupStrings(void* record, const char* strings) {
}
