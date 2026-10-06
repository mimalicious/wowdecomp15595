#include "WoW/Source/DB/Rec/LockRec.h"

// STATIC: 0x00C235DC/0x140A188C8
const DBCMeta LockRec::s_meta = { "DBFilesClient\\Lock.dbc", 5, 132, LockRec::FixupStrings, 0 };

// PROVEN: 52812e8c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LockRec::FixupStrings(void* record, const char* strings) {
}
