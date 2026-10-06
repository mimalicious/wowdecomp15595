#include "WoW/Source/DB/Rec/ItemGroupSoundsRec.h"

// STATIC: 0x00C2424C/0x140A19838
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C24254/0x140A19840
static const int32_t s_fieldSizes[2] = { 4, 16 };

// STATIC: 0x00C2425C/0x140A19848
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C24264/0x140A19850
const DBCMeta ItemGroupSoundsRec::s_meta = { "DBFilesClient\\ItemGroupSounds.dbc", 2, 20, ItemGroupSoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 761e0107 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemGroupSoundsRec::FixupStrings(void* record, const char* strings) {
}
