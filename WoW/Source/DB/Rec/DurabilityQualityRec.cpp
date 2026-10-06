#include "WoW/Source/DB/Rec/DurabilityQualityRec.h"

// STATIC: 0x00C25958/0x140A1B618
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C25960/0x140A1B620
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C25968/0x140A1B628
static const int32_t s_fieldTypes[2] = { 0, 3 };

// STATIC: 0x00C25970/0x140A1B630
const DBCMeta DurabilityQualityRec::s_meta = { "DBFilesClient\\DurabilityQuality.dbc", 2, 8, DurabilityQualityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 53e9c4e7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DurabilityQualityRec::FixupStrings(void* record, const char* strings) {
}
