#include "WoW/Source/DB/Rec/PowerDisplayRec.h"

// STATIC: %0x00B459AC
const DBCMeta PowerDisplayRec::s_meta = { "DBFilesClient\\PowerDisplay.dbc", 6, 16, PowerDisplayRec::FixupStrings, 0 };

// OFFSET: %0x0097C380
void PowerDisplayRec::FixupStrings(void* record, const char* strings) {
    PowerDisplayRec* rec = static_cast<PowerDisplayRec*>(record);
    rec->m_globalStringBaseTag = &strings[reinterpret_cast<uintptr_t>(rec->m_globalStringBaseTag)];
}

#if defined(WOWDECOMP_ARCH_64)
void PowerDisplayRec::ConvertRow(PowerDisplayRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_actualType, &row[0x0004], sizeof(rec->m_actualType));
    rec->m_globalStringBaseTag = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_red, &row[0x000C], sizeof(rec->m_red));
    memcpy(&rec->m_green, &row[0x000D], sizeof(rec->m_green));
    memcpy(&rec->m_blue, &row[0x000E], sizeof(rec->m_blue));
}
#endif
