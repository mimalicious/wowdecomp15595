#include "WoW/Source/DB/Rec/SpellTotemsRec.h"

// STATIC: 0x00C208E0/0x140A151E0
const DBCMeta SpellTotemsRec::s_meta = { "DBFilesClient\\SpellTotems.dbc", 3, 20, SpellTotemsRec::FixupStrings, 0 };

// PROVEN: c4949a0c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellTotemsRec::FixupStrings(void* record, const char* strings) {
}
