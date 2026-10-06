#include "WoW/Source/DB/Rec/ItemPurchaseGroupRec.h"

// STATIC: %0x00B45FE8
const DBCMeta ItemPurchaseGroupRec::s_meta = { "DBFilesClient\\ItemPurchaseGroup.dbc", 3, 40, ItemPurchaseGroupRec::FixupStrings, 0 };

// OFFSET: %0x0097C4B0
void ItemPurchaseGroupRec::FixupStrings(void* record, const char* strings) {
    ItemPurchaseGroupRec* rec = static_cast<ItemPurchaseGroupRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemPurchaseGroupRec::ConvertRow(ItemPurchaseGroupRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_itemID, &row[0x0004], sizeof(rec->m_itemID));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0024])];
}
#endif
