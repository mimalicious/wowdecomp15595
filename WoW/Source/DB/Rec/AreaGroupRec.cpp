#include "WoW/Source/DB/Rec/AreaGroupRec.h"

// STATIC: 0x00C26CF8/0x140A1CDF0
const DBCMeta AreaGroupRec::s_meta = { "DBFilesClient\\AreaGroup.dbc", 3, 32, AreaGroupRec::FixupStrings, 0 };

// PROVEN: c030281e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AreaGroupRec::FixupStrings(void* record, const char* strings) {
}
