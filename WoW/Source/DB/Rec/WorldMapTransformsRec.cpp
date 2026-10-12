#include "WoW/Source/DB/Rec/WorldMapTransformsRec.h"

// STATIC: 0x00C1F170/0x140A13648
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 16, 24, 28, 36, 40, 44 };

// STATIC: 0x00C1F194/0x140A13670
static const int32_t s_fieldSizes[9] = { 4, 4, 8, 8, 4, 8, 4, 4, 4 };

// STATIC: 0x00C1F1B8/0x140A13698
static const int32_t s_fieldTypes[9] = { 0, 0, 3, 3, 0, 3, 0, 0, 0 };

// STATIC: 0x00C1F1DC/0x140A136C0
const DBCMeta WorldMapTransformsRec::s_meta = { "DBFilesClient\\WorldMapTransforms.dbc", 9, 48, WorldMapTransformsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void WorldMapTransformsRec::FixupStrings(void* record, const char* strings) {
}
