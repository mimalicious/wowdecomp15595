#include "WoW/Source/DB/Rec/Cfg_CategoriesRec.h"

// STATIC: %0x00B44020
const DBCMeta Cfg_CategoriesRec::s_meta = { "DBFilesClient\\Cfg_Categories.dbc", 5, 20, Cfg_CategoriesRec::FixupStrings, 0 };

// OFFSET: %0x0097C480
void Cfg_CategoriesRec::FixupStrings(void* record, const char* strings) {
    Cfg_CategoriesRec* rec = static_cast<Cfg_CategoriesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void Cfg_CategoriesRec::ConvertRow(Cfg_CategoriesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_localeMask, &row[0x0004], sizeof(rec->m_localeMask));
    memcpy(&rec->m_createCharsetMask, &row[0x0008], sizeof(rec->m_createCharsetMask));
    memcpy(&rec->m_flags, &row[0x000C], sizeof(rec->m_flags));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
}
#endif
