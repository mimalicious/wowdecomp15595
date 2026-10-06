#include "WoW/Source/DB/Rec/SpellLevelsRec.h"

// STATIC: 0x00C21088/0x140A15B20
const DBCMeta SpellLevelsRec::s_meta = { "DBFilesClient\\SpellLevels.dbc", 4, 16, SpellLevelsRec::FixupStrings, 0 };

// PROVEN: 6426bab5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellLevelsRec::FixupStrings(void* record, const char* strings) {
}
