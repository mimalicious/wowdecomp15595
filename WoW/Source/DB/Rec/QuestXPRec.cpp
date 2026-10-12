#include "WoW/Source/DB/Rec/QuestXPRec.h"

// STATIC: 0x00C22C00/0x140A17C30
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C22C08/0x140A17C38
static const int32_t s_fieldSizes[2] = { 4, 40 };

// STATIC: 0x00C22C10/0x140A17C40
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C22C18/0x140A17C48
const DBCMeta QuestXPRec::s_meta = { "DBFilesClient\\QuestXP.dbc", 2, 44, QuestXPRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void QuestXPRec::FixupStrings(void* record, const char* strings) {
}
