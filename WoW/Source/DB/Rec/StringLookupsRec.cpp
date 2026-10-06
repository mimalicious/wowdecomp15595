#include "WoW/Source/DB/Rec/StringLookupsRec.h"

// STATIC: %0x00B44AF0
const DBCMeta StringLookupsRec::s_meta = { "DBFilesClient\\StringLookups.dbc", 2, 8, StringLookupsRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void StringLookupsRec::FixupStrings(void* record, const char* strings) {
    StringLookupsRec* rec = static_cast<StringLookupsRec*>(record);
    rec->m_string = &strings[reinterpret_cast<uintptr_t>(rec->m_string)];
}

#if defined(WOWDECOMP_ARCH_64)
void StringLookupsRec::ConvertRow(StringLookupsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_string = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
