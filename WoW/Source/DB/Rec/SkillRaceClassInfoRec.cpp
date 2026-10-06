#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"

// STATIC: 0x00C223A8/0x140A171F8
const DBCMeta SkillRaceClassInfoRec::s_meta = { "DBFilesClient\\SkillRaceClassInfo.dbc", 9, 36, SkillRaceClassInfoRec::FixupStrings, 0 };

// PROVEN: 9013b2aa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SkillRaceClassInfoRec::FixupStrings(void* record, const char* strings) {
}
