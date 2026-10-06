#include "WoW/Source/DB/Rec/QuestFactionRewardRec.h"

// STATIC: 0x00C22E00/0x140A17EB0
const DBCMeta QuestFactionRewardRec::s_meta = { "DBFilesClient\\QuestFactionReward.dbc", 2, 44, QuestFactionRewardRec::FixupStrings, 0 };

// PROVEN: 75b84850 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestFactionRewardRec::FixupStrings(void* record, const char* strings) {
}
