#include "WoW/Source/DB/Rec/ItemBagFamilyRec.h"

// STATIC: %0x00B4633C
const DBCMeta ItemBagFamilyRec::s_meta = { "DBFilesClient\\ItemBagFamily.dbc", 2, 8, ItemBagFamilyRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ItemBagFamilyRec::FixupStrings(void* record, const char* strings) {
    ItemBagFamilyRec* rec = static_cast<ItemBagFamilyRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemBagFamilyRec::ConvertRow(ItemBagFamilyRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
