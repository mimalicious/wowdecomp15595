#include "WoW/Source/DB/Rec/SpellAuraVisibilityRec.h"

// STATIC: 0x00C21FD4/0x140A16D38
const DBCMeta SpellAuraVisibilityRec::s_meta = { "DBFilesClient\\SpellAuraVisibility.dbc", 4, 16, SpellAuraVisibilityRec::FixupStrings, 0 };

// PROVEN: e6c52ac2 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellAuraVisibilityRec::FixupStrings(void* record, const char* strings) {
}
