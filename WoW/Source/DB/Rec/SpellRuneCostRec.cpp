#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"

// STATIC: 0x00C20C10/0x140A155B8
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C20C24/0x140A155D0
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C20C38/0x140A155E8
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C20C4C/0x140A15600
const DBCMeta SpellRuneCostRec::s_meta = { "DBFilesClient\\SpellRuneCost.dbc", 5, 20, SpellRuneCostRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 55483cd9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellRuneCostRec::FixupStrings(void* record, const char* strings) {
}
