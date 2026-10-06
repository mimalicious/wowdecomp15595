#include "WoW/Source/DB/Rec/ScalingStatValuesRec.h"

// STATIC: 0x00C22790/0x140A176D0
static const int32_t s_fieldOffsets[23] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 72, 88, 104, 120, 136, 152, 168, 184 };

// STATIC: 0x00C227F0/0x140A17730
static const int32_t s_fieldSizes[23] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 16, 16, 16, 16, 16, 16, 16, 16, 4 };

// STATIC: 0x00C22850/0x140A17790
static const int32_t s_fieldTypes[23] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C228AC/0x140A177F0
const DBCMeta ScalingStatValuesRec::s_meta = { "DBFilesClient\\ScalingStatValues.dbc", 23, 188, ScalingStatValuesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 18d315ec IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ScalingStatValuesRec::FixupStrings(void* record, const char* strings) {
}
