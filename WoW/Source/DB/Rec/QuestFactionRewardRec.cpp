#include "WoW/Source/DB/Rec/QuestFactionRewardRec.h"

// STATIC: 0x00C22DE8/0x140A17E98
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C22DF0/0x140A17EA0
static const int32_t s_fieldSizes[2] = { 4, 40 };

// STATIC: 0x00C22DF8/0x140A17EA8
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C22E00/0x140A17EB0
const DBCMeta QuestFactionRewardRec::s_meta = { "DBFilesClient\\QuestFactionReward.dbc", 2, 44, QuestFactionRewardRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 75b84850 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void QuestFactionRewardRec::FixupStrings(void* record, const char* strings) {
}
