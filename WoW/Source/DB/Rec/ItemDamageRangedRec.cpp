#include "WoW/Source/DB/Rec/ItemDamageRangedRec.h"

// STATIC: 0x00C244EC/0x140A19BB8
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C244F8/0x140A19BC8
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C24504/0x140A19BD8
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C24510/0x140A19BE8
const DBCMeta ItemDamageRangedRec::s_meta = { "DBFilesClient\\ItemDamageRanged.dbc", 3, 36, ItemDamageRangedRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageRangedRec::FixupStrings(void* record, const char* strings) {
}
