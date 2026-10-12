#include "WoW/Source/DB/Rec/ItemDamageAmmoRec.h"

// STATIC: 0x00C2462C/0x140A19D70
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C24638/0x140A19D80
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C24644/0x140A19D90
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C24650/0x140A19DA0
const DBCMeta ItemDamageAmmoRec::s_meta = { "DBFilesClient\\ItemDamageAmmo.dbc", 3, 36, ItemDamageAmmoRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageAmmoRec::FixupStrings(void* record, const char* strings) {
}
