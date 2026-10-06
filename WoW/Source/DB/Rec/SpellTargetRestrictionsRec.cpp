#include "WoW/Source/DB/Rec/SpellTargetRestrictionsRec.h"

// STATIC: 0x00C2092C/0x140A15248
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C20944/0x140A15260
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C2095C/0x140A15278
static const int32_t s_fieldTypes[6] = { 0, 3, 0, 0, 0, 0 };

// STATIC: 0x00C20974/0x140A15290
const DBCMeta SpellTargetRestrictionsRec::s_meta = { "DBFilesClient\\SpellTargetRestrictions.dbc", 6, 24, SpellTargetRestrictionsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: cbe75b3d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellTargetRestrictionsRec::FixupStrings(void* record, const char* strings) {
}
