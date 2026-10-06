#include "WoW/Source/DB/Rec/SkillTiersRec.h"

// STATIC: 0x00C222F4/0x140A17120
const DBCMeta SkillTiersRec::s_meta = { "DBFilesClient\\SkillTiers.dbc", 3, 132, SkillTiersRec::FixupStrings, 0 };

// PROVEN: 95f2390d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SkillTiersRec::FixupStrings(void* record, const char* strings) {
}
