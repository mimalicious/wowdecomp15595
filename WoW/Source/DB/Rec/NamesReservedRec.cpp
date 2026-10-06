#include "WoW/Source/DB/Rec/NamesReservedRec.h"

// STATIC: %0x00B437F0
const DBCMeta NamesReservedRec::s_meta = { "DBFilesClient\\NamesReserved.dbc", 3, 12, NamesReservedRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void NamesReservedRec::FixupStrings(void* record, const char* strings) {
    NamesReservedRec* rec = static_cast<NamesReservedRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void NamesReservedRec::ConvertRow(NamesReservedRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_language, &row[0x0008], sizeof(rec->m_language));
}
#endif
