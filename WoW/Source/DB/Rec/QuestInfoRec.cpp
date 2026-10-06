#include "WoW/Source/DB/Rec/QuestInfoRec.h"

// STATIC: %0x00B45908
const DBCMeta QuestInfoRec::s_meta = { "DBFilesClient\\QuestInfo.dbc", 2, 8, QuestInfoRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void QuestInfoRec::FixupStrings(void* record, const char* strings) {
    QuestInfoRec* rec = static_cast<QuestInfoRec*>(record);
    rec->m_infoName = &strings[reinterpret_cast<uintptr_t>(rec->m_infoName)];
}

#if defined(WOWDECOMP_ARCH_64)
void QuestInfoRec::ConvertRow(QuestInfoRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_infoName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
