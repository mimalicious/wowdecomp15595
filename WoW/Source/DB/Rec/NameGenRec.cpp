#include "WoW/Source/DB/Rec/NameGenRec.h"

// STATIC: %0x00B43858
const DBCMeta NameGenRec::s_meta = { "DBFilesClient\\NameGen.dbc", 4, 16, NameGenRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void NameGenRec::FixupStrings(void* record, const char* strings) {
    NameGenRec* rec = static_cast<NameGenRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void NameGenRec::ConvertRow(NameGenRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_raceID, &row[0x0008], sizeof(rec->m_raceID));
    memcpy(&rec->m_sex, &row[0x000C], sizeof(rec->m_sex));
}
#endif
