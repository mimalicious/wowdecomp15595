#include "WoW/Source/DB/Rec/TaxiPathRec.h"

// STATIC: 0x00C200B8/0x140A14830
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C200C8/0x140A14840
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C200D8/0x140A14850
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C200E8/0x140A14860
const DBCMeta TaxiPathRec::s_meta = { "DBFilesClient\\TaxiPath.dbc", 4, 16, TaxiPathRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void TaxiPathRec::FixupStrings(void* record, const char* strings) {
}
