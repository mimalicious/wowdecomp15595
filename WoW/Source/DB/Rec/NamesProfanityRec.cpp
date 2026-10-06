#include "WoW/Source/DB/Rec/NamesProfanityRec.h"

// STATIC: %0x00B43828
const DBCMeta NamesProfanityRec::s_meta = { "DBFilesClient\\NamesProfanity.dbc", 3, 12, NamesProfanityRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void NamesProfanityRec::FixupStrings(void* record, const char* strings) {
    NamesProfanityRec* rec = static_cast<NamesProfanityRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void NamesProfanityRec::ConvertRow(NamesProfanityRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_language, &row[0x0008], sizeof(rec->m_language));
}
#endif
