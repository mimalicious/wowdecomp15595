#include "WoW/Source/DB/Rec/CharTitlesRec.h"

// STATIC: %0x00B46FEC
const DBCMeta CharTitlesRec::s_meta = { "DBFilesClient\\CharTitles.dbc", 6, 24, CharTitlesRec::FixupStrings, 0 };

// OFFSET: %0x0097C460
void CharTitlesRec::FixupStrings(void* record, const char* strings) {
    CharTitlesRec* rec = static_cast<CharTitlesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_name1 = &strings[reinterpret_cast<uintptr_t>(rec->m_name1)];
}

#if defined(WOWDECOMP_ARCH_64)
void CharTitlesRec::ConvertRow(CharTitlesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_conditionID, &row[0x0004], sizeof(rec->m_conditionID));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_name1 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    memcpy(&rec->m_maskID, &row[0x0010], sizeof(rec->m_maskID));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
}
#endif
