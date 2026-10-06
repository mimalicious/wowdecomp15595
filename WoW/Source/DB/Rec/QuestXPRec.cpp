#include "WoW/Source/DB/Rec/QuestXPRec.h"

// STATIC: 0x00C22C18/0x140A17C48
const DBCMeta QuestXPRec::s_meta = { "DBFilesClient\\QuestXP.dbc", 2, 44, QuestXPRec::FixupStrings, 0 };

// PROVEN: 59426fed IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestXPRec::FixupStrings(void* record, const char* strings) {
}
