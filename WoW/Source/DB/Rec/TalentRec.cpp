#include "WoW/Source/DB/Rec/TalentRec.h"

// STATIC: 0x00C20408/0x140A14C20
const DBCMeta TalentRec::s_meta = { "DBFilesClient\\Talent.dbc", 10, 76, TalentRec::FixupStrings, 0 };

// PROVEN: bc046477 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TalentRec::FixupStrings(void* record, const char* strings) {
}
