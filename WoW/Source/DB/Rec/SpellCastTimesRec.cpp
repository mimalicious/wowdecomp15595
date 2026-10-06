#include "WoW/Source/DB/Rec/SpellCastTimesRec.h"

// STATIC: 0x00C21E4C/0x140A16B48
const DBCMeta SpellCastTimesRec::s_meta = { "DBFilesClient\\SpellCastTimes.dbc", 4, 16, SpellCastTimesRec::FixupStrings, 0 };

// PROVEN: e918ec1d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellCastTimesRec::FixupStrings(void* record, const char* strings) {
}
