#include "WoW/Source/DB/Rec/MountTypeRec.h"

// STATIC: 0x00C232DC/0x140A184F0
const DBCMeta MountTypeRec::s_meta = { "DBFilesClient\\MountType.dbc", 2, 100, MountTypeRec::FixupStrings, 0 };

// PROVEN: 64849ff8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MountTypeRec::FixupStrings(void* record, const char* strings) {
}
