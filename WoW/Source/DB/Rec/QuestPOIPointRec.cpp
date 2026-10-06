#include "WoW/Source/DB/Rec/QuestPOIPointRec.h"

// STATIC: %0x00B458A4
const DBCMeta QuestPOIPointRec::s_meta = { "DBFilesClient\\QuestPOIPoint.dbc", 4, 16, QuestPOIPointRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void QuestPOIPointRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void QuestPOIPointRec::ConvertRow(QuestPOIPointRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
}
#endif
