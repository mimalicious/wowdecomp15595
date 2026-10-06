#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"

// STATIC: %0x00B46054
const DBCMeta ItemLimitCategoryRec::s_meta = { "DBFilesClient\\ItemLimitCategory.dbc", 4, 16, ItemLimitCategoryRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ItemLimitCategoryRec::FixupStrings(void* record, const char* strings) {
    ItemLimitCategoryRec* rec = static_cast<ItemLimitCategoryRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemLimitCategoryRec::ConvertRow(ItemLimitCategoryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_quantity, &row[0x0008], sizeof(rec->m_quantity));
    memcpy(&rec->m_flags, &row[0x000C], sizeof(rec->m_flags));
}
#endif
