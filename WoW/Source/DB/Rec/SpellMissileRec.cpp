#include "WoW/Source/DB/Rec/SpellMissileRec.h"

// STATIC: 0x00C20FC0/0x140A15A28
const DBCMeta SpellMissileRec::s_meta = { "DBFilesClient\\SpellMissile.dbc", 15, 60, SpellMissileRec::FixupStrings, 0 };

// PROVEN: fe6a3b17 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellMissileRec::FixupStrings(void* record, const char* strings) {
}
