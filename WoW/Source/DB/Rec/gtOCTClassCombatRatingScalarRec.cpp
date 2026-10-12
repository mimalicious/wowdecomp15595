#include "WoW/Source/DB/Rec/gtOCTClassCombatRatingScalarRec.h"

// STATIC: 0x00C24D14/0x140A1A668
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C24D1C/0x140A1A670
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C24D24/0x140A1A678
static const int32_t s_fieldTypes[2] = { 0, 3 };

// STATIC: 0x00C24D2C/0x140A1A680
const DBCMeta gtOCTClassCombatRatingScalarRec::s_meta = { "DBFilesClient\\gtOCTClassCombatRatingScalar.dbc", 2, 8, gtOCTClassCombatRatingScalarRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void gtOCTClassCombatRatingScalarRec::FixupStrings(void* record, const char* strings) {
}
