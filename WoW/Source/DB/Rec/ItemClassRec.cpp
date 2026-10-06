#include "WoW/Source/DB/Rec/ItemClassRec.h"

// STATIC: %0x00B43AF0
const DBCMeta ItemClassRec::s_meta = { "DBFilesClient\\ItemClass.dbc", 4, 20, ItemClassRec::FixupStrings, 0 };

// OFFSET: %0x0097C480
void ItemClassRec::FixupStrings(void* record, const char* strings) {
    ItemClassRec* rec = static_cast<ItemClassRec*>(record);
    rec->m_className = &strings[reinterpret_cast<uintptr_t>(rec->m_className)];
}

#if defined(WOWDECOMP_ARCH_64)
void ItemClassRec::ConvertRow(ItemClassRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_classID, &row[0x0004], sizeof(rec->m_classID));
    memcpy(&rec->m_subclassMapID, &row[0x0008], sizeof(rec->m_subclassMapID));
    memcpy(&rec->m_flags, &row[0x000C], sizeof(rec->m_flags));
    rec->m_className = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
}
#endif
