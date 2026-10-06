#include "WoW/Source/DB/Rec/DurabilityCostsRec.h"

// STATIC: 0x00C259D8/0x140A1B6C0
const DBCMeta DurabilityCostsRec::s_meta = { "DBFilesClient\\DurabilityCosts.dbc", 3, 120, DurabilityCostsRec::FixupStrings, 0 };

// PROVEN: 0500123f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DurabilityCostsRec::FixupStrings(void* record, const char* strings) {
}
