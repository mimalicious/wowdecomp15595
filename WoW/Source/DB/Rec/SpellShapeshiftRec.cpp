#include "WoW/Source/DB/Rec/SpellShapeshiftRec.h"

// STATIC: 0x00C20B18/0x140A15490
const DBCMeta SpellShapeshiftRec::s_meta = { "DBFilesClient\\SpellShapeshift.dbc", 4, 24, SpellShapeshiftRec::FixupStrings, 0 };

// PROVEN: 7a07c7b6 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellShapeshiftRec::FixupStrings(void* record, const char* strings) {
}
