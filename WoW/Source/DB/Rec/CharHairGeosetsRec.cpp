#include "WoW/Source/DB/Rec/CharHairGeosetsRec.h"

// STATIC: 0x00C1E840/0x140A12B18
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C1E858/0x140A12B30
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1E870/0x140A12B48
static const int32_t s_fieldTypes[6] = { 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1E888/0x140A12B60
const DBCMeta CharHairGeosetsRec::s_meta = { "DBFilesClient\\CharHairGeosets.dbc", 6, 24, CharHairGeosetsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void CharHairGeosetsRec::FixupStrings(void* record, const char* strings) {
}
