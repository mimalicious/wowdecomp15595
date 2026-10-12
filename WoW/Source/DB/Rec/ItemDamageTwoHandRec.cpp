#include "WoW/Source/DB/Rec/ItemDamageTwoHandRec.h"

// STATIC: 0x00C2441C/0x140A19A98
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C24428/0x140A19AA8
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C24434/0x140A19AB8
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C24440/0x140A19AC8
const DBCMeta ItemDamageTwoHandRec::s_meta = { "DBFilesClient\\ItemDamageTwoHand.dbc", 3, 36, ItemDamageTwoHandRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageTwoHandRec::FixupStrings(void* record, const char* strings) {
}
