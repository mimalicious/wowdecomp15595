#include "WoW/Source/DB/Rec/CurrencyCategoryRec.h"

// STATIC: %0x00B46DFC
const DBCMeta CurrencyCategoryRec::s_meta = { "DBFilesClient\\CurrencyCategory.dbc", 3, 12, CurrencyCategoryRec::FixupStrings, 0 };

// OFFSET: %0x0097C380
void CurrencyCategoryRec::FixupStrings(void* record, const char* strings) {
    CurrencyCategoryRec* rec = static_cast<CurrencyCategoryRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void CurrencyCategoryRec::ConvertRow(CurrencyCategoryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
}
#endif
