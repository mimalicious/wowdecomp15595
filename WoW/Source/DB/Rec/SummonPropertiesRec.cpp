#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"

// STATIC: 0x00C2044C/0x140A14C80
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C20464/0x140A14C98
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C2047C/0x140A14CB0
static const int32_t s_fieldTypes[6] = { 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C20494/0x140A14CC8
const DBCMeta SummonPropertiesRec::s_meta = { "DBFilesClient\\SummonProperties.dbc", 6, 24, SummonPropertiesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SummonPropertiesRec::FixupStrings(void* record, const char* strings) {
}
