#include "WoW/Source/DB/Rec/NumTalentsAtLevelRec.h"

// STATIC: 0x00C23234/0x140A18410
const DBCMeta NumTalentsAtLevelRec::s_meta = { "DBFilesClient\\NumTalentsAtLevel.dbc", 2, 8, NumTalentsAtLevelRec::FixupStrings, 0 };

// PROVEN: cd3bd790 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void NumTalentsAtLevelRec::FixupStrings(void* record, const char* strings) {
}
