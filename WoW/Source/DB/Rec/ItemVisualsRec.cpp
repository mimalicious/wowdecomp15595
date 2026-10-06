#include "WoW/Source/DB/Rec/ItemVisualsRec.h"

// STATIC: 0x00C1D1A8/0x140A11020
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1D1B0/0x140A11028
static const int32_t s_fieldSizes[2] = { 4, 20 };

// STATIC: 0x00C1D1B8/0x140A11030
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1D1C0/0x140A11038
const DBCMeta ItemVisualsRec::s_meta = { "DBFilesClient\\ItemVisuals.dbc", 2, 24, ItemVisualsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: d392dd33 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemVisualsRec::FixupStrings(void* record, const char* strings) {
}
