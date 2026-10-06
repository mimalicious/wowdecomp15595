#include "WoW/Source/DB/Rec/ItemVisualEffectsRec.h"

// STATIC: %0x00B43A20
const DBCMeta ItemVisualEffectsRec::s_meta = { "DBFilesClient\\ItemVisualEffects.dbc", 2, 8, ItemVisualEffectsRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ItemVisualEffectsRec::FixupStrings(void* record, const char* strings) {
    ItemVisualEffectsRec* rec = static_cast<ItemVisualEffectsRec*>(record);
    rec->m_model = &strings[reinterpret_cast<uintptr_t>(rec->m_model)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemVisualEffectsRec::ConvertRow(ItemVisualEffectsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_model = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
