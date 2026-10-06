#include "WoW/Source/DB/Rec/ImportPriceQualityRec.h"

// STATIC: 0x00C24904/0x140A1A128
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C2490C/0x140A1A130
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C24914/0x140A1A138
static const int32_t s_fieldTypes[2] = { 0, 3 };

// STATIC: 0x00C2491C/0x140A1A140
const DBCMeta ImportPriceQualityRec::s_meta = { "DBFilesClient\\ImportPriceQuality.dbc", 2, 8, ImportPriceQualityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 2be11c5d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ImportPriceQualityRec::FixupStrings(void* record, const char* strings) {
}
