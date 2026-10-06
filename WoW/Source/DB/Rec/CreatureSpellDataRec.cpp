#include "WoW/Source/DB/Rec/CreatureSpellDataRec.h"

// STATIC: 0x00C25FFC/0x140A1BE28
const DBCMeta CreatureSpellDataRec::s_meta = { "DBFilesClient\\CreatureSpellData.dbc", 3, 36, CreatureSpellDataRec::FixupStrings, 0 };

// PROVEN: 86e4a389 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CreatureSpellDataRec::FixupStrings(void* record, const char* strings) {
}
