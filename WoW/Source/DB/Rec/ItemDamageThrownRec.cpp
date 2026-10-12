#include "WoW/Source/DB/Rec/ItemDamageThrownRec.h"

// STATIC: 0x00C24484/0x140A19B28
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C24490/0x140A19B38
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C2449C/0x140A19B48
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C244A8/0x140A19B58
const DBCMeta ItemDamageThrownRec::s_meta = { "DBFilesClient\\ItemDamageThrown.dbc", 3, 36, ItemDamageThrownRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageThrownRec::FixupStrings(void* record, const char* strings) {
}
