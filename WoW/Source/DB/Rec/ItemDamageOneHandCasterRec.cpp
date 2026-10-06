#include "WoW/Source/DB/Rec/ItemDamageOneHandCasterRec.h"

// STATIC: 0x00C2455C/0x140A19C50
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C24568/0x140A19C60
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C24574/0x140A19C70
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C24580/0x140A19C80
const DBCMeta ItemDamageOneHandCasterRec::s_meta = { "DBFilesClient\\ItemDamageOneHandCaster.dbc", 3, 36, ItemDamageOneHandCasterRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 3756692b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageOneHandCasterRec::FixupStrings(void* record, const char* strings) {
}
