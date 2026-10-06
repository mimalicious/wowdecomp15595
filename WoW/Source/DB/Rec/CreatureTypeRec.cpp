#include "WoW/Source/DB/Rec/CreatureTypeRec.h"

// STATIC: %0x00B46E64
const DBCMeta CreatureTypeRec::s_meta = { "DBFilesClient\\CreatureType.dbc", 3, 12, CreatureTypeRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void CreatureTypeRec::FixupStrings(void* record, const char* strings) {
    CreatureTypeRec* rec = static_cast<CreatureTypeRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void CreatureTypeRec::ConvertRow(CreatureTypeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_flags, &row[0x0008], sizeof(rec->m_flags));
}
#endif
