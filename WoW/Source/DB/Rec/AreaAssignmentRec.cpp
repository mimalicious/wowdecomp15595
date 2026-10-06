#include "WoW/Source/DB/Rec/AreaAssignmentRec.h"

// STATIC: 0x00C26B50/0x140A1CBD0
const DBCMeta AreaAssignmentRec::s_meta = { "DBFilesClient\\AreaAssignment.dbc", 4, 20, AreaAssignmentRec::FixupStrings, 0 };

// PROVEN: 15a4a205 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AreaAssignmentRec::FixupStrings(void* record, const char* strings) {
}
