#include "WoW/Source/DB/Rec/ItemPetFoodRec.h"

// STATIC: %0x00B4601C
const DBCMeta ItemPetFoodRec::s_meta = { "DBFilesClient\\ItemPetFood.dbc", 2, 8, ItemPetFoodRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ItemPetFoodRec::FixupStrings(void* record, const char* strings) {
    ItemPetFoodRec* rec = static_cast<ItemPetFoodRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemPetFoodRec::ConvertRow(ItemPetFoodRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
