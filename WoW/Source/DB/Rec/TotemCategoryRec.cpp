#include "WoW/Source/DB/Rec/TotemCategoryRec.h"

// STATIC: %0x00B448E4
const DBCMeta TotemCategoryRec::s_meta = { "DBFilesClient\\TotemCategory.dbc", 4, 16, TotemCategoryRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void TotemCategoryRec::FixupStrings(void* record, const char* strings) {
    TotemCategoryRec* rec = static_cast<TotemCategoryRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void TotemCategoryRec::ConvertRow(TotemCategoryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_totemCategoryType, &row[0x0008], sizeof(rec->m_totemCategoryType));
    memcpy(&rec->m_totemCategoryMask, &row[0x000C], sizeof(rec->m_totemCategoryMask));
}
#endif
