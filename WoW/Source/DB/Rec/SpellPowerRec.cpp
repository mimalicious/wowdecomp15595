#include "WoW/Source/DB/Rec/SpellPowerRec.h"

// STATIC: 0x00C20D50/0x140A15740
const DBCMeta SpellPowerRec::s_meta = { "DBFilesClient\\SpellPower.dbc", 8, 32, SpellPowerRec::FixupStrings, 0 };

// PROVEN: 444a9020 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellPowerRec::FixupStrings(void* record, const char* strings) {
}
