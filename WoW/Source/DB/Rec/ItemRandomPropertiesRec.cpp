#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"

// STATIC: %0x00B45FB0
const DBCMeta ItemRandomPropertiesRec::s_meta = { "DBFilesClient\\ItemRandomProperties.dbc", 4, 32, ItemRandomPropertiesRec::FixupStrings, 0 };

// OFFSET: %0x0097C390
void ItemRandomPropertiesRec::FixupStrings(void* record, const char* strings) {
    ItemRandomPropertiesRec* rec = static_cast<ItemRandomPropertiesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_nameLang = &strings[reinterpret_cast<uintptr_t>(rec->m_nameLang)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemRandomPropertiesRec::ConvertRow(ItemRandomPropertiesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(rec->m_enchantment, &row[0x0008], sizeof(rec->m_enchantment));
    rec->m_nameLang = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
}
#endif
