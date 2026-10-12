#include "WoW/Source/DB/Rec/DurabilityCostsRec.h"

// STATIC: 0x00C259B4/0x140A1B690
static const int32_t s_fieldOffsets[3] = { 0, 4, 88 };

// STATIC: 0x00C259C0/0x140A1B6A0
static const int32_t s_fieldSizes[3] = { 4, 84, 32 };

// STATIC: 0x00C259CC/0x140A1B6B0
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C259D8/0x140A1B6C0
const DBCMeta DurabilityCostsRec::s_meta = { "DBFilesClient\\DurabilityCosts.dbc", 3, 120, DurabilityCostsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void DurabilityCostsRec::FixupStrings(void* record, const char* strings) {
}
