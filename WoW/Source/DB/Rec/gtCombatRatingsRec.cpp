#include "WoW/Source/DB/Rec/gtCombatRatingsRec.h"

// STATIC: 0x00C24FD0/0x140A1AA10
const DBCMeta gtCombatRatingsRec::s_meta = { "DBFilesClient\\gtCombatRatings.dbc", 1, 8, gtCombatRatingsRec::FixupStrings, 0 };

// PROVEN: 13741388 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtCombatRatingsRec::FixupStrings(void* record, const char* strings) {
}
