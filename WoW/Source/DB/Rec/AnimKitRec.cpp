#include "WoW/Source/DB/Rec/AnimKitRec.h"

// STATIC: 0x00C1EB88/0x140A12F28
const DBCMeta AnimKitRec::s_meta = { "DBFilesClient\\AnimKit.dbc", 3, 12, AnimKitRec::FixupStrings, 0 };

// PROVEN: b770d67c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimKitRec::FixupStrings(void* record, const char* strings) {
}
