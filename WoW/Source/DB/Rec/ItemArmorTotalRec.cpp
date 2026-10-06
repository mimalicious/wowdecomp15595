#include "WoW/Source/DB/Rec/ItemArmorTotalRec.h"

// STATIC: 0x00C2479C/0x140A19F48
const DBCMeta ItemArmorTotalRec::s_meta = { "DBFilesClient\\ItemArmorTotal.dbc", 6, 24, ItemArmorTotalRec::FixupStrings, 0 };

// PROVEN: b0f7843b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemArmorTotalRec::FixupStrings(void* record, const char* strings) {
}
