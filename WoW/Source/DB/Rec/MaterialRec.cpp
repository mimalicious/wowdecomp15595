#include "WoW/Source/DB/Rec/MaterialRec.h"

// STATIC: 0x00C233F8/0x140A18650
const DBCMeta MaterialRec::s_meta = { "DBFilesClient\\Material.dbc", 5, 20, MaterialRec::FixupStrings, 0 };

// PROVEN: 7c2127f5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MaterialRec::FixupStrings(void* record, const char* strings) {
}
