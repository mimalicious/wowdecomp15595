#include "WoW/Source/DB/Rec/ZoneLightPointRec.h"

// STATIC: 0x00C278E8/0x140A1DE40
const DBCMeta ZoneLightPointRec::s_meta = { "DBFilesClient\\ZoneLightPoint.dbc", 4, 20, ZoneLightPointRec::FixupStrings, 0 };

// PROVEN: 7168c7c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ZoneLightPointRec::FixupStrings(void* record, const char* strings) {
}
