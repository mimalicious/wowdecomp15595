#include "WoW/Source/DB/Rec/RandPropPointsRec.h"

// STATIC: 0x00C22978/0x140A17900
static const int32_t s_fieldOffsets[4] = { 0, 4, 24, 44 };

// STATIC: 0x00C22988/0x140A17910
static const int32_t s_fieldSizes[4] = { 4, 20, 20, 20 };

// STATIC: 0x00C22998/0x140A17920
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C229A8/0x140A17930
const DBCMeta RandPropPointsRec::s_meta = { "DBFilesClient\\RandPropPoints.dbc", 4, 64, RandPropPointsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void RandPropPointsRec::FixupStrings(void* record, const char* strings) {
}
