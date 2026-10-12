#include "WoW/Source/DB/Rec/QuestInfoRec.h"

// STATIC: 0x00C22D88/0x140A17E20
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C22D90/0x140A17E28
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C22D98/0x140A17E30
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C22DA0/0x140A17E38
const DBCMeta QuestInfoRec::s_meta = { "DBFilesClient\\QuestInfo.dbc", 2, 8, QuestInfoRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void QuestInfoRec::FixupStrings(void* record, const char* strings) {
    QuestInfoRec* rec = static_cast<QuestInfoRec*>(record);
    DBCFixupString(rec->m_infoName, strings);
}
