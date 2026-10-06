#include "WoW/Source/DB/Rec/SpellShapeshiftRec.h"

// STATIC: 0x00C20AE8/0x140A15460
static const int32_t s_fieldOffsets[4] = { 0, 4, 12, 20 };

// STATIC: 0x00C20AF8/0x140A15470
static const int32_t s_fieldSizes[4] = { 4, 8, 8, 4 };

// STATIC: 0x00C20B08/0x140A15480
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C20B18/0x140A15490
const DBCMeta SpellShapeshiftRec::s_meta = { "DBFilesClient\\SpellShapeshift.dbc", 4, 24, SpellShapeshiftRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7a07c7b6 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellShapeshiftRec::FixupStrings(void* record, const char* strings) {
}
