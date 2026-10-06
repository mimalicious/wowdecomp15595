#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"

// STATIC: 0x00C219C8/0x140A16620
const DBCMeta SpellCooldownsRec::s_meta = { "DBFilesClient\\SpellCooldowns.dbc", 4, 16, SpellCooldownsRec::FixupStrings, 0 };

// PROVEN: 1e47ea09 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellCooldownsRec::FixupStrings(void* record, const char* strings) {
}
