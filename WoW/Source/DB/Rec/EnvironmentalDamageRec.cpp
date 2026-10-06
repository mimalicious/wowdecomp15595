#include "WoW/Source/DB/Rec/EnvironmentalDamageRec.h"

// STATIC: 0x00C2572C/0x140A1B358
const DBCMeta EnvironmentalDamageRec::s_meta = { "DBFilesClient\\EnvironmentalDamage.dbc", 3, 12, EnvironmentalDamageRec::FixupStrings, 0 };

// PROVEN: 20f306ec IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void EnvironmentalDamageRec::FixupStrings(void* record, const char* strings) {
}
