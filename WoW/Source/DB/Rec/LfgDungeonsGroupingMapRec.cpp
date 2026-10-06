#include "WoW/Source/DB/Rec/LfgDungeonsGroupingMapRec.h"

// STATIC: 0x00C23828/0x140A18B80
const DBCMeta LfgDungeonsGroupingMapRec::s_meta = { "DBFilesClient\\LfgDungeonsGroupingMap.dbc", 4, 16, LfgDungeonsGroupingMapRec::FixupStrings, 0 };

// PROVEN: 42e1ee6d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LfgDungeonsGroupingMapRec::FixupStrings(void* record, const char* strings) {
}
