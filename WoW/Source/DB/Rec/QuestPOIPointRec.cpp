#include "WoW/Source/DB/Rec/QuestPOIPointRec.h"

// STATIC: 0x00C22CAC/0x140A17D10
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C22CBC/0x140A17D20
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C22CCC/0x140A17D30
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C22CDC/0x140A17D40
const DBCMeta QuestPOIPointRec::s_meta = { "DBFilesClient\\QuestPOIPoint.dbc", 4, 16, QuestPOIPointRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void QuestPOIPointRec::FixupStrings(void* record, const char* strings) {
}
