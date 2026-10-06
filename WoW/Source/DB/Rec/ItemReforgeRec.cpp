#include "WoW/Source/DB/Rec/ItemReforgeRec.h"

// STATIC: 0x00C23F00/0x140A193F8
const DBCMeta ItemReforgeRec::s_meta = { "DBFilesClient\\ItemReforge.dbc", 5, 20, ItemReforgeRec::FixupStrings, 0 };

// PROVEN: 225f83aa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemReforgeRec::FixupStrings(void* record, const char* strings) {
}
