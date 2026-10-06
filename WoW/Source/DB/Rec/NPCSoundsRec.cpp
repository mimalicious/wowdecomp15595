#include "WoW/Source/DB/Rec/NPCSoundsRec.h"

// STATIC: 0x00C23288/0x140A18480
const DBCMeta NPCSoundsRec::s_meta = { "DBFilesClient\\NPCSounds.dbc", 2, 20, NPCSoundsRec::FixupStrings, 0 };

// PROVEN: 954e42c9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void NPCSoundsRec::FixupStrings(void* record, const char* strings) {
}
