#include "WoW/Source/DB/Rec/LockTypeRec.h"

// STATIC: %0x00B45CBC
const DBCMeta LockTypeRec::s_meta = { "DBFilesClient\\LockType.dbc", 5, 20, LockTypeRec::FixupStrings, 0 };

// OFFSET: %0x0097C340
void LockTypeRec::FixupStrings(void* record, const char* strings) {
    LockTypeRec* rec = static_cast<LockTypeRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_resourceName = &strings[reinterpret_cast<uintptr_t>(rec->m_resourceName)];
    rec->m_verb = &strings[reinterpret_cast<uintptr_t>(rec->m_verb)];
    rec->m_cursorName = &strings[reinterpret_cast<uintptr_t>(rec->m_cursorName)];
}

#if defined(WOWDECOMP_ARCH_64)
void LockTypeRec::ConvertRow(LockTypeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_resourceName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_verb = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
    rec->m_cursorName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
}
#endif
