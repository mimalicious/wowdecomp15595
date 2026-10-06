#include "WoW/Source/DB/Rec/SpellAuraRestrictionsRec.h"

// STATIC: 0x00C22088/0x140A16E10
const DBCMeta SpellAuraRestrictionsRec::s_meta = { "DBFilesClient\\SpellAuraRestrictions.dbc", 9, 36, SpellAuraRestrictionsRec::FixupStrings, 0 };

// PROVEN: e266f51d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellAuraRestrictionsRec::FixupStrings(void* record, const char* strings) {
}
