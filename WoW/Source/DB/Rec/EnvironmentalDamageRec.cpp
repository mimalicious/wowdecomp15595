#include "WoW/Source/DB/Rec/EnvironmentalDamageRec.h"

// STATIC: 0x00C25708/0x140A1B328
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C25714/0x140A1B338
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C25720/0x140A1B348
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C2572C/0x140A1B358
const DBCMeta EnvironmentalDamageRec::s_meta = { "DBFilesClient\\EnvironmentalDamage.dbc", 3, 12, EnvironmentalDamageRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 20f306ec IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void EnvironmentalDamageRec::FixupStrings(void* record, const char* strings) {
}
