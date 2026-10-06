#include "WoW/Source/DB/Rec/SpellDurationRec.h"

// STATIC: 0x00C215A0/0x140A16160
const DBCMeta SpellDurationRec::s_meta = { "DBFilesClient\\SpellDuration.dbc", 4, 16, SpellDurationRec::FixupStrings, 0 };

// PROVEN: 0cfcc569 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellDurationRec::FixupStrings(void* record, const char* strings) {
}
