#include "WoW/Source/DB/Rec/ItemDamageWandRec.h"

// STATIC: 0x00C24344/0x140A19970
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C24350/0x140A19980
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C2435C/0x140A19990
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C24368/0x140A199A0
const DBCMeta ItemDamageWandRec::s_meta = { "DBFilesClient\\ItemDamageWand.dbc", 3, 36, ItemDamageWandRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: e8de6c45 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageWandRec::FixupStrings(void* record, const char* strings) {
}
