#include "WoW/Source/DB/Rec/DeathThudLookupsRec.h"

// STATIC: 0x00C25D34/0x140A1BAB0
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C25D48/0x140A1BAC8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C25D5C/0x140A1BAE0
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C25D70/0x140A1BAF8
const DBCMeta DeathThudLookupsRec::s_meta = { "DBFilesClient\\DeathThudLookups.dbc", 5, 20, DeathThudLookupsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: d0a3cad0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DeathThudLookupsRec::FixupStrings(void* record, const char* strings) {
}
