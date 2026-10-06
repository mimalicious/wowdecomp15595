#include "WoW/Source/DB/Rec/SpellEffectRec.h"

// STATIC: 0x00C1CD48/0x140A109F8
const DBCMeta SpellEffectRec::s_meta = { "DBFilesClient\\SpellEffect.dbc", 22, 108, SpellEffectRec::FixupStrings, 0 };

// PROVEN: 2618e038 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellEffectRec::FixupStrings(void* record, const char* strings) {
}
