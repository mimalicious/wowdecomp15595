#include "WoW/Source/DB/Rec/HelmetGeosetVisDataRec.h"

// STATIC: 0x00C1D2DC/0x140A11188
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1D2E4/0x140A11190
static const int32_t s_fieldSizes[2] = { 4, 28 };

// STATIC: 0x00C1D2EC/0x140A11198
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1D2F4/0x140A111A0
const DBCMeta HelmetGeosetVisDataRec::s_meta = { "DBFilesClient\\HelmetGeosetVisData.dbc", 2, 32, HelmetGeosetVisDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void HelmetGeosetVisDataRec::FixupStrings(void* record, const char* strings) {
}
