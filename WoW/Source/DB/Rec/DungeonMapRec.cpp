#include "WoW/Source/DB/Rec/DungeonMapRec.h"

// STATIC: 0x00C25AE0/0x140A1B808
const DBCMeta DungeonMapRec::s_meta = { "DBFilesClient\\DungeonMap.dbc", 6, 32, DungeonMapRec::FixupStrings, 0 };

// PROVEN: 865a23a8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DungeonMapRec::FixupStrings(void* record, const char* strings) {
}
