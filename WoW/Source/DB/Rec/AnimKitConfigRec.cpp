#include "WoW/Source/DB/Rec/AnimKitConfigRec.h"

// STATIC: 0x00C1EB28/0x140A12EA0
const DBCMeta AnimKitConfigRec::s_meta = { "DBFilesClient\\AnimKitConfig.dbc", 2, 8, AnimKitConfigRec::FixupStrings, 0 };

// PROVEN: 6975b215 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimKitConfigRec::FixupStrings(void* record, const char* strings) {
}
