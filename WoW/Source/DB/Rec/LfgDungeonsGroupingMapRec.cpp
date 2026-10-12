#include "WoW/Source/DB/Rec/LfgDungeonsGroupingMapRec.h"

// STATIC: 0x00C237F8/0x140A18B50
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C23808/0x140A18B60
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C23818/0x140A18B70
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C23828/0x140A18B80
const DBCMeta LfgDungeonsGroupingMapRec::s_meta = { "DBFilesClient\\LfgDungeonsGroupingMap.dbc", 4, 16, LfgDungeonsGroupingMapRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void LfgDungeonsGroupingMapRec::FixupStrings(void* record, const char* strings) {
}
