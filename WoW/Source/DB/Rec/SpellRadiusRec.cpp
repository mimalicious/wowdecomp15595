#include "WoW/Source/DB/Rec/SpellRadiusRec.h"

// STATIC: 0x00C20E48/0x140A15868
const DBCMeta SpellRadiusRec::s_meta = { "DBFilesClient\\SpellRadius.dbc", 4, 16, SpellRadiusRec::FixupStrings, 0 };

// PROVEN: ff33f4ef IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellRadiusRec::FixupStrings(void* record, const char* strings) {
}
