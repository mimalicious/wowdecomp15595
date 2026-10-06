#include "WoW/Source/DB/Rec/QuestXPRec.h"

// STATIC: %0x00B45840
const DBCMeta QuestXPRec::s_meta = { "DBFilesClient\\QuestXP.dbc", 2, 44, QuestXPRec::FixupStrings, 0 };

// PROVEN: 59426fed IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestXPRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void QuestXPRec::ConvertRow(QuestXPRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_difficulty, &row[0x0004], sizeof(rec->m_difficulty));
}
#endif
