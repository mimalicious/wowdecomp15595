#include "WoW/Source/DB/Rec/QuestInfoRec.h"

// STATIC: 0x00C22DA0/0x140A17E38
const DBCMeta QuestInfoRec::s_meta = { "DBFilesClient\\QuestInfo.dbc", 2, 8, QuestInfoRec::FixupStrings, 0 };

// PROVEN: 7d66f6ef IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void QuestInfoRec::FixupStrings(void* record, const char* strings) {
    QuestInfoRec* rec = static_cast<QuestInfoRec*>(record);
    DBCFixupString(rec->m_infoName, strings);
}
