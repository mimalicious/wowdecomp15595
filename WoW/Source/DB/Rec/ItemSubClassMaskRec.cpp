#include "WoW/Source/DB/Rec/ItemSubClassMaskRec.h"

// STATIC: %0x00B45ED8
const DBCMeta ItemSubClassMaskRec::s_meta = { "DBFilesClient\\ItemSubClassMask.dbc", 3, 16, ItemSubClassMaskRec::FixupStrings, 0 };

// OFFSET: %0x0097C4E0
void ItemSubClassMaskRec::FixupStrings(void* record, const char* strings) {
    ItemSubClassMaskRec* rec = static_cast<ItemSubClassMaskRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemSubClassMaskRec::ConvertRow(ItemSubClassMaskRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_classID, &row[0x0004], sizeof(rec->m_classID));
    memcpy(&rec->m_mask, &row[0x0008], sizeof(rec->m_mask));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
}
#endif
