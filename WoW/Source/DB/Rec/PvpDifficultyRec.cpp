#include "WoW/Source/DB/Rec/PvpDifficultyRec.h"

// STATIC: 0x00C22E88/0x140A17F50
const DBCMeta PvpDifficultyRec::s_meta = { "DBFilesClient\\PvpDifficulty.dbc", 6, 24, PvpDifficultyRec::FixupStrings, 0 };

// PROVEN: b210418e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void PvpDifficultyRec::FixupStrings(void* record, const char* strings) {
}
