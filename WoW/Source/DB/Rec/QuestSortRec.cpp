#include "WoW/Source/DB/Rec/QuestSortRec.h"

// STATIC: 0x00C22C54/0x140A17CA0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C22C5C/0x140A17CA8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C22C64/0x140A17CB0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C22C6C/0x140A17CB8
const DBCMeta QuestSortRec::s_meta = { "DBFilesClient\\QuestSort.dbc", 2, 8, QuestSortRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 02c1a7bb IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void QuestSortRec::FixupStrings(void* record, const char* strings) {
    QuestSortRec* rec = static_cast<QuestSortRec*>(record);
    DBCFixupString(rec->m_sortName, strings);
}
