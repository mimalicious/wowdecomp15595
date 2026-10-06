#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"

// STATIC: 0x00C20C4C/0x140A15600
const DBCMeta SpellRuneCostRec::s_meta = { "DBFilesClient\\SpellRuneCost.dbc", 5, 20, SpellRuneCostRec::FixupStrings, 0 };

// PROVEN: 55483cd9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellRuneCostRec::FixupStrings(void* record, const char* strings) {
}
