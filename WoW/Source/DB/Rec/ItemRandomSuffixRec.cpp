#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"

// STATIC: %0x00B45F74
const DBCMeta ItemRandomSuffixRec::s_meta = { "DBFilesClient\\ItemRandomSuffix.dbc", 5, 52, ItemRandomSuffixRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void ItemRandomSuffixRec::FixupStrings(void* record, const char* strings) {
    ItemRandomSuffixRec* rec = static_cast<ItemRandomSuffixRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_internalName = &strings[reinterpret_cast<uintptr_t>(rec->m_internalName)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemRandomSuffixRec::ConvertRow(ItemRandomSuffixRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_internalName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(rec->m_enchantment, &row[0x000C], sizeof(rec->m_enchantment));
    memcpy(rec->m_allocationPct, &row[0x0020], sizeof(rec->m_allocationPct));
}
#endif
