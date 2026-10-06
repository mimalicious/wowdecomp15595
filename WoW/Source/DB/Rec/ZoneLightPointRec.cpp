#include "WoW/Source/DB/Rec/ZoneLightPointRec.h"

// STATIC: 0x00C278B8/0x140A1DE10
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 16 };

// STATIC: 0x00C278C8/0x140A1DE20
static const int32_t s_fieldSizes[4] = { 4, 4, 8, 4 };

// STATIC: 0x00C278D8/0x140A1DE30
static const int32_t s_fieldTypes[4] = { 0, 0, 3, 0 };

// STATIC: 0x00C278E8/0x140A1DE40
const DBCMeta ZoneLightPointRec::s_meta = { "DBFilesClient\\ZoneLightPoint.dbc", 4, 20, ZoneLightPointRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7168c7c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ZoneLightPointRec::FixupStrings(void* record, const char* strings) {
}
