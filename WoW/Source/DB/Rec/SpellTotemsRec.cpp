#include "WoW/Source/DB/Rec/SpellTotemsRec.h"

// STATIC: 0x00C208BC/0x140A151B0
static const int32_t s_fieldOffsets[3] = { 0, 4, 12 };

// STATIC: 0x00C208C8/0x140A151C0
static const int32_t s_fieldSizes[3] = { 4, 8, 8 };

// STATIC: 0x00C208D4/0x140A151D0
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C208E0/0x140A151E0
const DBCMeta SpellTotemsRec::s_meta = { "DBFilesClient\\SpellTotems.dbc", 3, 20, SpellTotemsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c4949a0c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellTotemsRec::FixupStrings(void* record, const char* strings) {
}
