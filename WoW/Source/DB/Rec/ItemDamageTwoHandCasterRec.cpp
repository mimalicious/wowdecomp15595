#include "WoW/Source/DB/Rec/ItemDamageTwoHandCasterRec.h"

// STATIC: 0x00C243B4/0x140A19A08
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C243C0/0x140A19A18
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C243CC/0x140A19A28
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C243D8/0x140A19A38
const DBCMeta ItemDamageTwoHandCasterRec::s_meta = { "DBFilesClient\\ItemDamageTwoHandCaster.dbc", 3, 36, ItemDamageTwoHandCasterRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageTwoHandCasterRec::FixupStrings(void* record, const char* strings) {
}
