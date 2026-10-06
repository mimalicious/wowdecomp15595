#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"

// STATIC: 0x00C22108/0x140A16EB8
const DBCMeta SpellAuraOptionsRec::s_meta = { "DBFilesClient\\SpellAuraOptions.dbc", 5, 20, SpellAuraOptionsRec::FixupStrings, 0 };

// PROVEN: 3f176059 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellAuraOptionsRec::FixupStrings(void* record, const char* strings) {
}
