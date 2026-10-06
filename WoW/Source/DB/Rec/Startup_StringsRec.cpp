#include "WoW/Source/DB/Rec/Startup_StringsRec.h"

// STATIC: %0x00AC8320
const DBCMeta Startup_StringsRec::s_meta = { "DBFilesClient\\Startup_Strings.dbc", 3, 12, Startup_StringsRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void Startup_StringsRec::FixupStrings(void* record, const char* strings) {
    Startup_StringsRec* rec = static_cast<Startup_StringsRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_message = &strings[reinterpret_cast<uintptr_t>(rec->m_message)];
}

#if defined(WOWDECOMP_ARCH_64)
void Startup_StringsRec::ConvertRow(Startup_StringsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_message = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
}
#endif
