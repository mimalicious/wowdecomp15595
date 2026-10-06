#include "WoW/Source/DB/Rec/QuestFactionRewardRec.h"

// STATIC: %0x00B45944
const DBCMeta QuestFactionRewardRec::s_meta = { "DBFilesClient\\QuestFactionReward.dbc", 2, 44, QuestFactionRewardRec::FixupStrings, 0 };

// PROVEN: 75b84850 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestFactionRewardRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void QuestFactionRewardRec::ConvertRow(QuestFactionRewardRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_difficulty, &row[0x0004], sizeof(rec->m_difficulty));
}
#endif
