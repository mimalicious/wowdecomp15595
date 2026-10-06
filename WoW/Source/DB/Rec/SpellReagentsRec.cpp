#include "WoW/Source/DB/Rec/SpellReagentsRec.h"

// STATIC: 0x00C20CB0/0x140A15688
const DBCMeta SpellReagentsRec::s_meta = { "DBFilesClient\\SpellReagents.dbc", 3, 68, SpellReagentsRec::FixupStrings, 0 };

// PROVEN: 539b9cc5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellReagentsRec::FixupStrings(void* record, const char* strings) {
}
