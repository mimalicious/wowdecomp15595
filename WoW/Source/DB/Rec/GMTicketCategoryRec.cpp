#include "WoW/Source/DB/Rec/GMTicketCategoryRec.h"

// STATIC: %0x00B467F8
const DBCMeta GMTicketCategoryRec::s_meta = { "DBFilesClient\\GMTicketCategory.dbc", 2, 8, GMTicketCategoryRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void GMTicketCategoryRec::FixupStrings(void* record, const char* strings) {
    GMTicketCategoryRec* rec = static_cast<GMTicketCategoryRec*>(record);
    rec->m_category = &strings[reinterpret_cast<uintptr_t>(rec->m_category)];
}

#if defined(WOWDECOMP_ARCH_64)
void GMTicketCategoryRec::ConvertRow(GMTicketCategoryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_category = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
