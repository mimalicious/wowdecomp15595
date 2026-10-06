#include "WoW/Source/DB/Rec/ImportPriceShieldRec.h"

// STATIC: 0x00C248A4/0x140A1A0B0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C248AC/0x140A1A0B8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C248B4/0x140A1A0C0
static const int32_t s_fieldTypes[2] = { 0, 3 };

// STATIC: 0x00C248BC/0x140A1A0C8
const DBCMeta ImportPriceShieldRec::s_meta = { "DBFilesClient\\ImportPriceShield.dbc", 2, 8, ImportPriceShieldRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 49f56832 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ImportPriceShieldRec::FixupStrings(void* record, const char* strings) {
}
