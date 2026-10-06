#include "WoW/Source/DB/Rec/TalentTabRec.h"

// STATIC: %0x00B44A54
const DBCMeta TalentTabRec::s_meta = { "DBFilesClient\\TalentTab.dbc", 10, 44, TalentTabRec::FixupStrings, 0 };

// OFFSET: %0x0097C1B0
void TalentTabRec::FixupStrings(void* record, const char* strings) {
    TalentTabRec* rec = static_cast<TalentTabRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_backgroundFile = &strings[reinterpret_cast<uintptr_t>(rec->m_backgroundFile)];
    rec->unk_001C = &strings[reinterpret_cast<uintptr_t>(rec->unk_001C)];
}

#if defined(WOWDECOMP_ARCH_64)
void TalentTabRec::ConvertRow(TalentTabRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->m_classMask, &row[0x000C], sizeof(rec->m_classMask));
    memcpy(&rec->m_categoryEnumID, &row[0x0010], sizeof(rec->m_categoryEnumID));
    memcpy(&rec->m_orderIndex, &row[0x0014], sizeof(rec->m_orderIndex));
    rec->m_backgroundFile = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    rec->unk_001C = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
}
#endif
