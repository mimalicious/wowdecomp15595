#include "WoW/Source/DB/Rec/Achievement_CriteriaRec.h"

// STATIC: %0x00B47320
const DBCMeta Achievement_CriteriaRec::s_meta = { "DBFilesClient\\Achievement_Criteria.dbc", 19, 92, Achievement_CriteriaRec::FixupStrings, 0 };

// OFFSET: %0x0097C4B0
void Achievement_CriteriaRec::FixupStrings(void* record, const char* strings) {
    Achievement_CriteriaRec* rec = static_cast<Achievement_CriteriaRec*>(record);
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
}

#if defined(WOWDECOMP_ARCH_64)
void Achievement_CriteriaRec::ConvertRow(Achievement_CriteriaRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0024])];
    memcpy(&rec->m_flags, &row[0x0028], sizeof(rec->m_flags));
    memcpy(&rec->m_timerStartEvent, &row[0x002C], sizeof(rec->m_timerStartEvent));
    memcpy(&rec->m_timerAssetID, &row[0x0030], sizeof(rec->m_timerAssetID));
    memcpy(&rec->m_timerTime, &row[0x0034], sizeof(rec->m_timerTime));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
    memcpy(&rec->unk_003C, &row[0x003C], sizeof(rec->unk_003C));
    memcpy(&rec->unk_0040, &row[0x0040], sizeof(rec->unk_0040));
    memcpy(&rec->unk_0044, &row[0x0044], sizeof(rec->unk_0044));
    memcpy(&rec->unk_0048, &row[0x0048], sizeof(rec->unk_0048));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
    memcpy(&rec->unk_0050, &row[0x0050], sizeof(rec->unk_0050));
    memcpy(&rec->unk_0054, &row[0x0054], sizeof(rec->unk_0054));
    memcpy(&rec->unk_0058, &row[0x0058], sizeof(rec->unk_0058));
}
#endif
