#include "WoW/Source/DB/Rec/SpellTargetRestrictionsRec.h"

// STATIC: 0x00C20974/0x140A15290
const DBCMeta SpellTargetRestrictionsRec::s_meta = { "DBFilesClient\\SpellTargetRestrictions.dbc", 6, 24, SpellTargetRestrictionsRec::FixupStrings, 0 };

// PROVEN: cbe75b3d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellTargetRestrictionsRec::FixupStrings(void* record, const char* strings) {
}
