#include "WoW/Source/DB/Rec/LanguagesRec.h"

// STATIC: %0x00B45E6C
const DBCMeta LanguagesRec::s_meta = { "DBFilesClient\\Languages.dbc", 2, 8, LanguagesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void LanguagesRec::FixupStrings(void* record, const char* strings) {
    LanguagesRec* rec = static_cast<LanguagesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void LanguagesRec::ConvertRow(LanguagesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
