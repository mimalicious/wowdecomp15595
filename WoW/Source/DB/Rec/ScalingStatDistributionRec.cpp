#include "WoW/Source/DB/Rec/ScalingStatDistributionRec.h"

// STATIC: 0x00C228F8/0x140A17858
static const int32_t s_fieldOffsets[5] = { 0, 4, 44, 84, 88 };

// STATIC: 0x00C2290C/0x140A17870
static const int32_t s_fieldSizes[5] = { 4, 40, 40, 4, 4 };

// STATIC: 0x00C22920/0x140A17888
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C22934/0x140A178A0
const DBCMeta ScalingStatDistributionRec::s_meta = { "DBFilesClient\\ScalingStatDistribution.dbc", 5, 92, ScalingStatDistributionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ScalingStatDistributionRec::FixupStrings(void* record, const char* strings) {
}
