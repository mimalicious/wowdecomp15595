#include "WoW/Source/DB/Rec/CharBaseInfoRec.h"

// STATIC: 0x00C1E8E0/0x140A12BD0
const DBCMeta CharBaseInfoRec::s_meta = { "DBFilesClient\\CharBaseInfo.dbc", 2, 8, CharBaseInfoRec::FixupStrings, 0 };

// PROVEN: 55d13938 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CharBaseInfoRec::FixupStrings(void* record, const char* strings) {
}
