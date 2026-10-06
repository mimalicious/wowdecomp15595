#include "WoW/Source/DB/Rec/SpellDifficultyRec.h"

// STATIC: 0x00C2167C/0x140A16280
const DBCMeta SpellDifficultyRec::s_meta = { "DBFilesClient\\SpellDifficulty.dbc", 2, 20, SpellDifficultyRec::FixupStrings, 0 };

// PROVEN: 52c001bf IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellDifficultyRec::FixupStrings(void* record, const char* strings) {
}
