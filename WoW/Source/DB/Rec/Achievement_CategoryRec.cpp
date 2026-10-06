#include "WoW/Source/DB/Rec/Achievement_CategoryRec.h"

// STATIC: %0x00B472E4
const DBCMeta Achievement_CategoryRec::s_meta = { "DBFilesClient\\Achievement_Category.dbc", 4, 16, Achievement_CategoryRec::FixupStrings, 0 };

// OFFSET: %0x0097C380
void Achievement_CategoryRec::FixupStrings(void* record, const char* strings) {
    Achievement_CategoryRec* rec = static_cast<Achievement_CategoryRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void Achievement_CategoryRec::ConvertRow(Achievement_CategoryRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_parent, &row[0x0004], sizeof(rec->m_parent));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_uiOrder, &row[0x000C], sizeof(rec->m_uiOrder));
}
#endif
