#include "WoW/Source/DB/Rec/QuestSortRec.h"

// STATIC: %0x00B45870
const DBCMeta QuestSortRec::s_meta = { "DBFilesClient\\QuestSort.dbc", 2, 8, QuestSortRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void QuestSortRec::FixupStrings(void* record, const char* strings) {
    QuestSortRec* rec = static_cast<QuestSortRec*>(record);
    rec->m_sortName = &strings[reinterpret_cast<uintptr_t>(rec->m_sortName)];
}

#if defined(WOWDECOMP_ARCH_64)
void QuestSortRec::ConvertRow(QuestSortRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_sortName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
